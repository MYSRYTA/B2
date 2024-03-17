//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::pointCount () const{
	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	return poiPosAttr.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ʒu��S�Đݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setAllPointPositions ( const std::vector<Vec3T<T>> &points ) {
	assert ( this->pointCount() == points.size() );
	Vec3AttributeT<T> &poiPosAttr = this->getPointPositionAttribute ();
	poiPosAttr.setValues ( points );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ʒu��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setPointPosition ( u32 pointID, const Vec3T<T> &position ) {
	assert ( pointID < this->pointCount () );
	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	poiPosAttr [pointID] = position;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ʒu��S�Ď擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec3T<T>> PolygonMeshT<T>::getAllPointPositions ( )const {
	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	return poiPosAttr.getValues ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ʒu���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> PolygonMeshT<T>::getPointPosition ( u32 pointID ) const {
	assert ( pointID < this->pointCount () );
	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	return poiPosAttr.getValue( pointID ) ;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X�ō\������钸�_��ID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPointID ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	return pointIDAttr [vertexID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S�����\�����钸�_ID�Q���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L8Array<u32> PolygonMeshT<T>::getPolygonPointIDs ( u32 polygonID )const {
	assert ( polygonID < this->polygonCount () );
	U32AttributeT<T>& polySizeAttr = this->getU32Attribute ( "polygonSizes" );
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	L8Array<u32> result(0);
	for ( u32 k = polySizeAttr [polygonID]; k<polySizeAttr [polygonID+1]; k++ ) {
		result.push ( pointIDAttr [k] );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �אڂ���S�Ă̒��_ID��Ԃ� (�ڑ�����|���S�����̗אڂ��Ȃ����_���܂߂�ꍇ�� includeAllPolygonPoints=true )
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16Array<u32> PolygonMeshT<T>::getPointSurroundingPoints ( u32 pointID, Bool includeAllPolygonPoints )const {
	assert ( pointID < this->pointCount () );
	L16Array<u32> polyIDs = this->getPointPolygonIDs ( pointID ); // pointID�̃|�C���g�ō\������|���S��ID�Q���擾
	std::set<u32> pointIDs; // set<>�ɂ��邱�ƂŁA�e�|���S���̃|�C���gID���i�[���Ă����ۂɏd������|�C���gID���Ȃ�
	for ( u32 i = 0; i<polyIDs.size (); i++ ) {
		if ( includeAllPolygonPoints ){
			L8Array<u32> poiIDs = this->getPolygonPointIDs ( polyIDs[i] );
			for ( u32 k = 0; k < poiIDs.size (); k++ ){
				pointIDs.insert ( poiIDs[k] );
			}
		} else{
			pointIDs.insert ( this->getNextPolygonPointID( polyIDs[i], pointID ) );
			pointIDs.insert ( this->getPreviousPolygonPointID( polyIDs[i], pointID ) );
		}
	}

	// set<>�̒l��vector<>�֕ϊ�
	L16Array<u32> result ( u32(pointIDs.size ()) );
	auto itr = pointIDs.begin ();
	for ( u32 i = 0; i < pointIDs.size (); i++ ) {
		result [i] = u32 ( *itr );
		itr++;
	}
	
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S�����̎�(�����)�̃|�C���gID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getNextPolygonPointID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );

	u32 currentLocalID = 0;
	L8Array<u32>& poiIDList = this->getPolygonPointIDs(polygonID);
	for (u32 i = 0; i < poiIDList.size(); i++) {
		if (pointID == poiIDList[i]) {
			currentLocalID = i;
			break;
		}
	}
	u32 nextLocalID = currentLocalID + 1;

	if (nextLocalID == poiIDList.size()) {
		return poiIDList[0];
	} else {
		return poiIDList[nextLocalID];
	}
	/*u32 vertexID = this->getVertexID ( polygonID, pointID );
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	U32AttributeT<T>& polygonSizeAttr = this->getU32Attribute ( "polygonSizes" );
	u32 remainder;
	u32 polyID = Burgess2MathFn::BinarySearchTree( polygonSizeAttr.getValues (), vertexID, remainder );
	if ( this->getPolygonSize ( polygonID ) - 1 == remainder ) { // vertexID���|���S���̍ŏI�o�[�e�b�N�X�������ꍇ
		return pointIDAttr [polygonSizeAttr [polygonID]];
	} else {
		return pointIDAttr [vertexID+1];
	}*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S����1�O(�E���)�̃|�C���gID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPreviousPolygonPointID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );
	
	u32 currentLocalID = 0;
	L8Array<u32>& poiIDList = this->getPolygonPointIDs(polygonID);
	for (u32 i = 0; i < poiIDList.size(); i++) {
		if (pointID == poiIDList[i]) {
			currentLocalID = i;
			break;
		}
	}

	if (currentLocalID == 0) {
		return poiIDList.back();
	} else {
		return poiIDList[currentLocalID-1];
	}

	/*u32 vertexID = this->getVertexID ( polygonID, pointID );
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	U32AttributeT<T>& polygonSizeAttr = this->getU32Attribute ( "polygonSizes" );
	u32 remainder;
	u32 polyID = Burgess2MathFn::BinarySearchTree( polygonSizeAttr.getValues (), vertexID, remainder );
	if ( 0 == remainder ) { // vertexID���|���S���̍ŏ��̃o�[�e�b�N�X�������ꍇ
		return pointIDAttr [polygonSizeAttr[polyID + 1] -1];
	} else {
		return pointIDAttr [vertexID-1];
	}*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|�C���g�̃��[�J���o�[�e�b�N�XID�̃o�[�e�b�N�X�ō\�����Ă���|���S��ID�ƃ|���S�����̃��[�J���o�[�e�b�N�XID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::getPointPolygonAndIndex ( u32 pointID, LocalIndex poiLocalID, u32 &polygonID, LocalIndex &polyLocalID )const{
	assert ( pointID < this->pointCount () );
	L16Array<u32> poiIDs = this->getSharedVertexIDs ( pointID );
	assert ( poiLocalID.id < poiIDs.size () );
	u32 vertexID = poiIDs[poiLocalID.id];
	polygonID = this->getVertexPolygonID ( vertexID );
	L8Array<u32> poiIDs2 = this->getPolygonPointIDs ( polygonID );
	for(u32 i=0; i<poiIDs2; i++ ){
		if( pointID == poiIDs2[i] ){
			polyLocalID.id = i;
			return;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ŋ��L���Ă���o�[�e�b�N�X�Q�ɑ΂��āA���i�����j�̃o�[�e�b�N�XID���擾����
//!		@warning �����̃|���S���A�C�����h��1�̒��_�����L���Ă���A�X�Ɏ��̃o�[�e�b�N�X���ʂ̃A�C�����h���w���ꍇ�͍����Ƃ͌���Ȃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getNextPointVertexID ( u32 vertexID )const{
	assert ( vertexID < this->vertexCount () );
	u32 poiID = this->getPointID ( vertexID );
	L16Array<u32> poiVerIDs = this->getSharedVertexIDs ( poiID );
	if ( poiVerIDs.size () == 0 ){ return BURGESS2::U32_INFINITE; }

	LocalIndex lID;
	for(u32 i =0; i<poiVerIDs.size(); ++i ){
		if( poiVerIDs[i] == vertexID ){
			lID = i;
			break;
		}
	}
	return lID.id == poiVerIDs.size ()-1 ? poiVerIDs[0] : poiVerIDs[lID.id + 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ŋ��L���Ă���o�[�e�b�N�X�Q�ɑ΂��āA�P�O�i�E���j�̃o�[�e�b�N�XID���擾����
//!		@warning �����̃|���S���A�C�����h��1�̒��_�����L���Ă���A�X��1�O�̃o�[�e�b�N�X���ʂ̃A�C�����h���w���ꍇ�͉E���Ƃ͌���Ȃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPreviousPointVertexID ( u32 vertexID )const{
	assert ( vertexID < this->vertexCount () );
	u32 poiID = this->getPointID ( vertexID );
	L16Array<u32> poiVerIDs = this->getSharedVertexIDs ( poiID );
	if ( poiVerIDs.size () == 0 ){ return BURGESS2::U32_INFINITE; }

	LocalIndex lID;
	for ( u32 i = 0; i<poiVerIDs.size (); ++i ){
		if ( poiVerIDs[i] == vertexID ){
			lID = i;
			break;
		}
	}
	return lID.id == 0 ? poiVerIDs[poiVerIDs.size()-1] : poiVerIDs[lID.id - 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|�C���g���|���S���{�[�_�[�ł����true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::isBorderPoint ( u32 pointID )const{	
	u32 polySize = this->getPointPolygonCount ( pointID );
	if ( polySize <= 1 ){ return true; }

	// �|�C���g�ō\�����Ă���G�b�W���{�[�_�[�G�b�W���ǂ��������ԂɊm�F����
	L16Array<u32> sVerIDs = this->getSharedVertexIDs( pointID );
	for ( u32 i = 0; i < sVerIDs.size(); i++ ) {
		// �|�C���g���X�^�[�g�ɂ���G�b�W���`�F�b�N
		u32 edgeID = sVerIDs[i]; // vertex ID = half-edge ID
		if ( this->isPolygonBorder( edgeID ) ) {
			return true;
		}
		// �|�C���g���G���h�ɂ���G�b�W���`�F�b�N
		u32 pEdgeID = this->getPreviousPolygonVertexID( sVerIDs[i] );
		if ( this->isPolygonBorder( pEdgeID ) ) {
			return true;
		}
	}
	return false;
}