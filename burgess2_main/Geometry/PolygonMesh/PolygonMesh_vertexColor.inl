//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X�J���[�}�b�v�̐���Ԃ� (�A�g���r���[�g�l���L���ȃo�[�e�b�N�X�J���[�}�b�v�̂�)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 PolygonMeshT<T>::vertexColorMapCount () const{
	std::vector<String> s = this->getAllVertexColorNames ();
	return u8(s.size ());
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   �o�[�e�b�N�X�J���[�}�b�v��ǉ����� (�����̃}�b�v�����ɑ��݂���ꍇ�̓X���[)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>& PolygonMeshT<T>::addVertexColorMap ( const String &colorMapName, Bool hasAlpha ) {
	Vec4AttributeT<T> &result = this->vertexColors.getOrCreatVec4Attribute ( colorMapName, this->vertexCount() );
	result.v[0] = hasAlpha ? 1 : 0; // Attribute.v[0]: 0=�A���t�@�`�����l�������A1=�L��
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
//!	    �o�[�e�b�N�X�J���[�̒l�ƒl�̔z�񐔂�ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::resetAllVertexColors ( const String &colorMapName, const std::vector<Vec4T<T>> &color ){
	assert ( this->hasVertexColorMap ( colorMapName ) );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	colorAttr.resetValues ( color );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X�J���[�̒l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setVertexColor ( const String &colorMapName, u32 vertexID, const Vec4T<T> &color ) {
	assert ( this->hasVertexColorMap ( colorMapName ) );
	assert ( vertexID < this->vertexCount () );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	colorAttr.setValue ( vertexID, color );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X�J���[�̑S�Ă̒l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec4T<T>> PolygonMeshT<T>::getAllVertexColor ( const String &colorMapName ) const{
	assert ( this->hasVertexColorMap ( colorMapName ) );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	return colorAttr.getValues ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X�J���[�̒l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4T<T> PolygonMeshT<T>::getVertexColor ( const String &colorMapName, u32 vertexID )const {
	assert ( this->hasVertexColorMap ( colorMapName ) );
	assert ( vertexID < this->vertexCount () );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	return colorAttr.getValue ( vertexID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�Ẵo�[�e�b�N�X�J���[�}�b�v�̖��O��Ԃ� (�A�g���r���[�g�l���L���ȃ}�b�v�̂�)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<String> PolygonMeshT<T>::getAllVertexColorNames ()const {
	return this->vertexColors.getAllAttributeNames ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X�J���[�}�b�v���폜����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deleteVertexColorMap ( const String &colorMapName ) {
	assert ( this->hasVertexColorMap ( colorMapName ) );
	this->vertexColors->deleteAttr ( colorMapName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�[�e�b�N�X�J���[�}�b�v�̑S�Ă̒l��Vec4T<T>(0.0, 0.0, 0.0, 0.0)�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::clearVertexColorValues ( const String &colorMapName ) {
	assert ( this->hasVertexColorMap ( colorMapName ) );
	this->vertexColors->clear ( colorMapName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O�̃o�[�e�b�N�X�J���[�}�b�v�����݂���ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasVertexColorMap ( const String &colorMapName ) const {
	return this->vertexColors.has ( colorMapName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O�̃o�[�e�b�N�X�J���[�}�b�v�ɃA���t�@�`�����l�������݂���ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasAlpha ( const String &colorMapName ) const{
	assert ( this->hasVertexColorMap ( colorMapName ) );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	return colorAttr.v[0] == 1; // Attribute.v[0]: 0=�A���t�@�`�����l�������A1=�L��
}