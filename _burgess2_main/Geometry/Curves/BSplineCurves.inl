//------------------------------------------------------------------------------------------------------------------------------------------------
//!		BSplineCurve��ǉ�����
//!
//!		points: LoopCurve(form��1or2)�̏ꍇ�A�d�����_�͓����Ŏ����I�ɍ쐬����邽�߈���"points"�ɂ͊܂߂Ȃ�<br>
//!		knots: ��Fpoints.size() = 4, degree = 2<br>
//!			open : knots = { 0, 0, 0, 1, 2, 2, 2 } /// points.size + degree + 1<br>
//!			close : knots = { 0, 0, 0, 1, 2, 3, 3, 3 } /// points.size + degree + 2<br>
//!			periodic : knots = { 0, 1, 2, 3, 4, 5, 6, 7, 8 } /// point.size + (degree * 2) + 1 <br>
//!		@param[in] points �J�[�u���_�iLoopCurve�ł��d�����_�͊܂߂Ȃ��j
//!		@param[in] degree ����
//!		@param[in] form 0=open, 1=close, 2=periodic
//!		@param[in] knots �m�b�g�x�N�g��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 BSplineCurvesT<T>::add ( const std::vector<Vec3T<T>> &points, u8 degree, u8 form, const std::vector<T> &knots ) {
	assert ( 2 == degree || 3 == degree );
	assert ( form < 3 );
	assert ( 3 <= points.size () );
	switch (form) {
		case 0:	assert(knots.size() == points.size() + degree + 1);	break;
		case 1:	assert(knots.size() == points.size() + degree + 2);	break;
		case 2:	assert(knots.size() == points.size() + degree*2 + 1); break;
	}

	u32 id = this->curveCount ();
	this->setCurveCount ( this->curveCount ()+1 );

	this->data [id].setForm ( form );
	this->data [id].v [0] = degree;

	/* ���_�ʒu�̓o�^�iLoop�̏ꍇ�͏d�����_���ݒ肷��jAttribute.v[0]=�d�����_�� */
	Vec3AttributeT<T> &v3Attr = this->getPointPositionAttribute ( id ); /* = Vec3Attribute "pointPositions" */
	v3Attr.resetValues ( points );
	u32 cnt = u32(points.size ());
	switch ( form ) {
		case 0: 
			v3Attr.v [0] = 0; break;
		case 1: 
			v3Attr.resize ( cnt + 1 );
			v3Attr [cnt] = v3Attr [0];
			v3Attr.v [0] = 1; break;
		case 2: 
			v3Attr.resize ( cnt + degree + 1 );
			for ( u8 i = 0; i<degree+1; i++ ) {
				v3Attr [cnt + i] = v3Attr [i];
			}
			v3Attr.v [0] = degree + 1; break;
	}

	/* �m�b�g�x�N�g���̓o�^ */
	F32AttributeT<T> & knotAttr = this->getOrCreateF32Attribute ( id, "knotVector" );
	knotAttr.resetValues(knots);

	/* Param��Length�̕ϊ��p�e�[�u�� */
	this->getOrCreatVec3Attribute ( id, "paramLengthTable" );
	this->incrementParamLengthTableVirsion ( id );
	this->recomputeParamLengthTable ( id );

	/*NormalValue�Ƀ_�~�[�f�[�^���Z�b�g*/
	Vec3AttributeT<T> &v3AttrD = this->getVec3Attribute ( id, "normals" );
	v3AttrD.resize ( 2 );
	v3AttrD [0] = Vec3T<T> ( T(0.001), 0, 1 );
	v3AttrD [1] = Vec3T<T> ( T(-0.001), 0, 1 );

	return id;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		BSplineCurve��ǉ����� �iknot�͎��������j
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 BSplineCurvesT<T>::add(const std::vector<Vec3T<T>> &points, u8 degree, u8 form) {
	std::vector<T> k = this->createBasicKnotVector(degree, u32(points.size()), form);
	return this->add(points, degree, form, k);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		KnotVector�𐶐�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::createBasicKnotVector(u8 degree, u32 pointSize, u8 form) {
	u32 s = 0;
	switch (form) {
		case 0:	s = pointSize + degree + 1; break;
		case 1:	s = pointSize + degree + 2; break;
		case 2:	s = pointSize + degree*2 + 1; break;
	}
	std::vector<T> result(s);
	if (form == 2) {
		for (u32 i = 0; i < s; i++) { result[i] = T(i); }
	}else{
		T k = 0;
		for (u32 i = 0; i < s; i++) {
			result[i] = k;
			if (degree <= i && i <= s - degree - 2) {
				k++;
			}
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		�������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u8 BSplineCurvesT<T>::getDegree ( u32 curveID ) const{
	assert ( curveID < this->curveCount () );
	return this->data [curveID].v[0];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		�m�b�g�x�N�^�[���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> BSplineCurvesT<T>::getKnotVector ( u8 curveID ) const {
	assert ( curveID < this->curveCount () );
	F32AttributeT<T> &refKnotAttr = this->getF32Attribute ( curveID, "knotVector" );
	return refKnotAttr.getValues();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		�J�[�u�𕡐�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 BSplineCurvesT<T>::duplicate ( u32 curveID ) {
	return this->copyCurveFrom ( *this, curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		�J�[�u��S�ăR�s�[����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 BSplineCurvesT<T>::copyFrom ( const BSplineCurvesT<T> &src ) {
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
u32 BSplineCurvesT<T>::copyCurveFrom ( const BSplineCurvesT<T> &src, u32 curveID ) {
	assert ( curveID < src.curveCount () );
	u32 id = this->curveCount ();
	this->setCurveCount ( this->curveCount ()+1 );

	this->data [id].setForm ( src.getForm ( curveID ) );
	this->data [id].v [0] = src.getDegree ( curveID );

	Vec3AttributeT<T> & refParamLen = src.getVec3Attribute ( curveID, "paramLengthTable" );
	Vec3AttributeT<T> & paramLenAttr = this->getOrCreatVec3Attribute ( id, "paramLengthTable" );
	paramLenAttr.resetValues ( refParamLen.getValues () );

	Vec3AttributeT<T> & refPoiPos = src.getPointPositionAttribute ( curveID );
	this->resetAllPointPositions ( id, refPoiPos.getValues() );

	F32AttributeT<T> & refKnot = src.getF32Attribute ( curveID, "knotVector" );
	F32AttributeT<T> & knotAttr = this->getOrCreateF32Attribute ( id, "knotVector" );
	knotAttr.resetValues ( refKnot.getValues() );

	this->paramLengthTableCached ( id );
	return id;
}