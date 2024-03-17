//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param値の位置を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getPositionAtParam(u32 curveID, T _param)const {
	Vec3T<T> result;
	assert(curveID < this->curveCount());

	u8 degree = this->getDegree(curveID);
	assert(2 == degree || 3 == degree);
	assert(3 <= this->pointCount(curveID));

	T periodicOffset = this->isPeriodic ( curveID ) ? T ( this->getDegree ( curveID ) ) : 0;
	T param = _param + periodicOffset;
	Vec3AttributeT<T> &refPoiAttr = this->getPointPositionAttribute(curveID);
	F32AttributeT<T> &refKnotAttr = this->getF32Attribute(curveID, "knotVector");

	if ( param < refKnotAttr[refKnotAttr.size() - 1] ){
		// paramがカーブの最大値よりも小さい場合
		u8 span;
		u8 domainA = degree;
		u8 domainB = refKnotAttr.size() - 1 - degree;
		if ( refKnotAttr[domainB] <= param ){ span = domainB - 1; } // last
		else if ( param <= refKnotAttr[domainA] ){ span = domainA; } // first
		else{
			for ( span = domainA; span < domainB; ++span ){
				if ( refKnotAttr[span] <= param && param <= refKnotAttr[span + 1] ){
					break;
				}
			}
		}

		std::vector<T> basisFunctions( degree + 1 );
		std::vector<T> left( degree + 1 );
		std::vector<T> right( degree + 1 );
		basisFunctions[0] = T( 1.0 );
		for ( s32 i = 1; i <= degree; ++i ){
			left[i] = param - refKnotAttr[span + 1 - i];
			right[i] = refKnotAttr[span + i] - param;

			T val( 0.0 );
			for ( s32 j = 0; j < i; ++j ){
				T rVal = right[j + 1];
				T lVal = left[i - j];
				T temp = basisFunctions[j] / ( rVal + lVal );
				basisFunctions[j] = val + rVal * temp;
				val = lVal * temp;
			}
			basisFunctions[i] = val;
		}

		// point.tは恐らくNURBSのウェイト値？
		for ( s32 i = 0; i <= degree; ++i ){
			Vec3T<T> point = refPoiAttr[span - degree + i];
			T basisVal = basisFunctions[i];
			T weight = basisVal;// * point.t();
			result += point * weight;
			//result.t() += point.t() * basisVal;
		}
	} else {
		// paramがカーブの最大値よりも大きな場合
		T e = ( param - refKnotAttr[refKnotAttr.size() - 1] ) / refKnotAttr[refKnotAttr.size() - 1];
		T l = this->getCurveLength( curveID ) * e;
		Vec3T<T> dir = this->getTangentAtLengthRatio( curveID, T( 0.99999 ) );
		result = refPoiAttr[refPoiAttr.size()-1] + dir * l;
	}

	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param値のタンジェントを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getTangentAtParam(u32 curveID, T param)const {
	assert ( curveID < this->curveCount () );
	Vec3T<T> v0 = this->getPositionAtParam(curveID, param);
	Vec3T<T> v1;
	T periodicOffset = this->isPeriodic ( curveID ) ? T ( this->getDegree ( curveID ) ) : T(0.0);
	if (Burgess2MathFn::AlmostEqual( this->getMaxParam(curveID), param + periodicOffset, T(0.001) ) ) {
		v1 = v0;
		v0 = this->getPositionAtParam(curveID, param - T(0.001));
	} else {
		v1 = this->getPositionAtParam(curveID, param + T(0.001));
	}
	assert ( ( v0 - v1 ).isZero () == false );
	return ( v1 - v0 ).normal ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param値のNormal Valueを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getNormalAtParam ( u32 curveID, T param )const {
	assert ( curveID < this->curveCount () );
	T lenRatio = this->getLengthRatioAtParam ( curveID, param );
	return this->getNormalAtLengthRatio ( curveID, lenRatio );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param値のXfoを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> BSplineCurvesT<T>::getXfoAtParam ( u32 curveID, T param, u8 directionType )const {
	assert ( curveID < this->curveCount () );
	T lenRatio = this->getLengthRatioAtParam ( curveID, param );
	return this->getXfoAtLengthRatio ( curveID, lenRatio, directionType );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     NormalizedParam値のポジションを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getPositionAtNormalizedParam(u32 curveID, T nParam)const {
	assert(curveID < this->curveCount());
	T param = this->getParamFromNormalizedParam(curveID, nParam);
	return this->getPositionAtParam(curveID, param);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    NormalizedParam値のタンジェントを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getTangentAtNormalizedParam(u32 curveID, T nParam)const {
	assert(curveID < this->curveCount());
	T param = this->getParamFromNormalizedParam(curveID, nParam);
	return this->getTangentAtParam(curveID, param);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    NormalizedParam値のNormal Valueを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getNormalAtNormalizedParam ( u32 curveID, T nParam )const {
	assert ( curveID < this->curveCount () );
	T lenRatio = this->getLengthRatioAtNormalizedParam ( curveID, nParam );
	return this->getNormalAtLengthRatio ( curveID, lenRatio );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    NormalizedParam値のXfoを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> BSplineCurvesT<T>::getXfoAtNormalizedParam ( u32 curveID, T nParam, u8 directionType )const {
	assert ( curveID < this->curveCount () );
	T lenRatio = this->getLengthRatioAtNormalizedParam ( curveID, nParam );
	return this->getXfoAtLengthRatio ( curveID, lenRatio, directionType );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio値のポジションを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getPositionAtLengthRatio ( u32 curveID, T _ratio )const {
	assert ( curveID < this->curveCount () );
	T ratio = _ratio;// Burgess2MathFn::Clamp( _ratio, T( 0.0 ), T( 1.0 ) );
	T rL = this->getCurveLength ( curveID ) * ratio;
	T param = this->getParamFromLength ( curveID, rL );
	return this->getPositionAtParam ( curveID, param );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio値のタンジェントを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getTangentAtLengthRatio ( u32 curveID, T _ratio )const {
	assert ( curveID < this->curveCount () );
	T ratio = Burgess2MathFn::Clamp( _ratio, T( 0.0 ), T( 1.0 ) );
	T rL = this->getCurveLength ( curveID ) * ratio;
	T param = this->getParamFromLength ( curveID, rL );
	return this->getTangentAtParam ( curveID, param );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio値のNormal Valueを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSplineCurvesT<T>::getNormalAtLengthRatio ( u32 curveID, T _ratio )const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T>& v3Attr = this->getNormalAttribute ( curveID );
	if ( v3Attr.size () == 0 ){ return Vec3T<T> ( 0, 1, 0 ); } // normalsアトリビュートが正しく登録されていない場合はYアップを返す

	// 配列数が１の場合はNormalAttribute座標への向きを返す
	if ( v3Attr.size () == 1 ){
		Vec3T<T> pos = this->getPositionAtLengthRatio ( curveID, _ratio );
		if ( pos.almostEqual ( v3Attr[0], T(0.001) ) ){
			return Vec3T<T> ( 0, 1, 0 );
		} else{
			return ( v3Attr[0] - pos ).unit ();
		}
	}
	// ratioがparam範囲外の場合は始点/終点の法線を返す
	if ( _ratio <= T ( 0.0 ) ){ return v3Attr[0]; }
	if ( T ( 1.0 ) <= _ratio ){ return v3Attr[v3Attr.size () - 1]; }

	// 配列数が2の場合はslerpを返す
	if ( v3Attr.size () ==2 ) {
		if ( v3Attr[0].almostEqual ( v3Attr[1], T(0.001) ) ){
			return v3Attr[0];
		} else{
			return (v3Attr[0].slerp ( v3Attr[1], _ratio )).unit(); // normalsアトリビュートが2点の場合は始点～終点間の球面線形補間の値を返す
		}
	}

	// 配列数が頂点数(重複頂点は含まない)の場合
	if ( this->pointCount ( curveID ) == v3Attr.size () ){
		u32 poiCnt = this->pointCount ( curveID );

		// この条件時にのみ使用するアトリビュート(CP間の距離リスト)を作るためにconstを一時的に外す
		BSplineCurvesT<T>* this2 = const_cast<BSplineCurvesT<T>*> ( this );
		F64AttributeT<T> &cpLenAttr = this2->getOrCreateF64Attribute ( curveID, "cpLengthList" );
		if ( cpLenAttr.size() != poiCnt + ( this->isOpen ( curveID ) ? 0 : 1 ) ){
			std::vector<f64> lenList ( poiCnt + ( this->isOpen ( curveID ) ? 0 : 1 ) );
			lenList[0] = T ( 0.0 );
			for ( u32 i = 1; i < poiCnt; ++i ){
				lenList[i] = lenList[i - 1] + this->getPointPosition ( curveID, i - 1 ).distanceTo ( this->getPointPosition ( curveID, i ) );
			}
			if ( this->isOpen ( curveID ) == false ){
				lenList[poiCnt] = lenList[poiCnt - 1] + this->getPointPosition ( curveID, poiCnt - 1 ).distanceTo ( this->getPointPosition ( curveID, 0 ) );
			}

			// double対応
			//cpLenAttr.resetValues ( lenList ); 

			cpLenAttr.resize ( u32(lenList.size ()) );
			#pragma omp parallel for
			for ( s32 i = 1; i < lenList.size (); ++i ) {
				cpLenAttr[i] = T ( lenList[i] );
			}
		}
		T cpTotalLength = cpLenAttr[cpLenAttr.size () - 1];
		u32 id = 0;
		T t{0.0};
		T ratioLen = cpTotalLength * _ratio;
		for ( u32 i = 0; i < cpLenAttr.size () - 1; ++i ){
			if ( cpLenAttr[i] <= ratioLen && ratioLen < cpLenAttr[i + 1] ){
				id = i;
				t = ( ratioLen - cpLenAttr[i] ) / ( cpLenAttr[i + 1] - cpLenAttr[i] );
				break;
			}
		}
		if ( v3Attr[id].almostEqual ( v3Attr[id + 1], T(0.001) ) ){
			return v3Attr[id];
		} else{
			return v3Attr[id].slerp ( v3Attr[id + 1], t );
		}
	}

	// 配列数が頂点数以上の場合はlengthRatio値のnormalを返す
	T fractions = T ( 1.0 ) / T ( v3Attr.size () - 1 );
	u32 quotient = u32(_ratio / fractions);
	T remainder = _ratio - fractions * quotient;
	T t = remainder / fractions;
	if ( v3Attr[quotient].almostEqual ( v3Attr[quotient + 1], T(0.001) ) ){
		return v3Attr[quotient];
	} else{
		return v3Attr[quotient].slerp ( v3Attr[quotient + 1], t );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio値のXfoを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> BSplineCurvesT<T>::getXfoAtLengthRatio ( u32 curveID, T _ratio, u8 directionType )const {
	assert ( curveID < this->curveCount () );
	Vec3T<T> pos = this->getPositionAtLengthRatio ( curveID, _ratio );
	Vec3T<T> tan = this->getTangentAtLengthRatio ( curveID, _ratio );
	Vec3T<T> upV = this->getNormalAtLengthRatio ( curveID, _ratio );
	QuatT<T> ori;
	ori.setFromDirectionAndUpvector ( tan, upV, directionType );
	return XfoT<T> ( Vec3T<T> ( T ( 1.0 )), ori, pos );
}