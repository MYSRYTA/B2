//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Attribute"paramLengthTable"�̍X�V���K�v�ȏꍇ�͍X�V����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void BSplineCurvesT<T>::recomputeParamLengthTable ( u32 curveID ) {
	if ( this->paramLengthTableIsDirty ( curveID ) ) {
		F32AttributeT<T> &refKnotAttr = this->getF32Attribute ( curveID, "knotVector" );
		Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );

		u8 domainA = this->getDegree ( curveID );
		u8 domainB = refKnotAttr.size () - 1 - this->getDegree ( curveID );
		u8 sample = this->data [curveID].getSample ();
		u32 cnt = ( domainB - domainA ) * sample + 1;

		T offset = this->isPeriodic ( curveID ) ? T ( this->getDegree ( curveID ) ) : 0;
		refParamLenTable.resize ( cnt );
		std::vector<Vec3T<T>> restPos ( cnt );
		for ( s32 i = 0; i<s32(cnt-1); i++ ) {
			u16 s = i / sample;
			T sVal = refKnotAttr [domainA + s + 1] - refKnotAttr [domainA + s];
			T f = T ( i - sample * s ) / T ( sample );
			T fVal = (refKnotAttr [domainA + s] - offset) + sVal * f;
			restPos [i] = this->getPositionAtParam ( curveID, fVal );
			refParamLenTable [i].x() = fVal;
		}
		restPos [cnt-1] = this->getPositionAtParam ( curveID, refKnotAttr [domainB] - offset );
		refParamLenTable [cnt-1].x() = refKnotAttr [domainB] - offset;

		refParamLenTable [0].y() = 0;
		for ( u32 i = 1; i<cnt; i++ ) {
			refParamLenTable [i].y() = refParamLenTable [i-1].y() + T ( restPos [i].distanceTo ( restPos [i-1] ) );
		}

		for (u32 i = 0; i < cnt-1; i++) {
			refParamLenTable[i].z() = T(1.0) / (refParamLenTable[i+1].y() - refParamLenTable[i].y()); /*�������Z�O�����g�̃��V�I�ɕϊ�����ۂɎg�p����l*/
		}

		refParamLenTable [refParamLenTable.size ()-1].z() = T ( 1.0 ) / refParamLenTable [refParamLenTable.size ()-1].y();/*�������J�[�u�S���̃��V�I�ɕϊ�����ۂɎg�p����l*/

		this->paramLengthTableCached ( curveID );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �J�[�u�̒������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getCurveLength ( u32 curveID ) {
	assert ( curveID < this->curveCount () );
	if ( this->paramLengthTableIsDirty ( curveID ) ) {
		this->recomputeParamLengthTable ( curveID );
	}
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return refParamLenTable[refParamLenTable.size ()-1].y();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �J�[�u�̒������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getCurveLength ( u32 curveID )const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T> &refParamLengthTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return refParamLengthTable [refParamLengthTable.size()-1].y();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �J�[�u�̒�������Pram���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getParamFromLength ( u32 curveID, T _length )const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	s32 end = refParamLenTable.size ();
	
	if ( Burgess2MathFn::AlmostEqual( _length, T(0.0)) ) {
		return T ( 0.0 );
	}
	if ( Burgess2MathFn::AlmostEqual ( _length, refParamLenTable [end - 1].y() ) ) {
		return refParamLenTable [end - 1].x() - refParamLenTable [0].x();
	}

	if ( this->getCurveLength( curveID ) < _length ){
		T lenRatio = _length / this->getCurveLength( curveID );
		return this->getParamFromNormalizedParam( curveID, lenRatio );
	}

	s32 low = 1;
	s32 high = end;
	/* �e�[�u������2�������Ȃ��猟�� */
	while ( low < high ) {
		s32 mid = ( low + high ) / 2;
		T value = refParamLenTable [mid].y();
		if ( value <= _length ) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}
	/* ���o�����Z�O�����g���Ő��`�⊮�ɂ��Param�̎擾 */
	T Len = _length - refParamLenTable [low - 1].y();
	T ratio = Len * refParamLenTable [low-1].z();
	
	return refParamLenTable[low - 1].x() + ( refParamLenTable[low].x() - refParamLenTable[low - 1].x() ) * ratio;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �J�[�u�̒�������NormalizedParam���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getNormalizedParamFromLength(u32 curveID, T _length)const {
	assert(curveID < this->curveCount());
	T p = this->getParamFromLength ( curveID, _length );
	return this->getNormalizedParamFromParam ( curveID, p );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio����Param���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getParamFromLengthRatio ( u32 curveID, T _ratio )const {
	assert ( curveID < this->curveCount () );
	T ratio = _ratio;// Burgess2MathFn::Clamp( _ratio, T( 0.0 ), T( 1.0 ) );
	T rL = this->getCurveLength ( curveID ) * ratio;
	return this->getParamFromLength ( curveID, rL );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio����NormalizedParam���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getNormalizedParamFromLengthRatio ( u32 curveID, T _ratio )const {
	assert ( curveID < this->curveCount () );
	T ratio = _ratio;// Burgess2MathFn::Clamp( _ratio, T( 0.0 ), T( 1.0 ) );
	T rL = this->getCurveLength ( curveID ) * ratio;
	return this->getNormalizedParamFromLength ( curveID, rL );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Pram�n�_�܂ł̃J�[�u�̒������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getLengthAtParam(u32 curveID, T param)const {
	assert(curveID < this->curveCount());
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute(curveID, "paramLengthTable");

	s32 end = refParamLenTable.size();

	if (param <= T(BURGESS2::TOLERANCE )) {
		return T(0.0);
	}
	if (refParamLenTable[end - 1].x() - T(BURGESS2::TOLERANCE ) <= param) {
		return refParamLenTable[end - 1].y();
	}

	s32 low = 1;
	s32 high = end;

	/* �e�[�u������2�������Ȃ��猟�� */
	while (low < high) {
		s32 mid = (low + high) / 2;
		T value = refParamLenTable[mid].x();
		if (value <= param) {
			low = mid + 1;
		}
		else {
			high = mid;
		}
	}

	/* ���o�����Z�O�����g���Ő��`�⊮�ɂ��Length�̎擾 */
	f32 localParam = param - refParamLenTable[low - 1].x();
	f32 ratio = localParam / (refParamLenTable[low].x() - refParamLenTable[low - 1].x());
	return refParamLenTable[low - 1].y() + (refParamLenTable[low].y() - refParamLenTable[low - 1].y()) * ratio;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     NormalizedParam�n�_�܂ł̃J�[�u�̒������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getLengthAtNormalizedParam(u32 curveID, T nParam)const {
	assert(curveID < this->curveCount());
	T param = this->getParamFromNormalizedParam(curveID, nParam);
	return this->getLengthAtParam ( curveID, param );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     LengthRaio�n�_�܂ł̃J�[�u�̒������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getLengthAtLengthRatio ( u32 curveID, T ratio )const {
	assert ( curveID < this->curveCount () );
	T totalLen = this->getCurveLength ( curveID );
	return totalLen * ratio;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     length��ratio���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getLengthRatioFromLength ( u32 curveID, T length )const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return length * refParamLenTable [refParamLenTable.size ()-1].z();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Param�n�_�܂ł�LengthRatio���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getLengthRatioAtParam ( u32 curveID, T param )const {
	assert ( curveID < this->curveCount () );
	T pL = this->getLengthAtParam ( curveID, param );
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return pL * refParamLenTable[refParamLenTable.size()-1].z();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     NormalizedParam�n�_�܂ł�LengthRatio���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BSplineCurvesT<T>::getLengthRatioAtNormalizedParam ( u32 curveID, T nParam )const {
	assert ( curveID < this->curveCount () );
	T nPL = this->getLengthAtNormalizedParam ( curveID, nParam );
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return nPL * refParamLenTable [refParamLenTable.size ()-1].z();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�𓖕�������LengthRatio[]���擾����@0.0���܂߂邽�ߖ߂�l�̔z�񐔂�count+1
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::getEvenlyDivideLengthRatio ( u32 curveID, u32 count ) const {
	assert ( curveID < this->curveCount () );
	std::vector<T> result ( 0 );
	if ( count == 0 ) { return result; }

	T r = T(1.0) / T ( count );
	result.resize ( count + 1 );
	result [0] = T ( 0.0 );
	#pragma omp parallel for
		for ( s32 i = 1; i < result.size(); i++ ) {
			result [i] = r * i;
		}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�����Ԋu�ŕ�������LengthRatio[]���擾�@[0]=0.0
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::getRegularIntervalLengthRatio ( u32 curveID, f32 _length ) const {
	assert ( curveID < this->curveCount () );

	T totalLength = this->getCurveLength ( curveID );
	T invTotalLength = T ( 1.0 ) / totalLength;
	u32 count = totalLength / _length;
	std::vector<T> result ( count + 1 );
	result [0] = T ( 0.0 );
	#pragma omp parallel for
		for ( s32 i = 1; i <= result.size(); i++ ) {
			result [i] = _length * i * invTotalLength;
		}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�𓮓I�Ԋu�ŕ�������LengthRatio[]���擾�@[0]=0.0
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::getDynamicIntervalLengthRatio ( u32 curveID, f32 _length ) const {
	assert ( curveID < this->curveCount () );

	T totalLength = this->getCurveLength ( curveID );
	T invTotalLength = T ( 1.0 ) / totalLength;
	u32 count = totalLength / _length;
	T b = totalLength - ( _length * count );
	T a = b / T ( count );
	std::vector<T> result ( count + 1 );
	result [0] = T ( 0.0 );
	#pragma omp parallel for
		for ( s32 i = 1; i <= result.size(); i++ ) {
			result [i] = (_length * i + a * i ) * invTotalLength;
		}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�������_���Ԋu�ŕ�������LengthRatio[]���擾	[0]=0.0
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::getRandomLengthRatio ( u32 curveID, u32 count, u32 seed ) const {
	assert ( curveID < this->curveCount () );

	T totalLength = this->getCurveLength ( curveID );
	T invTotalLength = T ( 1.0 ) / totalLength;
	T length = totalLength / T ( count );
	std::vector<T> result ( count );
	#pragma omp parallel for
		for ( s32 i = 1; i < result.size (); i++ ) {
			T val = Burgess2MathFn::RandomT ( seed + i * 7, i * 108 + 15, length*i, length*( i + 1 ) );
			result [i] = val * invTotalLength;
		}
	return result;
}