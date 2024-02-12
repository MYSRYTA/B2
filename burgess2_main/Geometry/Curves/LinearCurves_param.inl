//------------------------------------------------------------------------------------------------------------------------------------------------
//!		NormalizedParam��Param�ɕϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getParamFromNormalizedParam(u32 curveID, T _nParam)const {
	assert(curveID < this->curveCount());
	T nParam = Burgess2MathFn::Clamp ( _nParam, T ( 0.0 ), T ( 1.0 ) );
	return nParam * this->getMaxParam ( curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Param��NormalizedParam�ɕϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getNormalizedParamFromParam(u32 curveID, T _param)const {
	assert(curveID < this->curveCount());
	T param = Burgess2MathFn::Clamp ( _param, this->getMinParam ( curveID ), this->getMaxParam ( curveID ) );
	T tortalLength = this->getCurveLength ( curveID );
	T paramLength = this->getLengthAtParam(curveID, param);
	return paramLength / tortalLength;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param�̍ő�l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getMaxParam(u32 curveID)const {
	Vec3AttributeT<T> &refParamLengthTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return refParamLengthTable [refParamLengthTable.size ()-1].x();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param�̍ŏ��l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getMinParam(u32 curveID)const {
	return T ( 0 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�𓖕�������Pram[]���擾����@0.0���܂߂邽�ߖ߂�l�̔z�񐔂�count+1
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> LinearCurvesT<T>::getEvenlyDivideParams(u32 curveID, u32 count) const {
	assert(curveID < this->curveCount());
	if (count == 0) { return std::vector<T> (); }

	T totalLength = this->getCurveLength(curveID);
	T length = totalLength / T(count);
	std::vector<T> result(count + 1);
	result[0] = T(0.0);
	#pragma omp parallel for
		for (s32 i = 1; i < count; i++) {
			result[i] = this->getParamFromLength(curveID, length * i);
		}
	result[count] = this->getMaxParam(curveID);
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�����Ԋu�ŕ�������Param[]���擾�@[0]=0.0
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> LinearCurvesT<T>::getRegularIntervalParams(u32 curveID, f32 _length) const {
	assert(curveID < this->curveCount());

	T totalLength = this->getCurveLength(curveID);
	u32 count = totalLength / _length;
	std::vector<T> result(count + 1);
	result [0] = T ( 0.0 );
	#pragma omp parallel for
		for (s32 i = 1; i <= result.size(); i++) {
			result[i] = this->getParamFromLength(curveID, _length * i);
		}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�𓮓I�Ԋu�ŕ�������Param[]���擾�@[0]=0.0
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> LinearCurvesT<T>::getDynamicIntervalParams(u32 curveID, f32 _length) const {
	assert(curveID < this->curveCount());

	T totalLength = this->getCurveLength(curveID);
	u32 count = totalLength / _length;
	T b = totalLength - (_length * count);
	T a = b / T(count);
	std::vector<T> result(count + 1);
	result[0] = T(0.0);
	#pragma omp parallel for
		for (s32 i = 1; i <= result.size(); i++) {
			result[i] = this->getParamFromLength(curveID, (_length + a) * i );
		}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�������_���Ԋu�ŕ�������Param[]���擾
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> LinearCurvesT<T>::getRandomParams(u32 curveID, u32 count, u32 seed) const {
	assert(curveID < this->curveCount());

	T totalLength = this->getCurveLength(curveID);
	T length = totalLength / T(count);
	std::vector<T> result(count);
	#pragma omp parallel for
		for (s32 i = 0; i < count; i++) {
			T val = Burgess2MathFn::RandomT(seed, i * 108 + 15, length*i, length*(i + 1));
			result[i] = this->getParamFromLength(curveID, val);
		}
	return result;
}