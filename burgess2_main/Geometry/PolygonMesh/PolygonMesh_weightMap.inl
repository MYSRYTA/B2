//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ウェイトマップの数を返す (アトリビュート値が有効なウェイトマップのみ)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 PolygonMeshT<T>::weightMapCount () const{
	std::vector<String> s = this->getAllWeightMapNames ();
	return u8(s.size ());
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   ウェイトマップを追加する (同名のマップが既に存在する場合はスルー)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>& PolygonMeshT<T>::addWeightMap ( const String &weightMapName ) {
	F32AttributeT<T> &result = this->weightMaps.getOrCreateF32Attribute ( weightMapName, this->vertexCount() );
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
//!	    ウェイトマップの値と値の配列数を設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::resetAllWeightMapValues ( const String &weightMapName, const std::vector<f32> &val ){
	assert ( this->hasWeightMap ( weightMapName ) );
	F32AttributeT<T> &wMapAttr = this->weightMaps.getF32Attribute ( weightMapName );
	wMapAttr.resetValues ( val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ウェイトマップの値を設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setWeightMapValue ( const String &weightMapName, u32 vertexID, f32 val ) {
	assert ( this->hasWeightMap ( weightMapName ) );
	assert ( vertexID < this->vertexCount () );
	F32AttributeT<T> &wMapAttr = this->weightMaps.getF32Attribute ( weightMapName );
	wMapAttr.setValue ( vertexID, val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ウェイトマップの全ての値を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<f32> PolygonMeshT<T>::getAllWeightMapValues ( const String &weightMapName ) const{
	assert ( this->hasWeightMap ( weightMapName ) );
	F32AttributeT<T> &wMapAttr = this->weightMaps.getF32Attribute ( weightMapName );
	return wMapAttr.getValues ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ウェイトマップの値を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
f32 PolygonMeshT<T>::getWeightMapValue ( const String &weightMapName, u32 vertexID )const {
	assert ( this->hasWeightMap ( weightMapName ) );
	assert ( vertexID < this->vertexCount () );
	F32AttributeT<T> &wMapAttr = this->weightMaps.getF32Attribute ( weightMapName );
	return wMapAttr.getValue ( vertexID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    全てのウェイトマップの名前を返す (アトリビュート値が有効なマップのみ)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<String> PolygonMeshT<T>::getAllWeightMapNames ()const {
	return this->weightMaps.getAllAttributeNames ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ウェイトマップを削除する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deleteWeightMap ( const String &weightMapName ) {
	assert ( this->hasWeightMap ( weightMapName ) );
	this->weightMaps->deleteAttr ( weightMapName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ウェイトマップの全ての値を0.0fにする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::clearWeightMapValues ( const String &weightMapName ) {
	assert ( this->hasWeightMap ( weightMapName ) );
	this->weightMaps->clear ( weightMapName );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のウェイトマップが存在する場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasWeightMap ( const String &weightMapName ) const {
	return this->weightMaps.has ( weightMapName );
}
