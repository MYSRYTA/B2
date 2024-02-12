//------------------------------------------------------------------------------------------------------------------------------------------------
//!		LinearCurve��ǉ�����
//!
//!		points: LoopCurve(form��1or2)�̏ꍇ�A�d�����_�͓����Ŏ����I�ɍ쐬����邽�߈���"points"�ɂ͊܂߂Ȃ�<br>
//!		@param[in] points �J�[�u���_�iLoopCurve�ł��d�����_�͊܂߂Ȃ��j
//!		@param[in] form 0=open, 1=close, 2=periodic
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 LinearCurvesT<T>::add(const std::vector<Vec3T<T>> &points, u8 form){
	assert ( form < 3 );
	assert ( 2 <= points.size () );

	u32 id = this->curveCount ();
	this->setCurveCount ( this->curveCount ()+1 );

	this->data [id].setForm ( form );

	/* ���_�ʒu�̓o�^�iLoop�̏ꍇ�͏d�����_���ݒ肷��jAttribute.v[0]=�d�����_�� */
	Vec3AttributeT<T> &v3Attr = this->getPointPositionAttribute ( id ); /* = Vec3Attribute "pointPositions" */
	v3Attr.resetValues ( points );
	u32 cnt = u32(points.size ());
	if ( form == 0 ) {
		v3Attr.v [0] = 0;
	}else{
		v3Attr.resize ( cnt + 1 );
		v3Attr [cnt] = v3Attr [0];
		v3Attr.v [0] = 1; 
	}

	/* Param��Length�̕ϊ��p�e�[�u�� */
	this->getOrCreatVec3Attribute ( id, "paramLengthTable" );
	this->incrementParamLengthTableVirsion ( id );
	this->recomputeParamLengthTable ( id );

	/*NormalValue�Ƀ_�~�[�f�[�^���Z�b�g*/
	Vec3AttributeT<T> &v3AttrD = this->getVec3Attribute ( id, "normals" );
	v3AttrD.resize ( 2 );
	v3AttrD [0] = Vec3T<T>  ( T(0.001), 0, 1 );
	v3AttrD [1] = Vec3T<T>  ( T(-0.001), 0, 1 );

	return id;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		�o�E���f�B���O�{�b�N�X��XYZ���̂ǂꂩ�Ɠ��������̃��j�A�J�[�u��ǉ�����
//!
//!		�o�E���f�B���O�{�b�N�X��XYZ���̂ǂꂩ�Ɠ��������̃��j�A�J�[�u��ǉ�����
//!		@param[in] bBox �J�[�u���_�iLoopCurve�ł��d�����_�͊܂߂Ȃ��j
//!		@param[in] axis 0=X, 1=Y, 2=Z
//!		@param[in] offsetPos ���j�A�J�[�u�𐶐�����ʒu�̃I�t�Z�b�g
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 LinearCurvesT<T>::add ( const BBoxT<T> &bBox, u8 axis, const Vec3T<T> offsetPos ) {
	std::vector<Vec3T<T>> points ( 2 );
	points [0] = offsetPos;
	points [1] = offsetPos;
	switch ( axis ) {
		case 0:
			points [0].x() = bBox.min.x();
			points [1].x() = bBox.max.x();
			break;
		case 1:
			points [0].y() = bBox.min.y();
			points [1].y() = bBox.max.y();
			break;
		default:
			points [0].z() = bBox.min.z();
			points [1].z() = bBox.max.z();
	}
	return this->add ( points, 0 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		�J�[�u�𕡐�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 LinearCurvesT<T>::duplicate ( u32 curveID ) {
	return this->copyCurveFrom ( *this, curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		�J�[�u��S�ăR�s�[����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 LinearCurvesT<T>::copyFrom ( const LinearCurvesT<T> &src ) {
	u32 id = this->curveCount ();
	if ( src.curveCount () == 0 ) { return id -1; }
	for ( u32 i = 0; i<src.curveCount (); i++ ) {
		this->copyCurveFrom ( src, i );
	}
	return id;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		�J�[�u���R�s�[����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 LinearCurvesT<T>::copyCurveFrom ( const LinearCurvesT<T> &src, u32 curveID ) {
	assert ( curveID < src.curveCount () );
	u32 id = this->curveCount ();
	this->setCurveCount ( this->curveCount ()+1 );

	this->data [id].setForm ( src.getForm ( curveID ) );

	Vec3AttributeT<T> & refPoiPos = src.getPointPositionAttribute ( curveID );
	this->resetAllPointPositions ( id, refPoiPos.getValues () );

	Vec3AttributeT<T> & refParamLen = src.getVec3Attribute ( curveID, "paramLengthTable" );
	Vec3AttributeT<T> & paramLenAttr = this->getOrCreatVec3Attribute ( id, "paramLengthTable" );
	paramLenAttr.resetValues ( refParamLen.getValues () );

	this->paramLengthTableCached ( id );
	return id;
}