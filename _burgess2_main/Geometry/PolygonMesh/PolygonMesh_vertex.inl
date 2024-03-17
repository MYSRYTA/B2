//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::vertexCount () const{
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	return pointIDAttr.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ŋ��L���Ă���o�[�e�b�N�XID�Q��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16Array<u32> PolygonMeshT<T>::getSharedVertexIDs ( u32 pointID )const {
	L16ArrayU32Attribute &sharedVertexIDsAttr = this->getL16ArrayU32Attribute ( "sharedVertexIDs" );
	assert ( pointID < u32 ( sharedVertexIDsAttr.size () ) );
	return sharedVertexIDsAttr [pointID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S�����\������o�[�e�b�N�XID�Q��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L8Array<u32> PolygonMeshT<T>::getPolygonVertexIDs ( u32 polygonID )const {
	assert ( polygonID < u32 ( this->polygonCount () ) );
	u32 polyVertexSize = this->getPolygonSize ( polygonID );
	L8Array<u32> result ( polyVertexSize );
	result [0] = this->getPolygonFirstVertexID ( polygonID );
	for ( u32 i = 1; i<polyVertexSize; i++ ) {
		result [i] = result [0] + i;
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�̃o�[�e�b�N�XID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getVertexID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () && pointID < this->pointCount () );
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	u32 offset = this->getPolygonFirstVertexID ( polygonID );
	
	u32 result = BURGESS2::U32_INFINITE;
	for ( u32 i = 0; i<this->getPolygonSize ( polygonID ); i++ ) {
		if ( pointIDAttr [offset+i] == pointID ) {
			result = offset+i;
			break;
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�̃��[�J���o�[�e�b�N�XID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getLocalVertexID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () && pointID < this->pointCount () );
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	u32 offset = this->getPolygonFirstVertexID ( polygonID );
	
	u32 result = BURGESS2::U32_INFINITE;
	for ( u32 i = 0; i<this->getPolygonSize ( polygonID ); i++ ) {
		if ( pointIDAttr [offset+i] == pointID ) {
			result = i;
			break;
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���[�J���o�[�e�b�N�XID�̎��i�����j�̃��[�J���o�[�e�b�N�X�h�c��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getNextLocalVertexID(u32 polygonID, u32 localVertexID)const {
	assert(polygonID < this->polygonCount() && localVertexID < this->getPolygonSize(polygonID));

	if (this->getPolygonSize(polygonID) == localVertexID + 1) {
		return 0;
	} else {
		return localVertexID + 1;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���[�J���o�[�e�b�N�XID�̈�O�i�E���j�̃��[�J���o�[�e�b�N�X�h�c��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPreviousLocalVertexID(u32 polygonID, u32 localVertexID)const {
	assert(polygonID < this->polygonCount() && localVertexID < this->getPolygonSize(polygonID));
	
	if (localVertexID == 0 ) {
		return this->getPolygonSize(polygonID) - 1;
	} else {
		return localVertexID - 1;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S�����\������ŏ��̃o�[�e�b�N�XID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPolygonFirstVertexID ( u32 polygonID )const {
	assert ( polygonID < this->polygonCount () );
	U32AttributeT<T>& polySizeAttr = this->getU32Attribute ( "polygonSizes" );
	return polySizeAttr [polygonID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S�����̎�(�����)�̃o�[�e�b�N�XID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getNextPolygonVertexID ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	if(this->isPolygonLastVertex ( vertexID )){
		u32 polyID = this->getVertexPolygonID ( vertexID );
		U32AttributeT<T>& polygonSizeAttr = this->getU32Attribute ( "polygonSizes" );
		return polygonSizeAttr [polyID];
	} else {
		return vertexID+1;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S������1�O(�E���)�̃o�[�e�b�N�XID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPreviousPolygonVertexID ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	if ( this->isPolygonFirstVertex ( vertexID ) ) {
		u32 polyID = this->getVertexPolygonID ( vertexID );
		U32AttributeT<T>& polygonSizeAttr = this->getU32Attribute ( "polygonSizes" );
		return polygonSizeAttr [polyID+1] - 1;
	} else {
		return vertexID-1;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    vertexID���|���S�����\������ŏ��̃o�[�e�b�N�X�������ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
Bool PolygonMeshT<T>::isPolygonFirstVertex ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	U32AttributeT<T> &polygonSizeAttr = this->getU32Attribute ( "polygonSizes" );
	u32 remainder=0;
	u32 polyID = Burgess2MathFn::BinarySearchTree( polygonSizeAttr.getValues (), vertexID, remainder );
	return 0 == remainder;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    vertexID���|���S�����\������Ō�̃o�[�e�b�N�X�������ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::isPolygonLastVertex ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	U32AttributeT<T>& polygonSizeAttr = this->getU32Attribute ( "polygonSizes" );
	u32 remainder;
	u32 polyID = Burgess2MathFn::BinarySearchTree( polygonSizeAttr.getValues (), vertexID, remainder );
	return this->getPolygonSize ( polyID ) - 1 == remainder;
}*/


//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    vertexID���|���S�����\������ŏ��̃o�[�e�b�N�X�������ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::isPolygonFirstVertex( u32 vertexID )const{
	assert( vertexID < this->vertexCount() );
	if ( vertexID == 0 ){
		return true;
	} else if ( vertexID == this->vertexCount() - 1 ){
		return false;
	} else{
		U32AttributeT<T>& vertexPolygonIDAttr = this->getU32Attribute( "vertexPolygonID" );
		return vertexPolygonIDAttr[vertexID - 1] != vertexPolygonIDAttr[vertexID];
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    vertexID���|���S�����\������Ō�̃o�[�e�b�N�X�������ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::isPolygonLastVertex( u32 vertexID )const{
	assert( vertexID < this->vertexCount() );
	if ( vertexID == 0 ){
		return false;
	} else if ( vertexID == this->vertexCount() - 1 ){
		return true;
	} else{
		U32AttributeT<T>& vertexPolygonIDAttr = this->getU32Attribute( "vertexPolygonID" );
		return vertexPolygonIDAttr[vertexID] != vertexPolygonIDAttr[vertexID + 1];
	}
}