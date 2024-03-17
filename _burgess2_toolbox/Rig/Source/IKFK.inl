//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �{�[���̒�����ݒ肷�� type=boneLength�̔z��
//!	    @brief �{�[���̒�����ݒ肷��<br>
//!		type=boneLength�̔z�񐔂��猈�� (�z�񐔂��T�ȏゾ��NBone�^�C�v)
//!		@param[in] boneLength �ebone�̒����̃��X�g
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool IKFKT<T>::initialize ( const std::vector<T> _boneLength ) {
	u32 boneCnt = _boneLength.size ();
	if ( boneCnt < 2 ){ return false; }
	if( this->type != 0 && this->type != boneCnt ){ return false; }

	this->boneLength.resize ( boneCnt );
	this->totalLength = T ( 0.0 );
	for ( u32 i = 0; i < boneCnt; ++i ){
		this->boneLength [i] = Burgess2MathFn::Max ( _boneLength [i], T ( 0.01 ) );
		this->totalLength += this->boneLength [i];
	}
	this->type = Burgess2MathFn::Min ( u8(boneCnt), u8(5) );
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �{�[���̒�����ݒ肷�� type=boneLength�̔z��
//!	    @brief �{�[���̒�����ݒ肷��<br>
//!		type=boneGPose�̔z�񐔂��猈�� (�z�񐔂�6�ȏゾ��NBone�^�C�v)
//!		@param[in] boneGPose bone��Global-Mat4 (�z��̍Ō�͐�[�W���C���g��Global-Mat4)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool IKFKT<T>::initialize ( const std::vector<Mat4T<T>> boneGPose ) {
	u32 jointCnt = boneGPose.size ();
	if ( jointCnt < 3 ){ return false; }

	std::vector<T> len ( jointCnt - 1 );
	for ( u32 i = 0; i < jointCnt; ++i ){
		Vec3T<T> locV = ( boneGPose[i].inverse () * boneGPose[i + 1] ).getTranslation ();
		len[i] = locV.length ();
	}
	return this->initialize ( len );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �{�[���̒�����ݒ肷�� type=2Bone
//!	    @brief �{�[���̒�����ݒ肷��<br>
//!		type=2Bone
//!		@param[in] boneLength �ebone�̒����̃��X�g
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool IKFKT<T>::initialize ( const Vec2T<T> &_boneLength ){
	std::vector<T> len = _boneLength.toTypeArray ();
	return this->initialize ( len );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �{�[���̒�����ݒ肷�� type=2Bone
//!	    @brief �{�[���̒�����ݒ肷��<br>
//!		type=2Bone
//!		@param[in] bone0GMat4 bone0��Global-Mat4
//!		@param[in] bone1GMat4 bone1��Global-Mat4
//!		@param[in] endJointGMat4 ��[�W���C���g��Global-Mat4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool IKFKT<T>::initialize ( const Mat4T<T> &bone0GMat4,
						  const Mat4T<T> &bone1GMat4,
						  const Mat4T<T> &endJointGMat4 )
{
	std::vector<Mat4T<T>> bones ( 3 );
	bones[0] = bone0GMat4;
	bones[1] = bone1GMat4;
	bones[2] = endJointGMat4;
	return this->initialize ( bones );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �{�[���̒�����ݒ肷�� type=3Bone
//!	    @brief �{�[���̒�����ݒ肷��<br>
//!		type=3Bone
//!		@param[in] boneLength �ebone�̒����̃��X�g
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool IKFKT<T>::initialize ( const Vec3T<T> &_boneLength ){
	std::vector<T> len = _boneLength.toTypeArray ();
	return this->initialize ( len );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �{�[���̒�����ݒ肷�� type=3Bone
//!	    @brief �{�[���̒�����ݒ肷��<br>
//!		type=3Bone
//!		@param[in] bone0GMat4 bone0��Global-Mat4
//!		@param[in] bone1GMat4 bone1��Global-Mat4
//!		@param[in] bone2GMat4 bone2��Global-Mat4
//!		@param[in] endJointGMat4 ��[�W���C���g��Global-Mat4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool IKFKT<T>::initialize ( const Mat4T<T> &bone0GMat4,
							const Mat4T<T> &bone1GMat4,
							const Mat4T<T> &bone2GMat4,
							const Mat4T<T> &endJointGMat4 ){
	std::vector<Mat4T<T>> bones ( 4 );
	bones[0] = bone0GMat4;
	bones[1] = bone1GMat4;
	bones[2] = bone2GMat4;
	bones[3] = endJointGMat4;
	return this->initialize ( bones );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �{�[���̒�����ݒ肷�� type=3Bone
//!	    @brief �{�[���̒�����ݒ肷��<br>
//!		type=4Bone
//!		@param[in] boneLength �ebone�̒����̃��X�g
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool IKFKT<T>::initialize ( const Vec4T<T> &_boneLength ){
	std::vector<T> len = _boneLength.toTypeArray ();
	return this->initialize ( len );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �{�[���̒�����ݒ肷�� type=4Bone
//!	    @brief �{�[���̒�����ݒ肷��<br>
//!		type=4Bone
//!		@param[in] bone0GMat4 bone0��Global-Mat4
//!		@param[in] bone1GMat4 bone1��Global-Mat4
//!		@param[in] bone2GMat4 bone2��Global-Mat4
//!		@param[in] bone3GMat4 bone3��Global-Mat4
//!		@param[in] endJointGMat4 ��[�W���C���g��Global-Mat4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool IKFKT<T>::initialize ( const Mat4T<T> &bone0GMat4,
							const Mat4T<T> &bone1GMat4,
							const Mat4T<T> &bone2GMat4,
							const Mat4T<T> &bone3GMat4,
							const Mat4T<T> &endJointGMat4 ){
	std::vector<Mat4T<T>> bones ( 5 );
	bones[0] = bone0GMat4;
	bones[1] = bone1GMat4;
	bones[2] = bone2GMat4;
	bones[3] = bone3GMat4;
	bones[4] = endJointGMat4;
	return this->initialize ( bones );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �X�P�[����ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void IKFKT<T>::setScale ( T _scale ) {
	this->scale = _scale;
	//this->totalLength *= _scale;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �A�b�v�x�N�^�[�ł̐���@�@(0 or 1)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void IKFKT<T>::setUpvectorType ( u8 type ) {
	this->upvectorType = Math::Min ( type, u8(1) );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �\�t�gIK�̒l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void IKFKT<T>::setSoftness ( T softRatio ){
	this->softness = Math::Clamp ( softRatio, T ( 0.0 ), T ( 1.0 ) );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    NBoneSolver�����s����ۂ�UpVector�̗L���E������ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void IKFKT<T>::useUpVector ( Bool use ) {
	this->enableUpVector = use;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���䂷�鍜�̐���Ԃ��i��[�W���C���g�͊܂܂Ȃ��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 IKFKT<T>::boneCount ()const {
	return this->boneLength.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �^�C�v��Ԃ��@2=2Bone, 3=3Bone, 4=4Bone, 5=NBone
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u8 IKFKT<T>::getType ()const{
	return this->type;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
IKFKT<T> IKFKT<T>::clone()const{
	IKFKT<T> result;
	result.type = this->type;
	result.boneLength = this->boneLength;
	result.totalLength = this->totalLength;
	result.scale = this->scale;
	result.upvectorType = this->upvectorType;
	result.enableUpVector = this->enableUpVector;
	result.softness = this->softness;
	result.directionType = this->directionType;
	result.chainDirNeg = this->chainDirNeg;
	result.upvectorDirNeg = this->upvectorDirNeg;
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �R�s�[
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void IKFKT<T>::copy(const IKFKT<T>& other) {
	this->type = other.type;
	this->boneLength = other.boneLength;
	this->totalLength = other.totalLength;
	this->scale = other.scale;
	this->upvectorType = other.upvectorType;
	this->enableUpVector = other.enableUpVector;
	this->softness = other.softness;
	this->directionType = other.directionType;
	this->chainDirNeg = other.chainDirNeg;
	this->upvectorDirNeg = other.upvectorDirNeg;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    FK
//!	    @brief FK
//!		@param[in] root root��Global-Mat4�i�㗬���W���[���Ƃ̃R�l�N�^ / bone0��parent�j
//!		@param[in] fkHandle fkHandle��Global-May4
//!		@return �S�Ẵ{�[���Ɛ�[�W���C���g��Local-May4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> IKFKT<T>::fkSolve (
	const Mat4T<T> &root,
	const std::vector<Mat4T<T>> &fkHandle
)const{
	std::vector<Mat4T<T>> result ( this->boneCount () + 1 ); // +1 = ��[�W���C���g;
	if ( this->type == 0 ){ return result; }
	assert ( fkHandle.size () == this->boneCount () );

	result[0] = root.inverse() * fkHandle[0];
	result[0].setTranslation ( Vec3T<T> ( 0.0 ) );
	result[0].setScaling ( Vec3T<T> ( T ( 1.0 ) ) );
	for ( u32 i = 1, n = fkHandle.size (); i < n; ++i ){
		result[i].setUpperLeft ( (fkHandle[i-1].inverse() * fkHandle[i]).getRotationMat3 () );
		result[i].setTranslation ( Vec3T<T> ( this->boneLength[i - 1], T ( 0.0 ), T ( 0.0 ) ) );
	}
	u32 tipBoneID = result.size() - 1;
	result[tipBoneID] = result[tipBoneID - 1];
	result[tipBoneID].setTranslation ( Vec3T<T> ( this->boneLength[tipBoneID-1], T ( 0.0 ), T ( 0.0 ) ) );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    IK/FK (2,3,4,NBone��S�đΉ�)
//!	    @brief IK/FK (2,3,4,NBone��S�đΉ�)
//!		@param[in] root root��Global-Mat4�i�㗬���W���[���Ƃ̃R�l�N�^ / bone0��parent�j
//!		@param[in] fkHandle fkHandle��Global-May4 (�^�C�v�ɍ������z�񐔂��K�v ��F3Bone�^�C�v�ł͔z��=3)
//!		@param[in] ikUpVector  IK_UpVector��Global-Vec3
//!		@param[in] ikHandle ikHandle��Global-May4
//!		@param[in] blend  FK - IK �̃u�����h�l(0.0~1.0)
//!		@param[in] ikAnkleAngle  �����Ƃ̊p�x (3,4Bone�^�C�v�ł̂ݎg�p)
//!		@return �S�Ẵ{�[���Ɛ�[�W���C���g��Local-May4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> IKFKT<T>::solve (
	const Mat4T<T> &root,
	const std::vector<Mat4T<T>> &fkHandle,
	const Mat4T<T> &ikUpVector,
	const Mat4T<T> &ikHandle,
	T blend,
	T ikAnkleAngle
) const{
	if ( Math::AlmostEqual ( blend, T ( 0.0 ) ) ){ return this->fkSolve ( root, fkHandle ); }
	switch ( this->type ){
		case 2:
			return this->solve2Bone ( root, fkHandle, ikUpVector, ikHandle, blend );
		case 3:
			return this->solve3Bone ( root, fkHandle, ikUpVector, ikHandle, blend, ikAnkleAngle  );
		case 4:
			return this->solve4Bone ( root, fkHandle, ikUpVector, ikHandle, blend, ikAnkleAngle );
		default:
			return this->solveNBone ( root, fkHandle, ikUpVector, ikHandle, blend );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    IK/FK 2Bone
//!	    @brief IK/FK 2Bone
//!		@param[in] root root��Global-Mat4�i�㗬���W���[���Ƃ̃R�l�N�^ / bone0��parent�j
//!		@param[in] fkHandle fkHandle��Global-May4
//!		@param[in] ikUpVector  IK_UpVector��Global-Vec3
//!		@param[in] ikHandle ikHandle��Global-May4
//!		@param[in] blend  FK - IK �̃u�����h�l(0.0~1.0)
//!		@return �S�Ẵ{�[���Ɛ�[�W���C���g��root��Ԃɂ�����Local-May4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> IKFKT<T>::solve2Bone (
	const Mat4T<T> &root,
	const std::vector<Mat4T<T>> &fkHandle,
	const Mat4T<T> &ikUpVector,
	const Mat4T<T> &ikHandle,
	T blend
)const {
	std::vector<Mat4T<T>> result ( 3 );
	if ( this->type != 2 ){ return result; }
	assert ( fkHandle.size () == this->boneCount () );

	if ( Math::AlmostEqual ( blend, T ( 0.0 ) ) ){
		return this->fkSolve ( root, fkHandle );
	} else{ 
		Vec3T<T> ikHandleLPos = ikHandle.getTranslation () - root.getTranslation ();
		Vec3T<T> A = ikHandleLPos;

		T OLen = this->boneLength[1] * this->scale; // �Εӂ̒���
		T HLen = this->boneLength[0] * this->scale; // �Εӂ̒���
		T ALen = A.length (); // �וӂ̒���

		// �\�t�gIK �Q�ƌ� http://www.sakaiden.com/?p=142
		if ( T ( 0.0 ) < this->softness ){
			T OHLen = OLen + HLen;
			T softLen = OHLen * this->softness;
			T hardLen = OHLen - softLen;
			if ( hardLen < ALen ){
				T softValLen = hardLen + softLen * ( T ( 1.0 ) - exp ( -( ALen - hardLen ) / softLen ) );
				T softPow = softValLen / ALen;
				ikHandleLPos = A * softPow;
				ALen *= softPow;
			}
		}

		QuatT<T> rotOffset;
		T cos = Math::CosineFormula ( OLen, HLen, ALen ); // bone0��Xaxis��וӂƂ��ĎΕӂƑΕӂ���]���藝��bone0����bone1�ւ̊p�x�����߂�
		cos = Math::Clamp ( cos, T ( -1.0 ), T ( 1.0 ) );
		Vec3T<T> jiku;
		jiku.y = this->upvectorType == 0 ? T ( 1.0 ) : T ( -1.0 );
		rotOffset.setFromAxisAndAngle ( jiku, std::acos ( cos ) );

		// �]���藝�ŋ��߂��p�x����bone0��Matrix���o�͂���
		QuatT<T> rot;
		Vec3T<T> H = ikUpVector.getTranslation () - root.getTranslation ();
		rot.setFromDirectionAndUpvector ( A.unit (), H.unit (), 2, false, true );
		rot = rot * rotOffset;
		result[0].setUpperLeft ( Mat3T<T> ( rot ) );

		// bone0��Xaxis��bone1����ikHandle�ւ�2�̊p�x�����ɍ������Z�o����bone1��Matrix���o�͂���
		rotOffset.setFrom2Vectors ( rot.getXaxis (), ( ikHandleLPos - result[0] * Vec3T<T> { HLen, T ( 0.0 ), T ( 0.0 ) } ).unit () );
		QuatT<T> bone1Quat = rot.inverse () * ( rotOffset * rot );
		// IK/FK�̃u�����h����
		if ( Math::AlmostEqual ( blend, T ( 1.0 ) ) == false ){
			QuatT<T> fk0Quat = ( root.inverse () * fkHandle[0] ).getRotation ();
			result[0].setUpperLeft ( Mat3T<T> ( fk0Quat.slerp ( rot, blend ) ) );
			QuatT<T> fk1Quat = ( fkHandle[0].inverse() * fkHandle[1] ).getRotation ();
			bone1Quat = fk1Quat.slerp ( bone1Quat, blend );
		}
		result[1].setTranslation ( Vec3T<T> { HLen, T ( 0.0 ), T ( 0.0 ) } );
		result[1].setUpperLeft ( Mat3T<T> ( bone1Quat ) );

		// bone2(endJoint)��Matrix���o��
		result[2].setTranslation ( Vec3T<T> { OLen, T ( 0.0 ), T ( 0.0 ) } );

		return result;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    IK/FK 3Bone
//!	    @brief IK/FK 3Bone
//!		@param[in] root root��Global-Mat4�i�㗬���W���[���Ƃ̃R�l�N�^ / bone0��parent�j
//!		@param[in] ikUpVector  IK_UpVector��Global-Vec3
//!		@param[in] fkHandle fkHandle��Global-May4
//!		@param[in] blend  FK - IK �̃u�����h�l(0.0~1.0)
//!		@param[in] ikAnkleAngle IK�̂����Ƃ̃I�t�Z�b�g��]�l(Local-Angle(Radian)) 
//!		@return �S�Ẵ{�[���Ɛ�[�W���C���g��Local-May4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> IKFKT<T>::solve3Bone (
	const Mat4T<T> &root,
	const std::vector<Mat4T<T>> &fkHandle,
	const Mat4T<T> &ikUpVector,
	const Mat4T<T> &ikHandle,
	T blend,
	T ikAnkleAngle
)const{
	std::vector<Mat4T<T>> result ( 4 );
	if ( this->type != 3 ){ return result; }
	assert ( fkHandle.size () == this->boneCount () );
	
	if ( Math::AlmostEqual ( blend, T ( 0.0 ) ) ){
		return this->fkSolve ( root, fkHandle );
	} else{
		Deg3T<T> offsetDeg{ T ( 0.0 ), ikAnkleAngle, T ( 0.0 ) };
		Mat4T<T> offsetMat;
		offsetMat.setUpperLeft ( root.getRotationMat3 () * Mat3T<T>{offsetDeg} );
		Mat4T<T> fkikNHandle = offsetMat * ikHandle;

		result = this->solveNBone ( root, fkHandle, ikUpVector, fkikNHandle, T(1.0) );

		IKFKT<T> ankleIK;
		ankleIK.initialize ( Vec2T<T> {this->boneLength[1], this->boneLength[2]} );
		ankleIK.setScale ( this->scale );
		ankleIK.setUpvectorType ( this->upvectorType == 0 );
		Vec3T<T> r = ( result[0] * result[1] ).getTranslation ();

		Vec3T<T> upvectorTr = ikHandle.getTranslation () * Vec3T<T> ( 1.0, -1.0, -1.0 ) - r;
		Vec3T<T> ikHandleTr = ikHandle.getTranslation () - r;
		Mat4T<T> uvMat, ikHMat;
		uvMat.setTranslation ( upvectorTr );
		ikHMat.setTranslation ( ikHandleTr );
		std::vector<Mat4T<T>> ankleIKResult = ankleIK.solve2Bone ( root, std::vector<Mat4T<T>>(2),
																uvMat, ikHMat, T(1.0) );
		result[1].setUpperLeft ( ( result[0].inverse () * ankleIKResult[0] ).getUpperLeft () );
		result[2] = ankleIKResult[1];
		result[3] = ankleIKResult[2];

		if ( Math::AlmostEqual ( blend, T ( 1.0 ) ) == false ) {
			std::vector<Mat4T<T>> fkResult = this->fkSolve ( root, fkHandle );

			QuatT<T> rot;
			rot = fkResult[0].getRotation ().slerp ( result[0].getRotation (), blend );
			result[0].setIdentity ();
			result[0].setUpperLeft ( Mat3T<T> ( rot ) );
			rot = fkResult[1].getRotation ().slerp ( result[1].getRotation (), blend );
			result[1].setUpperLeft ( Mat3T<T> ( rot ) );
			rot = fkResult[2].getRotation ().slerp ( result[2].getRotation (), blend );
			result[2].setUpperLeft ( Mat3T<T> ( rot ) );
		}
		return result;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    IK/FK 4Bone
//!	    @brief IK/FK 4Bone
//!		@param[in] root root��Global-Mat4�i�㗬���W���[���Ƃ̃R�l�N�^ / bone0��parent�j
//!		@param[in] ikUpVector  IK_UpVector��Global-Vec3
//!		@param[in] fkHandle fkHandle��Global-May4
//!		@param[in] blend  FK - IK �̃u�����h�l(0.0~1.0)
//!		@param[in] ikAnkleAngle IK�̂����Ƃ̃I�t�Z�b�g��]�l(Local-Angle(Radian)) 
//!		@return �S�Ẵ{�[���Ɛ�[�W���C���g��Local-May4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> IKFKT<T>::solve4Bone (
	const Mat4T<T> &root,
	const std::vector<Mat4T<T>> &fkHandle,
	const Mat4T<T> &ikUpVector,
	const Mat4T<T> &ikHandle,
	T blend,
	T ikAnkleAngle
)const{
	std::vector<Mat4T<T>> result ( 5 );
	if ( this->type != 4 ){ return result; }
	assert ( fkHandle.size () == this->boneCount () );

	if ( Math::AlmostEqual ( blend, T ( 0.0 ) ) ){
		return this->fkSolve ( root, fkHandle );
	} else{
		Deg3T<T> offsetDeg{ T ( 0.0 ), ikAnkleAngle, T ( 0.0 ) };
		Mat4T<T> offsetMat;
		offsetMat.setUpperLeft ( root.getRotationMat3 () * Mat3T<T>{offsetDeg} );
		Mat4T<T> fkikNHandle = offsetMat * ikHandle;

		result = this->solveNBone ( root, fkHandle, ikUpVector, fkikNHandle, T ( 1.0 ) );

		IKFKT<T> ankleIK;
		ankleIK.initialize ( Vec2T<T> {this->boneLength[2], this->boneLength[3]} );
		ankleIK.setScale ( this->scale );
		ankleIK.setUpvectorType ( this->upvectorType == 1 );
		Vec3T<T> r = ( result[0] * result[1] * result[2] ).getTranslation ();

		Vec3T<T> upvectorTr = ikHandle.getTranslation () * Vec3T<T> ( 1.0, -1.0, -1.0 ) - r;
		Vec3T<T> ikHandleTr = ikHandle.getTranslation () - r;
		Mat4T<T> uvMat, ikHMat;
		uvMat.setTranslation ( upvectorTr );
		ikHMat.setTranslation ( ikHandleTr );
		std::vector<Mat4T<T>> ankleIKResult = ankleIK.solve2Bone ( root, std::vector<Mat4T<T>> ( 2 ),
																uvMat, ikHMat, T ( 1.0 ) );
		result[2].setUpperLeft ( ( (result[0] * result[1]).inverse () * ankleIKResult[0] ).getUpperLeft () );
		result[3] = ankleIKResult[1];
		result[4] = ankleIKResult[2];
		
		if ( Math::AlmostEqual ( blend, T ( 1.0 ) ) == false ){
			std::vector<Mat4T<T>> fkResult = this->fkSolve ( root, fkHandle );

			QuatT<T> rot;
			rot = fkResult[0].getRotation ().slerp ( result[0].getRotation (), blend );
			result[0].setIdentity ();
			result[0].setUpperLeft ( Mat3T<T> ( rot ) );
			rot = fkResult[1].getRotation ().slerp ( result[1].getRotation (), blend );
			result[1].setUpperLeft ( Mat3T<T> ( rot ) );
			rot = fkResult[2].getRotation ().slerp ( result[2].getRotation (), blend );
			result[2].setUpperLeft ( Mat3T<T> ( rot ) );
			rot = fkResult[3].getRotation ().slerp ( result[3].getRotation (), blend );
			result[3].setUpperLeft ( Mat3T<T> ( rot ) );
		}
		return result;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    IK/FK NBone (type��3,4Bone�̎����L��)
//!	    @brief IK/FK NBone (type��3,4Bone�̎����L��)
//!		@param[in] root root��Global-May4
//!		@param[in] fkHandle fkHandle��Global-May4 (IK�����̃j���[�g�����|�[�Y�Ƃ��Ă��g�p����)
//!		@param[in] ikHandle ikHandle��Global-Mat4
//!		@param[in] blend  FK - IK �̃u�����h�l(0.0~1.0)
//!		@return �S�Ẵ{�[���Ɛ�[�W���C���g��Local-May4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> IKFKT<T>::solveNBone (
	const Mat4T<T> &root,
	const std::vector<Mat4T<T>> &fkHandle,
	const Mat4T<T> &ikHandle,
	T blend
) const{
	return this->solveNBone ( root, fkHandle, Mat4T<T> (), ikHandle, blend );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    IK/FK NBone (type��3,4Bone�̎����L��)
//!	    @brief IK/FK NBone (type��3,4Bone�̎����L��)
//!		@param[in] root root��Global-May4
//!		@param[in] fkHandle fkHandle��Global-May4 (IK�����̃j���[�g�����|�[�Y�Ƃ��Ă��g�p����)
//!		@param[in] ikUpVector ikUpVector��Global-Mat4
//!		@param[in] ikHandle ikHandle��Global-Mat4
//!		@param[in] blend  FK - IK �̃u�����h�l(0.0~1.0)
//!		@return �S�Ẵ{�[���Ɛ�[�W���C���g��Local-May4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> IKFKT<T>::solveNBone (
	const Mat4T<T> &root,
	const std::vector<Mat4T<T>> &fkHandle,
	const Mat4T<T> &ikUpVector,
	const Mat4T<T> &ikHandle,
	T blend
) const {
	std::vector<Mat4T<T>> result ( this->boneCount () + 1 );
	if ( this->type <= 2 ){ return result; }
	assert ( fkHandle.size () == this->boneCount () );

	if ( Burgess2MathFn::AlmostEqual ( blend, T ( 0.0 ) ) ){
		return this->fkSolve ( root, fkHandle );
	} else{
		assert ( 3 <= this->boneCount () + 1 && fkHandle.size () == this->boneCount () );
		Mat4T<T> rootInv = root.inverse ();
		Vec3T<T> rootTr = root.getTranslation ();
		Vec3T<T> ikHandleTr = ikHandle.getTranslation ();

		u32 fkHandleCnt = fkHandle.size ();
		std::vector<Mat4T<T>> fkLHandle ( fkHandleCnt );
		fkLHandle[0] = rootInv * fkHandle[0];
		for ( u32 i = 1; i < fkHandleCnt; ++i ){
			fkLHandle[i] = fkHandle[i - 1].inverse() * fkHandle[i];
		}
		std::vector<Mat4T<T>> fkHandleG = fkHandle;
		if ( this->enableUpVector ){
			Vec3T<T> rootToTipUnitVec = ( fkHandleG[fkHandleG.size () - 1].getTranslation () - rootTr ).normal ();
			Vec3T<T> rootToIkHandleUnitVec = ( ikHandleTr - rootTr ).normal ();

			Vec3T<T> upVecG = ( ikUpVector.getTranslation () ) - rootTr;
			Vec3T<T> rootToUpVUnitVec = upVecG - ( upVecG.dot ( rootToIkHandleUnitVec ) * rootToIkHandleUnitVec );
			rootToUpVUnitVec.setNormal ();
			QuatT<T> offsetRot, rot;
			offsetRot.setFrom2Vectors ( rootToTipUnitVec, rootToIkHandleUnitVec );
			rot = offsetRot * fkHandleG[0].getRotation ();
			Vec3T<T> toUpVVector = rot.rotateVector ( this->upvectorType == 0 ? Vec3T<T>{0.0, 0.0, -1.0} : Vec3T<T> ( 0.0, 0.0, 1.0 ) );
			Vec3T<T> fkUpVUnitVec = toUpVVector - ( toUpVVector.dot ( rootToIkHandleUnitVec ) * rootToIkHandleUnitVec );
			fkUpVUnitVec.setNormal ();

			T angle = fkUpVUnitVec.angleTo ( rootToUpVUnitVec );
			if ( fkUpVUnitVec.cross ( rootToUpVUnitVec ).dot ( rootToIkHandleUnitVec ) < T ( 0.0 ) ){ angle *= T ( -1.0 ); }

			Mat4T<T> rest = fkHandleG[0];
			QuatT<T> offsetRotUpV;
			offsetRotUpV.setFromAxisAndAngle ( rootToIkHandleUnitVec, angle );
			offsetRot = offsetRotUpV * offsetRot;
			rot = offsetRot * fkHandleG[0].getRotation ();
			fkHandleG[0].setIdentity();
			fkHandleG[0].setUpperLeft ( Mat3T<T> ( rot ) );
			fkHandleG[0].setTranslation ( rootTr );
			for ( u32 i = 1, n = fkHandleG.size (); i < n; ++i ){
				rot = offsetRot * fkHandleG[i].getRotation ();
				Vec3T<T> tr = rest.inverse () * ( fkHandleG[i].getTranslation () );
				rest = fkHandleG[i];
				fkHandleG[i].setUpperLeft ( Mat3T<T> ( rot ) );
				fkHandleG[i].setTranslation ( fkHandleG[i - 1] * tr );
			}
		}

		Vec3T<T> tipTrL = Vec3T<T> ( this->boneLength[this->boneLength.size () - 1], 0.0, 0.0 );
		Vec3T<T> fkTipTr = fkHandleG[fkHandleCnt - 1] * tipTrL;

		Vec3T<T> A = ikHandleTr - rootTr;
		T ALen = A.length ();
		QuatT<T> offsetRot;
		T restTotalLength = this->totalLength;
		Mat4T<T> restRoot = root;
		for ( u32 i = 0, n = fkHandleCnt; i < n; ++i ){
			Vec3T<T> thisTr = fkHandleG[i].getTranslation ();
			QuatT<T> thisRot = fkHandleG[i].getRotation ();

			Vec3T<T> O;
			if ( i == 0 ){
				O = fkTipTr - thisTr;
			} else{
				O = fkTipTr - ( rootTr + ( offsetRot.rotateVector ( ( thisTr - rootTr ) ) ) );
				Vec3T<T> boneVec = Vec3T<T>{ this->boneLength[i - 1], T ( 0.0 ), T ( 0.0 ) } *this->scale;
				thisTr = restRoot * boneVec;
			}
			T OLen = O.length ();
			O /= OLen;

			A = ikHandleTr - thisTr;
			ALen = A.length ();
			A /= ALen;
			T boneLen = this->boneLength[i];

			if ( i == 0 ){
				offsetRot.setFrom2Vectors ( O, A );
				fkTipTr = thisTr + ( A * OLen );
				thisRot = offsetRot * thisRot;
			} else{
				QuatT<T> offset2;
				offset2.setFrom2Vectors ( O, A );
				thisRot = offset2 * offsetRot * thisRot;
			}

			if ( i <= ( fkHandleCnt - 2 ) ){
				restTotalLength -= boneLen;
				Vec3T<T> boneVec = fkLHandle[i + 1].getTranslation () * this->scale;
				Vec3T<T> boneLengthVec = thisRot.rotateVector ( boneVec.normal () );

				T dot = boneLengthVec.dot ( A );
				T cos = Burgess2MathFn::Clamp ( dot, T ( -1.0 ), T ( 1.0 ) );
				T fkBoneAngle = std::acos ( cos );

				if ( fkBoneAngle < BURGESS2::TOLERANCE ){ continue; }

				Vec3T<T> bendAxis = A.cross ( boneLengthVec ).normal ();
				T ikBoneAngle;

				if ( i == ( fkHandleCnt - 2 ) ){
					T cos = Burgess2MathFn::CosineFormula ( restTotalLength, boneLen, ALen );
					cos = Burgess2MathFn::Clamp ( cos, T ( -1.0 ), T ( 1.0 ) );
					ikBoneAngle = std::acos ( cos );
				} else{
					T maxFkBoneAngle;
					if ( restTotalLength < OLen ){
						T cos = Burgess2MathFn::CosineFormula ( restTotalLength, boneLen, OLen );
						cos = Burgess2MathFn::Clamp ( cos, T ( -1.0 ), T ( 1.0 ) );
						maxFkBoneAngle = std::acos ( cos );
					} else{
						T cos = Burgess2MathFn::Clamp ( T ( boneLen * 0.5 ) / restTotalLength, T ( 0.0 ), T ( 1.0 ) );
						maxFkBoneAngle = std::acos ( cos );
						maxFkBoneAngle += ( restTotalLength - OLen ) / boneLen;
					}
					T maxIkBoneAngle;
					if ( restTotalLength < ALen ){
						T cos = Burgess2MathFn::CosineFormula ( restTotalLength, boneLen, ALen );
						cos = Burgess2MathFn::Clamp ( cos, T ( -1.0 ), T ( 1.0 ) );
						maxIkBoneAngle = std::acos ( cos );
					} else{
						T cos = Burgess2MathFn::Clamp ( ( boneLen * 0.5 ) / restTotalLength, 0.0, 1.0 );
						maxIkBoneAngle = std::acos ( cos );
						maxIkBoneAngle += ( restTotalLength - ALen ) / boneLen;
					}
					ikBoneAngle = maxIkBoneAngle * ( fkBoneAngle / maxFkBoneAngle );
				}
				T deltaBoneAngle = ( ikBoneAngle - fkBoneAngle );

				QuatT<T> offset3;
				offset3.setFromAxisAndAngle ( bendAxis, deltaBoneAngle );
				thisRot = offset3 * thisRot;

			}
			XfoT<T> resultXfoG ( Vec3T<T> ( T ( 1.0 ) ), thisRot, thisTr );
			Mat4T<T> resultMat4G ( resultXfoG );
			result[i] = restRoot.inverse () * resultMat4G;
			restRoot = resultMat4G;
		}
		result[result.size () - 1].setIdentity ();
		result[result.size () - 1].setTranslation ( tipTrL );

		if ( Burgess2MathFn::AlmostEqual ( blend, T ( 1.0 ) ) == false ){
			std::vector<Mat4T<T>> fkResult = this->fkSolve ( root, fkHandle );
			for ( u32 i = 0, n = fkResult.size (); i < n; ++i ){
				QuatT<T> fkRot = fkResult[i].getRotation ();
				QuatT<T> ikRot = result[i].getRotation ();
				result[i].setUpperLeft ( Mat3T<T> ( fkRot.slerp ( ikRot, blend ) ) );
			}
		}
		return result;
	}
}
