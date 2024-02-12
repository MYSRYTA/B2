//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
OctreeT<T>::OctreeT (){
	this->branchOffset.resize ( 8 );
	this->branchOffset[0] = Vec3T<T>{ -1,-1,-1 };
	this->branchOffset[1] = Vec3T<T>{ -1,-1, 1 };
	this->branchOffset[2] = Vec3T<T>{ -1, 1,-1 };
	this->branchOffset[3] = Vec3T<T>{  1,-1,-1 };
	this->branchOffset[4] = Vec3T<T>{  1, 1,-1 };
	this->branchOffset[5] = Vec3T<T>{  1,-1, 1 };
	this->branchOffset[6] = Vec3T<T>{ -1, 1, 1 };
	this->branchOffset[7] = Vec3T<T>{  1, 1, 1 };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    全てのノードのXfoを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::getAllNodeTreeXfos ( const OctreeNodeT<T> &node, IO std::vector<XfoT<T>> &result, Bool includeEmptyNode )const {
	if ( node.isLeafNode () ) {
		if ( includeEmptyNode || node.polyCount () != 0 ) {
			result.push_back ( XfoT<T>( Vec3T<T>( node.sideLength ), QuatT<T>(), node.position ));
		}
	} else {
		for ( u32 i = 0; i<8; i++ ) {
			this->getAllNodeTreeXfos ( node.branches [i], result );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    全てのノードのポジションを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::getAllNodeTreePositions ( const OctreeNodeT<T> &node, IO std::vector<Vec3T<T>> &result, Bool includeEmptyNode )const {
	if ( node.isLeafNode () ) {
		if ( includeEmptyNode || node.polyCount () != 0 ) {
			result.push_back ( node.position );
		}
	} else {
		for ( u32 i = 0; i<8; i++ ) {
			this->getAllNodeTreePositions ( node.branches [i], result, includeEmptyNode );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    maxDist距離内の存在する全てのTriPolygonのIDを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<u32> OctreeT<T>::getAllClosestTriPolygonIDs ( const Vec3T<T> &startPosition, T maxDist )const {
	std::vector<u32> result;
	this->getTriPolyIDsShortestDistance ( this->nodeTree, startPosition, maxDist, result );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Rayと交差する全てのTriPolygonのIDを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<u32> OctreeT<T>::getAllRayCastTriPolygonIDs ( const Ray3T<T> &ray )const {
	std::vector<u32> result;
	this->getTriPolyIDsRayIntersect ( this->nodeTree, ray, result );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshからOctreeを構築する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::set ( const PolygonMeshT<T> &mesh ){
	std::vector<f32> dummy;
	this->set ( mesh, dummy, false );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshからOctreeを構築する
//!	    @brief	PolygonMeshからOctreeを構築する
//!		@param[in] mask VertexValue　
//!		@param[in] invertMask　マスクを反転
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::set ( const PolygonMeshT<T> &mesh, const std::vector<f32> &mask, Bool invertMask ) {
	assert ( 0<mesh.polygonCount () );

	PolygonMeshT<T> triMesh = mesh.createPureTriangleMesh ();
	Vec3AttributeT<T> &meshPointPosAttr = triMesh.getVec3Attribute ( "pointPositions" );
	U32AttributeT<T> &origPolyIDAttr = triMesh.getU32Attribute ( "OriginalPolygonIDs" );
	this->triPolygons.resize ( triMesh.polygonCount () );
	u32 cnt = triMesh.polygonCount ();
	#pragma omp parallel for
	for ( s32 i = 0; i<cnt; i++ ) {
		/* meshをオクトリートライアングルポリゴンに変換 */
		L8Array<u32> polyPoiIDs = triMesh.getPolygonPointIDs ( i );
		OctreeTriPolygonT<T> triPoly;
		{
			triPoly.p0 = meshPointPosAttr [polyPoiIDs [0]];
			triPoly.p1 = meshPointPosAttr [polyPoiIDs [1]];
			triPoly.p2 = meshPointPosAttr [polyPoiIDs [2]];
			triPoly.bBox = BBoxT<T> ( triPoly.p0, triPoly.p1 );
			triPoly.bBox.merge ( triPoly.p2 );
			triPoly.triPolygonID = u32 ( i );
			u32 loTriID;
			triPoly.origPolygonID = Burgess2MathFn::BinarySearchTree( origPolyIDAttr.getValuesRef (), i, loTriID );
			triPoly.localTriID = loTriID;
		}
		this->triPolygons [i] = triPoly;
	}

	/* octreeNodeツリーのルートを作成する */
	BBoxT<T> rootBBox ( meshPointPosAttr.getValuesRef () );
	this->nodeTree = OctreeNodeT<T> ( u8 ( 1 ), rootBBox.getCenter (), rootBBox.getLongestSideLength () );

	/* octreeNodeツリーを作成する (三角形ポリゴンを格納していく) */
	u32 triPolyCount = this->triPolygons.size ();
	if ( mask.size () == mesh.vertexCount () ) {
		/* Maskを使う場合はVertexAttributeをPolygonAttributeへ変換 */
		u32 polyCount = mesh.polygonCount ();
		std::vector<Bool> origPolyMask ( polyCount );
		#pragma omp parallel for if(1000 < polyCount)
		for ( s32 i = 0; i<polyCount; i++ ) {
			L8Array<u32> verIDs = mesh.getPolygonVertexIDs ( i );
			u32 verSize = verIDs.size ();
			u8 sum = 0;
			for ( u32 k = 0; k <verSize; k++ ) {
				if ( 0.0f < mask[ verIDs [k] ] ) {
					sum++;
				}
			}
			/* ポリゴンを構成する全てのバーテックス(mask)に値が設定されている場合はfalse, 値が０のバーテックスが1つでもあった場合はtrue */
			origPolyMask [i] = verSize == sum ? false : true;
		}
		/* origPolyMaskに即したtriPolygonをOctreeに格納する */
		for ( u32 i = 0; i<triPolyCount; i++ ) {
			u32 polyID = this->triPolygons[i].origPolygonID;
			if ( origPolyMask [polyID] == invertMask ) {
				this->buildNodeTreeTask ( this->nodeTree, this->triPolygons [i] );
			}
		}
	} else {
		/* 全てのTriPolygonを使用 */
		for ( u32 i = 0; i<triPolyCount; i++ ) {
			this->buildNodeTreeTask ( this->nodeTree, this->triPolygons [i] );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    三角形ポリゴンIDをOctreeNodeツリーに登録する (必要に応じて子階層の作成も行う)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::buildNodeTreeTask ( IO OctreeNodeT<T> &node, const OctreeTriPolygonT<T> &triPoly ){
	if (node.isLeafNode ()) { /* nodeが末端ノードの場合 */
		if (node.testIntersect ( triPoly.bBox )) { /* 三角ポリゴンのBBoxがnodeと接触or内包する場合 */
			if (node.polyCount () < BurgessOctree::MAX_TRIPOLYGON_COUNT ) { /* nodeに登録しているポリゴン数が限界に達していないかチェック */
				node.addTriPolyID ( triPoly.triPolygonID ); /* ポリゴンIDをnodeに登録して終了 */
			} else {
				node.addTriPolyID ( triPoly.triPolygonID ); /* 一旦このノードにポリゴンIDを登録 */
				if (node.level < BurgessOctree::MAX_OCTREE_LEVEL) {
					/* 子供階層がまだ作れる(ツリーの階層数が限界に達していない場合) */
					this->addBranchAndReallocate ( node ); /* 子供階層を作ってnodeに登録されていたポリゴンを子階層に再割り当てする */
				}
			}
		}
	} else {
		/* 末端ノードまで辿る */
		for (u32 i = 0; i<8; i++) {
			this->buildNodeTreeTask ( node.branches[i], triPoly );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    nodeの子供階層を作成し、nodeに格納していた三角形ポリゴンの情報を子階層へ再割り当てする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::addBranchAndReallocate ( IO OctreeNodeT<T> &node ) {
	/* nodeを8分割して子階層にする */
	Vec3T<T> childNodePos;
	for ( u32 i = 0; i < 8; i++ ) {
		childNodePos = node.position + node.sideLength * T ( 0.25 ) * this->branchOffset [i];
		node.addNode ( node.level + 1, childNodePos, node.sideLength * T ( 0.5 ) );
	}

	/* nodeに登録されていたポリゴンID群を子供階層に再割り当てする */
	for ( u32 i = 0; i<8; i++ ) {
		for ( u32 k = 0; k<node.polyCount (); k++ ) { 
			u32 triPolyID = node.triPolyIDs [k];
			if ( this->triPolygons [triPolyID].bBox.testIntersect ( node.branches [i] ) ) { /* 三角ポリゴンのBBoxがnodeの子供ノードと接触or内包する場合 */
				if ( node.branches [i].polyCount () < BurgessOctree::MAX_TRIPOLYGON_COUNT ) {/* nodeに登録できる三角形ポリゴンの数が限界値を超えていないかチェック */
					node.branches [i].addTriPolyID ( triPolyID ); /* 子ノードにポリゴンIDを登録して終了 */
				} else {
					if ( node.branches [i].level < BurgessOctree::MAX_OCTREE_LEVEL ) {
						this->addBranchAndReallocate ( node.branches [i] ); /* 更に子供階層の作成へ進む */
					}
				}
			}
		}
	}
	node.triPolyIDs.resize ( 0 ); /* nodeに登録されていたポリゴンID群をクリア */
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    一定距離内に存在するOctreeTriPolygonのIDを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::getTriPolyIDsShortestDistance ( const OctreeNodeT<T> &node, const Vec3T<T> &startPosition, T maxDist, OUT std::vector<u32> &result )const {
	BSphT<T> bs ( node.position, node.radius );
	if ( bs.testClosest ( startPosition, maxDist ) ) { // バウンディングスフィアで簡易な近傍検索を行う
		if ( node.testClosest ( startPosition, maxDist ) ) { // OctreeNode(バウンディングボックス)に対して正確な近傍検索を行う
			if ( node.isLeafNode () ) { // nodeが末端ノードかチェック
				// nodeに格納されている全ての三角ポリゴンIDを抜き出す
				for ( u32 i = 0; i<node.polyCount (); i++ ) {
					result.push_back( node.triPolyIDs [i] );
				}
			} else {
				// 末端のノードまで辿る
				for ( u32 i = 0; i<8; i++ ) {
					this->getTriPolyIDsShortestDistance ( node.branches [i], startPosition, maxDist, result );
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Rayと交差するOctreeTriPolygonのIDを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::getTriPolyIDsRayIntersect ( const OctreeNodeT<T> &node, const Ray3T<T> &ray, OUT std::vector<u32> &result )const {
	BSphT<T> bs ( node.position, node.radius );
	if (bs.testRayIntersect ( ray )) { // バウンディングスフィアで簡易な交差判定を行う
		if (node.testRayIntersect ( ray )) { // OctreeNode(バウンディングボックス)に対して正確な交差判定を行う
			if (node.isLeafNode ()) { // nodeが末端ノードかチェック
				//nodeに格納されている全ての三角ポリゴンIDを抜き出す
				for (u32 i = 0; i<node.polyCount (); i++) {
					result.push_back ( node.triPolyIDs[i] );
				}
			} else {
				// 末端のノードまで辿る
				for (u32 i = 0; i<8; i++) {
					this->getTriPolyIDsRayIntersect ( node.branches[i], ray, result );
				}
			}
		}
	}
}

template<typename T>
void OctreeT<T>::test_getTriPolyIDsRayIntersect ( const OctreeNodeT<T>& node, const Ray3T<T>& ray, OUT std::vector<u32>& result )const {
	result.resize ( 0 );
	BSphT<T> bs ( node.position, node.radius );
	if (bs.testRayIntersect ( ray )) { /* バウンディングスフィアで簡易な交差判定を行う */
		if (node.testRayIntersect ( ray )) { /* OctreeNode(バウンディングボックス)に対して正確な交差判定を行う */
			if (node.isLeafNode ()) { /* nodeが末端ノードかチェック */
				/* nodeに格納されている全ての三角ポリゴンIDを抜き出す */
				for (u32 i = 0; i < node.polyCount (); i++) {
					result.push_back ( node.triPolyIDs[i] );
				}
			}
			else {
				/* 末端のノードまで辿る */
				for (u32 i = 0; i < 8; i++) {
					this->getTriPolyIDsRayIntersect ( node.branches[i], ray, result );
				}
			}
		}
	}
}