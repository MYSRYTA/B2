//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    サブディビジョンメッシュを作成する
//!	    @brief サブディビジョンメッシュを作成する
//!		@param[in] type 分割タイプ 0=Linear, 1=Catmull-Clark
//!		@param[in] iteration 分割回数（最大5） 
//!		@return サブディビジョンメッシュ
//------------------------------------------------------------------------------------------------------------------------------------------------
#include <deque>

template<typename T>
PolygonMeshT<T> PolygonMeshT<T>::createSubdivideMesh ( u8 type, u8 _iteration )const{
	if ( _iteration == 0 ){ return *this; }
	PolygonMeshT<T> result, cage;
	u8 iteration = Burgess2MathFn::Min ( _iteration, u8(5) );

	for(u32 step = 0; step < iteration; ++step){
		cage = step == 0 ? *this : result;
		result = PolygonMeshT<T> ();

		u32 polySize = cage.polygonCount ();
		u32 edgeSize = cage.edgeCount ();
		u32 meshEdgeSize = cage.meshEdgeCount ();

		std::vector<u32> edgeID ( edgeSize );
		std::vector<Vec2u> edgePoiID ( edgeSize );
		std::vector<Bool> edgeIsBorder ( edgeSize );
		std::vector<u32> edgeFaceID ( edgeSize );		
		std::vector<Bool> edgeIsConsidered ( edgeSize );
		std::vector<Vec3T<T>> newPoiPos ( polySize + meshEdgeSize + cage.pointCount () );
		u32 iterator = 0;
		for ( u32 i = 0; i < polySize; ++i ){
			L8Array<u32> polyPoi = cage.getPolygonPointIDs ( i );
			Vec3T<T> v{ T ( 0.0 ) };
			for ( u32 j = 0; j < polyPoi.size(); ++j ){
				v += cage.getPointPosition ( polyPoi[j] );
				edgeFaceID[iterator] = i;
				edgeIsBorder[iterator] = cage.isPolygonBorder ( i, polyPoi[j] );
				edgeID[iterator] = cage.getPolygonPointEdgeID ( i, polyPoi[j] );
				edgePoiID[iterator] = cage.getEdgePointIDs ( edgeID[iterator] );
				edgeIsConsidered[iterator] = false;
				iterator++;
			}
			newPoiPos[i] = v / T( polyPoi.size() );
		}
		// 頂点リスト作成
		std::vector<u32> restEdgeID ( edgeSize );
		u32 edgeIterator{ 0 };
		for ( u32 i = 0; i < edgeSize; ++i ){
			if ( edgeIsConsidered[i] == false ){
				Vec2u edgePoiIDs = edgePoiID[edgeID[i]];
				if ( edgeIsBorder[i] ){
					Vec3T<T> pos = cage.getPointPosition ( edgePoiIDs.x() ).lerp ( cage.getPointPosition ( edgePoiIDs.y() ), T ( 0.5 ) );
					newPoiPos[polySize + edgeIterator] = pos;
					edgeIsConsidered[i] = true;
					restEdgeID[i] = edgeIterator;
					edgeIterator++;
				} else{
					Bool findPair = false;
					u32 adjEdgeID = cage.getEdgeAdjacentEdgeID ( edgeID[i] );
					for ( u32 j = i + 1; j < edgeSize; j++ ){
						if ( adjEdgeID == edgeID[j]){
							Vec3T<T> pos;
							switch ( type ){
								case 0: // Linear
									pos = cage.getPointPosition ( edgePoiIDs.x() ).lerp ( cage.getPointPosition ( edgePoiIDs.y() ), T ( 0.5 ) );
									break;
								case 1: // Catmul Clark
									Vec3T<T> p0 = newPoiPos[edgeFaceID[i]];
									Vec3T<T> p1 = newPoiPos[edgeFaceID[j]];
									Vec3T<T> p2 = cage.getPointPosition ( edgePoiIDs.x() );
									Vec3T<T> p3 = cage.getPointPosition ( edgePoiIDs.y() );
									pos = ( p0 + p1 + p2 + p3 ) * T ( 0.25 );
									break;
							}
							newPoiPos[polySize + edgeIterator] = pos;
							restEdgeID[i] = edgeIterator;
							restEdgeID[j] = edgeIterator;
							edgeIterator++;
							findPair = true;
							edgeIsConsidered[i] = true;
							edgeIsConsidered[j] = true;
							j = edgeSize;
						}
					}
					if ( findPair == false ){
						Vec3T<T> pos = cage.getPointPosition ( edgePoiIDs.x() ).lerp ( cage.getPointPosition ( edgePoiIDs.y() ), T ( 0.5 ) );
						newPoiPos[polySize + edgeIterator] = pos;
						edgeIsConsidered[i] = true;
						restEdgeID[i] = edgeIterator;
						edgeIterator++;
					}
				}
			}
		}
		// トポロジーリスト作成
		std::deque<u32> polygonSizesD(1), packedPointIDsD;
		polygonSizesD[0] = 0;
		u32 iter2{ 0 };
		for ( u32 i = 0; i < polySize; i++ ){
			L8Array< u32 > polyPoiIDs = cage.getPolygonPointIDs ( i );
			for ( u32 j = 0, n = polyPoiIDs.size (); j < n; j++ ){
				u32 prevEdgeIndex = j == 0 ? iter2 + n - 1 : iter2 - 1;
				packedPointIDsD.push_back ( i );
				packedPointIDsD.push_back ( polySize + restEdgeID[iter2] );
				packedPointIDsD.push_back ( polySize + meshEdgeSize + polyPoiIDs[j] );
				packedPointIDsD.push_back ( polySize + restEdgeID[prevEdgeIndex] );
				polygonSizesD.push_back ( polygonSizesD[polygonSizesD.size()-1] + 4 );
				iter2++;
			}
		}
		std::vector<u32> polygonSizes ( polygonSizesD.size () );
		std::vector<u32> packedPointIDs ( packedPointIDsD.size () );
		#pragma omp parallel for
		for ( s32 i = 0; i < polygonSizesD.size (); ++i ){
			polygonSizes[i] = polygonSizesD[i];
		}
		#pragma omp parallel for
		for ( s32 i = 0; i < packedPointIDsD.size (); ++i ){
			packedPointIDs[i] = packedPointIDsD[i];
		}

		result.beginStructureChanges ();
		result.resetPoints ( newPoiPos );
		result.resetTopology ( polygonSizes, packedPointIDs, true );
		result.endStructureChanges ();

		// 頂点の位置を再設定
		#pragma omp parallel for
		for(s32 i=0; i<cage.pointCount (); ++i){
			L16Array<u32> poiPolyIDs = cage.getPointPolygonIDs ( i );

			if ( poiPolyIDs.size() == 1 ){
				Vec3T<T> pos = cage.getPointPosition ( i );
				result.setPointPosition ( polySize + meshEdgeSize + i, pos );
				continue;
			}

			Vec3T<T> sumPos;
			Bool isBorder = false;
			for ( u32 j = 0; j < poiPolyIDs.size(); j++ ){
				if ( isBorder == false && cage.isPolygonBorder ( poiPolyIDs[j], i ) ){
					isBorder = true;
				}
				sumPos += result.getPointPosition ( poiPolyIDs[j] );
			}
			L16Array<u32> edgeIDs = cage.getPointEdgeIDs ( i, true );
			Vec3T<T> sumPos2;
			if ( isBorder ){
				u32 edgeCnt{ 0 };
				for ( u32 j = 0; j < edgeIDs.size(); ++j ){
					Bool isBorder2 = false;
					u32 adjEdgeID = cage.getEdgeAdjacentEdgeID ( edgeIDs[j] );
					for ( u32 k = 0; k < edgeIsBorder.size (); ++k ){
						if ( edgeIsBorder[k] && edgeID[k] == edgeIDs[j] ){
							isBorder2 = true;
							break;
						}
					}
					if ( isBorder2 ){
						Vec2u edgePoiIDs2 = edgePoiID[edgeIDs[j]];
						sumPos2 += cage.getPointPosition ( edgePoiIDs2.x() ).lerp( cage.getPointPosition ( edgePoiIDs2.y() ), T(0.5) );
						edgeCnt++;
					}
				}
				Vec3T<T> pos = ( cage.getPointPosition ( i ) + sumPos2 / T( edgeCnt ) ) * T(0.5);
				result.setPointPosition ( polySize + meshEdgeSize + i, pos );
			} else{
				for ( u32 j = 0; j < edgeIDs.size(); ++j ){
					Vec2u edgePoiIDs3 = edgePoiID[edgeIDs[j]];
					sumPos2 += cage.getPointPosition ( edgePoiIDs3.x() ).lerp( cage.getPointPosition ( edgePoiIDs3.y() ), T(0.5) );
				}
				Vec3T<T> pos;
				switch ( type ){
					case 0:
						pos = cage.getPointPosition ( i );
						break;
					case 1:
						Vec3T<T> p0 = cage.getPointPosition ( i ) * ( T ( T ( poiPolyIDs.size () ) - T(3.0) ) / T ( poiPolyIDs.size () ) );
						Vec3T<T> p1 = sumPos / T ( poiPolyIDs.size () ) * ( T(1.0) / T ( poiPolyIDs.size () ) );
						Vec3T<T> p2 = sumPos2 / T ( edgeIDs.size () ) * ( T ( 2.0 ) / T ( poiPolyIDs.size () ) );
						pos = p0 + p1 + p2;
						break;
				}
				result.setPointPosition ( polySize + meshEdgeSize + i, pos );
			}
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ピュアトライアングルメッシュを作成する (注意！法線アトリビュートは生成しない)
//!	    @brief ピュアトライアングルメッシュを作成する (注意！法線アトリビュートは生成しない)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T> PolygonMeshT<T>::createPureTriangleMesh ()const{
	assert ( 0 < this->polygonCount () );
	PolygonMeshT<T> result;
	result.beginStructureChanges ();

	result.resetPoints ( this->getAllPointPositions () );

	std::vector<u32> polygonSizes;
	polygonSizes.reserve ( this->polygonCount () * 2 );
	polygonSizes.push_back ( 0 );
	std::vector<u32> origPolyIDs;
	origPolyIDs.reserve ( this->polygonCount () * 2 );
	origPolyIDs.push_back ( 0 );
	std::vector<u32> packedPointIDs;
	packedPointIDs.reserve ( this->polygonCount () * 6 );

	u32 q = 1;
	u32 loopMax = this->polygonCount ();
	for ( u32 i = 0; i<loopMax; i++ ){
		L8Array<u32> polyPoiIDs = this->getPolygonPointIDs ( i );
		u32 maxItr = polyPoiIDs.size () - 2;
		for ( u32 k = 0; k<maxItr; k++ ){
			polygonSizes.push_back ( q * 3 );
			packedPointIDs.push_back ( polyPoiIDs[0] );
			packedPointIDs.push_back ( polyPoiIDs[k + 1] );
			packedPointIDs.push_back ( polyPoiIDs[k + 2] );
			q++;
		}
		origPolyIDs.push_back ( origPolyIDs[i] + maxItr );
	}
	result.resetTopology ( polygonSizes, packedPointIDs, true );

	U32AttributeT<T> &origPolyIDAttr = result.getOrCreateU32Attribute ( "OriginalPolygonIDs" );
	origPolyIDAttr.resetValues ( origPolyIDs );

	result.endStructureChanges ();
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    デュアルメッシュを作成する
//!	    @brief デュアルメッシュを作成する
//!		@param[in] scale デュアルメッシュの各面をローカルスケーリングする（1.0の場合はスケーリングせずに頂点をマージする）
//!		@param[in] mask デュアルメッシュ化の処理対象とする頂点番号のリスト（配列数が0の場合は全頂点を対象とする）
//!		@return デュアルメッシュ
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T> PolygonMeshT<T>::createDualMesh ( T scale, const std::vector<u32> &mask )const{
	std::vector<u32> dummy(1);
	return this->createDualMesh ( scale, mask, dummy );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    デュアルメッシュを作成する
//!	    @brief デュアルメッシュを作成する
//!		@param[in] scale デュアルメッシュの各面をローカルスケーリングする（1.0の場合はスケーリングせずに頂点をマージする）
//!		@param[in] mask デュアルメッシュ化の処理対象とする頂点番号のリスト（配列数が0の場合は全頂点を対象とする）
//!		@param[out] dualFaceToSourcePoint 生成したDualMeshのフェースとソースメッシュの頂点を紐づけるリスト
//!		@return デュアルメッシュ
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T> PolygonMeshT<T>::createDualMesh ( T scale, const std::vector<u32> &mask, OUT std::vector<u32> &dualFaceToSourcePoint )const{
	assert ( 3 < this->polygonCount () );
	PolygonMeshT<T> result;
	if ( scale <= T ( 0.0 ) ){ return result; }

	Bool merge = Burgess2MathFn::AlmostEqual ( scale, T ( 1.0 ) );

	// sourceMeshの頂点⇔DualMeshのフェースを紐づけるリスト（対象が存在しない場合の要素値は-1）
	// 配列番号 = sourceMeshの頂点番号
	// 配列要素 = dualMeshのフェース番号
	std::vector<s32> sourcePointToDualFace ( this->pointCount (), -1 );// 全ての値を-1でリストを作成

	Bool useMask = 0 < mask.size ();

	result.beginStructureChanges ();
	// 頂点ごとにループ
	u32 n = useMask ? mask.size () : this->pointCount ();
	for ( u32 id = 0; id < n; ++id ){
		u32 i = useMask ? mask[id] : id;
		if ( this->pointCount () <= i ){ continue; } // maskで指定した頂点IDが不正な場合は終了

		L16Array <u32> poiPolyIDs = this->getPointPolygonIDs ( i ); // 頂点で構成しているポリゴンIDを取得
		if ( poiPolyIDs.size() < 2 ) { continue; } // 隣接するポリゴン数が１個の場合は終了
		
		Vec3T<T> center;
		L8Array<Vec3T<T>> newPoints(0);
		// 隣接するポリゴンをループ
		for ( u32 j = 0, m = poiPolyIDs.size (); j < m; ++j ){
			u32 polyID = poiPolyIDs[j]; // ポリゴンID
			L4Array<u8> triIDs = this->getTriangleIDs ( polyID, i ); // ポリゴン内で頂点を使用する三角形ポリゴンのID群
			
			for ( u32 h = 0; h < triIDs.size (); ++h ){
				L4Array<u32> triPoiIDs = this->getTrianglePointIDs ( polyID, triIDs[h] );// 三角形ポリゴンを構成する頂点ID
				Vec3T<T> p;
				p = this->getPointPosition ( triPoiIDs[0] );
				p += this->getPointPosition ( triPoiIDs[1] );
				p += this->getPointPosition ( triPoiIDs[2] );
				p *= T ( 0.3333333 ); // 三角ポリゴンの中央位置を取得
				center += p;
				newPoints.push_back ( p );
			}
		}
		u32 newPolySize = newPoints.size ();
		if ( newPolySize < 3 ){ continue; } // 頂点が２以下の場合はポリゴンを構成できないので終了

		// newPointsの中央位置を取得
		center /= T ( newPolySize );
				
		// スケールONの場合、centerを中心にして頂点座標をスケーリングする
		if ( scale < T ( 1.0 ) ){
			for ( u32 j = 0, m = newPoints.size (); j < m; ++j ){
				Vec3T<T> v = newPoints[j];
				v = ( v - center ) * scale;
				newPoints[j] = center + v;
			}
		}
		// ポリゴンを追加
		u32 polyID = result.addPolygon ( newPoints, merge, T(0.001) );
		
		// ソースの頂点番号とDualMeshのフェースを紐づけるリストの作成
		sourcePointToDualFace[i] = polyID;
	}
	result.endStructureChanges ();
	if ( result.polygonCount () == 0 ){ 
		result.createPureTriangleMesh();
		return result;
	}

	if ( dualFaceToSourcePoint.size () == 0 ){ // dualFaceToSourcePointの算出が不要な場合はsize==0以外の引数が渡っている
											   // sourcePointToDualFaceを"DualMeshのフェース番号→sourceMeshの頂点番号"のリストに作り替える
											   // 配列番号 = dualMeshのフェース番号
											   // 配列要素 = sourceMeshの頂点番号
		std::vector<u32> origID2 ( this->pointCount () );
		for ( u32 i = 0; i < this->pointCount (); ++i ){ origID2[i] = i; }
		Burgess2MathFn::Sort ( sourcePointToDualFace, origID2, true );
		for ( u32 i = 0; i < sourcePointToDualFace.size (); ++i ){
			if ( sourcePointToDualFace[i] < 0 ){ continue; }
			dualFaceToSourcePoint.push_back ( origID2[i] );
		}
		std::reverse ( dualFaceToSourcePoint.begin (), dualFaceToSourcePoint.end () );
		assert ( dualFaceToSourcePoint.size () == result.polygonCount () );
	}

	return result;
}