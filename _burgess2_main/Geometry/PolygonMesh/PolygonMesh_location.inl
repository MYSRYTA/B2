//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
PolygonMeshSpatialQueryT<T> PolygonMeshT<T>::toSpatialQuery ()const {
	assert ( 0<this->polygonCount () );
	PolygonMeshSpatialQueryT<T> result;
	result.setup( *this );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshSpatialQueryT<T> PolygonMeshT<T>::toSpatialQuery ( const String &colorMapName, Bool invertMask )const {
	assert ( 0<this->polygonCount () );
	if ( this->hasVertexColorMap ( colorMapName ) && this->vertexColors.getType ( colorMapName ) == Burgess2AttrType::Vec4Attr ){
		// バーテックスカラーをマスクにする場合
		Vec4AttributeT<T> &colorAttr = this->getVertexColorAttribute ( colorMapName );
		s32 cnt = colorAttr.size ();
		std::vector<f32> mask ( cnt );
		#pragma omp parallel for
		for ( s32 i = 0; i < cnt; i++ ){
			mask[i] = Burgess2MathFn::Clamp ( colorAttr[i].x() + colorAttr[i].y() + colorAttr[i].z() + colorAttr[i].t(), 0.0f, 1.0f );
		}
		return PolygonMeshSpatialQueryT<T> ( *this, mask, invertMask );

	}else if ( this->hasWeightMap ( colorMapName ) && this->weightMaps.getType ( colorMapName ) == Burgess2AttrType::F32Attr ){
		// ウェイトマップをマスクにする場合
		F32AttributeT<T> &colorAttr = this->getWeightMapAttribute ( colorMapName );
		return PolygonMeshSpatialQueryT<T> ( *this, colorAttr.getValuesRef(), invertMask );

	} else {
		return PolygonMeshSpatialQueryT<T> ( *this );
	}
}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ロケーションのポイントIDとウェイト値をまとめて取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::getLocationPointsAndWeights( const GeometryLocationT<T>& location, OUT Vec3u& poiID, OUT Vec3T<T>& weights )const {
	if ( location.isValid() == false ) { return; }
	assert( location.getType() == 3 );

	L8Array<u32> polyPoiIDs = this->getPolygonPointIDs( location.getIndex() );
	poiID.x() = polyPoiIDs[0];
	poiID.y() = polyPoiIDs[location.getSubIndex() + 1];
	poiID.z() = polyPoiIDs[location.getSubIndex() + 2];

	weights = location.getWeight();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ロケーションのポジションを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> PolygonMeshT<T>::getPositionAtLocation ( const GeometryLocationT<T> &location ) const{
	if ( location.isValid () == false ) { return Vec3T<T> ( T(BURGESS2::F32_INFINITE) ); }
	assert ( location.getType () == 3 );

	L8Array<u32> polyPoiIDs = this->getPolygonPointIDs ( location.getIndex () );
	Vec3T<T> weight = location.getWeight ();
	Vec3T<T> result;
	result  = this->getPointPosition ( polyPoiIDs [0] ) * weight.x();
	result += this->getPointPosition ( polyPoiIDs [location.getSubIndex () + 1] ) * weight.y();
	result += this->getPointPosition ( polyPoiIDs [location.getSubIndex () + 2] ) * weight.z();
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ロケーションの法線を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> PolygonMeshT<T>::getNormalAtLocation ( const GeometryLocationT<T> &location ) const {
	if ( location.isValid () == false ) { return Vec3T<T> ( T(BURGESS2::F32_INFINITE) ); }
	assert ( location.getType () == 3 );

	L8Array<u32> polyPoiIDs = this->getPolygonPointIDs ( location.getIndex () );
	Vec3T<T> p0 = this->getPointPosition ( polyPoiIDs[0] );
	Vec3T<T> p1 = this->getPointPosition ( polyPoiIDs[location.getSubIndex () + 1] );
	Vec3T<T> p2 = this->getPointPosition ( polyPoiIDs[location.getSubIndex () + 2] );
	return Burgess2MathFn::GetTriangleNormal ( p0, p1, p2 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ロケーションのUVを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2T<T> PolygonMeshT<T>::getUVAtLocation ( const GeometryLocationT<T> &location, const String &uvName ) const {
	if ( location.isValid () == false ) { return Vec2T<T> ( T(BURGESS2::F32_INFINITE) ); }
	assert ( location.getType () == 3 );
	assert ( this->hasUVMap ( uvName ) );

	L8Array<u32> polyVerDs = this->getPolygonVertexIDs ( location.getIndex () );
	Vec3T<T> weight = location.getWeight ();
	Vec2AttributeT<T> &uvAttr = this->uvs.getVec2Attribute ( uvName );
	Vec2T<T> result;
	result = uvAttr [polyVerDs [0] ] * weight.x();
	result += uvAttr [polyVerDs [location.getSubIndex () + 1]] * weight.y();
	result += uvAttr [polyVerDs [location.getSubIndex () + 2]] * weight.z();
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ロケーションのバーテックスカラーを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4T<T> PolygonMeshT<T>::getVertexColorAtLocation ( const GeometryLocationT<T> &location, const String &colorName ) const {
	if ( location.isValid () == false ) { return Vec4T<T> ( T(BURGESS2::F32_INFINITE) ); }
	assert ( location.getType () == 3 );
	assert ( this->hasVertexColorMap ( colorName ) );

	L8Array<u32> polyVerDs = this->getPolygonVertexIDs ( location.getIndex () );
	Vec3T<T> weight = location.getWeight ();
	Vec4AttributeT<T> &colorAttr = this->getVertexColorAttribute ( colorName );
	Vec4T<T> result;
	result = colorAttr [polyVerDs [0]] * weight.x();
	result += colorAttr [polyVerDs [location.getSubIndex () + 1]] * weight.y();
	result += colorAttr [polyVerDs [location.getSubIndex () + 2]] * weight.z();
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ロケーションのウェイトマップ値を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
f32 PolygonMeshT<T>::getWeightMapValueAtLocation ( const GeometryLocationT<T> &location, const String &weightMapName ) const{
	if ( location.isValid () == false ){ return BURGESS2::F32_INFINITE; }
	assert ( location.getType () == 3 );
	assert ( this->hasWeightMap ( weightMapName ) );

	L8Array<u32> polyVerDs = this->getPolygonVertexIDs ( location.getIndex () );
	Vec3T<T> weight = location.getWeight ();
	F32AttributeT<T> &wMapAttr = this->getWeightMapAttribute ( weightMapName );
	f32 result;
	result  = wMapAttr[polyVerDs[0]] * weight.x();
	result += wMapAttr[polyVerDs[location.getSubIndex () + 1]] * weight.y();
	result += wMapAttr[polyVerDs[location.getSubIndex () + 2]] * weight.z();
	return result;
}