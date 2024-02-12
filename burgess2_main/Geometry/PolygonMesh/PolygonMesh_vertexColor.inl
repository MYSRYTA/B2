//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスカラーマップの数を返す (アトリビュート値が有効なバーテックスカラーマップのみ)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 PolygonMeshT<T>::vertexColorMapCount () const{
	std::vector<String> s = this->getAllVertexColorNames ();
	return u8(s.size ());
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   バーテックスカラーマップを追加する (同名のマップが既に存在する場合はスルー)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>& PolygonMeshT<T>::addVertexColorMap ( const String &colorMapName, Bool hasAlpha ) {
	Vec4AttributeT<T> &result = this->vertexColors.getOrCreatVec4Attribute ( colorMapName, this->vertexCount() );
	result.v[0] = hasAlpha ? 1 : 0; // Attribute.v[0]: 0=アルファチャンネル無し、1=有り
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスカラーの全ての値を設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
void PolygonMeshT<T>::setAllVertexColor ( const String &colorMapName, const std::vector<Vec4T<T>> &color ) {
	assert ( this->hasVertexColorMap ( colorMapName ) );
	assert ( this->vertexCount () == color.size () );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	colorAttr.setValues ( color );
}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスカラーの値と値の配列数を設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::resetAllVertexColors ( const String &colorMapName, const std::vector<Vec4T<T>> &color ){
	assert ( this->hasVertexColorMap ( colorMapName ) );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	colorAttr.resetValues ( color );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスカラーの値を設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setVertexColor ( const String &colorMapName, u32 vertexID, const Vec4T<T> &color ) {
	assert ( this->hasVertexColorMap ( colorMapName ) );
	assert ( vertexID < this->vertexCount () );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	colorAttr.setValue ( vertexID, color );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスカラーの全ての値を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec4T<T>> PolygonMeshT<T>::getAllVertexColor ( const String &colorMapName ) const{
	assert ( this->hasVertexColorMap ( colorMapName ) );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	return colorAttr.getValues ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスカラーの値を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4T<T> PolygonMeshT<T>::getVertexColor ( const String &colorMapName, u32 vertexID )const {
	assert ( this->hasVertexColorMap ( colorMapName ) );
	assert ( vertexID < this->vertexCount () );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	return colorAttr.getValue ( vertexID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    全てのバーテックスカラーマップの名前を返す (アトリビュート値が有効なマップのみ)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<String> PolygonMeshT<T>::getAllVertexColorNames ()const {
	return this->vertexColors.getAllAttributeNames ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスカラーマップを削除する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deleteVertexColorMap ( const String &colorMapName ) {
	assert ( this->hasVertexColorMap ( colorMapName ) );
	this->vertexColors->deleteAttr ( colorMapName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスカラーマップの全ての値をVec4T<T>(0.0, 0.0, 0.0, 0.0)にする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::clearVertexColorValues ( const String &colorMapName ) {
	assert ( this->hasVertexColorMap ( colorMapName ) );
	this->vertexColors->clear ( colorMapName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のバーテックスカラーマップが存在する場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasVertexColorMap ( const String &colorMapName ) const {
	return this->vertexColors.has ( colorMapName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のバーテックスカラーマップにアルファチャンネルが存在する場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasAlpha ( const String &colorMapName ) const{
	assert ( this->hasVertexColorMap ( colorMapName ) );
	Vec4AttributeT<T> &colorAttr = this->vertexColors.getVec4Attribute ( colorMapName );
	return colorAttr.v[0] == 1; // Attribute.v[0]: 0=アルファチャンネル無し、1=有り
}