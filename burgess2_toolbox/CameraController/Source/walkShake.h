#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Noise.h>

//******************************************************************************
//! @file	WalkShake.h
//! @brief	 歩きノイズ
//******************************************************************************
/** @defgroup Tools
*　@{
*/
/**	@defgroup CameraController
*　	@{
*/
/**		@brief	歩きノイズ<br>
*			alias : \b WalkShake = WalkShakeT<float> // \b WalkShaked = WalkShakeT<double> <br>
*			#include <burgess2_toolbox/CameraController/WalkShake.h> <br>
*/

template<typename T>
class WalkShakeT {
	Vec3T<T> camraLocAPF{ T ( 0.0 ) };
	Vec3T<T> camraLocAPF2{ T ( 0.0 ) };

	Vec3T<T> solveCamPos (  
		const Vec3T<T> &inCamPos, 
		const Vec2T<T> &shakePow, 
		T timeScale = T ( 1.0 ) 
	);

	void solveCamDirAndUpV (
		OUT Vec3T<T> &outInterestPos,
		OUT Vec3T<T> &outUpVector,
		const Vec3T<T> &inInterestPos,
		const Vec2T<T> &interestShakePow,
		T rollShakePow,
		T timeScale = T ( 1.0 )
	);
public:
	WalkShakeT () {};
	virtual  ~WalkShakeT () {};

	void solve (
		OUT Vec3T<T> &outCameraPos,
		OUT Vec3T<T> &outInterestPos,
		OUT Vec3T<T> &outUpVector,
		Bool enable,
		T time,
		const Mat4T<T> &inCameraPose,
		const Vec3T<T> &inInterestPos,
		const Vec2T<T> &posShakePow,
		const Vec2T<T> &interestShakePow,
		T rollShakePow,
		T timeScale = T ( 1.0 )
	);
};

#include "walkShake.inl"
/** @} */
/** @} */
using WalkShake = WalkShakeT<f32>;
using WalkShaked = WalkShakeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************