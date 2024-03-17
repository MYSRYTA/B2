//------------------------------------------------------------------------------------------------------------------------------------------------
//!		CurveSpatialQueryを作成する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
LinesSpatialQueryT<T> BSplineCurvesT<T>::toSpatialQuery ( u32 curveID )const {
	assert ( 0 < this->curveCount () );
	u32 s = this->pointCount ( curveID ) + ( this->getForm ( curveID ) == CurveData::CurveFrom::Open ? 0 : 1 ) - 1; /*カーブのセグメント数を取得*/
	std::vector<T> p = this->getReductionParams ( curveID );
	std::vector<Vec3T<T>> pp ( p.size () );
	#pragma omp parallel for
		for ( s32 k = 0; k<pp.size (); k++ ) {
			pp [k] = this->getPositionAtParam ( curveID, p[k] );
		}
	LinesSpatialQueryT<T> result ( pp );
	result.v [0] = curveID;
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationの位置を取得する(GeometryLocationに保持されたカーブIDを使用)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getPositionAtLocation ( const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T>(); }
	return this->getPositionAtLengthRatio ( loc.getIndex (), loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationの位置を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getPositionAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getPositionAtLengthRatio ( curveID, loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのタンジェントを取得する(GeometryLocationに保持されたカーブIDを使用)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getTangentAtLocation ( const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getTangentAtLengthRatio ( loc.getIndex (), loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのタンジェントを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getTangentAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getTangentAtLengthRatio ( curveID, loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのNormalValueを取得する(GeometryLocationに保持されたカーブIDを使用)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getNormalAtLocation ( const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getNormalAtLengthRatio ( loc.getIndex (), loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのNormalValueを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getNormalAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getNormalAtLengthRatio ( curveID, loc.getCurveLengthRatio () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのXfo(position,tangent,normalを使用)を取得する(GeometryLocationに保持されたカーブIDを使用)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> BSplineCurvesT<T>::getXfoAtLocation ( const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	return this->getXfoAtLocation ( loc.getIndex (), loc );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		GeometryLocationのXfo(position,tangent,normalを使用)を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> BSplineCurvesT<T>::getXfoAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const {
	if ( loc.isValid () == false ) { return Vec3T<T> (); }
	Vec3T<T> pos = this->getPositionAtLengthRatio ( curveID, loc.getCurveLengthRatio () );
	Vec3T<T> tan = this->getTangentAtLengthRatio ( curveID, loc.getCurveLengthRatio () );
	Vec3T<T> upV = this->getNormalAtLengthRatio ( curveID, loc.getCurveLengthRatio () );
	QuatT<T> ori;
	ori.setFromDirectionAndUpvector ( tan, upV );
	return XfoT<T> ( Vec3T<T> ( T ( 1.0 ) ), ori, pos );
}