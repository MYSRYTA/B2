//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshSpatialQueryT_Octree<T>::PolygonMeshSpatialQueryT_Octree() {
	this->attr.getOrCreatVec3Attribute ( "octreeNodePositions" );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshから空間分析のための高速化構造を構築する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshSpatialQueryT_Octree<T>::PolygonMeshSpatialQueryT_Octree( const PolygonMeshT<T> &mesh ) {
	assert ( 0<mesh.polygonCount () );
	this->octree.set ( mesh );
	std::vector<Vec3T<T>> allNodePos;
	this->octree.getAllNodeTreePositions ( this->octree.nodeTree, allNodePos );
	assert ( 0 < allNodePos.size () );
	Vec3AttributeT<T> &octNodePosAttr = this->attr.getOrCreatVec3Attribute ( "octreeNodePositions" );
	octNodePosAttr.resetValues ( allNodePos );
	this->initialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshから空間分析のための高速化構造を構築する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshSpatialQueryT_Octree<T>::PolygonMeshSpatialQueryT_Octree( const PolygonMeshT<T> &mesh, const std::vector<f32> &maskVal, Bool invertMask ) {
	assert ( 0<mesh.polygonCount () );
	this->octree.set ( mesh, maskVal, invertMask );
	std::vector<Vec3T<T>> allNodePos;
	this->octree.getAllNodeTreePositions ( this->octree.nodeTree, allNodePos );
	assert ( 0 < allNodePos.size () );
	Vec3AttributeT<T> &octNodePosAttr = this->attr.getOrCreatVec3Attribute ( "octreeNodePositions" );
	octNodePosAttr.resetValues ( allNodePos );
	this->initialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    startPositionからPolygonMeshSpatialQueryの表面上への最も近いロケーションを取得する
//!		@brief	startPositionからPolygonMeshSpatialQueryの表面上への最も近いロケーションを取得する
//!		@param[in] startPosition　スタート位置
//!		@param[in] maxDistance　検索距離
//!		@param[in] faceSide　検索対象のポリゴンフェース面 0=両面、1=表面、2=裏面
//!		@return GeometryLocation
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
GeometryLocationT<T> PolygonMeshSpatialQueryT_Octree<T>::getClosestLocation ( const Vec3T<T> &startPosition, T maxDistance, u8 faceSide, T tolerance ) const {
	assert ( this->isValid () );
	GeometryLocationT<T> result;
	
	// OctreeNodeの中心座標群から最も近い座標を見つける
	Vec3AttributeT<T> &octNodePosAttr = this->attr.getVec3Attribute ( "octreeNodePositions" );
	u32 poiID = Burgess2MathFn::GetClosestPointID ( octNodePosAttr.getValuesRef (), startPosition );
	if ( poiID == BURGESS2::U32_INFINITE ) { return result; }
	T closestDis = startPosition.distanceTo ( octNodePosAttr[poiID] );
	// 最短距離内に存在する全てのTriPolygonのIDを取得
	std::vector<u32> triPolyIDs = this->octree.getAllClosestTriPolygonIDs ( startPosition, closestDis );
	if ( triPolyIDs.size () == 0 ) { return result; }
	
	// 各TriPolygonへの最も近い座標を計算する
	std::vector<Bool> done ( this->octree.triangleCount () );
	s32 triPolyIDSize = triPolyIDs.size ();
	std::vector<T> dist2 ( triPolyIDSize );
	#pragma omp parallel for 
	for ( s32 i = 0; i<triPolyIDSize; i++ ) {
		u32 triPolyID = triPolyIDs [i];
		if ( done[triPolyID] ){
			dist2[i] = maxDistance + 1.0;// T( BURGESS2::F32_INFINITE );
			continue;
		}else{
			done[triPolyID] = true;
		}
		OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyID];
		Vec3T<T> closestPos = Burgess2MathFn::GetClosestPositionOnTriangle ( startPosition, triPoly.p0, triPoly.p1, triPoly.p2 );
		// フェースの裏表を確認して条件に合った場合は配列に登録していく
		if ( startPosition.almostEqual( closestPos, tolerance ) ) {
			dist2[i] = T( 0.0 );
		} else {
			switch ( faceSide ) {
				case 0:// PolygonMeshSpatialQueryT::Side::bothFaces:
					dist2[i] = ( startPosition - closestPos ).length();
					break;
				case 1:// PolygonMeshSpatialQueryT::Side::frontFace:
				{
					Vec3T<T> N = Burgess2MathFn::GetTriangleNormal( triPoly.p0, triPoly.p1, triPoly.p2 );
					Vec3T<T> sToP = startPosition.almostEqual( closestPos, tolerance ) ? N : ( startPosition - closestPos ).unit();
					if ( N.dot( sToP ) < T( 0.0 ) ) {
						// 検索結果が裏面だった場合は無効値を格納 
						dist2[i] = T( BURGESS2::F32_INFINITE );
					} else {
						dist2[i] = ( startPosition - closestPos ).length();
					}
					break;
				}
				case 2: //PolygonMeshSpatialQueryT::Side::backFace:
				{
					Vec3T<T> N = Burgess2MathFn::GetTriangleNormal( triPoly.p0, triPoly.p1, triPoly.p2 );
					Vec3T<T> sToP = startPosition.almostEqual( closestPos, tolerance ) ? N : ( startPosition - closestPos ).unit();
					if ( T( 0.0 ) < N.dot( sToP ) ) {
						// 検索結果が表面だった場合は無効値を格納 
						dist2[i] = T( BURGESS2::F32_INFINITE );
					} else {
						dist2[i] = ( startPosition - closestPos ).length();
					}
					break;
				}
			}
		}
	}
	std::vector<T> dist3 = dist2;
	// 最も距離の近いTriPolygonの配列番号を取得
	std::vector<u32> arrayOrigID;
	Burgess2MathFn::Sort( dist2, arrayOrigID );
	u32 id = arrayOrigID[0];
	
	// 検索結果が有効値ではなかった場合、または検索結果が最大距離以上の場合は終了
	if ( dist2[0] == T( BURGESS2::F32_INFINITE ) || maxDistance < dist2[0] ) { return result; }

	// GeometryLocationT型で返す
	OctreeTriPolygonT<T> triPoly = this->octree.triPolygons [triPolyIDs[id]];
	Vec3T<T> closestPos = Burgess2MathFn::GetClosestPositionOnTriangle ( startPosition, triPoly.p0, triPoly.p1, triPoly.p2 );
	Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric ( closestPos, triPoly.p0, triPoly.p1, triPoly.p2, tolerance );
	result = GeometryLocationT<T> ( triPoly.origPolygonID, triPoly.localTriID, weight, 3 );

	// 同じ距離に複数のロケーションがあった場合
	// ポリゴン番号が小さい方、更にトライアングル番号の小さい方でresultを上書きする
	for ( u32 i = 0; i < dist2.size(); i++ ) {
		if ( Burgess2MathFn::AlmostEqual( dist2[0], dist2[i], tolerance ) == false ) {
			break;
		}
		u32 id2 = arrayOrigID[i];
		OctreeTriPolygonT<T> triPoly2 = this->octree.triPolygons[triPolyIDs[id2]];
		if ( triPoly2.origPolygonID < result.getIndex() ) {
			Vec3T<T> closestPos2 = Burgess2MathFn::GetClosestPositionOnTriangle( startPosition, triPoly2.p0, triPoly2.p1, triPoly2.p2 );
			Vec3T<T> weight2 = Burgess2MathFn::GetTriBarycentric( closestPos2, triPoly2.p0, triPoly2.p1, triPoly2.p2, tolerance );
			result = GeometryLocationT<T>( triPoly2.origPolygonID, triPoly2.localTriID, weight2, 3 );
		}
		if ( triPoly2.origPolygonID == result.getIndex() ) {
			if ( triPoly2.localTriID < result.getSubIndex() ) {
				Vec3T<T> closestPos2 = Burgess2MathFn::GetClosestPositionOnTriangle( startPosition, triPoly2.p0, triPoly2.p1, triPoly2.p2 );
				Vec3T<T> weight2 = Burgess2MathFn::GetTriBarycentric( closestPos2, triPoly2.p0, triPoly2.p1, triPoly2.p2, tolerance );
				result = GeometryLocationT<T>( triPoly2.origPolygonID, triPoly2.localTriID, weight2, 3 );
			}
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshSpatialQueryに対してRayが最初に交差したロケーションを取得する
//!		@brief	PolygonMeshSpatialQueryに対してRayが最初に交差したロケーションを取得する
//!		@param[in] ray
//!		@param[in] maxDistance　検索距離
//!		@param[in] faceSide　検索対象のポリゴンフェース面 0=両面、1=表面、2=裏面
//!		@return GeometryLocation
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
GeometryLocationT<T> PolygonMeshSpatialQueryT_Octree<T>::raycast ( const Ray3T<T> &ray, T maxDistance, u8 faceSide, T tolerance )const {
	assert ( this->isValid () );
	GeometryLocationT<T> result;

	// Rayと交差する可能性のある全てのTriPolygonのIDを取得
	std::vector<u32> triPolyIDs = this->octree.getAllRayCastTriPolygonIDs ( ray );
	if ( triPolyIDs.size () == 0 ){ return result; }
	
	// 各TriPolygonに対してRayと交差する座標までの距離を計算する
	s32 triPolyIDSize = triPolyIDs.size ();
	std::vector<T> dist ( triPolyIDSize );
	#pragma omp parallel for
	for ( s32 i = 0; i < triPolyIDSize; i++ ){
		u32 triPolyID = triPolyIDs[i];
		OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyID];
		if ( Burgess2MathFn::TestRayIntersectPlane ( ray, triPoly.p0, Burgess2MathFn::GetTriangleNormal ( triPoly.p0, triPoly.p1, triPoly.p2 ) ) ){
			Vec3T<T> raycastPos = Burgess2MathFn::GetRayIntersectPositionToTriangle ( ray, triPoly.p0, triPoly.p1, triPoly.p2, faceSide, tolerance );
			if ( raycastPos.almostEqual ( Vec3T<T> ( BURGESS2::F32_INFINITE ), tolerance ) == false ){
				dist[i] = ray.getStartPosition ().distanceTo ( raycastPos );
				continue;
			}
		}
		dist[i] = T ( BURGESS2::F32_INFINITE );
	}

// maxDistance以内の距離でRayが交差したTriPolygonだけのリストを作成
std::vector<u32> triPolyIDs2;
std::vector<T> dist2;
for ( u32 i = 0; i < triPolyIDSize; i++ ) {
	if ( Burgess2MathFn::AlmostEqual( dist[i], T( BURGESS2::F32_INFINITE ), tolerance ) == false && dist[i] < maxDistance ) {
		dist2.push_back( dist[i] );
		triPolyIDs2.push_back( triPolyIDs[i] );
	}
}
if ( dist2.size() == 0 ) { return result; }

// 最も距離の近いTriPolygonの配列番号を取得
std::vector<u32> arrayOrigID;
Burgess2MathFn::Sort( dist2, arrayOrigID );
u32 id = arrayOrigID[0];

// GeometryLocationT型で返す
OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyIDs2[id]];
Vec3T<T> raycastPos = Burgess2MathFn::GetRayIntersectPositionToTriangle( ray, triPoly.p0, triPoly.p1, triPoly.p2, faceSide, tolerance );
Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric( raycastPos, triPoly.p0, triPoly.p1, triPoly.p2 );
result = GeometryLocationT<T>( triPoly.origPolygonID, triPoly.localTriID, weight, 3 );

// 同じ距離に複数のロケーションがあった場合
// ポリゴン番号が小さい方、更にトライアングル番号の小さい方でresultを上書きする
for ( u32 i = 0; i < dist2.size(); i++ ) {
	if ( Burgess2MathFn::AlmostEqual( dist2[0], dist2[i], tolerance ) == false ) {
		break;
	}
	u32 id2 = arrayOrigID[i];
	OctreeTriPolygonT<T> triPoly2 = this->octree.triPolygons[triPolyIDs2[id2]];
	if ( triPoly2.origPolygonID < result.getIndex() ) {
		Vec3T<T> raycastPos2 = Burgess2MathFn::GetRayIntersectPositionToTriangle( ray, triPoly2.p0, triPoly2.p1, triPoly2.p2, faceSide, tolerance );
		Vec3T<T> weight2 = Burgess2MathFn::GetTriBarycentric( raycastPos2, triPoly2.p0, triPoly2.p1, triPoly2.p2 );
		result = GeometryLocationT<T>( triPoly2.origPolygonID, triPoly2.localTriID, weight2, 3 );
	}
	if ( triPoly2.origPolygonID == result.getIndex() ) {
		if ( triPoly2.localTriID < result.getSubIndex() ) {
			Vec3T<T> raycastPos2 = Burgess2MathFn::GetRayIntersectPositionToTriangle( ray, triPoly2.p0, triPoly2.p1, triPoly2.p2, faceSide, tolerance );
			Vec3T<T> weight2 = Burgess2MathFn::GetTriBarycentric( raycastPos2, triPoly2.p0, triPoly2.p1, triPoly2.p2 );
			result = GeometryLocationT<T>( triPoly2.origPolygonID, triPoly2.localTriID, weight2, 3 );
		}
	}
}
return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Rayが交差した全てのロケーションを近い順で取得する
//!		@brief	Rayが交差した全てのロケーションを近い順で取得する
//!		@param[in] ray
//!		@param[in] maxDistance　検索距離
//!		@param[in] faceSide　検索対象のポリゴンフェース面 0=両面、1=表面、2=裏面
//!		@param[in] omitDuplication ロケーションがオンエッジ/オンポイントの際に、隣接する複数のポリゴンの中からポリゴン番号が最も小さいロケーションデータ以外を省く場合はtrue
//!		@return std::vector<GeometryLocation> 交差した全てのロケーション（近い順）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<GeometryLocationT<T>> PolygonMeshSpatialQueryT_Octree<T>::getAllRayIntersect( const Ray3T<T>& ray, T maxDistance, u8 faceSide, Bool omitDuplication, T tolerance )const {
	assert( this->isValid() );
	std::vector<GeometryLocationT<T>> result( 0 );

	// Rayと交差する可能性のある全てのTriPolygonのIDを取得
	std::vector<u32> triPolyIDs = this->octree.getAllRayCastTriPolygonIDs( ray );
	if ( triPolyIDs.size() == 0 ) { return result; }

	// 各TriPolygonに対してRayと交差する座標までの距離を計算する
	s32 triPolyIDSize = triPolyIDs.size();
	std::vector<T> distSQ( triPolyIDSize );
	std::vector<Vec3T<T>> raycastPos( triPolyIDSize );
	#pragma omp parallel for 
	for ( s32 i = 0; i < triPolyIDSize; i++ ) {
		u32 triPolyID = triPolyIDs[i];
		OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyID];
		if ( Burgess2MathFn::TestRayIntersectPlane( ray, triPoly.p0, Burgess2MathFn::GetTriangleNormal( triPoly.p0, triPoly.p1, triPoly.p2 ) ) ) {
			raycastPos[i] = Burgess2MathFn::GetRayIntersectPositionToTriangle( ray, triPoly.p0, triPoly.p1, triPoly.p2, faceSide, tolerance );
			if ( raycastPos[i] != Vec3T<T>( BURGESS2::F32_INFINITE ) ) {
				distSQ[i] = ( ray.getStartPosition () - raycastPos[i] ).lengthSquared();
				continue;
			}
		}
		distSQ[i] = T( BURGESS2::F32_INFINITE );
	}

	// maxDistance以内の距離でRayが交差したTriPolygonだけのリストを作成
	std::vector<u32> triPolyIDs2;
	std::vector<T> distSQ2;
	std::vector<Vec3T<T>> raycastPos2;
	triPolyIDs2.reserve ( triPolyIDSize );
	distSQ2.reserve ( triPolyIDSize );
	raycastPos2.reserve ( triPolyIDSize );
	for ( u32 i = 0; i < triPolyIDSize; i++ ) {
		if (distSQ[i] != T( BURGESS2::F32_INFINITE ) && distSQ[i] < SQ(maxDistance) ) {
			distSQ2.push_back( distSQ[i] );
			triPolyIDs2.push_back( triPolyIDs[i] );
			raycastPos2.push_back( raycastPos[i] );
		}
	}
	if (distSQ2.size() == 0 ) { return result; }

	// 距離の近い順にソートする
	std::vector< u32 > arrayOrigID;
	Burgess2MathFn::Sort( distSQ2, arrayOrigID );
	
	// GeometryLocationT型で返す
	u32 resultSize = distSQ2.size();
	if ( omitDuplication == true ) {
		// 空間分析の位置が同じロケーションの場合、ロケーションデータのポリゴン番号が最も小さいもの、更にトライアングルの番号が小さいものを選別する
		std::vector<Vec3T<T>> usedPos( 0 );
		for ( u32 i = 0; i < resultSize; i++ ) {
			OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyIDs2[arrayOrigID[i]]];
			Bool find = false;
			for ( u32 j = 0; j < usedPos.size(); j++ ) {
				// 既にresultに登録済みのロケーション内に同じ位置のロケーションがあるか検索
				if ( raycastPos2[arrayOrigID[i]].almostEqual( usedPos[j], tolerance ) == true ) {
					// ロケーションのポリゴン番号を比べて、自身の方が小さければresultを上書き
					if ( triPoly.origPolygonID < result[j].getIndex() ) {
						Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric( raycastPos2[arrayOrigID[i]], triPoly.p0, triPoly.p1, triPoly.p2, tolerance );
						result[j] = GeometryLocationT<T>( triPoly.origPolygonID, triPoly.localTriID, weight, 3 );
					}
					// ロケーションのポリゴン番号も同じ場合、トライアングルの番号を比べて、自身の方が小さければresultを上書き
					if ( triPoly.origPolygonID == result[j].getIndex() ) {
						if ( triPoly.localTriID < result[j].getSubIndex() ) {
							Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric( raycastPos2[arrayOrigID[i]], triPoly.p0, triPoly.p1, triPoly.p2, tolerance );
							result[j] = GeometryLocationT<T>( triPoly.origPolygonID, triPoly.localTriID, weight, 3 );
						}
					}
					find = true;
					break;
				}
			}
			// 同じロケーションが見つかった場合はスキップ　（resultの上書きが必要であれば↑のfor内で処理済み）
			if ( find == true ) { continue; }
			// resultに新規ロケーションデータを追加
			Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric( raycastPos2[arrayOrigID[i]], triPoly.p0, triPoly.p1, triPoly.p2, tolerance );
			result.push_back( GeometryLocationT<T>( triPoly.origPolygonID, triPoly.localTriID, weight, 3 ) );
			
			usedPos.push_back( raycastPos2[arrayOrigID[i]] );
		}
	} else {
		// 全ての結果を返す
		// ロケーションがトライアングルのオンエッジ/オンポイントの場合、隣接する全てのポリゴンごとのロケーションが返される
		// ポリゴンではなくトライアングルのオンエッジだった場合は隣接するトライアングルのロケーションが返されるので注意
		result.resize( resultSize );
		for ( u32 i = 0; i < resultSize; i++ ) {
			OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyIDs2[arrayOrigID[i]]];
			Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric( raycastPos2[arrayOrigID[i]], triPoly.p0, triPoly.p1, triPoly.p2, tolerance );
			result[i] = GeometryLocationT<T>( triPoly.origPolygonID, triPoly.localTriID, weight, 3 );
		}
	}
	
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Lineが交差した全てのロケーションをstartに近い順で取得する
//!		@brief	Lineが交差した全てのロケーションをstartに近い順で取得する
//!		@param[in] start　ラインのスタート位置
//!		@param[in] end　ラインのエンド位置
//!		@param[in] faceSide　検索対象のポリゴンフェース面 0=両面、1=表面、2=裏面
//!		@param[in] omitDuplication ロケーションがオンエッジ/オンポイントの場合、隣接する複数のポリゴンの中からポリゴン番号が最も小さいロケーションデータ以外を省く場合はtrue
//!		@return std::vector<GeometryLocation> 交差した全てのロケーション（近い順）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<GeometryLocationT<T>> PolygonMeshSpatialQueryT_Octree<T>::getAllLineIntersect ( const Vec3T<T> &start, const Vec3T<T> &end, u8 faceSide, Bool omitDuplication, T tolerance ) const {
	Ray3T<T> ray ( start, end, false );
	T length = ( end-start ).length ();
	return this->getAllRayIntersect ( ray, length, faceSide, omitDuplication, tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshSpatialQueryが有効な場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshSpatialQueryT_Octree<T>::isValid () const {
	return this->initialized;
}
