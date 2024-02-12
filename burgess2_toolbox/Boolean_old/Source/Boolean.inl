
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BooleanCrossDataT<T>::push( const u32 id, const Vec3T<T>& pos, const Vec2s& poly, const s32 edgeStartPoiID, const T ratio ) {
	Bool find = false;
	for ( u32 i = 0; i < this->crossPos[id].size(); i++ ) {
		if (this->crossPos[id][i].almostEqual(pos, T( 0.001 ) ) &&
			this->crossPoly[id][i] == poly //&&
			//this->splitEdgeSPoiID[id][i] == edgeID &&
			//Burgess2MathFn::AlmostEqual( this->splitRatio[id][i], r, T(0.001) )
		) {
			find = true;
			break;
		}
	}
	
	if ( find ) {
		return;
	} else {
		this->crossPos[id].push_back( pos );
		this->crossPoly[id].push_back( poly );
		this->splitEdgeSPoiID[id].push_back( edgeStartPoiID );
		this->splitRatio[id].push_back( ratio );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    エッジに隣接するポリゴン番号を比較して小さい方のポリゴン番号を有するハーフエッジの始点のバーテックスIDを返す
//!	    @brief エッジに隣接するポリゴン番号を比較して小さい方のポリゴン番号を有するハーフエッジの始点のバーテックスIDを返す
//!		@param[in] mesh
//!		@return ハーフエッジの始点のバーテックスID
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<u32> BooleanT<T>::createEdgeList( const PolygonMeshT<T>& mesh )const {
	std::vector<u32> result;
	for ( u32 i = 0; i < mesh.polygonCount(); i++ ) {
		// ポリゴンを構成するバーテックスID(ハーフエッジID)を取得
		L8Array<u32> edgeIDs = mesh.getPolygonVertexIDs( i );
 
		// エッジを走査
		for ( u32 j = 0; j < edgeIDs.size(); j++ ) {
			if ( mesh.isPolygonBorder( edgeIDs[j] ) ) {
				// 隣接するポリゴンが無い場合はエッジIDを格納する
				result.push_back( edgeIDs[j] );
			} else {
				// エッジに隣接するポリゴンの番号を取得
				u32 adjPolyID = mesh.getPolygonVertexAdjacentPolygonID( edgeIDs[j] );
				// ポリゴンIDを比較し、現在ループ処理しているポリゴンIDの方が小さい場合はエッジIDを格納する
				if ( i < adjPolyID ) {
					result.push_back( edgeIDs[j] );
				}
			}
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] mesh
//!		@return 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BooleanT<T>::CrossDataSet(
	IO BooleanCrossDataT<T>& crossData0,
	IO BooleanCrossDataT<T>& crossData1,
	const std::vector<u32>& edgeList0,
	const PolygonMeshT<T>& mesh0,
	const PolygonMeshT<T>& mesh1,
	const PolygonMeshSpatialQueryT<T>& sq1,
	const T tolerance
) {
	// エッジの交差ロケーションを取得する
	std::vector < std::vector<GeometryLocationT<T>>> allLocList( edgeList0.size () );
	#pragma omp parallel for 
	for (s32 i = 0; i < edgeList0.size (); i++) {
		Vec2u edgePoiIDs = mesh0.getEdgePointIDs ( edgeList0[i] );
		Vec3T<T> startPos = mesh0.getPointPosition ( edgePoiIDs[0] );
		Vec3T<T> endpos = mesh0.getPointPosition ( edgePoiIDs[1] );
		allLocList[i] = sq1.getAllLineIntersect ( startPos, endpos, 0 ); // ロケーションがオンポイント/オンエッジだった場合、隣接するポリゴンからポリゴン番号が一番若いロケーションだけを返す
	}

	// エッジとポリゴンの交差情報をBooleanCrossDataに格納していく
	for ( u32 i = 0; i < edgeList0.size(); i++ ) {
		std::vector<GeometryLocationT<T>>& locList1 = allLocList[i];
		// 交差が無い場合はスキップ
		if ( locList1.size() == 0 ) { continue; }

		// エッジの情報を取得する
		Vec2u edgePoiIDs = mesh0.getEdgePointIDs ( edgeList0[i] );
		Vec3T<T> startPos = mesh0.getPointPosition ( edgePoiIDs[0] );
		Vec3T<T> endpos = mesh0.getPointPosition ( edgePoiIDs[1] );

		// エッジに隣接するポリゴンIDを取得する
		std::vector<u32> edgeNeighberPoly0;
		edgeNeighberPoly0.push_back( mesh0.getEdgeLeftPolygonID( edgeList0[i] ) );
		u32 rPoly = mesh0.getEdgeRightPolygonID( edgeList0[i] );
		if ( rPoly < mesh0.polygonCount() ) {
			// ポリゴンボーダーでなければエッジの右側のポリゴンIDも格納する
			edgeNeighberPoly0.push_back( rPoly );
		}
		
		// CrossData.setData()
		for ( u32 j = 0; j < locList1.size(); j++ ) {
			GeometryLocationT<T>& loc1 = locList1[j]; // mesh0のエッジがmesh1に交差したロケーション
			Vec3T<T> edgeCrossPos1 = mesh1.getPositionAtLocation( loc1 ); // mesh0のエッジがmesh1に交差した位置
			T ratio0;
			if (startPos.almostEqual ( edgeCrossPos1, tolerance )) {
				ratio0 = T ( 0.0 );
			} else if (endpos.almostEqual ( edgeCrossPos1, tolerance )) {
				ratio0 = T ( 1.0 );
			} else {
				T edgeLen = startPos.distanceTo ( endpos );
				T val = startPos.distanceTo ( edgeCrossPos1 ) / edgeLen;
				ratio0 = Burgess2MathFn::Clamp( val, T( 0.0 ), T( 1.0 ) );
			}

			///////////////////////////////////////////////////////////////////////////////////////////////
			// ロケーションデータが、ロケーションが頂点上 / エッジ上 / ポリゴンエリア上のどれに当たるか検査する

			Vec3u locPoiIDs1;  // ロケーションのトライアングルの頂点ＩＤ３個
			Vec3T<T> locWeights1;  // ロケーションのトライアングルのウェイト値
			mesh1.getLocationPointsAndWeights( loc1, locPoiIDs1, locWeights1 ); // ロケーションのトライアングルの頂点ＩＤ３個とそれぞれへのウェイト値
			u32 crossPolyID1 = loc1.getIndex(); // ロケーションのポリゴンID

			// ウェイトが0.0の頂点を省いたリストを作る
			std::vector<u32> poiIDs1;
			Bool notZero[3];
			for ( u32 k = 0; k < 3; k++ ) {
				if ( locWeights1[k] < T(0.0001) ) {
					notZero[k] = false;
				} else {
					poiIDs1.push_back( locPoiIDs1[k] );
					notZero[k] = true;
				}
			}
			///////////////////////////////
			// poiIDsの数が2の場合
			// ロケーションが三角形の最後のエッジ上だった場合、エッジのスタート・エンドの順にIDが並ぶように整理しておく
			if ( poiIDs1.size() == 2 ) {
				if ( notZero[0] && notZero[2] ) {
					u32 lastPoiID = mesh1.getPolygonPoint ( crossPolyID1, mesh1.getPolygonSize ( crossPolyID1 ) - 1 );
					if (poiIDs1[1] == lastPoiID) {
						u32 rest = poiIDs1[0];
						poiIDs1[0] = poiIDs1[1];
						poiIDs1[1] = rest;
					}
					
				}
			}
			// サブトライアングルのエッジ上かポリゴンのエッジ上かを事前に判定しておく
			Bool onPolygonEdge = false;
			if ( poiIDs1.size() == 2 ) {
				u32 nextPoiID = mesh1.getNextPolygonPointID( crossPolyID1, poiIDs1[0] );
				onPolygonEdge = poiIDs1[1] == nextPoiID;
			}

			if ( poiIDs1.size() == 1 ) { // ロケーションがメッシュ頂点上の場合
				for ( u32 k = 0; k < edgeNeighberPoly0.size(); k++ ) {
					L16Array<u32> polyIDs = mesh1.getPointPolygonIDs( poiIDs1[0] );
					s32 edgePoiID;
					T r;
					if (k == 0) {
						edgePoiID = edgePoiIDs[0];
						r = ratio0;
					} else if (k == 1) {
						edgePoiID = edgePoiIDs[1];
						r = T ( 1.0 ) - ratio0;
					} else {
						break;
					}
					for ( u32 l = 0; l < polyIDs.size(); l++ ) {
						crossData0.push( edgeNeighberPoly0[k], edgeCrossPos1, Vec2s{ s32( polyIDs[l] ), -1 }, edgePoiID, r );
					}
				}
			} else if ( poiIDs1.size() == 2 && onPolygonEdge ) { // ロケーションがメッシュエッジ上の場合
				// エッジに隣接するポリゴンIDを格納する
				for ( u32 k = 0; k < edgeNeighberPoly0.size(); k++ ) {
					s32 edgePoiID;
					T r;
					if (k == 0) {
						edgePoiID = edgePoiIDs[0];
						r = ratio0;
					} else if (k == 1) {
						edgePoiID = edgePoiIDs[1];
						r = T ( 1.0 ) - ratio0;
					} else {
						break;
					}
					u32 edgeID1 = mesh1.getPolygonPointEdgeID ( crossPolyID1, poiIDs1[0] );
					crossData0.push( edgeNeighberPoly0[k], edgeCrossPos1, Vec2s{ s32( mesh1.getEdgeLeftPolygonID( edgeID1 ) ), -1 }, edgePoiID, r );
					crossData0.push( edgeNeighberPoly0[k], edgeCrossPos1, Vec2s{ s32( mesh1.getEdgeRightPolygonID( edgeID1 ) ), -1 }, edgePoiID, r );
				}
			} else { // ロケーションがポリゴンエリア上の場合
				for ( u32 k = 0; k < edgeNeighberPoly0.size(); k++ ) {
					if ( k == 0 ) {
						// edgeNeighberPoly0[0](=エッジの左側)を登録
						crossData0.push( edgeNeighberPoly0[k], edgeCrossPos1, Vec2s{ s32( crossPolyID1 ), -1 }, edgePoiIDs[0], ratio0 );
					} else if ( k == 1 ) {
						// edgeNeighberPoly0[1](=エッジの右側)を登録
						crossData0.push( edgeNeighberPoly0[k], edgeCrossPos1, Vec2s{ s32( crossPolyID1 ), -1 }, edgePoiIDs[1], T( 1.0 ) - ratio0 );
					}
				}
				if ( edgeNeighberPoly0.size() == 1 ) {
					crossData1.push( crossPolyID1, edgeCrossPos1, Vec2s{ s32( edgeNeighberPoly0[0] ), -1 }, -1, T( -1.0 ) );
				} else {
					crossData1.push( crossPolyID1, edgeCrossPos1, Vec2s{ s32( edgeNeighberPoly0[0] ), s32( edgeNeighberPoly0[1]) }, -1, T( -1.0 ) );
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] mesh
//!		@return 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BooleanT<T>::MakeSplitPolygonsList(
	OUT std::vector<u32>& polygonIDs,
	OUT std::vector<u32>& pointIDs1,
	OUT std::vector<f32>& ratio1,
	OUT std::vector<u32>& pointIDs2,
	OUT std::vector<f32>& ratio2,
	OUT std::vector<std::vector<Vec3T<T>>>& spritPositionsList,
	const PolygonMeshT<T>& mesh0,
	const BooleanCrossDataT<T>& crossData0,
	const Bool hole
) {
	// crossPoly0のVec2.x/.yに格納されたポリゴンIDを繋いでいき、ポリゴン分割用のスタート・エンド・追加頂点のリストを作る
	for ( u32 j = 0; j < mesh0.polygonCount(); j++ ) {
		std::vector<Bool> used( crossData0.crossPoly[j].size(), false );
		while ( true ) {
			std::vector<Vec3T<T>> rSplitPositions( 0 );
			s32 chainPoly = -2, endPoly = -1;

			// ポイントIDが正しい値か確認するために、一旦仮の変数に保存する
			s32 restIDs[2] = { -1,-1 };
			T restRatios[2];

			// オンエッジ/オンポイント(Vec2.y=-1)を見つけて使用済みのフラグをＯＮにする
			for ( u32 k = 0; k < crossData0.crossPoly[j].size(); k++ ) {
				if ( used[k] == false && crossData0.crossPoly[j][k].y() == -1 ) {
					restIDs[0] = crossData0.splitEdgeSPoiID[j][k];
					restRatios[0] = crossData0.splitRatio[j][k];
					//rSplitPositions.push_back( crossPos0[j][k] );
					chainPoly = crossData0.crossPoly[j][k].x();
					used[k] = true;
					break;
				}
			}

			// オンエッジ/オンポイント(Vec2.y=-1)が見つからず、更にまだ未使用の要素が残っている場合
			// PolyHoleの処理用のリストを作る
			if ( hole && chainPoly == -2 ) {
				for ( u32 k = 0; k < crossData0.crossPoly[j].size(); k++ ) {
					if ( used[k] == false ) {
						rSplitPositions.push_back( crossData0.crossPos[j][k] );
						chainPoly = crossData0.crossPoly[j][k].x();
						endPoly = crossData0.crossPoly[j][k].y();
						used[k] = true;
						break;
					}
				}
			}

			while ( true ) {
				// crossPoly0のVec2.x/.yに格納されたポリゴンIDを繋いでいく
				s32 rID = -1;
				for ( u32 k = 0; k < crossData0.crossPoly[j].size(); k++ ) {
					if ( used[k] == true ) { continue; }

					if ( chainPoly == crossData0.crossPoly[j][k].x() || chainPoly == crossData0.crossPoly[j][k].y() ) {
						rID = k;
						if ( crossData0.crossPoly[j][k].x() + crossData0.crossPoly[j][k].y() - chainPoly != -1 ) {
							break;
						}
					}
				}
				if ( rID != -1 ) {
					chainPoly = crossData0.crossPoly[j][rID].x() + crossData0.crossPoly[j][rID].y() - chainPoly;
					if ( chainPoly == -1 ) {
						restIDs[1] = crossData0.splitEdgeSPoiID[j][rID];
						restRatios[1] = crossData0.splitRatio[j][rID];
					} else {
						rSplitPositions.push_back( crossData0.crossPos[j][rID] );
					}
					used[rID] = true;
				} else {
					break;
				}

				if ( chainPoly == endPoly ) {
					// スタート・エンドのみで、更に分割位置が同じ場合はスキップ　
					// SplitEdgesAndPolygons()で処理対象外に分類してくれるので、ここで整理処理は行わないでおく
					/*if ( rSplitPositions.size() == 2 ) {
						if ( rSplitPositions[0].almostEqual( rSplitPositions[1] ) ) {
							break;
						}
					}*/

					// ポイントIDが不正値の場合はスキップ
					if ( restIDs[0] == -1 || restIDs[1] == -1 ) {
						break;
					}
					
					pointIDs1.push_back( restIDs[0] );
					pointIDs2.push_back( restIDs[1] );
					ratio1.push_back( Burgess2MathFn::Clamp ( restRatios[0], T ( 0.0 ), T ( 1.0 ) ) );
					ratio2.push_back( Burgess2MathFn::Clamp ( restRatios[1], T ( 0.0 ), T ( 1.0 ) ) );
					polygonIDs.push_back( j );
					spritPositionsList.push_back( rSplitPositions );
					break;
				}
			}
			// 全ての要素が使用済みになったら終了
			if ( chainPoly == -2 ) {
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] mesh
//!		@return 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool BooleanT<T>::initalize ( const PolygonMeshT<T>& meshA, const PolygonMeshT<T>& meshB ) {
	if (this->initalized == true || meshA.polygonCount () == 0 || meshB.polygonCount () == 0) { return false; }

	this->edgeListA = this->createEdgeList ( meshA );
	this->pointSizeA = meshA.pointCount ();
	this->polygonSizeA = meshA.polygonCount ();
	this->sqA.setup( meshA );
	
	this->edgeListB = this->createEdgeList ( meshB );
	this->pointSizeB = meshB.pointCount ();
	this->polygonSizeB = meshB.polygonCount ();
	this->sqB.setup( meshB );
	
	this->initalized = true;
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] mesh
//!		@return 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool BooleanT<T>::meshReset ( const PolygonMeshT<T>& mesh, Bool isMeshA ){
	if (this->initalized == false || mesh.polygonCount () == 0 ) { return false; }

	if (isMeshA == true) {
		this->sqA.setup( mesh );
		if (this->pointSizeA != mesh.pointCount () || this->polygonSizeA != mesh.polygonCount ()) {
			this->edgeListA = this->createEdgeList ( mesh );
			this->pointSizeA = mesh.pointCount ();
			this->polygonSizeA = mesh.polygonCount ();
		}
	} else {
		this->sqB.setup( mesh );
		if (this->pointSizeB != mesh.pointCount () || this->polygonSizeB != mesh.polygonCount ()) {
			this->edgeListB = this->createEdgeList ( mesh );
			this->pointSizeB = mesh.pointCount ();
			this->polygonSizeB = mesh.polygonCount ();			
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] mesh
//!		@return 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool BooleanT<T>::core( OUT std::vector<PolygonMeshT<T>>& result, const PolygonMeshT<T>& meshA, const PolygonMeshT<T>& meshB, const T tolerance ) {
	if (this->initalized == false || this->pointSizeA == 0 || this->pointSizeB == 0 || this->polygonSizeA == 0 || this->polygonSizeB == 0) { return false; }

	// Boolean_CrossData
	BooleanCrossDataT<T> crossDataA ( meshA.polygonCount () );
	BooleanCrossDataT<T> crossDataB ( meshB.polygonCount () );
	this->CrossDataSet ( crossDataA, crossDataB, edgeListA, meshA, meshB, this->sqB, tolerance );
	this->CrossDataSet ( crossDataB, crossDataA, edgeListB, meshB, meshA, this->sqA, tolerance );

	result.resize ( 2 );
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			result[0] = meshA.clone ();
			std::vector<u32> polygonIDs;
			std::vector<u32> pointIDs1;
			std::vector<f32> ratio1;
			std::vector<u32> pointIDs2;
			std::vector<f32> ratio2;
			std::vector<std::vector<Vec3>> addPointPositions;
			this->MakeSplitPolygonsList ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, meshA, crossDataA, false );
			result[0].beginStructureChanges ();
			result[0].splitEdgesAndPolygons ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, tolerance );
			if (0 < polygonIDs.size () && result[0].polygonCount () == meshA.polygonCount ()) {
				result[0].splitEdgesAndPolygons ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, tolerance );
			}
			result[0].endStructureChanges ();
		}

		#pragma omp section
		{
			result[1] = meshB.clone ();
			std::vector<u32> polygonIDs;
			std::vector<u32> pointIDs1;
			std::vector<f32> ratio1;
			std::vector<u32> pointIDs2;
			std::vector<f32> ratio2;
			std::vector<std::vector<Vec3>> addPointPositions;
			this->MakeSplitPolygonsList ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, meshB, crossDataB, false );
			result[1].beginStructureChanges ();
			result[1].splitEdgesAndPolygons ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, tolerance );
			if (0 < polygonIDs.size () && result[1].polygonCount () == meshB.polygonCount ()) {
				result[1].splitEdgesAndPolygons ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, tolerance );
			}
			result[1].endStructureChanges ();
		}
	}
	
	return meshA.pointCount () < result[0].pointCount () || meshB.pointCount () < result[1].pointCount ();
}