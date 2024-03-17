//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    vertexIDを始点とするハーフエッジIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeID ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	return vertexID;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ハーフエッジの始点バーテックスIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeStartVertexID ( u32 halfEdgeID )const {
	assert ( halfEdgeID < this->edgeCount () );
	return halfEdgeID;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ハーフエッジの始点・終点のポイントIDを返す  Vec2.x=startPointID, .y=endPointID
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2u PolygonMeshT<T>::getEdgePointIDs ( u32 halfEdgeID )const {
	assert ( halfEdgeID < this->edgeCount () );
	u32 startVerID = this->getEdgeStartVertexID ( halfEdgeID );
	u32 startPoiID = this->getPointID ( startVerID );
	u32 polyID = this->getVertexPolygonID ( startVerID );

	Vec2u result;
	result[0] = startPoiID;
	result[1] = this->getNextPolygonPointID ( polyID, startPoiID );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ハーフエッジの始点・終点のバーテックスIDを返す  Vec2.x=startVertexID, .y=endVertexID
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2u PolygonMeshT<T>::getEdgeVertexIDs ( u32 halfEdgeID )const {
	assert ( halfEdgeID < this->edgeCount () );
	u32 startVerID = this->getEdgeStartVertexID ( halfEdgeID );

	Vec2u result;
	result[0] = startVerID;
	result[1] = this->getNextPolygonVertexID ( startVerID );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    start/endPointIDで構成されるハーフエッジIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeIDFromStartEnd ( u32 startPointID, u32 endPointID )const {
	assert ( startPointID < this->pointCount () );
	assert ( endPointID < this->pointCount () );
	L16Array<u32> sharedVerIDs = this->getSharedVertexIDs ( startPointID );
	for ( u32 i = 0; i<sharedVerIDs.size (); i++ ) {
		u32 nextVerID = this->getNextPolygonVertexID ( sharedVerIDs [i] );
		if ( endPointID == this->getPointID ( nextVerID ) ) {
			return this->getEdgeID( sharedVerIDs [i] );
		}
	}
	return BURGESS2::U32_INFINITE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    pointIDで構成される全てのハーフエッジIDを取得する (oneWay=trueの場合はpointMeshEdgeCount()と同じ数のhalfEdgeを返す)
//!	    @brief pointIDで構成される全てのハーフエッジIDを取得する (oneWay=trueの場合はpointMeshEdgeCount()と同じ数のhalfEdgeを返す)
//!		@param[in] pointID 
//!		@param[in] oneWay 共通のメッシュエッジを構成するhalfEdgeを含まない場合はtrue 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16Array<u32> PolygonMeshT<T>::getPointEdgeIDs ( u32 pointID, Bool oneWay ) const {
	assert ( pointID < this->pointCount () );
	L16Array<u32> result = this->getSharedVertexIDs ( pointID );
	if ( oneWay ){
		for ( u32 i = 0; i < result.size (); i++ ){
			u32 e = this->getPreviousPolygonVertexID ( result[i] );
			if ( this->isPolygonBorder ( e ) ){
				result.push ( e );
			}
		}
	}else{
		for ( u32 i = 0; i < result.size (); i++ ){
			result.push ( this->getPreviousPolygonVertexID ( result[i] ) );
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴン内でpointIDを始点とするハーフエッジIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPolygonPointEdgeID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );
	L16Array<u32> sharedVerIDs = this->getSharedVertexIDs ( pointID );
	for ( u32 i = 0; i<sharedVerIDs.size (); i++ ) {
		u32 polyID = this->getVertexPolygonID ( sharedVerIDs [i] );
		if ( polyID == polygonID ) {
			return this->getEdgeID ( sharedVerIDs [i] );
		}
	}
	return BURGESS2::U32_INFINITE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    halfEdgeIDに隣接する (同一の頂点で構成される) ハーフエッジIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeAdjacentEdgeID ( u32 halfEdgeID )const {
	assert ( halfEdgeID < this->edgeCount () );
	Vec2u edgePoiIDs = this->getEdgePointIDs ( halfEdgeID );
	return this->getEdgeIDFromStartEnd ( edgePoiIDs[1], edgePoiIDs[0] );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴン内でpointIDを始点とするハーフエッジに隣接する (同一の頂点で構成される) ハーフエッジIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeAdjacentEdgeID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );
	u32 verID = this->getVertexID ( polygonID, pointID );
	return this->getEdgeAdjacentEdgeID ( verID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ハーフエッジがボーダー (境界エッジ) の場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::isPolygonBorder ( u32 halfEdgeID )const {
	assert ( halfEdgeID < this->edgeCount () );
	Vec2u poiID = this->getEdgePointIDs ( halfEdgeID );
	L16Array<u32> sharedVerIDs = this->getSharedVertexIDs ( poiID[1] ); /* .y=endPointID */
	for ( u32 i = 0; i<sharedVerIDs.size (); i++ ) {
		u32 nextVerID = this->getNextPolygonVertexID ( sharedVerIDs [i] );
		if ( this->getPointID ( nextVerID ) == poiID[0] ) { /* .y=startPointID */
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ハーフエッジの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::edgeCount ()const {
	return this->vertexCount ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点で構成しているハーフエッジの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::pointEdgeCount ( u32 pointID )const{	
	return this->getPointEdgeIDs ( pointID ).size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点に接続しているエッジの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::pointMeshEdgeCount ( u32 pointID )const{
	return this->getPointSurroundingPoints ( pointID, false ).size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    エッジの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::meshEdgeCount ()const{
	u32 edgesCount{ 0 };
	for ( u32 i = 0; i < this->pointCount (); ++i ){
		edgesCount += this->pointMeshEdgeCount ( i );
	}
	return edgesCount / 2;
}