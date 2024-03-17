//------------------------------------------------------------------------------------------------------------------------------------------------
//!		CurveSpatialQueryを作成する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
LinesSpatialQueryT<T> LinearCurvesT<T>::toSpatialQuery ( u32 curveID )const {
	assert ( 0 < this->curveCount () );
	std::vector<Vec3T<T>> pp = this->getCurveFormAllPointPositions ( curveID );
	LinesSpatialQueryT<T> result ( pp );
	result.v [0] = curveID;
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationの位置を取得する(GeometryLocationに保持されたカーブIDを使用)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getPositionAtLocation ( const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T>(); }
	return this->getPositionAtLengthRatio ( loc.getIndex (), loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationの位置を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getPositionAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getPositionAtLengthRatio ( curveID, loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのタンジェントを取得する(GeometryLocationに保持されたカーブIDを使用)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getTangentAtLocation ( const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getTangentAtLengthRatio ( loc.getIndex (), loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのタンジェントを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getTangentAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getTangentAtLengthRatio ( curveID, loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのNormalValueを取得する(GeometryLocationに保持されたカーブIDを使用)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getNormalAtLocation ( const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getNormalAtLengthRatio ( loc.getIndex (), loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのNormalValueを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getNormalAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getNormalAtLengthRatio ( curveID, loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのXfoを取得する(GeometryLocationに保持されたカーブIDを使用)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> LinearCurvesT<T>::getXfoAtLocation ( const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getXfoAtLengthRatio ( loc.getIndex (), loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのXfoを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> LinearCurvesT<T>::getXfoAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getXfoAtLengthRatio ( curveID, loc.getCurveLengthRatio () );
}