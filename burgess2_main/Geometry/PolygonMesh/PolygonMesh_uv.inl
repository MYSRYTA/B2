//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV�}�b�v�̐���Ԃ� (�A�g���r���[�g�l���L����UV�}�b�v�̂�)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 PolygonMeshT<T>::uvCount () const{
	std::vector<String> s = this->getAllUVNames ();
	return u8(s.size ());
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV�}�b�v�̒��_����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::uvPointCount ( const String &_uvName ) const {
	if ( this->hasUVMap ( _uvName )  == false ) { return 0; }
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( _uvName );
	return uvAttr.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV�}�b�v��ǉ����� (�����̃}�b�v�����ɑ��݂���ꍇ�̓X���[)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>& PolygonMeshT<T>::addUVMap ( const String &_uvName ) {
	/*"map1"��"map1PackedUVVertexIDs"�łP�p�b�P�[�W*/
	this->uvs.getOrCreateU32Attribute ( _uvName + "packedUVVertexIDs" );
	return this->uvs.getOrCreatVec2Attribute ( _uvName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV�}�b�v�̒l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
void PolygonMeshT<T>::setAllUVValues ( const String &uvName, const std::vector<Vec2T<T>> &uv ) {
	assert ( this->hasUVMap ( uvName ) );
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( uvName );
	uvAttr.setValues ( uv );
}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV�}�b�v�̒l�ƒl�̔z�񐔂�ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::resetAllUVValues ( const String &uvName, const std::vector<Vec2T<T>> &uv, const std::vector<u32> &packedUVPointIDs ) {
	assert ( this->hasUVMap ( uvName ) );
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( uvName );
	uvAttr.resetValues ( uv );
	U32AttributeT<T> &uvPoiIDsAttr = this->uvs.getU32Attribute ( uvName + "packedUVVertexIDs" );
	uvPoiIDsAttr.resetValues ( packedUVPointIDs );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV�}�b�v�̒l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setUVValue ( const String &uvName, u32 vertexID, const Vec2T<T> &uv ) {
	assert ( this->hasUVMap ( uvName ) );
	assert ( vertexID < this->vertexCount () );
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( uvName );
	uvAttr.setValue ( vertexID, uv );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV�}�b�v�̒l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec2T<T>> PolygonMeshT<T>::getAllUVValues ( const String &uvName ) const{
	assert ( this->hasUVMap ( uvName ) );
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( uvName );
	return uvAttr.getValues ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV�l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2T<T> PolygonMeshT<T>::getUVValue ( const String &uvName, u32 vertexID )const {
	assert ( this->hasUVMap ( uvName ) );
	assert ( vertexID < this->vertexCount () );
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( uvName );
	return uvAttr.getValue ( vertexID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�Ă�UV�}�b�v�̖��O��Ԃ� (�A�g���r���[�g�l���L����UV�}�b�v�̂�)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<String> PolygonMeshT<T>::getAllUVNames ()const {
	std::vector<String> result;
	std::vector<String> names = this->uvs.getAllAttributeNames ();
	for ( u32 i = 0; i<names.size (); i++ ) {
		if ( this->uvs.getType ( names [i] ) == Burgess2AttrType::Vec2Attr ) {/* "***packedUVVertexIDs"�̃A�g���r���[�g�͏Ȃ� */
			result.push_back ( names [i] );
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV�}�b�v���폜����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deleteUV ( const String &uvName ) {
	assert ( this->hasUVMap ( uvName ) );
	this->uvs->deleteAttr ( uvName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV�}�b�v�̒l��0.0�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::clearUV ( const String &uvName ) {
	assert ( this->hasUVMap ( uvName ) );
	this->uvs->clear ( uvName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��UV�}�b�v�����݂���ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasUVMap ( const String &uvName ) const {
	return this->uvs.has ( uvName );
}