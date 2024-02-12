//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�Z�N�V�����ƃC���^�[�o���𔽓]����@��F[0.0, 0.2][0.8, 0.9]�@���@[0.2, 0.8][0.9, 1.0]
//!		���]�ł���͈͂��Ȃ��ꍇ�͏��������s���Ȃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool CurveSectionsT<T>::flip (){
	if ( this->isValid () == false ) { return false; }
	
	CurveSectionsT<T> rest = *this;
	this->resize (0);

	/* section�̏��͑O���珇�Ԃɕۑ�����Ă��Ȃ��\�������邽�߁A 0.0,1.0�ȊO�̑S�Ă̒l����Urest�Ɋi�[���Ă���*/
	/* rest���\�[�g������ɑO���珇��Vec2�ɂ��Ă��� */
	std::vector<f32> restXY;
	for ( u32 j = 0; j<rest.sectionCount (); j++ ) {
		Vec2T<T> v = rest.getValue ( j );
		if ( v.almostEqual ( Vec2T<T> ( 0.0, 1.0 ) ) ) { break; }
		if ( j == 0 && v.x != T(0.0) ) { restXY.push_back ( T(0.0) ); }
		if ( v.x != T ( 0.0 ) ) { restXY.push_back ( v.x ); }
		if ( v.y != T ( 1.0 ) ) { restXY.push_back ( v.y ); }
		if ( j == rest.sectionCount () - 1 && v.y != T(1.0) ) { restXY.push_back ( T(1.0) ); }
		Burgess2MathFn::Sort ( restXY );
	}
	for ( u32 j = 0; j < restXY.size () / 2; j++ ) {
		if ( restXY [j * 2]<T(0.0) || restXY [j * 2 + 1]<T(0.0) || 1.0<restXY [j * 2] || 1.0<restXY [j * 2 + 1] 
			|| restXY [j * 2 + 1] < restXY [j * 2] || abs ( restXY [j * 2]-restXY [j * 2 + 1] )<BURGESS2::TOLERANCE ) {
			continue;
		}
		this->add ( restXY [j * 2], restXY [j * 2 + 1] );
	}
	if ( this->sectionCount () == 0 ) { return false; }
	this->setDirty ();
	return this->isValid ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    @brief �J�[�u�Z�N�V�������g�k����
//!		@param[in] scale section�̊g�k�l
//!		@param[in] clamp section�̒l��0.0~1.0�ɃN�����v����ꍇ��true
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurveSectionsT<T>::scaling ( u32 sectionID, T scale, Bool clamp ) {
	assert ( sectionID < this->sectionCount () );
	f32 val = this->values [sectionID].y - this->values [sectionID].x;
	f32 addVal = val * ( scale - T(1.0) );
	if ( clamp ) {
		this->values [sectionID].x = Burgess2MathFn::Clamp ( this->values [sectionID].x - addVal * T ( 0.5 ), T ( 0.0 ), T ( 1.0 ) );
		this->values [sectionID].y = Burgess2MathFn::Clamp ( this->values [sectionID].y + addVal * T ( 0.5 ), T ( 0.0 ), T ( 1.0 ) );
	} else {
		this->values [sectionID].x -= addVal * T ( 0.5 );
		this->values [sectionID].y += addVal * T ( 0.5 );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    @brief �S�ẴJ�[�u�Z�N�V�������g�k����
//!		@param[in] scale section�̊g�k�l
//!		@param[in] clamp section�̒l��0.0~1.0�ɃN�����v����ꍇ��true
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurveSectionsT<T>::allScaling ( T scale, Bool clamp ) {
	for ( int i = 0; i < this->sectionCount (); i++ ) {
		this->scaling ( i, scale, clamp );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    @brief �J�[�u�Z�N�V�����ɒl�����Z����
//!		@param[in] startSide section�̃X�^�[�g�ɉ��Z����l
//!		@param[in] endSide section�̃G���h�ɉ��Z����l
//!		@param[in] clamp section�̒l��0.0~1.0�ɃN�����v����ꍇ��true
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurveSectionsT<T>::addStartEndValue ( u32 sectionID, T startSide, T endSide, Bool clamp ) {
	if ( clamp ) {
		this->values [sectionID].x = Burgess2MathFn::Clamp ( this->values [sectionID].x - startSide, T ( 0.0 ), T ( 1.0 ) );
		this->values [sectionID].y = Burgess2MathFn::Clamp ( this->values [sectionID].y + endSide, T ( 0.0 ), T ( 1.0 ) );
	} else {
		this->values [sectionID].x -= startSide;
		this->values [sectionID].y += endSide;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    @brief �S�ẴJ�[�u�Z�N�V�����ɒl�����Z����
//!		@param[in] startSide section�̃X�^�[�g�ɉ��Z����l
//!		@param[in] endSide section�̃G���h�ɉ��Z����l
//!		@param[in] clamp section�̒l��0.0~1.0�ɃN�����v����ꍇ��true
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurveSectionsT<T>::addAllStartEndValues ( T startSide, T endSide, Bool clamp ) {
	for ( u32 i = 0; i < this->sectionCount (); i++ ) {
		this->addValue ( i, startSide, endSide, clamp );
	}
}