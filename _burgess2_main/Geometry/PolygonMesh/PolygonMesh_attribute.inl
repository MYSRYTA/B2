//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のBoolAttributeが無ければ新規作成し、参照を返す
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
//!	    指定した名前のF32Attributeが無ければ新規作成し、参照を返す
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
//!	    指定した名前のF64Attributeが無ければ新規作成し、参照を返す
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
//!	    指定した名前のU32Attributeが無ければ新規作成し、参照を返す
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
//!	    指定した名前のL16ArrayU32Attributeが無ければ新規作成し、参照を返す
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
//!	    指定した名前のS32Attributeが無ければ新規作成し、参照を返す
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
//!	    指定した名前のVec2Attributeが無ければ新規作成し、参照を返す
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
//!	    指定した名前のVec3Attributeが無ければ新規作成し、参照を返す
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
//!	    指定した名前のVec4Attributeが無ければ新規作成し、参照を返す
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
//!	    指定した名前のSkinningAttributeが無ければ新規作成し、参照を返す
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
//!	    頂点に隣接する頂点IDリストのアトリビュートが無ければ新規作成し、参照を返す　（アトリビュート名は"surroundingPointIDs"）
//!	    @brief	頂点に隣接する頂点IDリストのアトリビュートが無ければ新規作成し、参照を返す　（アトリビュート名は"surroundingPointIDs"）<br>
//!		@ref getPointSurroundingPoints "getPointSurroundingPoints()"の結果をアトリビュートに格納する
//!		@param[in] includeAllPolygonPoints @ref getPointSurroundingPoints "getPointSurroundingPoints()"を参照
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
//!	    頂点位置のアトリビュート(非共有頂点の情報も含まれる)を参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& PolygonMeshT<T>::getPointPositionAttribute () const {
	assert ( this->attr.has ( "pointPositions" ) );
	Vec3AttributeT<T>& v3Attr = this->attr.getVec3Attribute ( "pointPositions" );
	return v3Attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    法線のアトリビュートを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& PolygonMeshT<T>::getNormalAttribute () const {
	assert ( this->attr.has ( "normals" ) );
	Vec3AttributeT<T>& v3Attr = this->attr.getVec3Attribute ( "normals" );
	return v3Attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のUVマップを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>& PolygonMeshT<T>::getUVAttribute ( const String &_uvName ) const {
	assert ( this->uvs.has ( _uvName ) );
	Vec2AttributeT<T>& uvAttr = this->uvs.getVec2Attribute ( _uvName );
	return uvAttr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のUVPointIDsを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
U32AttributeT<T>& PolygonMeshT<T>::getPackedUVPointIDsAttribute ( const String &_uvName ) const {
	assert ( this->uvs.has ( _uvName + "packedUVVertexIDs" ) );
	U32AttributeT<T>& uvAttr = this->uvs.getU32Attribute ( _uvName + "packedUVVertexIDs" );
	return uvAttr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のウェイトマップを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>& PolygonMeshT<T>::getWeightMapAttribute ( const String &_weightMapName ) const{
	assert ( this->hasWeightMap ( _weightMapName ) );
	F32AttributeT<T>& wMapAttr = this->weightMaps.getF32Attribute ( _weightMapName );
	return wMapAttr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のバーテックスカラーマップを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>& PolygonMeshT<T>::getVertexColorAttribute ( const String &_vertexColorMapName ) const {
	assert ( this->vertexColors.has ( _vertexColorMapName ) );
	Vec4AttributeT<T>& colorAttr = this->vertexColors.getVec4Attribute ( _vertexColorMapName );
	return colorAttr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のBoolAttributeを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BoolAttributeT<T>& PolygonMeshT<T>::getBoolAttribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	BoolAttributeT<T>& attr = this->attr.getBoolAttribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のF32Attributeを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>& PolygonMeshT<T>::getF32Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	F32AttributeT<T>& attr = this->attr.getF32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のF64Attributeを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F64AttributeT<T>& PolygonMeshT<T>::getF64Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	F64AttributeT<T>& attr = this->attr.getF64Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のU32Attributeを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
U32AttributeT<T>& PolygonMeshT<T>::getU32Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	U32AttributeT<T>& attr = this->attr.getU32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のS32Attributeを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
S32AttributeT<T>& PolygonMeshT<T>::getS32Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	S32AttributeT<T>& attr = this->attr.getS32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のL16ArrayU32AttributeTを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16ArrayU32AttributeT<T>& PolygonMeshT<T>::getL16ArrayU32Attribute ( const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	L16ArrayU32AttributeT<T>& attr = this->attr.getL16ArrayU32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のVec2Attributeを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>& PolygonMeshT<T>::getVec2Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	Vec2AttributeT<T>& attr = this->attr.getVec2Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のVec3Attributeを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& PolygonMeshT<T>::getVec3Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	Vec3AttributeT<T>& attr = this->attr.getVec3Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のVec4Attributeを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>& PolygonMeshT<T>::getVec4Attribute (  const String &_name  ) const {
	assert ( this->attr.has ( _name ) );
	Vec4AttributeT<T>& attr = this->attr.getVec4Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のSkinningAttributeTを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
SkinningAttributeT<T>& PolygonMeshT<T>::getSkinningAttribute ( const String &_name ) const{
	assert ( this->attr.has ( _name ) );
	SkinningAttributeT<T>& attr = this->attr.getSkinningAttribute( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点に隣接する頂点IDリストのアトリビュートを参照で返す
//!	    @brief	頂点に隣接する頂点IDリストのアトリビュートを参照で返す<br>
//!		アトリビュートの生成は @ref getOrCreateSurroundingPointIDsAttribute "getOrCreateSurroundingPointIDsAttribute()" 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16ArrayU32AttributeT<T>& PolygonMeshT<T>::getSurroundingPointIDsAttribute () const{
	assert ( this->attr.has ( "surroundingPointIDs" ) );
	L16ArrayU32AttributeT<T>& attr = this->attr.getL16ArrayU32Attribute ( "surroundingPointIDs" );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点に隣接する頂点IDリストのアトリビュートを更新する
//!	    @brief	頂点に隣接する頂点IDリストのアトリビュートを更新する<br>
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
//!	    頂点に隣接する頂点IDリストのアトリビュートを更新する(既にアトリビュートが存在する場合は上書き)
//!	    @brief	頂点に隣接する頂点IDリストのアトリビュートを更新する(既にアトリビュートが存在する場合は上書き)
//!		@param[in] includeAllPolygonPoints @ref getPointSurroundingPoints "getPointSurroundingPoints()"を参照
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
//!	    指定した名前のAttributeが存在する場合は削除する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deleteAttribute(const String& _name) {
	this->attr.deleteAttr(_name);
	this->uvs.deleteAttr ( _name );
	this->weightMaps.deleteAttr ( _name );
	this->vertexColors.deleteAttr ( _name );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のAttributeが存在する場合はアトリビュート値を全て0.0にする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::clearAttribute(const String& _name) {
	this->attr.clear(_name);
	this->uvs.clear ( _name );
	this->weightMaps.clear ( _name );
	this->vertexColors.clear ( _name );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のAttributeが存在する場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasAttribute ( const String &_name ) const {
	return this->attr.has ( _name ) || this->uvs.has ( _name ) || this->weightMaps.has ( _name ) || this->vertexColors.has ( _name );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のSkinningAttributeが存在する場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasSkinningAttribute ( const String &_name ) const{
	return this->attr.has ( _name );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のAttributeのデータタイプを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 PolygonMeshT<T>::getAttrType ( const String& _name ) const {
	u8 id;
	if (this->attr.has ( _name, id )) { 
		return this->attr.getType ( id );
	}
}