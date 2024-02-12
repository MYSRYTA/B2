//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��BoolAttribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BoolAttributeT<T>& PolygonMeshT<T>::getOrCreateBoolAttribute (  const String &_name  ) {
	if ( this->attr.has ( _name ) ) {
		BoolAttributeT<T>& attr = this->getBoolAttribute ( _name );
		return attr;
	} else {
		BoolAttributeT<T>& attr = this->attr.getOrCreateBoolAttribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��F32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>& PolygonMeshT<T>::getOrCreateF32Attribute (  const String &_name  ) {
	if ( this->attr.has ( _name ) ) {
		F32AttributeT<T>& attr = this->getF32Attribute ( _name );
		return attr;
	} else {
		F32AttributeT<T>& attr = this->attr.getOrCreateF32Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��F64Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F64AttributeT<T>& PolygonMeshT<T>::getOrCreateF64Attribute ( const String &_name ){
	if ( this->attr.has ( _name ) ){
		F64AttributeT<T>& attr = this->getF64Attribute ( _name );
		return attr;
	} else{
		F64AttributeT<T>& attr = this->attr.getOrCreateF64Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��U32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
U32AttributeT<T>& PolygonMeshT<T>::getOrCreateU32Attribute (  const String &_name  ) {
	if ( this->attr.has ( _name ) ) {
		U32AttributeT<T>& attr = this->getU32Attribute ( _name );
		return attr;
	} else {
		U32AttributeT<T>& attr = this->attr.getOrCreateU32Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��L16ArrayU32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16ArrayU32AttributeT<T>& PolygonMeshT<T>::getOrCreateL16ArrayU32Attribute ( const String &_name  ) {
	if ( this->attr.has ( _name ) ) {
		L16ArrayU32AttributeT<T>& attr = this->getL16ArrayU32Attribute ( _name );
		return attr;
	} else {
		L16ArrayU32AttributeT<T>& attr = this->attr.getOrCreateL16ArrayU32Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��S32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
S32AttributeT<T>& PolygonMeshT<T>::getOrCreateS32Attribute (  const String &_name  ) {
	if ( this->attr.has ( _name ) ) {
		S32AttributeT<T>& attr = this->getS32Attribute ( _name );
		return attr;
	} else {
		S32AttributeT<T>& attr = this->attr.getOrCreateS32Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec2Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>& PolygonMeshT<T>::getOrCreatVec2Attribute (  const String &_name  ) {
	if ( this->attr.has ( _name ) ) {
		Vec2AttributeT<T>& attr = this->getVec2Attribute ( _name );
		return attr;
	} else {
		Vec2AttributeT<T>& attr = this->attr.getOrCreatVec2Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec3Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& PolygonMeshT<T>::getOrCreatVec3Attribute (  const String &_name  ) {
	if ( this->attr.has ( _name ) ) {
		Vec3AttributeT<T>& attr = this->getVec3Attribute ( _name );
		return attr;
	} else {
		Vec3AttributeT<T>& attr = this->attr.getOrCreatVec3Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec4Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>& PolygonMeshT<T>::getOrCreatVec4Attribute (  const String &_name  ) {
	if ( this->attr.has ( _name ) ) {
		Vec4AttributeT<T>& attr = this->getVec4Attribute ( _name );
		return attr;
	} else {
		Vec4AttributeT<T>& attr = this->attr.getOrCreatVec4Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��SkinningAttribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
SkinningAttributeT<T>& PolygonMeshT<T>::getOrCreateSkinningAttribute ( const String &_name ){
	if ( this->attr.has ( _name ) ){
		SkinningAttributeT<T>& attr = this->getSkinningAttribute ( _name );
		return attr;
	} else{
		SkinningAttributeT<T>& attr = this->attr.getOrCreateSkinningAttribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ɗאڂ��钸�_ID���X�g�̃A�g���r���[�g��������ΐV�K�쐬���A�Q�Ƃ�Ԃ��@�i�A�g���r���[�g����"surroundingPointIDs"�j
//!	    @brief	���_�ɗאڂ��钸�_ID���X�g�̃A�g���r���[�g��������ΐV�K�쐬���A�Q�Ƃ�Ԃ��@�i�A�g���r���[�g����"surroundingPointIDs"�j<br>
//!		@ref getPointSurroundingPoints "getPointSurroundingPoints()"�̌��ʂ��A�g���r���[�g�Ɋi�[����
//!		@param[in] includeAllPolygonPoints @ref getPointSurroundingPoints "getPointSurroundingPoints()"���Q��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16ArrayU32AttributeT<T>& PolygonMeshT<T>::getOrCreateSurroundingPointIDsAttribute ( Bool includeAllPolygonPoints ){
	if ( this->attr.has ( "surroundingPointIDs" ) ){
		L16ArrayU32AttributeT<T>& attr = this->getSurroundingPointIDsAttribute ();
		return attr;
	} else{
		u32 poiCnt = this->pointCount ();
		L16ArrayU32AttributeT<T>& attr = this->getOrCreateL16ArrayU32Attribute ( "surroundingPointIDs" );
		attr.resize ( poiCnt );
		#pragma omp parallel for
		for ( s32 i = 0; i < poiCnt; ++i ){
			attr[i] = this->getPointSurroundingPoints ( i, includeAllPolygonPoints );
		}
		attr.v[0] = includeAllPolygonPoints ? 1 : 0;
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ʒu�̃A�g���r���[�g(�񋤗L���_�̏����܂܂��)���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& PolygonMeshT<T>::getPointPositionAttribute () const {
	assert ( this->attr.has ( "pointPositions" ) );
	Vec3AttributeT<T>& v3Attr = this->attr.getVec3Attribute ( "pointPositions" );
	return v3Attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �@���̃A�g���r���[�g���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& PolygonMeshT<T>::getNormalAttribute () const {
	assert ( this->attr.has ( "normals" ) );
	Vec3AttributeT<T>& v3Attr = this->attr.getVec3Attribute ( "normals" );
	return v3Attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��UV�}�b�v���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>& PolygonMeshT<T>::getUVAttribute ( const String &_uvName ) const {
	assert ( this->uvs.has ( _uvName ) );
	Vec2AttributeT<T>& uvAttr = this->uvs.getVec2Attribute ( _uvName );
	return uvAttr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��UVPointIDs���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
U32AttributeT<T>& PolygonMeshT<T>::getPackedUVPointIDsAttribute ( const String &_uvName ) const {
	assert ( this->uvs.has ( _uvName + "packedUVVertexIDs" ) );
	U32AttributeT<T>& uvAttr = this->uvs.getU32Attribute ( _uvName + "packedUVVertexIDs" );
	return uvAttr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O�̃E�F�C�g�}�b�v���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>& PolygonMeshT<T>::getWeightMapAttribute ( const String &_weightMapName ) const{
	assert ( this->hasWeightMap ( _weightMapName ) );
	F32AttributeT<T>& wMapAttr = this->weightMaps.getF32Attribute ( _weightMapName );
	return wMapAttr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O�̃o�[�e�b�N�X�J���[�}�b�v���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>& PolygonMeshT<T>::getVertexColorAttribute ( const String &_vertexColorMapName ) const {
	assert ( this->vertexColors.has ( _vertexColorMapName ) );
	Vec4AttributeT<T>& colorAttr = this->vertexColors.getVec4Attribute ( _vertexColorMapName );
	return colorAttr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��BoolAttribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BoolAttributeT<T>& PolygonMeshT<T>::getBoolAttribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	BoolAttributeT<T>& attr = this->attr.getBoolAttribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��F32Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>& PolygonMeshT<T>::getF32Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	F32AttributeT<T>& attr = this->attr.getF32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��F64Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F64AttributeT<T>& PolygonMeshT<T>::getF64Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	F64AttributeT<T>& attr = this->attr.getF64Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��U32Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
U32AttributeT<T>& PolygonMeshT<T>::getU32Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	U32AttributeT<T>& attr = this->attr.getU32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��S32Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
S32AttributeT<T>& PolygonMeshT<T>::getS32Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	S32AttributeT<T>& attr = this->attr.getS32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��L16ArrayU32AttributeT���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16ArrayU32AttributeT<T>& PolygonMeshT<T>::getL16ArrayU32Attribute ( const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	L16ArrayU32AttributeT<T>& attr = this->attr.getL16ArrayU32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec2Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>& PolygonMeshT<T>::getVec2Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	Vec2AttributeT<T>& attr = this->attr.getVec2Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec3Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& PolygonMeshT<T>::getVec3Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	Vec3AttributeT<T>& attr = this->attr.getVec3Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec4Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>& PolygonMeshT<T>::getVec4Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	Vec4AttributeT<T>& attr = this->attr.getVec4Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��SkinningAttributeT���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
SkinningAttributeT<T>& PolygonMeshT<T>::getSkinningAttribute ( const String &_name ) const{
	assert ( this->attr.has ( _name ) );
	SkinningAttributeT<T>& attr = this->attr.getSkinningAttribute( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ɗאڂ��钸�_ID���X�g�̃A�g���r���[�g���Q�ƂŕԂ�
//!	    @brief	���_�ɗאڂ��钸�_ID���X�g�̃A�g���r���[�g���Q�ƂŕԂ�<br>
//!		�A�g���r���[�g�̐����� @ref getOrCreateSurroundingPointIDsAttribute "getOrCreateSurroundingPointIDsAttribute()" 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16ArrayU32AttributeT<T>& PolygonMeshT<T>::getSurroundingPointIDsAttribute () const{
	assert ( this->attr.has ( "surroundingPointIDs" ) );
	L16ArrayU32AttributeT<T>& attr = this->attr.getL16ArrayU32Attribute ( "surroundingPointIDs" );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ɗאڂ��钸�_ID���X�g�̃A�g���r���[�g���X�V����
//!	    @brief	���_�ɗאڂ��钸�_ID���X�g�̃A�g���r���[�g���X�V����<br>
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16ArrayU32AttributeT<T>& PolygonMeshT<T>::updateSurroundingPointIDsAttribute () const{
	assert ( this->attr.has ( "surroundingPointIDs" ) );
	L16ArrayU32AttributeT<T>& attr = this->attr.getL16ArrayU32Attribute ( "surroundingPointIDs" );
	s32 poiCnt = s32(this->pointCount());
	Bool includeAllPolygonPoints = attr.v[0] == 1 ? true : false;
	#pragma omp parallel for
	for ( s32 i = 0; i < poiCnt; ++i ){
		attr[i] = this->getPointSurroundingPoints ( i, includeAllPolygonPoints );
	}
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�ɗאڂ��钸�_ID���X�g�̃A�g���r���[�g���X�V����(���ɃA�g���r���[�g�����݂���ꍇ�͏㏑��)
//!	    @brief	���_�ɗאڂ��钸�_ID���X�g�̃A�g���r���[�g���X�V����(���ɃA�g���r���[�g�����݂���ꍇ�͏㏑��)
//!		@param[in] includeAllPolygonPoints @ref getPointSurroundingPoints "getPointSurroundingPoints()"���Q��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16ArrayU32AttributeT<T>& PolygonMeshT<T>::recreateSurroundingPointIDsAttribute(Bool includeAllPolygonPoints) const {
	if (this->attr.has("surroundingPointIDs") == false) {
		return this->getOrCreateSurroundingPointIDsAttribute(includeAllPolygonPoints);
	}
	L16ArrayU32AttributeT<T>& attr = this->attr.getL16ArrayU32Attribute("surroundingPointIDs");
	u32 poiCnt = this->pointCount();
	attr.resize(poiCnt);
	#pragma omp parallel for
	for (s32 i = 0; i < poiCnt; ++i) {
		attr[i] = this->getPointSurroundingPoints(i, includeAllPolygonPoints);
	}
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Attribute�����݂���ꍇ�͍폜����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deleteAttribute(const String& _name) {
	this->attr.deleteAttr(_name);
	this->uvs.deleteAttr ( _name );
	this->weightMaps.deleteAttr ( _name );
	this->vertexColors.deleteAttr ( _name );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Attribute�����݂���ꍇ�̓A�g���r���[�g�l��S��0.0�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::clearAttribute(const String& _name) {
	this->attr.clear(_name);
	this->uvs.clear ( _name );
	this->weightMaps.clear ( _name );
	this->vertexColors.clear ( _name );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Attribute�����݂���ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasAttribute ( const String &_name ) const {
	return this->attr.has ( _name ) || this->uvs.has ( _name ) || this->weightMaps.has ( _name ) || this->vertexColors.has ( _name );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��SkinningAttribute�����݂���ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasSkinningAttribute ( const String &_name ) const{
	return this->attr.has ( _name );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Attribute�̃f�[�^�^�C�v��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 PolygonMeshT<T>::getAttrType ( const String& _name ) const {
	u8 id;
	if (this->attr.has ( _name, id )) { 
		return this->attr.getType ( id );
	}
}