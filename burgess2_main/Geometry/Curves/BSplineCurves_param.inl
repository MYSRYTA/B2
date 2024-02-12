//------------------------------------------------------------------------------------------------------------------------------------------------
//!		NormalizedParam��Param�ɕϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getParamFromNormalizedParam(u32 curveID, T _nParam)const {
	assert(curveID < this->curveCount());
	T nParam = _nParam;// Burgess2MathFn::Clamp( _nParam, T( 0.0 ), T( 1.0 ) );
	F32AttributeT<T> &refKnotAttr = this->getF32Attribute(curveID, "knotVector");
	return refKnotAttr[0] + nParam * ( refKnotAttr[refKnotAttr.size() - 1] - refKnotAttr[0] );
}


//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Param��NormalizedParam�ɕϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getNormalizedParamFromParam(u32 curveID, T _param)const {
	assert(curveID < this->curveCount());
	T offset = ( this->isPeriodic ( curveID ) ? T ( this->getDegree ( curveID ) ) : 0 );
	T param = Burgess2MathFn::Clamp ( _param, this->getMinParam ( curveID ), this->getMaxParam ( curveID ) );
	
	F32AttributeT<T> &refKnotAttr = this->getF32Attribute ( curveID, "knotVector" );

	u8 domainA = this->getDegree ( curveID );
	u8 domainB = refKnotAttr.size () - 1 - this->getDegree ( curveID );

	T low = refKnotAttr [domainA] - offset;
	T high = refKnotAttr [domainB] - offset;

	return param / ( high - low ) - low;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param�̍ő�l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getMaxParam ( u32 curveID )const {
	Vec3AttributeT<T> &refParamLengthTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return refParamLengthTable [refParamLengthTable.size ()-1].x();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param�̍ŏ��l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getMinParam(u32 curveID)const {
	Vec3AttributeT<T> &refParamLengthTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return refParamLengthTable [0].x();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�𓖕�������Pram[]���擾����@0.0���܂߂邽�ߖ߂�l�̔z�񐔂�count+1
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::getEvenlyDivideParams(u32 curveID, u32 count) const {
	assert(curveID < this->curveCount());
	std::vector<T> result (0);
	if (count == 0) { return result; }

	T totalLength = this->getCurveLength(curveID);
	T length = totalLength / T(count);
	result.resize(count + 1);
	result[0] = T(0.0);
	#pragma omp parallel for
		for (s32 i = 1; i < result.size (); i++) {
			result[i] = this->getParamFromLength(curveID, length * i);
		}
	result[count] = this->getMaxParam(curveID);
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�����Ԋu�ŕ�������Param[]���擾�@[0]=0.0
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::getRegularIntervalParams(u32 curveID, f32 _length) const {
	assert(curveID < this->curveCount());

	T totalLength = this->getCurveLength(curveID);
	u32 count = totalLength / _length;
	std::vector<T> result(count + 1);
	result [0] = T ( 0.0 );
	#pragma omp parallel for
		for (s32 i = 1; i <= result.size (); i++) {
			result[i] = this->getParamFromLength(curveID, _length * i);
		}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�𓮓I�Ԋu�ŕ�������Param[]���擾�@[0]=0.0
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::getDynamicIntervalParams(u32 curveID, f32 _length) const {
	assert(curveID < this->curveCount());

	T totalLength = this->getCurveLength(curveID);
	u32 count = totalLength / _length;
	T b = totalLength - (_length * count);
	T a = b / T(count);
	std::vector<T> result(count + 1);
	result[0] = T(0.0);
	#pragma omp parallel for
		for (s32 i = 1; i <= result.size (); i++) {
			result[i] = this->getParamFromLength(curveID, _length * i + a * i);
		}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�������_���Ԋu�ŕ�������Param[]���擾
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::getRandomParams(u32 curveID, u32 count, u32 seed) const {
	assert(curveID < this->curveCount());

	T totalLength = this->getCurveLength(curveID);
	T length = totalLength / T(count);
	std::vector<T> result(count);
	#pragma omp parallel for
		for (s32 i = 0; i < result.size (); i++) {
			T val = Burgess2MathFn::RandomT(seed + i * 7, i * 108 + 15, length*i, length*(i + 1));
			result[i] = this->getParamFromLength( curveID, val );
		}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̌`�����萸�x�ŕێ�����Param[]���擾 
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::getReductionParams ( u32 curveID, T maxDegree, u8 precision ) const {
	assert ( curveID < this->curveCount () );
	T maxRadian = Burgess2MathFn::DegToRad ( maxDegree );
	u32 s = this->pointCount ( curveID ) + ( this->getForm ( curveID ) == CurveData::CurveFrom::Open ? 0 : 1 ) - 1; /*�J�[�u�̃Z�O�����g�����擾*/
	u8 pre = Burgess2MathFn::Clamp ( precision, u8(1), u8(4) );
	std::vector<T> p = this->getEvenlyDivideParams ( curveID, s * this->data [curveID].getSample () * pre ); /* �Z�O�����g*�T���v����*precision�̐��ŋϓ��ɕ�������Param[]���擾 */

	std::vector<T> result;
	result.push_back ( T ( 0.0 ) );
	T sum = 0;
	Vec3T<T> last = ( this->getPositionAtParam ( curveID, p [1] ) - this->getPositionAtParam ( curveID, p [0] ) ).unit ();
	for ( u32 i = 1; i<p.size()-1; i++ ) {
		Vec3T<T> v = ( this->getPositionAtParam ( curveID, p [i+1] ) - this->getPositionAtParam ( curveID, p [i] ) ).unit ();
		sum += v.angleTo ( last );
		if ( maxRadian <= sum ) {
			result.push_back ( p [i] );
			sum = 0;
		}
		last = v;
	}
	result.push_back ( this->getMaxParam ( curveID ) );
	return result;
}