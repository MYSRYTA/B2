#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Noise.h>
#include "HandShake.h"
#include "walkShake.h"

//******************************************************************************
//! @file	CameraController.h
//! @brief	 手振れ付きカメラコントローラ
//******************************************************************************
/** @defgroup Tools
*　@{
*/
/**	@defgroup CameraController
*　	@{
*/
/**		@brief	手振れ付きカメラコントローラ<br>
*			alias : \b CameraController = CameraControllerT<float> // \b CameraController = CameraControllerT<double> <br>
*			#include <Burgess_Tools/CameraController/CameraController.h> <br>
*/

template<typename T>
class CameraControllerT {
	HandShakeT<T> hShake{ HandShakeT<T> () };
	WalkShakeT<T> wShake { WalkShakeT<T> () };
	Vec3T<T> interestOffset0 { T ( 0.004 ), T ( 0.0025 ), T ( -10.0 ) };
	Vec3T<T> interestOffset1 { T ( -0.004 ), T ( 0.0025 ), T ( 10.0 ) };
	Bool freeCamera;
public:
	CameraControllerT () {};
	CameraControllerT ( T offsetScale ) { this->setOffsetScale ( offsetScale );};
	virtual  ~CameraControllerT () {};

	void setFreeCamera ( Bool freeCamera );
	Bool isFreeCamera ()const;
	void reset ();
	void setOffsetScale ( T s );

	void solve (
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
		const T walkShakeTimeScale );

	Mat4T<T> solve (
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
		const T walkShakeTimeScale );
};

#include "CameraController.inl"
/** @} */
/** @} */
using CameraController = CameraControllerT<f32>;
using CameraControllerd = CameraControllerT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************