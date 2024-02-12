//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �E�F�C�g�}�b�v�̐���Ԃ� (�A�g���r���[�g�l���L���ȃE�F�C�g�}�b�v�̂�)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 PolygonMeshT<T>::weightMapCount () const{
	std::vector<String> s = this->getAllWeightMapNames ();
	return u8(s.size ());
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   �E�F�C�g�}�b�v��ǉ����� (�����̃}�b�v�����ɑ��݂���ꍇ�̓X���[)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>& PolygonMeshT<T>::addWeightMap ( const String &weightMapName ) {
	F32AttributeT<T> &result = this->weightMaps.getOrCreateF32Attribute ( weightMapName, this->vertexCount() );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X�J���[�̑S�Ă̒l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
void PolygonMeshT<T>::setAllVertexColor ( const String &colorMapName, const std::vector<Vec4T<T>> &color ) {
	assert ( this->hasVertexColorMap ( colorMapName ) );
	assert ( this->vertexCount () == color.size () );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	colorAttr.setValues ( color );
}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �E�F�C�g�}�b�v�̒l�ƒl�̔z�񐔂�ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::resetAllWeightMapValues ( const String &weightMapName, const std::vector<f32> &val ){
	assert ( this->hasWeightMap ( weightMapName ) );
	F32AttributeT<T> &wMapAttr = this->weightMaps.getF32Attribute ( weightMapName );
	wMapAttr.resetValues ( val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �E�F�C�g�}�b�v�̒l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setWeightMapValue ( const String &weightMapName, u32 vertexID, f32 val ) {
	assert ( this->hasWeightMap ( weightMapName ) );
	assert ( vertexID < this->vertexCount () );
	F32AttributeT<T> &wMapAttr = this->weightMaps.getF32Attribute ( weightMapName );
	wMapAttr.setValue ( vertexID, val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �E�F�C�g�}�b�v�̑S�Ă̒l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<f32> PolygonMeshT<T>::getAllWeightMapValues ( const String &weightMapName ) const{
	assert ( this->hasWeightMap ( weightMapName ) );
	F32AttributeT<T> &wMapAttr = this->weightMaps.getF32Attribute ( weightMapName );
	return wMapAttr.getValues ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �E�F�C�g�}�b�v�̒l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
f32 PolygonMeshT<T>::getWeightMapValue ( const String &weightMapName, u32 vertexID )const {
	assert ( this->hasWeightMap ( weightMapName ) );
	assert ( vertexID < this->vertexCount () );
	F32AttributeT<T> &wMapAttr = this->weightMaps.getF32Attribute ( weightMapName );
	return wMapAttr.getValue ( vertexID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�ẴE�F�C�g�}�b�v�̖��O��Ԃ� (�A�g���r���[�g�l���L���ȃ}�b�v�̂�)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<String> PolygonMeshT<T>::getAllWeightMapNames ()const {
	return this->weightMaps.getAllAttributeNames ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �E�F�C�g�}�b�v���폜����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deleteWeightMap ( const String &weightMapName ) {
	assert ( this->hasWeightMap ( weightMapName ) );
	this->weightMaps->deleteAttr ( weightMapName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �E�F�C�g�}�b�v�̑S�Ă̒l��0.0f�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::clearWeightMapValues ( const String &weightMapName ) {
	assert ( this->hasWeightMap ( weightMapName ) );
	this->weightMaps->clear ( weightMapName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O�̃E�F�C�g�}�b�v�����݂���ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasWeightMap ( const String &weightMapName ) const {
	return this->weightMaps.has ( weightMapName );
}
