//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Attribute"paramLengthTable"�̍X�V���K�v�ȏꍇ�͍X�V����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void LinearCurvesT<T>::recomputeParamLengthTable ( u32 curveID ) {
	if ( this->paramLengthTableIsDirty ( curveID ) ) {
		Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
		Vec3AttributeT<T> &refPoiPosition = this->getPointPositionAttribute (curveID );
		
		refParamLenTable.resize ( refPoiPosition.size() );
		refParamLenTable [0] = Vec3T<T> (0.0);
		for ( u32 i = 1; i<refParamLenTable.size (); i++ ) {
			refParamLenTable [i].x() = T ( i );
			refParamLenTable [i].y() = refParamLenTable [i-1].y() + refPoiPosition [i].distanceTo ( refPoiPosition [i-1] );
		}

		for ( u32 i = 0; i < refParamLenTable.size ()-1; i++ ) {
			refParamLenTable [i].z() = T ( 1.0 ) / ( refParamLenTable [i+1].y() - refParamLenTable [i].y() );
		}
		refParamLenTable [refParamLenTable.size ()-1].z() = T ( 1.0 ) / refParamLenTable [refParamLenTable.size ()-1].y();
		this->paramLengthTableCached ( curveID );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �J�[�u�̒������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getCurveLength ( u32 curveID ) {
	assert ( curveID < this->curveCount () );
	if ( this->paramLengthTableIsDirty ( curveID ) ) {
		this->recomputeParamLengthTable ( curveID );
	}
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return refParamLenTable [refParamLenTable.size ()-1].y();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �J�[�u�̒������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getCurveLength ( u32 curveID )const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T> &refParamLengthTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return refParamLengthTable [refParamLengthTable.size ()-1].y();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �J�[�u�̒�������Pram���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getParamFromLength ( u32 curveID, T _length )const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );

	s32 end = refParamLenTable.size ();

	if ( _length <= T ( BURGESS2::TOLERANCE ) ) {
		return T ( 0.0 );
	}
	if ( refParamLenTable [end - 1].y() - T ( BURGESS2::TOLERANCE ) <= _length ) {
		return refParamLenTable [end - 1].x();
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
	f32 Len = _length - refParamLenTable [low - 1].y();
	f32 ratio = Len * refParamLenTable [low-1].z();
	return refParamLenTable [low - 1].x() + ( refParamLenTable [low].x() - refParamLenTable [low - 1].x() ) * ratio;
}


//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �J�[�u�̒�������NormalizedParam���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getNormalizedParamFromLength ( u32 curveID, T _length )const {
	assert ( curveID < this->curveCount () );
	T totalLength = this->getCurveLength ( curveID );
	return _length / totalLength;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio����Param���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getParamFromLengthRatio ( u32 curveID, T _ratio )const {
	assert ( curveID < this->curveCount () );
	T ratio = Burgess2MathFn::Clamp ( _ratio, T ( 0.0 ), T ( 1.0 ) );
	T rL = this->getCurveLength ( curveID ) * ratio;
	return this->getParamFromLength ( curveID, rL );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    LengthRatio����NormalizedParam���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getNormalizedParamFromLengthRatio ( u32 curveID, T _ratio )const {
	assert ( curveID < this->curveCount () );
	T ratio = Burgess2MathFn::Clamp ( _ratio, T ( 0.0 ), T ( 1.0 ) );
	T rL = this->getCurveLength ( curveID ) * ratio;
	return this->getNormalizedParamFromLength ( curveID, rL );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Pram�n�_�܂ł̃J�[�u�̒������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getLengthAtParam ( u32 curveID, T param )const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );

	s32 end = refParamLenTable.size ();

	if ( param <= T ( BURGESS2::TOLERANCE ) ) {
		return T ( 0.0 );
	}
	if ( refParamLenTable [end - 1].x() - T ( BURGESS2::TOLERANCE ) <= param ) {
		return refParamLenTable [end - 1].y();
	}

	s32 low = 1;
	s32 high = end;

	/* �e�[�u������2�������Ȃ��猟�� */
	while ( low < high ) {
		s32 mid = ( low + high ) / 2;
		T value = refParamLenTable [mid].x();
		if ( value <= param ) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}

	/* ���o�����Z�O�����g���Ő��`�⊮�ɂ��Length�̎擾 */
	f32 localParam = param - refParamLenTable [low - 1].x();
	f32 ratio = localParam / ( refParamLenTable [low].x() - refParamLenTable [low - 1].x() );
	return refParamLenTable [low - 1].y() + ( refParamLenTable [low].y() - refParamLenTable [low - 1].y() ) * ratio;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     NormalizedParam�n�_�܂ł̃J�[�u�̒������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getLengthAtNormalizedParam ( u32 curveID, T nParam )const {
	assert ( curveID < this->curveCount () );
	T param = this->getParamFromNormalizedParam ( curveID, nParam );
	return this->getLengthAtParam ( curveID, param );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Param�n�_�܂ł�LengthRatio���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getLengthRatioAtParam ( u32 curveID, T param )const {
	assert ( curveID < this->curveCount () );
	T pL = this->getLengthAtParam ( curveID, param );
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return pL * refParamLenTable [refParamLenTable.size ()-1].z();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     NormalizedParam�n�_�܂ł�LengthRatio���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinearCurvesT<T>::getLengthRatioAtNormalizedParam ( u32 curveID, T nParam )const {
	assert ( curveID < this->curveCount () );
	T nPL = this->getLengthAtNormalizedParam ( curveID, nParam );
	Vec3AttributeT<T> &refParamLenTable = this->getVec3Attribute ( curveID, "paramLengthTable" );
	return nPL * refParamLenTable [refParamLenTable.size ()-1].z();
}