//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshSpatialQueryT<T>::PolygonMeshSpatialQueryT() {
	// デバイスとシーンを初期化
	this->device = rtcNewDevice(NULL);
	this->scene = rtcNewScene(this->device);
	rtcCommitScene(this->scene);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshから空間分析のための高速化構造を構築する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshSpatialQueryT<T>::setup ( const PolygonMeshT<T> &mesh ) {
	if (0 == mesh.polygonCount()) { return false; }

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// embreeのシーンを構築に必要なデータを準備する
	// 
	// メッシュのポイントポジションアトリビュートをコピー
	this->pointPositions = mesh.getVec3Attribute("pointPositions").getValues();

	s32 polyCnt = mesh.polygonCount ();
	// メッシュのpackedPointIDsをコピー
	if (mesh.isPureTriangleMesh ()) {
		// 三角ポリゴンメッシュの場合
		this->packedPointIDs = mesh.getU32Attribute("packedPointIDs").getValues();
		// BurgessEmbree::trianglePolygonInfoの登録
		this->packedPointIDs.resize ( polyCnt );
		#pragma omp parallel for 
		for (s32 i = 0; i < polyCnt; ++i) {
			this->triangleInfo[i].origPolygonID = i;
			this->triangleInfo[i].localTriID = 0;
		}
	} else {
		// 多角形ポリゴンメッシュの場合
		this->triangleInfo.reserve ( polyCnt * 2 ); // 全ポリゴンが四角形だと仮定してメモリを確保
		this->packedPointIDs.reserve ( polyCnt * 2 * 3 ); // 全ポリゴンが四角形だと仮定してメモリを確保
		// 三角形ポリゴンのリストに変換して登録
		for (s32 i = 0; i < polyCnt; ++i) {
			L8Array<u32> polyPoiIDs = mesh.getPolygonPointIDs ( i );
			s8 maxItr = s8(polyPoiIDs.size () - 2);
			for (s8 j = 0; j < maxItr; ++j) {
				this->packedPointIDs.push_back ( polyPoiIDs[0] );
				this->packedPointIDs.push_back ( polyPoiIDs[j + 1] );
				this->packedPointIDs.push_back ( polyPoiIDs[j + 2] );

				// BurgessEmbree::trianglePolygonInfoの登録
				BurgessEmbree::TrianglePolygonInfo<T> info;
				info.origPolygonID = i;
				info.localTriID = j;
				this->triangleInfo.push_back ( info );
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// embreeのシーンを構築する

	// デバイスを作成
	this->device = rtcNewDevice ( NULL );
	// シーンを作成
	this->scene = rtcNewScene ( this->device );
	// ジオメトリを準備
	RTCGeometry geom = rtcNewGeometry ( this->device, RTC_GEOMETRY_TYPE_TRIANGLE );
	// ジオメトリに頂点情報をセットする
	s32 poiCnt = s32(this->pointPositions.size ());
	T* vertices = (float*)rtcSetNewGeometryBuffer ( geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof ( T ), poiCnt );

	// Githubで提案されていた精度のバグ対応
/*	for ( s32 i = 0; i < poiCnt; ++i ){
		Vec3T<T> poiNml = mesh.getPointAverageNormal( i );
		this->pointPositions[i] += ( poiNml * T( 0.00001 ) );
	}*/
	for (s32 i = 0; i < poiCnt; ++i) {
		Vec3T<T> poiPos = this->pointPositions[ i ];
		vertices[i * 3] = poiPos.x();
		vertices[i * 3 + 1] = poiPos.y();
		vertices[i * 3 + 2] = poiPos.z();
	}
	// ジオメトリに三角ポリゴンのポイントパックをセットする
	u32 triPolyCnt = u32(this->packedPointIDs.size () / 3);
	s32 verCnt = s32(this->packedPointIDs.size ());
	unsigned* indices = (unsigned*)rtcSetNewGeometryBuffer ( geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof ( unsigned ), triPolyCnt );
	for (s32 i = 0; i < verCnt; ++i) {
		indices[i] = this->packedPointIDs[i];
	}

	// embreeシーンへ登録
	rtcSetGeometryPointQueryFunction ( geom, BurgessEmbree::closestPointFunc<T> );
	rtcCommitGeometry ( geom );
	rtcAttachGeometry ( this->scene, geom );
	rtcReleaseGeometry ( geom );
	rtcCommitScene ( this->scene );
	
	// ロバストモード ON
	RTCSceneFlags flags = rtcGetSceneFlags( this->scene );
	rtcSetSceneFlags( this->scene, RTC_SCENE_FLAG_ROBUST | flags );
	
	this->initialized = true;
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshから空間分析のための高速化構造を構築する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshSpatialQueryT<T>::setup ( const PolygonMeshT<T> &mesh, const String& maskAttrName, Bool invertMask ) {
	/*　動作テストの時間がないので一旦OFF
	s32 polyCnt = mesh.polygonCount ();
	if (polyCnt == 0) { this->initialized = false; return; }

	if (mesh.hasAttribute ( maskAttrName ) == false ) {
		// マスクとして使用するアトリビュートが存在しなかった場合
		*this = PolygonMeshSpatialQueryT<T> ( mesh );
	} else {
		const u8 attrType = mesh.getAttrType ( maskAttrName );
		if (attrType != 9 && attrType != 10 && attrType != 12 && attrType != 13 && attrType != 14) {
			*this = PolygonMeshSpatialQueryT<T> ( mesh );
		}
		
		std::vector<Bool> en ( polyCnt );
		#pragma omp parallel for 
		for (s32 i = 0; i < polyCnt; ++i) {
			L8Array<u32> polyPoiIDs = mesh.getPolygonPointIDs ( i );
			for (u32 j = 0; j < polyPoiIDs.size (); ++j) {
				u32 id = polyPoiIDs[j];
				Bool b;
				switch (attrType) {
					case 9:
						b = Burgess2MathFn::AlmostZero ( mesh.getF32Attribute ( maskAttrName )[id], f32 ( 0.001 ) );
						break;
					case 10:
						b = Burgess2MathFn::AlmostZero ( mesh.getF64Attribute ( maskAttrName )[id], f64 ( 0.001 ) );
						break;
					case 12:
						Vec2T<T> *v = mesh.getVec2Attribute ( maskAttrName )[id];
						b = v->almostEqual ( Vec2T<T>, T ( 0.001 ) );
						break;
					case 13:
						Vec3T<T> *v = mesh.getVec3Attribute ( maskAttrName )[id];
						b = v->almostEqual ( Vec3T<T>, T ( 0.001 ) );
						break;
					case 14:
						Vec4T<T> *v = mesh.getVec4Attribute ( maskAttrName )[id];
						b = v->almostEqual ( Vec4T<T>, T ( 0.001 ) );
						break;
				}
				if(b==true){
					en[i] = true;
					break;
				}
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////
		// embreeのシーンを構築に必要なデータを準備する

		// メッシュのポイントポジションアトリビュートをコピー
		Vec3Attribute* posAttr = &mesh.getVec3Attribute ( "pointPositions" );
		this->pointPositions.reserve ( posAttr->size () );
		std::copy ( posAttr->getValuesRef ().begin (), posAttr->getValuesRef ().end (), this->pointPositions.begin () );

		s32 polyCnt = mesh.polygonCount ();
		// メッシュのpackedPointIDsをコピー
		if (mesh.isPureTriangleMesh ()) {
			// 三角ポリゴンメッシュの場合
			U32Attribute* poiIDsAttr = &mesh.getU32Attribute ( "packedPointIDs" );
			this->packedPointIDs.reserve ( poiIDsAttr->size () );
			std::copy ( poiIDsAttr->getValuesRef ().begin (), poiIDsAttr->getValuesRef ().end (), this->packedPointIDs.begin () ); マスクのポリゴンだけ登録するように変更
			
			// embree::trianglePolygonInfoの登録
			this->packedPointIDs.resize ( polyCnt );
			#pragma omp parallel for 
			for (s32 i = 0; i < polyCnt; ++i) {
				this->triangleInfo[i].origPolygonID = i;
				this->triangleInfo[i].localTriID = 0;
			}
		}
		else {
			// 多角形ポリゴンメッシュの場合
			this->triangleInfo.reserve ( polyCnt * 2 ); // 全ポリゴンが四角形だと仮定してメモリを確保
			this->packedPointIDs.reserve ( polyCnt * 2 * 3 ); // 全ポリゴンが四角形だと仮定してメモリを確保
			// 三角形ポリゴンのリストに変換して登録
			for (u32 i = 0; i < polyCnt; ++i) {
				if (invertMask == false) {
					if (en[i] == false) { continue; }
				} else {
					if (en[i] == true) { continue; }
				}
				L8Array<u32> polyPoiIDs = mesh.getPolygonPointIDs ( i );
				u32 maxItr = polyPoiIDs.size () - 2;
				for (u32 j = 0; j < maxItr; ++j) {
					this->packedPointIDs.push_back ( polyPoiIDs[0] );
					this->packedPointIDs.push_back ( polyPoiIDs[j + 1] );
					this->packedPointIDs.push_back ( polyPoiIDs[j + 2] );

					// embree::trianglePolygonInfoの登録
					embree::TrianglePolygonInfo info;
					info.origPolygonID = i;
					info.localTriID = j;
					this->triangleInfo.push_back ( info );
				}
			}
		}
	}*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    startPositionからPolygonMeshSpatialQueryの表面上への最も近い位置を取得する
//!		@brief	startPositionからPolygonMeshSpatialQueryの表面上への最も近い位置を取得する
//!		@param[in] startPosition　スタート位置
//!		@param[in] maxDistance　検索距離
//!		@param[in] faceSide　検索対象のポリゴンフェース面 0=両面、1=表面、2=裏面
//!		@return 位置
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> PolygonMeshSpatialQueryT<T>::getClosestPosition ( const Vec3T<T>& startPosition, T maxDistance, u8 faceSide )const {
	if (this->isValid () == false) { return Vec3T<T>(T(0.0)); }

	BurgessEmbree::PointQuery<T> closestFuncData = this->getClosest_task(startPosition, maxDistance, faceSide);
	
	return closestFuncData.restClosestPosition;
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
GeometryLocationT<T> PolygonMeshSpatialQueryT<T>::getClosestLocation ( const Vec3T<T> &startPosition, T maxDistance, u8 faceSide ) const {
	GeometryLocationT<T> result;
	if (this->isValid () == false) { return result; }

	BurgessEmbree::PointQuery<T> closestFuncData = this->getClosest_task(startPosition, maxDistance, faceSide);

	Vec3T<T> locPos = closestFuncData.restClosestPosition;
	u32 locTriID = closestFuncData.restClosestTriangleID;
	u32 polyPoiIDs[3] = { this->packedPointIDs[locTriID * 3], this->packedPointIDs[locTriID * 3 + 1], this->packedPointIDs[locTriID * 3 + 2] };
	Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric ( locPos, this->pointPositions[polyPoiIDs[0]], this->pointPositions[polyPoiIDs[1]], this->pointPositions[polyPoiIDs[2]] );

	BurgessEmbree::TrianglePolygonInfo<T> locTriInfo = this->triangleInfo[locTriID];
	return GeometryLocationT<T>( locTriInfo.origPolygonID, locTriInfo.localTriID, weight, 3 );	
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
GeometryLocationT<T> PolygonMeshSpatialQueryT<T>::raycast ( const Ray3T<T> &ray, T maxDistance, u8 faceSide ) const {
	GeometryLocationT<T> result;
	if (this->isValid () == false) { return result; }

	// レイを生成する
	struct RTCRayHit rayhit = BurgessEmbree::toEmbreeRay<T>(ray, maxDistance);

	// 交差判定
	struct RTCIntersectContext context2;
	rtcInitIntersectContext ( &context2 );
	BurgessEmbree::HitList<T> hList = BurgessEmbree::getRayIntersect_task<T> ( this->scene, rayhit, faceSide );
	
	// GeometryLocationを作成
	u32 triID = hList.hits[hList.begin].primID;
	T uVal = Burgess2MathFn::Clamp(hList.hits[hList.begin].u, T(0.0), T(1.0));
	T vVal = Burgess2MathFn::Clamp(hList.hits[hList.begin].v, T(0.0), T(1.0));
	T wVal = Burgess2MathFn::Max(T(0.0), T(1.0) - uVal - vVal );
	return GeometryLocationT<T>(this->triangleInfo[triID].origPolygonID, this->triangleInfo[triID].localTriID, Vec3T<T>{ wVal, uVal, vVal }, 3 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Rayが交差した全てのロケーションを近い順で取得する
//!		@brief	Rayが交差した全てのロケーションを近い順で取得する
//!		@param[in] ray
//!		@param[in] maxDistance　検索距離
//!		@param[in] faceSide　検索対象のポリゴンフェース面 0=両面、1=表面、2=裏面
//!		@param[in] tolerance　交差地点がオンエッジ・オンポイントかを判定する精度 (入力値はBURGESS2::TOLERANCE~0.1)
//!		@return std::vector<GeometryLocation> 交差した全てのロケーション（近い順）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<GeometryLocationT<T>> PolygonMeshSpatialQueryT<T>::getAllRayIntersect( const Ray3T<T>& ray, T maxDistance, u8 faceSide, T tolerance ) const {
	std::vector<GeometryLocationT<T>> result( 0 );
	if (this->isValid () == false) { return result; }
	
	// レイを生成する
	struct RTCRayHit rayhit = BurgessEmbree::toEmbreeRay<T> ( ray, maxDistance );
	
	// 交差判定
	struct RTCIntersectContext context2;
	rtcInitIntersectContext ( &context2 );
	BurgessEmbree::HitList<T> hList = BurgessEmbree::getRayIntersect_task<T> ( this->scene, rayhit, faceSide);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ロケーションがオンエッジ/オンポイントだった場合
	// 隣接する複数のポリゴンの中から最も若いポリゴン番号、更にトライアングルの番号が小さいものでGeometryLocationを登録する
	
	result.reserve(hList.size());

	// 登録済みのオンポイント・オンエッジを判定するための変数を準備
	std::vector<u32> pushedPoi;
	std::vector<Vec2u> pushdEdgePoi;
	std::vector<Vec2T<T>> pushedEdgeWei;
	pushedPoi.reserve(hList.size());
	pushdEdgePoi.reserve(hList.size());
	pushedEdgeWei.reserve(hList.size());
	T t = Burgess2MathFn::Clamp( tolerance, T(BURGESS2::TOLERANCE), T( 0.1 ) );

	for (u32 i = hList.begin; i < hList.end; ++i) {
		// ロケーションのポリゴンの番号
		u32 locTriID = hList.hits[i].primID;

		// 重心ウェイト値を作成
		T uVal = Burgess2MathFn::Clamp(hList.hits[i].u, T(0.0), T(1.0));
		T vVal = Burgess2MathFn::Clamp(hList.hits[i].v, T(0.0), T(1.0));
		T wVal = Burgess2MathFn::Clamp(T(1.0) - uVal - vVal, T(0.0), T(1.0));
		Vec3T<T> weight{ wVal, uVal, vVal };
		if ( Burgess2MathFn::AlmostZero( weight.x() ) ){
			weight.x() = T( 0.0 );
			weight.y() = weight.y() / ( weight.y() + weight.z() );
			weight.z() = weight.z() / ( weight.y() + weight.z() );
		} else if ( Burgess2MathFn::AlmostZero( weight.y() ) ){
			weight.x() = weight.x() / ( weight.x() + weight.z() );
			weight.y() = T( 0.0 );
			weight.z() = weight.z() / ( weight.x() + weight.z() );
		} else if ( Burgess2MathFn::AlmostZero( weight.z() ) ){
			weight.x() = weight.x() / ( weight.x() + weight.y() );
			weight.y() = weight.y() / ( weight.x() + weight.y() );
			weight.z() = T( 0.0 );
		}

		// ウェイト値が0ではない頂点番号を抜き出す
		L4Array<u32> locPoiIDs;
		L4Array<T> locPoiWeight;
		for (u32 j = 0; j < 3; ++j) { 
			if (Burgess2MathFn::AlmostZero(weight[j], t ) == false) {
				locPoiIDs.push(this->packedPointIDs[locTriID *3+j]);
				locPoiWeight.push(weight[j]);
			} 
		}

		// 抜き出した頂点番号の合計数が1or2の場合はオンポイント・オンエッジであると判定
		switch (locPoiIDs.size()) {
			case 0: break;
			case 1: {// オンポイント
				// 既にオンポイントが登録済みか確認
				Bool pushed = false;
				for (u32 k = 0; k < pushedPoi.size(); ++k) {
					if (locPoiIDs[0] == pushedPoi[k]) { pushed = true; break; }
				}
				if (pushed == true) { break; } 

				// ポイントに隣接する最も若い番号のポリゴンを検出
				for (u32 j = 0; j < this->packedPointIDs.size(); ++j) {
					if (locPoiIDs[0] == this->packedPointIDs[j]) {
						u32 triID = j / 3; 
						if (locTriID != triID) {
							// 空間分析の結果よりも若い番号のポリゴンが見つかった場合、頂点配列番号に合わせて重心ウェイト値の設定を再設定する
							for (u32 k = 0; k < 3; ++k) {
								if (locPoiIDs[0] == this->packedPointIDs[triID * 3 + k]) {
									weight[k] = T(1.0);
								} else {
									weight[k] = T(0.0);
								}
							}
						}
						// ロケーションデータとして登録する
						BurgessEmbree::TrianglePolygonInfo<T> locTriInfo = this->triangleInfo[triID];
						result.push_back(GeometryLocationT<T>{locTriInfo.origPolygonID, locTriInfo.localTriID, weight, 3});

						// オンポイントが登録済みであるとマーキングする
						pushedPoi.push_back(locPoiIDs[0]);
						break;
					}
				}
				break;
			}
			case 2: { // オンエッジ
				// 既にオンエッジが登録済みか確認
				Bool pushed = false;
				for (u32 k = 0; k < pushdEdgePoi.size(); ++k) {
					if (locPoiIDs[0] == pushdEdgePoi[k].x() && locPoiIDs[1] == pushdEdgePoi[k].y() ) {
						if (Burgess2MathFn::AlmostEqual(locPoiWeight[0], pushedEdgeWei[k].x() ) && Burgess2MathFn::AlmostEqual(locPoiWeight[1], pushedEdgeWei[k].y() )) {
							pushed = true; break;
						}
					}
					if (locPoiIDs[1] == pushdEdgePoi[k].x() && locPoiIDs[0] == pushdEdgePoi[k].y() ) {
						if (Burgess2MathFn::AlmostEqual(locPoiWeight[1], pushedEdgeWei[k].x() ) && Burgess2MathFn::AlmostEqual(locPoiWeight[0], pushedEdgeWei[k].y() )) {
							pushed = true; break;
						}
					}
				}
				if (pushed == true) { break; }

				// エッジに隣接する最も若い番号のポリゴンを検出
				for (u32 j = 0; j < this->packedPointIDs.size(); ++j) {
					if (locPoiIDs[0] == this->packedPointIDs[j]) {
						u32 triID = j / 3;
						Bool find = false;
						for (u32 k = 0; k < 3; ++k) {
							if (locPoiIDs[1] == this->packedPointIDs[triID * 3 + k]) {
								find = true; break;
							}
						}
						if (find == false) { continue; }
						
						if (locTriID != triID) {
							// 空間分析の結果よりも若い番号のポリゴンが見つかった場合、頂点配列番号に合わせて重心ウェイト値の設定を再設定する
							for (u32 k = 0; k < 3; ++k) {
								if (locPoiIDs[0] == this->packedPointIDs[triID * 3 + k]) {
									weight[k] = locPoiWeight[0];
								} else if (locPoiIDs[1] == this->packedPointIDs[triID * 3 + k]) {
									weight[k] = locPoiWeight[1];
								} else {
									weight[k] = T(0.0);
								}
							}
						}
						// ロケーションデータとして登録する
						BurgessEmbree::TrianglePolygonInfo<T> locTriInfo = this->triangleInfo[triID];
						result.push_back(GeometryLocationT<T>{locTriInfo.origPolygonID, locTriInfo.localTriID, weight, 3});

						// オンエッジ登録済みであるとマーキングする
						pushdEdgePoi.push_back(Vec2u{ locPoiIDs[0],locPoiIDs[1] });
						pushedEdgeWei.push_back(Vec2T<T>{ locPoiWeight[0],locPoiWeight[1] });
						break;
					}
				}
				break;
			}
			case 3: { // オンフェース
				// ロケーションデータとして登録する
				BurgessEmbree::TrianglePolygonInfo<T> locTriInfo = this->triangleInfo[locTriID];
				result.push_back(GeometryLocationT<T>{locTriInfo.origPolygonID, locTriInfo.localTriID, weight, 3});
				break;
			}
		}
	}

	result.shrink_to_fit();
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Lineが交差した全てのロケーションをstartに近い順で取得する
//!		@brief	Lineが交差した全てのロケーションをstartに近い順で取得する
//!		@param[in] start　ラインのスタート位置
//!		@param[in] end　ラインのエンド位置
//!		@param[in] faceSide　検索対象のポリゴンフェース面 0=両面、1=表面、2=裏面
//!		@param[in] tolerance　交差地点がオンエッジ・オンポイントかを判定する精度 (入力値はBURGESS2::TOLERANCE~0.1)
//!		@return std::vector<GeometryLocation> 交差した全てのロケーション（近い順）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<GeometryLocationT<T>> PolygonMeshSpatialQueryT<T>::getAllLineIntersect ( const Vec3T<T> &start, const Vec3T<T> &end, u8 faceSide, T tolerance ) const {
	Ray3T<T> ray ( start, (end-start).unit() );
	T length = ( end-start ).length ();
	return this->getAllRayIntersect ( ray, length, faceSide, tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshSpatialQueryが有効な場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshSpatialQueryT<T>::isValid () const {
	return this->initialized;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
BurgessEmbree::PointQuery<T> PolygonMeshSpatialQueryT<T>::getClosest_task(const Vec3T<T>& startPosition, T maxDistance, u8 _faceSide)const {
	BurgessEmbree::PointQuery<T> result;
	if (this->isValid() == false) { return result; }

	// コード参照先　https://github.com/embree/embree/blob/master/tutorials/closest_point/closest_point_device.cpp 551-563
	RTCPointQuery query;
	query.x = startPosition.x();
	query.y = startPosition.y();
	query.z = startPosition.z();
	query.radius = maxDistance;
	query.time = 0.f;

	RTCPointQueryContext context;
	rtcInitPointQueryContext(&context);

	result.pointPositionsPtr = const_cast<std::vector<Vec3T<T>>*>(&this->pointPositions);
	result.packedPointIDsPtr = const_cast<std::vector<u32>*>(&this->packedPointIDs);
	result.closestFaceSide = _faceSide;

	rtcPointQuery(this->scene, &query, &context, nullptr, (void*)&result);
	return result;
}
