//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴン数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::polygonCount () const{
	U32AttributeT<T>& attr = this->getU32Attribute ( "polygonSizes" );
	return attr.size() - 1; /* [0]を省く */
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    各ポリゴンを構成する頂点数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<u32> PolygonMeshT<T>::getAllPolygonSizes ( Bool getAdditionStyle )const {
	U32AttributeT<T>& attr = this->getU32Attribute ( "polygonSizes" );
	std::vector<u32> result = attr.getValues();
	if ( getAdditionStyle == false ) {
		result.resize ( result.size()-1 );
		#pragma omp parallel for
		for ( s32 i = 0; i<result.size (); i++ ) {
			result [i] = attr [i+1] - attr [i];
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴンを構成する頂点数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPolygonSize ( u32 polygonID )const {
	assert ( polygonID < this->polygonCount () );
	U32AttributeT<T>& attr = this->getU32Attribute ( "polygonSizes" );
	return attr [polygonID+1] - attr [polygonID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポイントに接続する (ポイントで構成されている) ポリゴンのID群を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
L16Array<u32> PolygonMeshT<T>::getPointPolygonIDs ( u32 pointID )const {
	assert ( pointID < this->pointCount () );
	L16Array<u32> verIDs = this->getSharedVertexIDs ( pointID );
	U32AttributeT<T>& polySizeAttr = this->getU32Attribute ( "polygonSizes" );

	L16Array<u32> result ( verIDs.size () );
	for ( u32 i = 0; i<verIDs.size(); i++ ) {
		result [i] = Burgess2MathFn::BinarySearchTree( polySizeAttr.getValuesRef(), verIDs[i] );
	}
	return result;
}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポイントに接続する (ポイントで構成されている) ポリゴンのID群を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16Array<u32> PolygonMeshT<T>::getPointPolygonIDs ( u32 pointID )const {
	assert ( pointID < this->pointCount () );
	L16Array<u32>& verIDs = this->getSharedVertexIDs ( pointID );
	U32AttributeT<T>& vertexPolygonIDAttr = this->getU32Attribute ( "vertexPolygonID" );

	L16Array<u32> result ( verIDs.size () );
	for ( u32 i = 0; i<verIDs.size(); i++ ) {
		result [i] = vertexPolygonIDAttr[verIDs[i]];
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポイントに接続する (ポイントで構成されている) ポリゴンの数を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPointPolygonCount ( u32 pointID )const{
	assert ( pointID < this->pointCount () );
	L16Array<u32> verIDs = this->getSharedVertexIDs ( pointID );
	return verIDs.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスで構成しているポリゴンIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
u32 PolygonMeshT<T>::getVertexPolygonID ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	U32AttributeT<T>& polygonSizeAttr = this->getU32Attribute ( "polygonSizes" );
	return Burgess2MathFn::BinarySearchTree( polygonSizeAttr.getValues (), vertexID );
}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスで構成しているポリゴンIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getVertexPolygonID ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	U32AttributeT<T>& vertexPolygonIDAttr = this->getU32Attribute ( "vertexPolygonID" );
	return vertexPolygonIDAttr[vertexID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    エッジの左側 (edgeIDで構成しているポリゴン) のIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeLeftPolygonID ( u32 edgeID )const {
	assert ( edgeID < this->edgeCount () );
	u32 startVerID = this->getEdgeStartVertexID ( edgeID );
	return this->getVertexPolygonID ( startVerID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    エッジの右側  (edgeIDを境にした隣接ポリゴン) のIDを返す （edgeIDが境界エッジの場合は無限値を返す）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeRightPolygonID ( u32 edgeID )const {
	assert ( edgeID < this->edgeCount () );
	if ( this->isPolygonBorder ( edgeID ) ){ return BURGESS2::U32_INFINITE; }
	return this->getPolygonVertexAdjacentPolygonID ( edgeID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    edgeIDを境にした隣接ポリゴンのIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPolygonPointAdjacentPolygonID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );
	u32 edgeID = this->getEdgeID ( polygonID, pointID );
	return this->getPolygonVertexAdjacentPolygonID ( edgeID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    edgeIDを境にした隣接ポリゴンのIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPolygonVertexAdjacentPolygonID ( u32 edgeID )const {
	assert ( edgeID < this->edgeCount () );
	u32 adjEdgeID = this->getEdgeAdjacentEdgeID ( edgeID );
	if ( adjEdgeID < this->edgeCount () ) { // 隣接エッジが見つかった場合 
		u32 startVerID = this->getEdgeStartVertexID ( adjEdgeID );
		return this->getVertexPolygonID ( startVerID );
	}
	return BURGESS2::U32_INFINITE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴン内のN番目の頂点番号を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPolygonPoint ( u32 polygonID, LocalIndex polylocalID )const{
	assert ( polygonID < this->polygonCount () );
	L8Array<u32> poiIDs = this->getPolygonPointIDs ( polygonID );
	assert ( polylocalID < poiIDs.size () );
	return poiIDs[polylocalID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点で共有しているポリゴン群に対して、次（左回り）のポリゴンIDを取得する
//!		@warning 複数のポリゴンアイランドが1つの頂点を共有しており、更に次のポリゴンが別のアイランドを指す場合は左回りとは限らない
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getNextPolygonID ( u32 vertexID )const{
	assert ( vertexID < this->vertexCount () );
	u32 nextVerID = this->getNextPointVertexID ( vertexID );
	assert ( nextVerID < this->vertexCount () );
	return this->getVertexPolygonID( nextVerID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点で共有しているポリゴン群に対して、1つ前（右回り）のポリゴンIDを取得する
//!		@warning 複数のポリゴンアイランドが1つの頂点を共有しており、更に1つ前のポリゴンが別のアイランドを指す場合は右回りとは限らない
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPreviousPolygonID ( u32 vertexID )const{
	assert ( vertexID < this->vertexCount () );
	u32 prevVerID = this->getPreviousPointVertexID ( vertexID );
	assert ( prevVerID < this->vertexCount () );
	return this->getVertexPolygonID ( prevVerID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴンに含まれる三角形メッシュの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 PolygonMeshT<T>::triangleCount ( u32 polygonID )const{
	assert ( polygonID < this->polygonCount () );
	return u8( this->getPolygonSize ( polygonID ) - 2 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    三角形メッシュを構成する頂点ID（３つ）を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L4Array<u32> PolygonMeshT<T>::getTrianglePointIDs ( u32 polygonID, u8 triangleID )const{
	assert ( polygonID < this->polygonCount () );
	assert ( triangleID < this->triangleCount ( polygonID ) );
	L4Array<u32> result ( 3 );
	u32 verID = this->getPolygonFirstVertexID ( polygonID );
	result[0] = this->getPointID ( verID );
	result[1] = this->getPointID ( verID + 1 + triangleID );
	result[2] = this->getPointID ( verID + 2 + triangleID );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定ポリゴン内の指定頂点を使用するポリゴン内三角形メッシュのIDを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L4Array<u8> PolygonMeshT<T>::getTriangleIDs ( u32 polygonID, u32 pointID )const{
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );

	L4Array<u8> result;
	u32 poiVerLID = this->getLocalVertexID ( polygonID, pointID ); // ポリゴン内のローカルバーテックスIDを取得
	u8 triSize = this->triangleCount ( polygonID );
	if ( triSize == 1 ){
		// ポリゴンが三角形の場合
		result.push ( u8 ( 0 ) );
	} else if ( poiVerLID == 0 ){ 
		// ポリゴン内の最初のバーテックスの場合は全ての三角ポリゴンIDを返す
		result.resize ( triSize );
		for ( u8 i = 0, n = triSize; i < n; ++i ){ result[i] = i; }
	} else if ( poiVerLID == 1 ){ 
		// １つ目のバーテックスの場合は最初の三角ポリゴンIDを返す
		result.push ( u8 ( 0 ) );
	} else if ( poiVerLID == u8(this->getPolygonSize ( polygonID ) - 1) ){ 
		// 最後のバーテックスの場合は最後の三角ポリゴンIDを返す
		result.push ( triSize - u8(1) );
	} else{ 
		// 上記以外　バーテックスを共有する隣り合う三角形ポリゴンのIDを返す
		result.push ( poiVerLID - u8 ( 1 ) );
		result.push ( poiVerLID - u8 ( 2 ) );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定バーテックスを使用するポリゴン内三角形メッシュのIDを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L4Array<u8> PolygonMeshT<T>::getTriangleIDs ( u32 vertexID )const{
	assert ( vertexID < this->vertexCount () );

	u32 polygonID = this->getVertexPolygonID ( vertexID );
	u32 pointID = this->getPointID ( vertexID );
	return this->getTriangleID ( polygonID, pointID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴン内でpointIDを始点とするハーフエッジがボーダー (境界エッジ) の場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::isPolygonBorder ( u32 polygonID, u32 pointID )const{
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );
	u32 verID = this->getVertexID ( polygonID, pointID );
	return this->isPolygonBorder ( verID );
}