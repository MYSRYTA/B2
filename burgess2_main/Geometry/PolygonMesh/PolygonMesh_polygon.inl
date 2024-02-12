//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S������Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::polygonCount () const{
	U32AttributeT<T>& attr = this->getU32Attribute ( "polygonSizes" );
	return attr.size() - 1; /* [0]���Ȃ� */
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �e�|���S�����\�����钸�_����Ԃ�
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
//!	    �|���S�����\�����钸�_����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPolygonSize ( u32 polygonID )const {
	assert ( polygonID < this->polygonCount () );
	U32AttributeT<T>& attr = this->getU32Attribute ( "polygonSizes" );
	return attr [polygonID+1] - attr [polygonID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|�C���g�ɐڑ����� (�|�C���g�ō\������Ă���) �|���S����ID�Q���擾����
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
//!	    �|�C���g�ɐڑ����� (�|�C���g�ō\������Ă���) �|���S����ID�Q���擾����
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
//!	    �|�C���g�ɐڑ����� (�|�C���g�ō\������Ă���) �|���S���̐����擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPointPolygonCount ( u32 pointID )const{
	assert ( pointID < this->pointCount () );
	L16Array<u32> verIDs = this->getSharedVertexIDs ( pointID );
	return verIDs.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X�ō\�����Ă���|���S��ID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
u32 PolygonMeshT<T>::getVertexPolygonID ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	U32AttributeT<T>& polygonSizeAttr = this->getU32Attribute ( "polygonSizes" );
	return Burgess2MathFn::BinarySearchTree( polygonSizeAttr.getValues (), vertexID );
}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X�ō\�����Ă���|���S��ID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getVertexPolygonID ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	U32AttributeT<T>& vertexPolygonIDAttr = this->getU32Attribute ( "vertexPolygonID" );
	return vertexPolygonIDAttr[vertexID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �G�b�W�̍��� (edgeID�ō\�����Ă���|���S��) ��ID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeLeftPolygonID ( u32 edgeID )const {
	assert ( edgeID < this->edgeCount () );
	u32 startVerID = this->getEdgeStartVertexID ( edgeID );
	return this->getVertexPolygonID ( startVerID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �G�b�W�̉E��  (edgeID�����ɂ����אڃ|���S��) ��ID��Ԃ� �iedgeID�����E�G�b�W�̏ꍇ�͖����l��Ԃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getEdgeRightPolygonID ( u32 edgeID )const {
	assert ( edgeID < this->edgeCount () );
	if ( this->isPolygonBorder ( edgeID ) ){ return BURGESS2::U32_INFINITE; }
	return this->getPolygonVertexAdjacentPolygonID ( edgeID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    edgeID�����ɂ����אڃ|���S����ID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPolygonPointAdjacentPolygonID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );
	u32 edgeID = this->getEdgeID ( polygonID, pointID );
	return this->getPolygonVertexAdjacentPolygonID ( edgeID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    edgeID�����ɂ����אڃ|���S����ID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPolygonVertexAdjacentPolygonID ( u32 edgeID )const {
	assert ( edgeID < this->edgeCount () );
	u32 adjEdgeID = this->getEdgeAdjacentEdgeID ( edgeID );
	if ( adjEdgeID < this->edgeCount () ) { // �אڃG�b�W�����������ꍇ 
		u32 startVerID = this->getEdgeStartVertexID ( adjEdgeID );
		return this->getVertexPolygonID ( startVerID );
	}
	return BURGESS2::U32_INFINITE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S������N�Ԗڂ̒��_�ԍ���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPolygonPoint ( u32 polygonID, LocalIndex polylocalID )const{
	assert ( polygonID < this->polygonCount () );
	L8Array<u32> poiIDs = this->getPolygonPointIDs ( polygonID );
	assert ( polylocalID < poiIDs.size () );
	return poiIDs[polylocalID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ŋ��L���Ă���|���S���Q�ɑ΂��āA���i�����j�̃|���S��ID���擾����
//!		@warning �����̃|���S���A�C�����h��1�̒��_�����L���Ă���A�X�Ɏ��̃|���S�����ʂ̃A�C�����h���w���ꍇ�͍����Ƃ͌���Ȃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getNextPolygonID ( u32 vertexID )const{
	assert ( vertexID < this->vertexCount () );
	u32 nextVerID = this->getNextPointVertexID ( vertexID );
	assert ( nextVerID < this->vertexCount () );
	return this->getVertexPolygonID( nextVerID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ŋ��L���Ă���|���S���Q�ɑ΂��āA1�O�i�E���j�̃|���S��ID���擾����
//!		@warning �����̃|���S���A�C�����h��1�̒��_�����L���Ă���A�X��1�O�̃|���S�����ʂ̃A�C�����h���w���ꍇ�͉E���Ƃ͌���Ȃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPreviousPolygonID ( u32 vertexID )const{
	assert ( vertexID < this->vertexCount () );
	u32 prevVerID = this->getPreviousPointVertexID ( vertexID );
	assert ( prevVerID < this->vertexCount () );
	return this->getVertexPolygonID ( prevVerID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S���Ɋ܂܂��O�p�`���b�V���̐���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 PolygonMeshT<T>::triangleCount ( u32 polygonID )const{
	assert ( polygonID < this->polygonCount () );
	return u8( this->getPolygonSize ( polygonID ) - 2 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �O�p�`���b�V�����\�����钸�_ID�i�R�j���擾����
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
//!	    �w��|���S�����̎w�蒸�_���g�p����|���S�����O�p�`���b�V����ID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L4Array<u8> PolygonMeshT<T>::getTriangleIDs ( u32 polygonID, u32 pointID )const{
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );

	L4Array<u8> result;
	u32 poiVerLID = this->getLocalVertexID ( polygonID, pointID ); // �|���S�����̃��[�J���o�[�e�b�N�XID���擾
	u8 triSize = this->triangleCount ( polygonID );
	if ( triSize == 1 ){
		// �|���S�����O�p�`�̏ꍇ
		result.push ( u8 ( 0 ) );
	} else if ( poiVerLID == 0 ){ 
		// �|���S�����̍ŏ��̃o�[�e�b�N�X�̏ꍇ�͑S�Ă̎O�p�|���S��ID��Ԃ�
		result.resize ( triSize );
		for ( u8 i = 0, n = triSize; i < n; ++i ){ result[i] = i; }
	} else if ( poiVerLID == 1 ){ 
		// �P�ڂ̃o�[�e�b�N�X�̏ꍇ�͍ŏ��̎O�p�|���S��ID��Ԃ�
		result.push ( u8 ( 0 ) );
	} else if ( poiVerLID == u8(this->getPolygonSize ( polygonID ) - 1) ){ 
		// �Ō�̃o�[�e�b�N�X�̏ꍇ�͍Ō�̎O�p�|���S��ID��Ԃ�
		result.push ( triSize - u8(1) );
	} else{ 
		// ��L�ȊO�@�o�[�e�b�N�X�����L����ׂ荇���O�p�`�|���S����ID��Ԃ�
		result.push ( poiVerLID - u8 ( 1 ) );
		result.push ( poiVerLID - u8 ( 2 ) );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w��o�[�e�b�N�X���g�p����|���S�����O�p�`���b�V����ID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L4Array<u8> PolygonMeshT<T>::getTriangleIDs ( u32 vertexID )const{
	assert ( vertexID < this->vertexCount () );

	u32 polygonID = this->getVertexPolygonID ( vertexID );
	u32 pointID = this->getPointID ( vertexID );
	return this->getTriangleID ( polygonID, pointID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S������pointID���n�_�Ƃ���n�[�t�G�b�W���{�[�_�[ (���E�G�b�W) �̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::isPolygonBorder ( u32 polygonID, u32 pointID )const{
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );
	u32 verID = this->getVertexID ( polygonID, pointID );
	return this->isPolygonBorder ( verID );
}