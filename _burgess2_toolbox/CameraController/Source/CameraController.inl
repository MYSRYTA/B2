//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �J�����^�C�v���t���[�J�����ɂ���
//!	    @brief �J�����^�C�v���t���[�J�����ɂ���
//!		@param[in] _freeCamera true = free camera, false = interest camera
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CameraControllerT<T>::setFreeCamera ( Bool _freeCamera ) {
	this->freeCamera = _freeCamera;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �J�����^�C�v���t���[�J�����̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool CameraControllerT<T>::isFreeCamera ()const {
	return this->freeCamera;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ��U��V�~�����[�V�����Ŏg�p���郁���o�[�ϐ�������������
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CameraControllerT<T>::reset () {
	this->hShake = HandShakeT<T> ();
	this->wShake = WalkShakeT<T> ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �t���[�J����������Ă��钍���_����J�����܂ł̋������X�P�[������
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CameraControllerT<T>::setOffsetScale ( T s ) {
	this->interestOffset0 *= s;
	this->interestOffset1 *= s;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �e�폈�������s���A�J�����̃|�W�V�����E�����_�E�A�b�v�x�N�^�[���擾����
//!	    @brief �e�폈�������s���A�J�����̃|�W�V�����E�����_�E�A�b�v�x�N�^�[���擾����
//!		@param[out] outCameraPosition �J�����̃|�W�V���� Local-Vec3
//!		@param[out] outInterestPosition �J�����̒����_ Local-Vec3
//!		@param[out] outUpvector �J�����̃A�b�v�x�N�^�[ Local-Vec3
//!		@param[in] cameraGlobalPose �J�����̃R���g���[�� Global-Mat4
//!		@param[in] InterestGlobalPosition �����_�� Global-Mat4
//!		@param[in] time �J�����g�t���[��
//!		@param[in] roll �J�������[��
//!		@param[in] handShake ��U��m�C�Y��ON/OFF
//!		@param[in] handShakePow ��U��̋����i���E�c�����j
//!		@param[in] handShakeTimeScale ��U��̃^�C���X�P�[��
//!		@param[in] vibration �U���m�C�Y��ON/OFF
//!		@param[in] vibrationPow �U���̋����i���E�c�����j
//!		@param[in] vibrationTimeScale �U���̃^�C���X�P�[��
//!		@param[in] walkShake �����m�C�Y��ON/OFF
//!		@param[in] walkShakeCameraPow �����̃J�����̃m�C�Y�̋����i���E�c�����j
//!		@param[in] walkShakeInterestPow �����̒����_�̃m�C�Y�̋����i���E�c�����j
//!		@param[in] walkShakeRollPow �����̃J�������[���̃m�C�Y�̋���
//!		@param[in] walkShakeTimeScale �����m�C�Y�̃^�C���X�P�[��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CameraControllerT<T>::solve (
	OUT Vec3T<T> &outCameraPosition,
	OUT Vec3T<T> &outInterestPosition,
	OUT Vec3T<T> &outUpvector,
	const Mat4T<T> &cameraGlobalPose,
	const Vec3T<T> &InterestGlobalPosition,
	const T time,
	const T roll,
	const Bool handShake,
	const Vec2T<T> &handShakePow,
	const T handShakeTimeScale,
	const Bool vibration,
	const Vec2T<T> &vibrationPow,
	const T vibrationTimeScale,
	const Bool walkShake,
	const Vec2T<T> &walkShakeCameraPow,
	const Vec2T<T> &walkShakeInterestPow,
	const T walkShakeRollPow,
	const T walkShakeTimeScale ) 
{
	QuatT<T> cameraGlobalRot = cameraGlobalPose.getRotation ();
	// �m�C�Y�����p�̃J����Mat4���쐬
	Vec3T<T> dir;// �J���������_
	if ( this->freeCamera ) {
		dir = cameraGlobalRot.rotateVector ( this->interestOffset0 ); // -Z���Ƀ_�~�[�̒����_�𐶐�
	} else {
		dir = InterestGlobalPosition - cameraGlobalPose.getTranslation (); // �����_�R���g���[���̈ʒu���g�p
	}

	QuatT<T> rot;// �J�����̉�]�l
	rot.setFromDirectionAndUpvector ( dir, cameraGlobalRot.rotateVector( Vec3T<T> { 0.0, 1.0, 0.0 } ));
	rot = rot.addAngle ( Deg3T<T>{0.0, 0.0, roll} ); // �J�������[���l

	Mat4T<T> cameraPose;// �m�C�Y�����p�̃J����Mat4
	cameraPose.setTranslation ( cameraGlobalPose.getTranslation () );
	cameraPose.setUpperLeft ( Mat3T<T> ( rot ) );
	
	// ������U��m�C�Y
	this->wShake.solve (
		outCameraPosition,
		outInterestPosition,
		outUpvector,
		walkShake,
		time,
		cameraPose,
		cameraPose * this->interestOffset1,
		walkShakeCameraPow,
		walkShakeInterestPow,
		walkShakeRollPow,
		walkShakeTimeScale );
	// ��U��/�U���m�C�Y�𐶐�
	Vec2T<T> addShake;
	if ( handShake ) {
		// ��U��m�C�Y
		this->hShake.solve ( 
			addShake,
			time, 
			cameraPose.getTranslation(),
			handShakePow, 
			handShakeTimeScale );
	}
	if ( vibration ) {
		// �U���m�C�Y
		Vec3T<T> vib = Noise::Vibration ( time, Vec3T<T> ( vibrationPow.x, vibrationPow.y, T ( 0.0 ) ), vibrationTimeScale );
		addShake.x += vib.x;
		addShake.y += vib.y;
	}
	// ��U��/�U���m�C�Y���J�����̒����_�ƃA�b�v�x�N�^�[�ɉ��Z
	outInterestPosition = rot.rotateVector ( Vec3T<T>{ outInterestPosition.x + addShake.x, outInterestPosition.y + addShake.y, T ( 10.0 ) } );
	outUpvector.x += addShake.x;
	outUpvector.y += addShake.y;
	// ���[�J���l�ɕϊ�
	rot = cameraGlobalRot.inverse ();
	outInterestPosition = rot.rotateVector ( outInterestPosition );
	outUpvector = rot.rotateVector ( outUpvector );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �e�폈�������s���A�J������Local-Mat4���擾����
//!	    @brief �e�폈�������s���A�J������Local-Mat4���擾����
//!		@param[in] cameraGlobalPose �J�����̃R���g���[�� Global-Mat4
//!		@param[in] InterestGlobalPosition �����_�� Global-Mat4
//!		@param[in] time �J�����g�t���[��
//!		@param[in] roll �J�������[��
//!		@param[in] handShake ��U��m�C�Y��ON/OFF
//!		@param[in] handShakePow ��U��̋����i���E�c�����j
//!		@param[in] handShakeTimeScale ��U��̃^�C���X�P�[��
//!		@param[in] vibration �U���m�C�Y��ON/OFF
//!		@param[in] vibrationPow �U���̋����i���E�c�����j
//!		@param[in] vibrationTimeScale �U���̃^�C���X�P�[��
//!		@param[in] walkShake �����m�C�Y��ON/OFF
//!		@param[in] walkShakeCameraPow �����̃J�����̃m�C�Y�̋����i���E�c�����j
//!		@param[in] walkShakeInterestPow �����̒����_�̃m�C�Y�̋����i���E�c�����j
//!		@param[in] walkShakeRollPow �����̃J�������[���̃m�C�Y�̋���
//!		@param[in] walkShakeTimeScale �����m�C�Y�̃^�C���X�P�[��
//!		@return �J�����̃|�[�Y Local-Mat4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat4T<T> CameraControllerT<T>::solve (
	const Mat4T<T> &cameraGlobalPose,
	const Vec3T<T> &InterestGlobalPosition,
	const T time,
	const T roll,
	const Bool handShake,
	const Vec2T<T> &handShakePow,
	const T handShakeTimeScale,
	const Bool vibration,
	const Vec2T<T> &vibrationPow,
	const T vibrationTimeScale,
	const Bool walkShake,
	const Vec2T<T> &walkShakeCameraPow,
	const Vec2T<T> &walkShakeInterestPow,
	const T walkShakeRollPow,
	const T walkShakeTimeScale ) {
	Vec3T<T> outCameraPosition, outInterestPosition, outUpvector;
	this->solve ( outCameraPosition, outInterestPosition, outUpvector,
		cameraGlobalPose,
		InterestGlobalPosition,
		time,
		roll,
		handShake,
		handShakePow,
		handShakeTimeScale,
		vibration,
		vibrationPow,
		vibrationTimeScale,
		walkShake,
		walkShakeCameraPow,
		walkShakeInterestPow,
		walkShakeRollPow,
		walkShakeTimeScale );

	Mat4T<T> result;
	QuatT<T> rot;
	rot.setFromDirectionAndUpvector ( outCameraPosition - outInterestPosition , outUpvector - outCameraPosition );
	result.setUpperLeft ( Mat3T<T> ( rot ) );
	result.setTranslation ( outCameraPosition );
	return result;
}