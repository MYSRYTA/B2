namespace BurgessEmbree
{
	template<typename T>
	void gather_all_hits ( const struct RTCFilterFunctionNArguments* args ) {
		IntersectContext<T>* context = (IntersectContext<T>*)args->context;
		HitList<T>& hits = context->hits;
		RTCRay* ray = (RTCRay*)args->ray;
		RTCHit* hit = (RTCHit*)args->hit;
		args->valid[0] = 0; // ignore all hits

		// avoid overflow of hits array
		if (hits.end >= hits.maxHitCount) return;

		if (hits.side == 1 || hits.side == 2) {
			Vec3T<T> r{ ray->dir_x, ray->dir_y, ray->dir_z };
			Vec3T<T> d{ hit->Ng_x, hit->Ng_y, hit->Ng_z };
			T dot = r.dot(d);
			switch (hits.side) {
				case 1:	if (T(0.0) <= dot) { return; }break;
				case 2: if (dot <= T(0.0)) { return; }break;
			}
		}

		// check if geometry is opaque
		RTCGeometry* geometry = (RTCGeometry*)args->geometryUserPtr;
		bool opaque = !true;// g_enable_opacity;
		
		// add hit to list
		hits.hits[hits.end++] = HitList<T>::Hit ( opaque, ray->tfar, hit->u, hit->v, hit->primID, hit->geomID, hit->instID[0] );
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	HitList<T> getRayIntersect_task ( const RTCScene& scene, RTCRayHit& rayhit, u8 faceSide, u32 maxHitCount ) {
		HitList<T> hits_o ( maxHitCount );
		hits_o.side = faceSide;
		// trace ray to gather all hits
		IntersectContext<T> context ( hits_o );
		rtcInitIntersectContext ( &context.context );
		context.context.filter = BurgessEmbree::gather_all_hits<T>;
		rtcIntersect1 ( scene, &context.context, &rayhit );
		
		// Rayの交差がオンポイントorオンエッジの場合に交差判定が行われない問題(レポート#174,#181)の対応
		// ただし検出されなかったオンポイントorオンエッジ以外で交差があった場合はこの対応では不十分
		if ( hits_o.size() == 0 ){
			rayhit.ray.org_x += BURGESS2::TOLERANCE;
			rayhit.ray.org_y += BURGESS2::TOLERANCE;
			rayhit.ray.org_z += BURGESS2::TOLERANCE;
			rtcIntersect1( scene, &context.context, &rayhit );
		}
		if ( hits_o.size() == 0 ){
			rayhit.ray.org_x -= ( BURGESS2::TOLERANCE * 2 );
			rayhit.ray.org_y -= ( BURGESS2::TOLERANCE * 2 );
			rayhit.ray.org_z -= ( BURGESS2::TOLERANCE * 2 );
			rtcIntersect1( scene, &context.context, &rayhit );
		}

		// 交差が無ければ終了
		if ( hits_o.size() == 0 ){
			return hits_o;
		}

		if ( 0 < hits_o.size() ){
			// sort hits by extended order
			std::sort( &context.hits.hits[context.hits.begin], &context.hits.hits[context.hits.end] );

			// ignore duplicated hits that can occur for tesselated primitives
			unsigned int i = 0, j = 1;
			for ( ; j < hits_o.size(); j++ ){
				if ( hits_o.hits[i] == hits_o.hits[j] ) continue;
				hits_o.hits[++i] = hits_o.hits[j];
			}
			hits_o.end = i + 1;

			// drop hits in case we found too many
			hits_o.end = Burgess2MathFn::Min( hits_o.end, maxHitCount );
		}
		return hits_o;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	Bool closestPointFunc ( RTCPointQueryFunctionArguments* args ) {
		if (args->userPtr == false) { return false; }
		const u32 triID = args->primID;

		PointQuery<T>* result = (PointQuery<T>*)args->userPtr;
		u32 p0ID = result->packedPointIDsPtr->at(triID * 3 );
		u32 p1ID = result->packedPointIDsPtr->at(triID * 3 + 1);
		u32 p2ID = result->packedPointIDsPtr->at(triID * 3 + 2);
		Vec3T<T> start{ args->query->x, args->query->y, args->query->z };
		Vec3T<T> &p0 = result->pointPositionsPtr->at(p0ID);
		Vec3T<T> &p1 = result->pointPositionsPtr->at(p1ID);
		Vec3T<T> &p2 = result->pointPositionsPtr->at(p2ID);
		Vec3T<T> locPosition = Burgess2MathFn::GetClosestPositionOnTriangle ( start, p0, p1, p2 );
		if(Burgess2MathFn::AlmostEqual(locPosition.x(), BURGESS2::F32_INFINITE)){ return false; }

		// maxDistance and more closest
		T dis = start.distanceTo ( locPosition );
		if (args->query->radius <= dis) {
			return false;
		}
		
		// 面の裏表を判別
		switch (result->closestFaceSide) {
			case 0: // bothFaces
				break;
			case 1: // frontFace
			case 2: // backFace
			{
				Vec3T<T> locNormal = Burgess2MathFn::GetTriangleNormal ( p0, p1, p2 );
				Vec3T<T> locToS = (start - locPosition) / dis;
				T d = locNormal.dot ( locToS );
				switch (result->closestFaceSide) {
					case 1:	if (d < -T(BURGESS2::TOLERANCE)) { return false; } break;
					case 2:	if (T(BURGESS2::TOLERANCE) < d) { return false; } break;
				}
				break;
			}
		}
		args->query->radius = dis;
		result->restClosestPosition = locPosition;
		result->restClosestTriangleID = triID;
		return true;
	}

	template<typename T>
	RTCRayHit toEmbreeRay(const Ray3T<T>& ray3, T maxDistance ) {
		// レイを生成する
		struct RTCRayHit rayhit;
		// レイの始点
		Vec3T<T> pos = ray3.getStartPosition();
		rayhit.ray.org_x = pos.x();
		rayhit.ray.org_y = pos.y();
		rayhit.ray.org_z = pos.z();
		// レイの方向 
		Vec3T<T> dir = ray3.getDirection();
		rayhit.ray.dir_x = dir.x();
		rayhit.ray.dir_y = dir.y();
		rayhit.ray.dir_z = dir.z();
		// 交差判定する範囲を指定 
		rayhit.ray.tnear = 0.0f;     // 範囲の始点
		rayhit.ray.tfar = maxDistance;  // 範囲の終点．交差判定後には交差点までの距離が格納される．

		return rayhit;
	}

}