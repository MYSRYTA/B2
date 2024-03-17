//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UVマップの数を返す (アトリビュート値が有効なUVマップのみ)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 PolygonMeshT<T>::uvCount () const{
	std::vector<String> s = this->getAllUVNames ();
	return u8(s.size ());
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UVマップの頂点数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::uvPointCount ( const String &_uvName ) const {
	if ( this->hasUVMap ( _uvName )  == false ) { return 0; }
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( _uvName );
	return uvAttr.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UVマップを追加する (同名のマップが既に存在する場合はスルー)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>& PolygonMeshT<T>::addUVMap ( const String &_uvName ) {
	/*"map1"と"map1PackedUVVertexIDs"で１パッケージ*/
	this->uvs.getOrCreateU32Attribute ( _uvName + "packedUVVertexIDs" );
	return this->uvs.getOrCreatVec2Attribute ( _uvName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UVマップの値を設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
void PolygonMeshT<T>::setAllUVValues ( const String &uvName, const std::vector<Vec2T<T>> &uv ) {
	assert ( this->hasUVMap ( uvName ) );
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( uvName );
	uvAttr.setValues ( uv );
}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UVマップの値と値の配列数を設定する
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
//!	    UVマップの値を設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setUVValue ( const String &uvName, u32 vertexID, const Vec2T<T> &uv ) {
	assert ( this->hasUVMap ( uvName ) );
	assert ( vertexID < this->vertexCount () );
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( uvName );
	uvAttr.setValue ( vertexID, uv );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UVマップの値を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec2T<T>> PolygonMeshT<T>::getAllUVValues ( const String &uvName ) const{
	assert ( this->hasUVMap ( uvName ) );
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( uvName );
	return uvAttr.getValues ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UV値を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2T<T> PolygonMeshT<T>::getUVValue ( const String &uvName, u32 vertexID )const {
	assert ( this->hasUVMap ( uvName ) );
	assert ( vertexID < this->vertexCount () );
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( uvName );
	return uvAttr.getValue ( vertexID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    全てのUVマップの名前を返す (アトリビュート値が有効なUVマップのみ)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<String> PolygonMeshT<T>::getAllUVNames ()const {
	std::vector<String> result;
	std::vector<String> names = this->uvs.getAllAttributeNames ();
	for ( u32 i = 0; i<names.size (); i++ ) {
		if ( this->uvs.getType ( names [i] ) == Burgess2AttrType::Vec2Attr ) {/* "***packedUVVertexIDs"のアトリビュートは省く */
			result.push_back ( names [i] );
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UVマップを削除する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deleteUV ( const String &uvName ) {
	assert ( this->hasUVMap ( uvName ) );
	this->uvs->deleteAttr ( uvName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    UVマップの値を0.0にする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::clearUV ( const String &uvName ) {
	assert ( this->hasUVMap ( uvName ) );
	this->uvs->clear ( uvName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のUVマップが存在する場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasUVMap ( const String &uvName ) const {
	return this->uvs.has ( uvName );
}