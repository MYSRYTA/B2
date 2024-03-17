#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Noise.h>

//******************************************************************************
//! @file	HandShake.h
//! @brief	 èU‚êƒmƒCƒY
//******************************************************************************
/** @defgroup Tools
*@@{
*/
/**	@defgroup CameraController
*@	@{
*/
/**		@brief	èU‚êƒmƒCƒY<br>
*			alias : \b HandShake = HandShakeT<float> // \b HandShaked = HandShakeT<double> <br>
*			#include <burgess2_toolbox/CameraController/HandShake.h> <br>
*/

template<typename T>
class HandShakeT
{
	PinkT<T> core;
public:
	HandShakeT () { this->core = PinkT<T> (); };
	virtual  ~HandShakeT (){}

	void solve ( OUT Vec2T<T> &outHandShake, T time, const Vec3T<T> &cameraPos, const Vec2T<T> &shakePow, T timeScale = T(1.0) );
	void solve ( OUT Vec3T<T> &outHandShake, T time, const Vec3T<T> &cameraPos, const Vec3T<T> &shakePow, T timeScale = T ( 1.0 ) );
};

#include "HandShake.inl"
/** @} */
/** @} */
using HandShake = HandShakeT<f32>;
using HandShaked = HandShakeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************