//------------------------------------------------------------------------------------------------------------------------------------------------
//!		NURBSCurve��ǉ�����
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
//!		@param[in] weights ���_�ւ̃E�F�C�g�l�i0.0~1.0�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 NURBSCurvesT<T>::add(const std::vector<Vec3T<T>> &points, u8 degree, u8 form, const std::vector<T> &knots, const std::vector<f32> &weights) {
	assert ( 2 == degree || 3 == degree );
	assert ( form < 3 );
	assert ( 3 <= points.size () );
	assert ( points.size () == weights.size () );
	switch ( form ) {
		case 0:	assert ( knots.size () == points.size () + degree + 1 );	break;
		case 1:	assert ( knots.size () == points.size () + degree + 2 );	break;
		case 2:	assert ( knots.size () == points.size () + degree*2 + 1 );	break;
	}

	u32 id = this->curveCount ();
	this->setCurveCount ( this->curveCount ()+1 );
	this->data [id].setForm ( form );
	this->data [id].v [0] = degree;

	/* �E�F�C�g�̓o�^ �iLoop�̏ꍇ�͏d�����_���ݒ肷��j*/
	F32AttributeT<T> & weightAttr = this->getOrCreateF32Attribute ( id, "rationalWeight" );
	weightAttr.resetValues ( weights );
	/* ���_�ʒu�̓o�^�iLoop�̏ꍇ�͏d�����_���ݒ肷��jAttribute.v[0]=�d�����_�� */
	Vec3AttributeT<T> &v3Attr = this->getPointPositionAttribute ( id ); /* = Vec3Attribute "pointPositions" */
	v3Attr.resetValues ( points );
	u32 cnt = u32(points.size ());
	switch ( form ) {
	case 0:
		v3Attr.v [0] = 0; break;
	case 1:
		weightAttr.resize ( cnt + 1 );
		weightAttr [cnt] = weightAttr [0];
		v3Attr.resize ( cnt + 1 );
		v3Attr [cnt] = v3Attr [0];
		v3Attr.v [0] = 1; break;
	case 2:
		weightAttr.resize ( cnt + degree + 1 );
		for ( u8 i = 0; i<degree+1; i++ ) {
			weightAttr [cnt + i] = weightAttr [i];
		}
		v3Attr.resize ( cnt + degree + 1 );
		for ( u8 i = 0; i<degree+1; i++ ) {
			v3Attr [cnt + i] = v3Attr [i];
		}
		v3Attr.v [0] = degree + 1; break;
	}
	
	/* �m�b�g�x�N�g���̓o�^ */
	F32AttributeT<T> & knotAttr = this->getOrCreateF32Attribute ( id, "knotVector" );
	knotAttr.resetValues ( knots );

	/* Param��Length�̕ϊ��p�e�[�u�� */
	this->getOrCreatVec3Attribute ( id, "paramLengthTable" );
	this->incrementParamLengthTableVirsion ( id );
	this->recomputeParamLengthTable ( id );
	return id;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		NURBSCurve��ǉ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 NURBSCurvesT<T>::add ( const std::vector<Vec3T<T>> &points, u8 degree, u8 form, const std::vector<f32> &weights ) {
	std::vector<T> knot = this->createBasicKnotVector ( degree, points.size(), form );
	return this->add ( points, degree, form, knot, weights );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Weight��ݒ肷��i������Vec3�z��ɂ�Loop�̏d�����_�͊܂߂Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void NURBSCurvesT<T>::setAllRationalWeight ( u32 curveID, const std::vector<f32> &weights ) {
	assert ( curveID < this->curveCount () );
	assert ( weights.size () == this->pointCount ( curveID ) );
	F32AttributeT<T> & weightAttr = this->createF32Attribute ( curveID, "rationalWeight" );
	for ( u32 i = 0; i<weights.size (); i++ ) {
		weightAttr.setValue ( i, weights [i] );
	}

	/// �d�����_���X�V����
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	u32 c = this->pointCount ( curveID );
	for ( u8 i = 0; i<v3Attr.v [0]; i++ ) {
		weightAttr [c+i] = weightAttr [i];
	}
	this->incrementParamLengthTableVirsion ( curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Weight��ݒ肷��i������pointID��Loop�̏d�����_ID�͎w��ł��Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void NURBSCurvesT<T>::setRationalWeight ( u32 curveID, u32 pointID, f32 weight ) {
	assert ( curveID < this->curveCount () );
	assert ( pointID < this->pointCount ( curveID ) );
	F32AttributeT<T> & weightAttr = this->createF32Attribute ( curveID, "rationalWeight" );
	weightAttr.setValue ( pointID, weight );

	/// �d�����_���X�V����
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	if ( pointID < v3Attr.v [0] ) {
		u32 c = this->pointCount ( curveID );
		weightAttr [c+pointID] = weightAttr [pointID];
	}
	this->incrementParamLengthTableVirsion ( curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Weight���擾����i�߂�l�ɂ�Loop�̏d�����_�͊܂܂�Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<f32> NURBSCurvesT<T>::getAllRationalWeight ( u32 curveID )const {
	F32AttributeT<T> & weightAttr = this->createF32Attribute ( curveID, "rationalWeight" );
	std::vector<f32> result = weightAttr.getValues();
	if ( this->getForm ( curveID ) != 0 ) {
		u32 cnt = this->pointCount ( curveID ); /// �d�����_�����Ȃ������_�����擾
		result.resize ( cnt );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Weight���擾����i������pointID��Loop�̏d�����_ID�͎w��ł��Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
f32 NURBSCurvesT<T>::getRationalWeight ( u32 curveID, u32 pointID )const {
	assert ( curveID < this->curveCount () );
	assert ( pointID < this->pointCount ( curveID ) );
	F32AttributeT<T> & weightAttr = this->createF32Attribute ( curveID, "rationalWeight" );
	return weightAttr.getValue ( pointID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param�l�̈ʒu���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> NURBSCurvesT<T>::getPositionAtParam ( u32 curveID, T _param )const {
	Vec3T<T> result;
	assert ( curveID < this->curveCount () );

	u8 degree = this->getDegree ( curveID );
	u32 poiCount = this->pointCount ( curveID );
	assert ( 2 == degree || 3 == degree );
	assert ( 3 <= poiCount );

	T periodicOffset = this->isPeriodic ( curveID ) ? T ( this->getDegree ( curveID ) ) : 0;
	T param = _param + periodicOffset;
	Vec3AttributeT<T> &refPoiAttr = this->getPointPositionAttribute ( curveID );
	F32AttributeT<T> &refKnotAttr = this->getF32Attribute ( curveID, "knotVector" );
	F32AttributeT<T> &refWeightAttr = this->getF32Attribute ( curveID, "rationalWeight" );

	u8 domainA = degree;
	u8 domainB = refKnotAttr.size () - 1 - degree;

	if ( param < refKnotAttr [domainA] || refKnotAttr [domainB] < param ) { return result; }

	/// s = segmentIndex
	u8 s;
	for ( s = domainA; s < domainB; s++ ) {
		if ( refKnotAttr [s] <= param && param <= refKnotAttr [s + 1] ) {
			break;
		}
	}

	std::vector<Vec3T<T>> V = refPoiAttr.getValues ();
	std::vector<f32> W = refWeightAttr.getValues ();
	for ( u8 h = 1; h <= degree + 1; h++ ) {
		for ( u8 i = s; i > s - degree - 1 + h; i-- ) {
			T alpha = ( param - refKnotAttr [i] ) / ( refKnotAttr [i + degree + 1 - h] - refKnotAttr [i] );
			V [i] = ( T ( 1.0 ) - alpha ) * V [i - 1] + alpha * V [i];
			W [i] = ( T ( 1.0 ) - alpha ) * W [i-1] + alpha * W [i];
		}
	}
	return V [s] * W [s];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		�J�[�u���R�s�[����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 NURBSCurvesT<T>::copyCurveFrom ( const NURBSCurvesT<T> &src, u32 curveID ) {
	u32 id = BSplineCurves::copyCurveFrom ( src, curveID );

	F32AttributeT<T> & refWeight = src.getVec3Attribute ( curveID, "rationalWeight" );
	F32AttributeT<T> & weightAttr = this->createF32Attribute ( id, "rationalWeight" );
	weightAttr.resetValues ( refWeight.getValues () );

	return id;
}