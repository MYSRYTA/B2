//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     •à‚«èU‚ê‚ÌƒJƒƒ‰ˆÊ’u‚ğZo‚·‚é
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> WalkShakeT<T>::solveCamPos ( const Vec3T<T> &inCamPos, const Vec2T<T> &shakePow, T timeScale ) {
	this->camraLocAPF = this->camraLocAPF.almostEqual ( inCamPos, T ( 0.01 ) ) ? inCamPos : inCamPos.lerp ( this->camraLocAPF, T ( 0.1 ) );

	Vec3T<T> ptVec3 = Noise::PerlinTurbulent ( T ( 0.0 ), inCamPos, Vec3T<T>{ T ( 0.0 ) }, Vec3T<T>{T ( 1.0 )}, Vec3T<T>{timeScale}, Vec3T<T>{timeScale * T ( 1.1 )}, true );

	Vec3T<T> result = ptVec3 - this->camraLocAPF;
	result.x = result.x * shakePow.x * T ( 0.01 );
	result.y = result.y * shakePow.y * T ( 0.075 );
	result.z = T ( 0.0 );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     •à‚«èU‚ê‚ÌƒJƒƒ‰‚ÌŒX‚«‚ğZo‚·‚é
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void WalkShakeT<T>::solveCamDirAndUpV (
	OUT Vec3T<T> &interest,
	OUT Vec3T<T> &uvVector,
	const Vec3T<T> &inIntePos,
	const Vec2T<T> &inteShakePow,
	const T rollShakePow,
	T timeScale ) {
	this->camraLocAPF2 = this->camraLocAPF2.almostEqual ( inIntePos ) ? inIntePos : inIntePos.lerp ( this->camraLocAPF2, T ( 0.5 ) );

	Vec3T<T> ptVec3 = Noise::PerlinTurbulent ( T ( 0.0 ), inIntePos, Vec3T<T>{T ( 0.0 )}, Vec3T<T>{T ( 0.5 )}, Vec3T<T>{timeScale}, Vec3T<T>{timeScale * T ( 1.1 )} );
	Vec3T<T> restIntePos = ptVec3 + ( inIntePos - this->camraLocAPF2 );

	uvVector.x = rollShakePow * restIntePos.x * restIntePos.y * T ( -0.15 ); // Šp“x10.0“x
	uvVector.y = T ( 3.0 );
	uvVector.z = T ( 0.0 );

	interest.x = restIntePos.x * inteShakePow.x * T ( -0.02 );
	interest.y = restIntePos.y * inteShakePow.y * T ( 0.05 );
	interest.z = T ( -10.0 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     •à‚«èU‚ê‚ÌƒJƒƒ‰ˆÊ’u‚ÆŒX‚«‚ğZo‚·‚é
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void WalkShakeT<T>::solve (
	OUT Vec3T<T> &cameraPos,
	OUT Vec3T<T> &interest,
	OUT Vec3T<T> &upVector,
	Bool enable,
	T time,
	const Mat4T<T> &inCameraPose,
	const Vec3T<T> &inIntePos,
	const Vec2T<T> &posShakePow,
	const Vec2T<T> &inteShakePow,
	T rollShakePow,
	T timeScale ) {
	if ( enable ) {
		cameraPos = this->solveCamPos ( inCameraPose.getTranslation(), posShakePow, timeScale );
		this->solveCamDirAndUpV ( interest, upVector, inIntePos, inteShakePow, rollShakePow, timeScale );
	} else {
		cameraPos = Vec3T<T> { 0.0 };
		interest = Vec3T<T> { 0.0, 0.0, -10.0 };
		upVector = Vec3T<T> { 0.0, 3.0, 0.0 };
	}

	QuatT<T> camRot = inCameraPose.getRotation ();
	cameraPos = camRot.rotateVector ( cameraPos );
	upVector = camRot.rotateVector ( upVector );
}