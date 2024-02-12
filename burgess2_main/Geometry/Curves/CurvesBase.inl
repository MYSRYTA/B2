//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̒��_���i�z�񐔁A�ʒu���j��S�ď���������iLoop�̏d�����_���܂߂�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::resetAllPointPositions ( u32 curveID, const std::vector<Vec3T<T>> &positions ) {
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	v3Attr.resetValues ( positions );
	this->incrementParamLengthTableVirsion ( curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̑S�Ă̒��_�ʒu��ݒ肷��i������Vec3�z��ɂ�Loop�̏d�����_�͊܂߂Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::setAllPointPositions ( u32 curveID, const std::vector<Vec3T<T>> &positions ) {
	assert ( curveID < this->curveCount () );
	assert ( positions.size() == this->pointCount ( curveID ) );
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	for ( u32 i = 0; i<positions.size (); i++ ) {
		v3Attr.setValue ( i, positions[i] );
	}

	/// �d�����_���X�V����
	u32 c = this->pointCount ( curveID );
	for ( u8 i = 0; i<v3Attr.v [0]; i++ ) {
		v3Attr [c+i] = v3Attr [i];
	}
	this->incrementParamLengthTableVirsion ( curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̒��_�ʒu��ݒ肷��i������index��Loop�̏d�����_ID�͎w��ł��Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::setPointPosition ( u32 curveID, u32 index, const Vec3T<T> & position ) {
	assert ( curveID < this->curveCount () );
	assert ( index < this->pointCount ( curveID ) );
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	v3Attr.setValue ( index, position );

	/// �d�����_���X�V����
	if ( index < v3Attr.v [0] ) {
		u32 c = this->pointCount ( curveID );
		v3Attr [c+index] = v3Attr [index];
	}
	this->incrementParamLengthTableVirsion ( curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u��NormalValue�i�z�񐔁A�������j��S�ď���������iLoop�̏d�����_���܂߂�j
//!	    @brief �J�[�u��NormalValue�i�z�񐔁A�������j��S�ď���������iLoop�̏d�����_���܂߂�j
//!		@param[in] curveID �ΏۂƂ���J�[�uID
//!		@param[in] normal �@�����i�J�[�u��̋ϓ��ȋ����ɂ�����@���j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::resetAllNormals ( u32 curveID, const std::vector<Vec3T<T>> &normal ) {
	assert ( curveID < this->curveCount () );
	assert ( 2 <= normal.size () );
	Vec3AttributeT<T>& v3Attr = this->getNormalAttribute ( curveID );
	v3Attr.resetValues ( normal );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u��NormalValue��o�^����@�Q�ƌ�:MJCG compound
//!	    @brief �J�[�u��NormalValue��o�^����
//!		@param[in] curveID �ΏۂƂ���J�[�uID
//!		@param[in] sample ���x�i�J�[�u���_��*sample�ŃJ�[�u���ϓ���������PTF�����s���邱�ƂɂȂ�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::computeNormals(u32 curveID, u32 sample) {
	assert(curveID < this->curveCount());
	s32 s = this->pointCount(curveID) * (sample <= 1 ? 1 : sample);
	T fraction = T(1.0) / T(s);

	std::vector<Vec3T<T>> normal(s, Vec3T<T>(0,1,0));
	#pragma omp parallel for if(300 < s)
	for (u32 i = 0; i < s; i++) {
		T r, fr, br;
		if(i==0){ 
			r = T(0.01);
			fr = T(0.0);
			br = T(0.02);
		}else if (i == s - 1) {
			r = T(0.99);
			fr = T(0.98);
			br = T(1.0);
		} else {
			r = fraction * i;
			fr = fraction * i - T(0.01);
			br = fraction * i + T(0.01);
		}
		Vec3T<T> rPos = this->getPositionAtLengthRatio(curveID, r);
		Vec3T<T> frPos = this->getPositionAtLengthRatio(curveID, fr);
		Vec3T<T> brPos = this->getPositionAtLengthRatio(curveID, br);
		Vec3T<T> ddx = (brPos - rPos).unit();
		Vec3T<T> dx = (frPos - rPos).unit();
		if (ddx.almostParallel(dx) == false) {
			normal[i] = ddx.cross(dx);
			normal[i].setUnit();
		}
		if (ddx.almostEqual(normal[i])) {
			normal[i] = Vec3T<T>(0, 0, 1);
		}
	}
	
	this->resetAllNormals(curveID, normal);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PTF�ŎZ�o�����J�[�u��NormalValue��o�^����@�Q�ƌ�:MJCG compound / https://janakiev.com/blog/framing-parametric-curves/
//!	    @brief PTF�ŎZ�o�����J�[�u��NormalValue��o�^����
//!		@param[in] curveID �ΏۂƂ���J�[�uID
//!		@param[in] upvector �J�[�u�̍��{�̖@���iPTF�̎n�_�Ƃ��Ă��g�p�j
//!		@param[in] sample ���x�i�J�[�u���_��*sample�ŃJ�[�u���ϓ���������PTF�����s���邱�ƂɂȂ�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::computePTFNormals ( u32 curveID, const Vec3T<T> &upvector, u32 sample ){
	assert ( curveID < this->curveCount () );
	u32 s = this->pointCount ( curveID ) * ( sample <= 1 ? 1 : sample );
	T fraction = T ( 1.0 ) / T ( s );

	std::vector<Vec3T<T>> normal ( s );
	normal [0] = upvector.unit();
	Vec3T<T> rest = normal[0];
	for (u32 i = 1; i < s; i++){
		Vec3T<T> tan = this->getTangentAtLengthRatio ( curveID, fraction * i );
		if ( rest.almostParallel ( tan ) ){ tan = ( tan + Vec3T<T> ( T ( 0.01 ) ) ).unit (); } // �A�b�v�x�N�^�[�ƃ^���W�F���g�����s�̏ꍇ��0.001�̌덷������
		normal [i] = tan.cross ( rest ).cross ( tan );
		assert ( normal [i].isNotZeroLength () );
		normal [i].setNormal ();
		rest = normal[i];
	}
	if (this->isClose ( curveID )){ normal.push_back ( upvector ); }

	this->resetAllNormals ( curveID, normal );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    new! PTF�ŎZ�o�����J�[�u��NormalValue��o�^���� �Q�ƌ�:MJCG compound
//!	    @brief new! PTF�ŎZ�o�����J�[�u��NormalValue��o�^����
//!		@param[in] curveID �ΏۂƂ���J�[�uID
//!		@param[in] upvector �J�[�u�̍��{�̖@���iPTF�̎n�_�Ƃ��Ă��g�p�j
//!		@param[in] sample ���x�i�J�[�u���_��*sample�ŃJ�[�u���ϓ���������PTF�����s���邱�ƂɂȂ�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::computePTFNormals2(u32 curveID, const Vec3T<T>& upvector, u32 sample) {
	assert(curveID < this->curveCount());
	u32 s = this->pointCount(curveID) * (sample <= 1 ? 1 : sample);
	
	T fraction = T(1.0) / T(s);
	std::vector<Vec3T<T>> restTangents(s);
	for (u32 i = 0; i < s; i++) {
		restTangents[i] = this->getTangentAtLengthRatio(curveID, fraction * i);
	}

	std::vector<Vec3T<T>> normal(s); 
	Vec3T<T> rest = upvector.unit();
	Vec3T<T> c = restTangents[0].cross(rest);
	rest = (c.cross(restTangents[0])).unit();
	normal[0] = rest;
	for (u32 i = 1; i < s; i++) {
		Vec3T<T> tan = restTangents[i-1];
		Vec3T<T> tan2 = restTangents[ i ];
		if (tan.almostParallel(tan2)) { tan = (tan + Vec3T<T>(T(0.01))).unit(); }
		Vec3T<T> c = tan.cross(tan2);
		QuatT<T> q;
		q.setFromAxisAndAngle(c, std::acos(tan.dot(tan2)));
		normal[i] = q.rotatVector(rest);
		rest = normal[i];
	}
	if (this->isClose(curveID) ) {	normal.push_back(normal[0]); } 

	this->resetAllNormals(curveID, normal);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    FrenetFrame�ŎZ�o�����J�[�u��NormalValue��o�^����@�Q�ƌ�:MJCG compound
//!	    @brief FrenetFrame�ŎZ�o�����J�[�u��NormalValue��o�^����
//!		@param[in] curveID �ΏۂƂ���J�[�uID
//!		@param[in] upvector �J�[�u�̍��{�̖@���iPTF�̎n�_�Ƃ��Ă��g�p�j
//!		@param[in] sample ���x�i�J�[�u���_��*sample�ŃJ�[�u���ϓ���������PTF�����s���邱�ƂɂȂ�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::computeFrenetFrameNormals(u32 curveID, const Vec3T<T>& upvector, u32 sample) {
	assert(curveID < this->curveCount());
	u32 s = this->pointCount(curveID) * (sample <= 1 ? 1 : sample);
	T fraction = T(1.0) / T(s);

	std::vector<Vec3T<T>> normal(s);
	Vec3T<T> uUpv = upvector.unit();
	for (u32 i = 0; i < s; i++) {
		Vec3T<T> tan = this->getTangentAtLengthRatio(curveID, fraction * i);
		if (uUpv.almostParallel(tan)) { tan = (tan + Vec3T<T>(T(-0.01))).unit(); } // �A�b�v�x�N�^�[�ƃ^���W�F���g�����s�̏ꍇ��0.001�̌덷������
		normal[i] = tan.cross(uUpv);
		normal[i] = normal[i].cross(tan);
		normal[i].setNormal();
	}
	if (this->isClose(curveID)) { normal.push_back(normal[0]); }

	this->resetAllNormals(curveID, normal);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    DoubleReflection�ŎZ�o�����J�[�u��NormalValue��o�^���� �Q�ƌ�:MJCG compound
//!	    @brief DoubleReflection�ŎZ�o�����J�[�u��NormalValue��o�^����
//!		@param[in] curveID �ΏۂƂ���J�[�uID
//!		@param[in] upvector �J�[�u�̍��{�̖@���iPTF�̎n�_�Ƃ��Ă��g�p�j
//!		@param[in] sample ���x�i�J�[�u���_��*sample�ŃJ�[�u���ϓ���������PTF�����s���邱�ƂɂȂ�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::computeDoubleReflectionNormals(u32 curveID, const Vec3T<T>& upvector, u32 sample) {
	assert(curveID < this->curveCount());
	u32 s = this->pointCount(curveID) * (sample <= 1 ? 1 : sample);

	T fraction = T(1.0) / T(s);
	std::vector<Vec3T<T>> restPos(s), restTan(s);
	for (u32 i = 0; i < s; i++) {
		restPos[i] = this->getPositionAtLengthRatio(curveID, fraction * i);
		restTan[i] = this->getTangentAtLengthRatio(curveID, fraction * i);
	}

	std::vector<Vec3T<T>> normal(s);
	Vec3T<T> rest = upvector.unit();
	rest = restTan[0].cross(rest).cross(restTan[0]).unit();
	normal[0] = rest;
	for (u32 i = 1; i < s; i++) {
		Vec3T<T> pos0 = restPos[i - 1];
		Vec3T<T> pos1 = restPos[i];
		Vec3T<T> v1 = pos1 - pos0;
		T c1 = T(2.0) / v1.dot(v1);
		Vec3T<T> ri = rest - (v1 * v1.dot(rest) * c1);

		Vec3T<T> tan0 = restTan[i - 1];
		Vec3T<T> tan1 = restTan[i];
		Vec3T<T> ti = tan0 - (v1 * v1.dot(tan0) * c1);
		Vec3T<T> v2 = tan1 - ti;

		T c2 = T(2.0) / v2.dot(v2);
		normal[i] = ri - (v2 * v2.dot(ri) * c2);
		rest = normal[i];
	}
	if (this->isClose(curveID)) { normal.push_back(normal[0]); }

	this->resetAllNormals(curveID, normal);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�ẴJ�[�u��NormalValue��V�K�쐬����
//!	    @brief �S�ẴJ�[�u��NormalValue��V�K�쐬����i�S�ẴJ�[�u�̃T���v�����͓���ɂȂ邱�Ƃɒ��Ӂj
//!		@param[in] sample ���x
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::recomputeAllNormals(u32 sample) {
	u32 curveCnt = this->curveCount();

	std::vector<T> fractionList(curveCnt);
	std::vector<s32> poiCntPack(curveCnt + 1, 0);
	for (s32 i = 0; i < curveCnt; i++) {
		s32 poiCnt = this->pointCount(i) * (sample <= 1 ? 1 : sample);
		poiCntPack[i + 1] = poiCntPack[i] + poiCnt;
		fractionList[i] = T(1.0) / T(poiCnt);

		Vec3AttributeT<T>& normalAttr = this->getNormalAttribute(i);
		normalAttr.resize(poiCnt);
	}
	s32 s = poiCntPack.back();

	#pragma omp parallel for if(300 < s)
	for (s32 j = 0; j < s; j++) {
		s32 poiID;
		s32 curveID = Burgess2MathFn::BinarySearchTree(poiCntPack, j, poiID);

		T r, fr, br;
		if (poiID == 0) {
			r = T(0.01);
			fr = T(0.0);
			br = T(0.02);
		} else if (poiID == s - 1) {
			r = T(0.99);
			fr = T(0.98);
			br = T(1.0);
		} else {
			r = fractionList[curveID] * poiID;
			fr = fractionList[curveID] * poiID - T(0.01);
			br = fractionList[curveID] * poiID + T(0.01);
		}
		Vec3T<T> rPos = this->getPositionAtLengthRatio(curveID, r);
		Vec3T<T> frPos = this->getPositionAtLengthRatio(curveID, fr);
		Vec3T<T> brPos = this->getPositionAtLengthRatio(curveID, br);
		Vec3T<T> ddx = (brPos - rPos).unit();
		Vec3T<T> dx = (frPos - rPos).unit();
		
		Vec3T<T> nml;
		if (ddx.almostParallel(dx) == false) {
			nml = ddx.cross(dx).unit();
		} else {
			nml = Vec3T<T>(0, 1, 0);
		}
		if (ddx.almostEqual(nml)) {
			nml = Vec3T<T>(0, 0, 1);
		}

		Vec3AttributeT<T>& normalAttr = this->getNormalAttribute(curveID);
		normalAttr.setValue(poiID, nml);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u��NormalValue�ɔC�ӂ̒l��ݒ肷��i�z��͂Q�ȏ�`���_��)(Loop�̏d�����_�͊܂߂Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
void CurvesBaseT<T>::setAllNormals ( u32 curveID, const std::vector<Vec3T<T>> &normal ) {
	assert ( curveID < this->curveCount () );
	assert ( 2 <= normal.size () );
	Vec3AttributeT<T>& v3Attr = this->getNormalAttribute ( curveID );
	v3Attr.setValues ( normal );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u��NormalValue�ɔC�ӂ̒l��ݒ肷��i������index��Loop�̏d�����_ID�͎w��ł��Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::setAllNormal ( u32 curveID, u32 index, const Vec3T<T> &normal ) {
	assert ( curveID < this->curveCount () );
	assert ( index < this->normalCount() );
	Vec3AttributeT<T>& v3Attr = this->getNormalAttribute ( curveID );
	v3Attr.setValue ( index, normal );
}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		normal�ʂŔ��]����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurvesBaseT<T>::setMirroring(u32 curveID, Vec3T<T> normal) {
	assert(curveID < this->curveCount());
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute(curveID);
	for (u32 i = 0; i < v3Attr.size(); i++) {
		v3Attr[i] = v3Attr[i].reflect(normal);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̑S�Ă̒��_�ʒu���擾����i�߂�l�ɂ�Loop�̏d�����_�͊܂܂�Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec3T<T>> CurvesBaseT<T>::getAllPointPositions ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T> & v3Attr = this->getPointPositionAttribute ( curveID );
	std::vector<Vec3T<T>> result = v3Attr.getValues();
	if ( this->getForm ( curveID ) != 0 ) {
		u32 cnt = this->pointCount ( curveID ); /// �d�����_�����Ȃ������_�����擾
		result.resize ( cnt );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�t�H�[���ɂ��d�����_���܂߂��S�Ă̒��_�ʒu���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec3T<T>> CurvesBaseT<T>::getCurveFormAllPointPositions ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T> & v3Attr = this->getPointPositionAttribute ( curveID );
	return v3Attr.getValues ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̒��_�ʒu���擾����i������index��Loop�̏d�����_ID�͎w��ł��Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> CurvesBaseT<T>::getPointPosition ( u32 curveID, u8 index ) const {
	assert ( curveID < this->curveCount () );
	assert ( index < this->pointCount ( curveID ) );
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	return v3Attr.getValue ( index );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̃t�H�[�����擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 CurvesBaseT<T>::getForm ( u32 curveID ) const{
	assert ( curveID < this->curveCount () );
	return this->data [curveID].getForm ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̐����擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 CurvesBaseT<T>::curveCount () const {
	return u32(this->data.size ());
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̒��_�����擾����iLoop�̏d�����_�͊܂߂Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 CurvesBaseT<T>::pointCount ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	u32 sub = u32(v3Attr.v [0]); /// �d�����_�����擾
	return v3Attr.size () - sub;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u��NormalValue���̐����擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 CurvesBaseT<T>::normalCount ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T>& v3Attr = this->getNormalAttribute ( curveID );
	u8 sub = v3Attr.v [0]; /// �d�����_�����擾
	return v3Attr.size () - sub;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̒��_�����擾����iLoop�̏d�����_���܂߂�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 CurvesBaseT<T>::curveFormPointCount ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	return v3Attr.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u��NormalValue�����擾����iLoop�̏d�����_���܂߂�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 CurvesBaseT<T>::curveFormNormalCount ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T>& v3Attr = this->getNormalAttribute ( curveID );
	return v3Attr.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�ẴJ�[�u�̒��_���̍��v���擾����iLoop�̏d�����_�͊܂߂Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 CurvesBaseT<T>::allPointCount () const {
	u32 result = 0;
	for ( u32 i = 0; i<this->curveCount (); i++ ) {
		result += this->pointCount ( i );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u���g�����X�t�H�[������
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::applyTransform ( u32 curveID, const Mat4T<T> &transform ){
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	for ( u32 i = 0; i<v3Attr.size (); i++ ) {
		v3Attr [i] = transform * v3Attr [i];
	}
	Vec3AttributeT<T>& v3Attr2 = this->getNormalAttribute ( curveID );
	for ( u32 i = 0; i<v3Attr2.size (); i++ ) {
		v3Attr2 [i] = transform * v3Attr2 [i];
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u���g�����X�t�H�[������
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::applyTransform ( u32 curveID, const XfoT<T> &transform ) {
	assert ( curveID < this->curveCount () );
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	for ( u32 i = 0; i<v3Attr.size (); i++ ) {
		v3Attr [i] = transform * v3Attr [i];
	}
	Vec3AttributeT<T>& v3Attr2 = this->getNormalAttribute ( curveID );
	for ( u32 i = 0; i<v3Attr2.size (); i++ ) {
		v3Attr2 [i] = transform * v3Attr2 [i];
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u����ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void CurvesBaseT<T>::setCurveCount ( u32 count ) {
	this->data.resize ( count );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		CurveForm��open�̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool CurvesBaseT<T>::isOpen ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	return this->getForm ( curveID ) == CurveData::CurveFrom::Open;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		CurveForm��close��������periodic�̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool CurvesBaseT<T>::isClose ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	return this->getForm ( curveID ) == CurveData::CurveFrom::Close || this->getForm ( curveID ) == CurveData::CurveFrom::Periodic;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		CurveForm��periodic�̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool CurvesBaseT<T>::isPeriodic(u32 curveID) const {
	assert(curveID < this->curveCount());
	return this->getForm( curveID ) == CurveData::CurveFrom::Periodic;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		curveLengthCache���X�V���ׂ��ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool CurvesBaseT<T>::paramLengthTableIsDirty ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	return this->data[curveID].paramLengthTableIsDirty();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		curveLengthCache���ŐV�̏�ԂɂȂ������Ƃ��L�^����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurvesBaseT<T>::paramLengthTableCached ( u32 curveID ) {
	assert ( curveID < this->curveCount () );
	this->data [curveID].paramLengthTableCached ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		curveLengthCache�̍X�V�𑣂���Ԃɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurvesBaseT<T>::incrementParamLengthTableVirsion ( u32 curveID ) {
	assert ( curveID < this->curveCount () );
	this->data [curveID].incrementParamLengthTableVirsion ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��BoolAttribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BoolAttributeT<T>& CurvesBaseT<T>::getOrCreateBoolAttribute ( u32 curveID,  const String &_name  ) {
	assert ( curveID < this->curveCount () );
	if ( this->data [curveID].attr.has ( _name ) ) {
		BoolAttributeT<T>& attr = this->getBoolAttribute ( curveID, _name );
		return attr;
	}else{
		BoolAttributeT<T>& attr = this->data [curveID].attr.getOrCreateBoolAttribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��F32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>& CurvesBaseT<T>::getOrCreateF32Attribute ( u32 curveID,  const String &_name  ) {
	assert ( curveID < this->curveCount () );
	if ( this->data [curveID].attr.has ( _name ) ) {
		F32AttributeT<T>& attr = this->getF32Attribute ( curveID, _name );
		return attr;
	} else {
		F32AttributeT<T>& attr = this->data [curveID].attr.getOrCreateF32Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��F64Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F64AttributeT<T>& CurvesBaseT<T>::getOrCreateF64Attribute ( u32 curveID, const String &_name ){
	assert ( curveID < this->curveCount () );
	if ( this->data[curveID].attr.has ( _name ) ){
		F64AttributeT<T>& attr = this->getF64Attribute ( curveID, _name );
		return attr;
	} else{
		F64AttributeT<T>& attr = this->data[curveID].attr.getOrCreateF64Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��U32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
U32AttributeT<T>& CurvesBaseT<T>::getOrCreateU32Attribute ( u32 curveID,  const String &_name  ) {
	assert ( curveID < this->curveCount () );
	if ( this->data [curveID].attr.has ( _name ) ) {
		U32AttributeT<T>& attr = this->getU32Attribute ( curveID, _name );
		return attr;
	} else {
		U32AttributeT<T>& attr = this->data [curveID].attr.getOrCreateU32Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��S32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
S32AttributeT<T>& CurvesBaseT<T>::getOrCreateS32Attribute ( u32 curveID,  const String &_name  ) {
	assert ( curveID < this->curveCount () );
	if ( this->data [curveID].attr.has ( _name ) ) {
		S32AttributeT<T>& attr = this->getS32Attribute ( curveID, _name );
		return attr;
	} else {
		S32AttributeT<T>& attr = this->data [curveID].attr.getOrCreateS32Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec2Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>& CurvesBaseT<T>::getOrCreatVec2Attribute ( u32 curveID,  const String &_name  ) {
	assert ( curveID < this->curveCount () );
	if ( this->data [curveID].attr.has ( _name ) ) {
		Vec2AttributeT<T>& attr = this->getVec2Attribute ( curveID, _name );
		return attr;
	} else {
		Vec2AttributeT<T>& attr = this->data [curveID].attr.getOrCreatVec2Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec3Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& CurvesBaseT<T>::getOrCreatVec3Attribute ( u32 curveID,  const String &_name  ) {
	assert ( curveID < this->curveCount () );
	if ( this->data [curveID].attr.has ( _name ) ) {
		Vec3AttributeT<T>& attr = this->getVec3Attribute ( curveID, _name );
		return attr;
	} else {
		Vec3AttributeT<T>& attr = this->data [curveID].attr.getOrCreatVec3Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec4Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>& CurvesBaseT<T>::getOrCreatVec4Attribute ( u32 curveID,  const String &_name  ) {
	assert ( curveID < this->curveCount () );
	if ( this->data [curveID].attr.has ( _name ) ) {
		Vec4AttributeT<T>& attr = this->getVec4Attribute ( curveID, _name );
		return attr;
	} else {
		Vec4AttributeT<T>& attr = this->data [curveID].attr.getOrCreatVec4Attribute ( _name );
		return attr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u�̒��_�ʒu�̔z����Q�ƂŎ擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& CurvesBaseT<T>::getPointPositionAttribute ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	assert ( this->data [curveID].attr.has ( "pointPositions" ) );
	Vec3AttributeT<T>& v3Attr = this->data [curveID].attr.getVec3Attribute ( "pointPositions" );
	return v3Attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�[�u��NormalValue�̔z����Q�ƂŎ擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& CurvesBaseT<T>::getNormalAttribute ( u32 curveID ) const {
	assert ( curveID < this->curveCount () );
	assert ( this->data [curveID].attr.has ( "normals" ) );
	Vec3AttributeT<T>& v3Attr = this->data [curveID].attr.getVec3Attribute ( "normals" );
	return v3Attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��BoolAttribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BoolAttributeT<T>& CurvesBaseT<T>::getBoolAttribute ( u32 curveID,  const String &_name  ) const {
	assert ( curveID < this->curveCount () );
	assert ( this->data [curveID].attr.has ( _name ) );
	BoolAttributeT<T>& attr = this->data [curveID].attr.getBoolAttribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��F32Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>& CurvesBaseT<T>::getF32Attribute ( u32 curveID,  const String &_name  ) const {
	assert ( curveID < this->curveCount () );
	assert ( this->data [curveID].attr.has ( _name ) );
	F32AttributeT<T>& attr = this->data [curveID].attr.getF32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��F64Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F64AttributeT<T>& CurvesBaseT<T>::getF64Attribute ( u32 curveID,  const String &_name  ) const {
	assert ( curveID < this->curveCount () );
	assert ( this->data [curveID].attr.has ( _name ) );
	F64AttributeT<T>& attr = this->data [curveID].attr.getF64Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��U32Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
U32AttributeT<T>& CurvesBaseT<T>::getU32Attribute ( u32 curveID,  const String &_name  ) const {
	assert ( curveID < this->curveCount () );
	assert ( this->data [curveID].attr.has ( _name ) );
	U32AttributeT<T>& attr = this->data [curveID].attr.getU32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��S32Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
S32AttributeT<T>& CurvesBaseT<T>::getS32Attribute ( u32 curveID,  const String &_name  ) const {
	assert ( curveID < this->curveCount () );
	assert ( this->data [curveID].attr.has ( _name ) );
	S32AttributeT<T>& attr = this->data [curveID].attr.getS32Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec2Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>& CurvesBaseT<T>::getVec2Attribute ( u32 curveID,  const String &_name  ) const {
	assert ( curveID < this->curveCount () );
	assert ( this->data [curveID].attr.has ( _name ) );
	Vec2AttributeT<T>& attr = this->data [curveID].attr.getVec2Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec3Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>& CurvesBaseT<T>::getVec3Attribute ( u32 curveID,  const String &_name  ) const {
	assert ( curveID < this->curveCount () );
	assert ( this->data [curveID].attr.has ( _name ) );
	Vec3AttributeT<T>& attr = this->data [curveID].attr.getVec3Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Vec4Attribute���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>& CurvesBaseT<T>::getVec4Attribute ( u32 curveID,  const String &_name  ) const {
	assert ( curveID < this->curveCount () );
	assert ( this->data [curveID].attr.has ( _name ) );
	Vec4AttributeT<T>& attr = this->data [curveID].attr.getVec4Attribute ( _name );
	return attr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵�����O��Attribute�����݂���ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool CurvesBaseT<T>::has ( u32 curveID, const String &_name ) const {
	return this->data [curveID].attr.has ( _name );
}