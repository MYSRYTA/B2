//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param値の位置を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getPositionAtParam(u32 curveID, T _param)const {
	assert(curveID < this->curveCount());
	assert(2 <=  this->pointCount ( curveID ));
	
	T param = Burgess2MathFn::Clamp ( _param, this->getMinParam ( curveID ), this->getMaxParam ( curveID ) );

	if ( Burgess2MathFn::AlmostEqual ( this->getMinParam ( curveID ), param ) ) {
		return this->getPointPosition ( curveID, 0 );
	} else if ( this->getForm ( curveID ) != CurveData::CurveFrom::Open && Burgess2MathFn::AlmostEqual ( this->getMaxParam ( curveID ), param ) ) {
		return this->getPointPosition ( curveID, 0 );
	} else if ( Burgess2MathFn::AlmostEqual ( this->getMaxParam ( curveID ), param ) ) {
		return this->getPointPosition ( curveID, this->pointCount ( curveID )-1 );
	}

	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );

	s32 low = 1;
	s32 high = refParamLenTable.size();

	/* テーブル内を2分割しながら検索 */
	while ( low < high ) {
		s32 mid = ( low + high ) / 2;
		T value = refParamLenTable [mid].x();
		if ( value <= param ) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}

	f32 localParam = param - refParamLenTable [low - 1].x();
	f32 ratio = localParam / ( refParamLenTable [low].x() - refParamLenTable [low-1].x() );
	Vec3AttributeT<T> &refPoiPos = this->getPointPositionAttribute ( curveID );
	return refPoiPos [low-1].lerp ( refPoiPos [low], ratio );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param値のタンジェントを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getTangentAtParam(u32 curveID, T _param)const {
	assert ( curveID < this->curveCount () );
	assert ( 2 <=  this->pointCount ( curveID ) );

	T param = Burgess2MathFn::Clamp ( _param, this->getMinParam ( curveID ), this->getMaxParam ( curveID ) );

	Vec3AttributeT<T> &refPoiPos = this->getPointPositionAttribute ( curveID );
	if ( Burgess2MathFn::AlmostEqual ( this->getMinParam ( curveID ), param ) ) {
		return ( refPoiPos[1] - refPoiPos[0] ).unit();
	} else if ( this->getForm ( curveID ) != CurveData::CurveFrom::Open && Burgess2MathFn::AlmostEqual ( this->getMaxParam ( curveID ), param ) ) {
		return ( refPoiPos [1] - refPoiPos [0] ).unit ();
	} else if ( Burgess2MathFn::AlmostEqual ( this->getMaxParam ( curveID ), param ) ) {
		return ( refPoiPos [this->allPointCount()-1] - refPoiPos [this->allPointCount ()-2] ).unit ();
	}

	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );

	s32 low = 1;
	s32 high = refParamLenTable.size ();

	/* テーブル内を2分割しながら検索 */
	while ( low < high ) {
		s32 mid = ( low + high ) / 2;
		T value = refParamLenTable [mid].x();
		if ( value <= param ) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}

	return (refPoiPos [low] - refPoiPos [low-1]).normal();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param値のNormalValueを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getNormalAtParam ( u32 curveID, T param )const {
	assert ( curveID < this->curveCount () );
	assert ( 2 <=  this->pointCount ( curveID ) );
	T _param = Burgess2MathFn::Clamp ( param, this->getMinParam(curveID), this->getMaxParam(curveID) );
	T lenRatio = this->getLengthRatioAtParam ( curveID, _param );
	return this->getNormalAtLengthRatio ( curveID, lenRatio );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param値のXfoを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> LinearCurvesT<T>::getXfoAtParam ( u32 curveID, T _param, u8 directionType )const {
	assert ( curveID < this->curveCount () );
	assert ( 2 <=  this->pointCount ( curveID ) );
	Vec3T<T> pos = this->getPositionAtParam ( curveID, _param );
	Vec3T<T> tan = this->getTangentAtParam ( curveID, _param );
	Vec3T<T> upV = this->getNormalAtParam ( curveID, _param );
	QuatT<T> ori;
	ori.setFromDirectionAndUpvector ( tan, upV, directionType );
	return XfoT<T> ( Vec3T<T> ( T ( 1.0 ) ), ori, pos );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     NormalizedParam値のポジションを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getPositionAtNormalizedParam(u32 curveID, T nParam)const {
	assert(curveID < this->curveCount());
	T param = this->getParamFromNormalizedParam(curveID, nParam);
	return this->getPositionAtParam(curveID, param);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    NormalizedParam値のタンジェントを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getTangentAtNormalizedParam(u32 curveID, T nParam)const {
	assert(curveID < this->curveCount());
	T param = this->getParamFromNormalizedParam(curveID, nParam);
	return this->getTangentAtParam(curveID, param);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    NormalizedParam値のNormalValueを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getNormalAtNormalizedParam ( u32 curveID, T nParam )const {
	assert ( curveID < this->curveCount () );
	T param = this->getParamFromNormalizedParam ( curveID, nParam );
	return this->getNormalAtParam ( curveID, param );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    NormalizedParam値のXfoを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> LinearCurvesT<T>::getXfoAtNormalizedParam ( u32 curveID, T nParam, u8 directionType )const {
	assert ( curveID < this->curveCount () );
	T param = this->getParamFromNormalizedParam ( curveID, nParam );
	return this->getXfoAtParam ( curveID, param, directionType );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio値のポジションを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getPositionAtLengthRatio ( u32 curveID, T _ratio )const {
	assert ( curveID < this->curveCount () );
	T ratio = Burgess2MathFn::Clamp ( _ratio, T ( 0.0 ), T ( 1.0 ) );
	T rL = this->getCurveLength ( curveID ) * ratio;
	T param = this->getParamFromLength ( curveID, rL );
	return this->getPositionAtParam ( curveID, param );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio値のタンジェントを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getTangentAtLengthRatio ( u32 curveID, T _ratio )const {
	assert ( curveID < this->curveCount () );
	T ratio = Burgess2MathFn::Clamp ( _ratio, T ( 0.0 ), T ( 1.0 ) );
	T rL = this->getCurveLength ( curveID ) * ratio;
	T param = this->getParamFromLength ( curveID, rL );
	return this->getTangentAtParam ( curveID, param );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio値のNormalValueを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinearCurvesT<T>::getNormalAtLengthRatio ( u32 curveID, T _ratio )const {
	assert ( curveID < this->curveCount () );
	T ratio = Burgess2MathFn::Clamp ( _ratio, T ( 0.0 ), T ( 1.0 ) );

	Vec3AttributeT<T> &dirValueAttr = this->getNormalAttribute ( curveID );
	if ( dirValueAttr.size () == 0 ){ return Vec3T<T> ( 0, 1, 0 ); } // normalsアトリビュートが正しく登録されていない場合はYアップを返す

	// 配列数が１の場合はNormalAttribute座標への向きを返す
	if ( dirValueAttr.size () == 1 ){
		Vec3T<T> pos = this->getPositionAtLengthRatio ( curveID, ratio );
		if ( pos.almostEqual ( dirValueAttr[0] ) ){
			return Vec3T<T> ( 0, 1, 0 );
		} else{
			return ( dirValueAttr[0] - pos ).unit ();
		}
	}

	// ratioが範囲外の場合は始点/終点の法線を返す
	if ( ratio <= T(0.0) ){ return dirValueAttr[0]; }
	if ( T(1.0) <= ratio ){ return dirValueAttr[dirValueAttr.size () - 1]; }

	// 配列数が2の場合はslerpを返す
	if ( dirValueAttr.size () == 2 ){
		if ( dirValueAttr[0].almostEqual ( dirValueAttr[1] ) ){
			return dirValueAttr[0];
		} else{
			return dirValueAttr[0].slerp ( dirValueAttr[1], ratio ); // normalsアトリビュートが2点の場合は始点～終点間の球面線形補間の値を返す
		}
	}
	
	// 配列数が頂点数と同じはparam値でnormalを算出
	if ( this->pointCount ( curveID ) == dirValueAttr.size () ){
		T rL = this->getCurveLength ( curveID ) * ratio;
		Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
		s32 low = 1;
		s32 high = refParamLenTable.size ();
		// テーブル内を2分割しながら検索
		while ( low < high ){
			s32 mid = ( low + high ) / 2;
			T value = refParamLenTable[mid].y();
			if ( value <= rL ){
				low = mid + 1;
			} else{
				high = mid;
			}
		}
		f32 localLen = rL - refParamLenTable[low - 1].y();
		T ratio2 = localLen / ( refParamLenTable[low].y() - refParamLenTable[low - 1].y() );
		return dirValueAttr[low - 1].slerp ( dirValueAttr[low], ratio2 );
	}

	// 配列数が頂点数以上の場合はlengthRatio値のnormalを返す
	T fractions = T ( 1.0 ) / T ( dirValueAttr.size () - 1 );
	u32 quotient = u32( ratio / fractions );
	T remainder = ratio - fractions * quotient;
	T t = remainder / fractions;
	if ( dirValueAttr[quotient].almostEqual ( dirValueAttr[quotient + 1] ) ){
		return dirValueAttr[quotient];
	} else{
		return dirValueAttr[quotient].slerp ( dirValueAttr[quotient + 1], t );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio値のXfoを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> LinearCurvesT<T>::getXfoAtLengthRatio ( u32 curveID, T _ratio, u8 directionType )const {
	assert ( curveID < this->curveCount () );
	T ratio = Burgess2MathFn::Clamp ( _ratio, T ( 0.0 ), T ( 1.0 ) );
	T rL = this->getCurveLength ( curveID ) * ratio;
	T param = this->getParamFromLength ( curveID, rL );
	return this->getXfoAtParam ( curveID, param, directionType );
}