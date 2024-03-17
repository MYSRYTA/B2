//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    vertexID���n�_�Ƃ���n�[�t�G�b�WID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeID ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	return vertexID;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �n�[�t�G�b�W�̎n�_�o�[�e�b�N�XID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeStartVertexID ( u32 halfEdgeID )const {
	assert ( halfEdgeID < this->edgeCount () );
	return halfEdgeID;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �n�[�t�G�b�W�̎n�_�E�I�_�̃|�C���gID��Ԃ�  Vec2.x=startPointID, .y=endPointID
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
//!	    �n�[�t�G�b�W�̎n�_�E�I�_�̃o�[�e�b�N�XID��Ԃ�  Vec2.x=startVertexID, .y=endVertexID
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
//!	    start/endPointID�ō\�������n�[�t�G�b�WID��Ԃ�
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
//!	    pointID�ō\�������S�Ẵn�[�t�G�b�WID���擾���� (oneWay=true�̏ꍇ��pointMeshEdgeCount()�Ɠ�������halfEdge��Ԃ�)
//!	    @brief pointID�ō\�������S�Ẵn�[�t�G�b�WID���擾���� (oneWay=true�̏ꍇ��pointMeshEdgeCount()�Ɠ�������halfEdge��Ԃ�)
//!		@param[in] pointID 
//!		@param[in] oneWay ���ʂ̃��b�V���G�b�W���\������halfEdge���܂܂Ȃ��ꍇ��true 
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
//!	    �|���S������pointID���n�_�Ƃ���n�[�t�G�b�WID��Ԃ�
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
//!	    halfEdgeID�ɗאڂ��� (����̒��_�ō\�������) �n�[�t�G�b�WID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeAdjacentEdgeID ( u32 halfEdgeID )const {
	assert ( halfEdgeID < this->edgeCount () );
	Vec2u edgePoiIDs = this->getEdgePointIDs ( halfEdgeID );
	return this->getEdgeIDFromStartEnd ( edgePoiIDs[1], edgePoiIDs[0] );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S������pointID���n�_�Ƃ���n�[�t�G�b�W�ɗאڂ��� (����̒��_�ō\�������) �n�[�t�G�b�WID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeAdjacentEdgeID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );
	u32 verID = this->getVertexID ( polygonID, pointID );
	return this->getEdgeAdjacentEdgeID ( verID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �n�[�t�G�b�W���{�[�_�[ (���E�G�b�W) �̏ꍇ��true��Ԃ�
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
//!	    �n�[�t�G�b�W�̐���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::edgeCount ()const {
	return this->vertexCount ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ō\�����Ă���n�[�t�G�b�W�̐���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::pointEdgeCount ( u32 pointID )const{	
	return this->getPointEdgeIDs ( pointID ).size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ɐڑ����Ă���G�b�W�̐���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::pointMeshEdgeCount ( u32 pointID )const{
	return this->getPointSurroundingPoints ( pointID, false ).size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �G�b�W�̐���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::meshEdgeCount ()const{
	u32 edgesCount{ 0 };
	for ( u32 i = 0; i < this->pointCount (); ++i ){
		edgesCount += this->pointMeshEdgeCount ( i );
	}
	return edgesCount / 2;
}