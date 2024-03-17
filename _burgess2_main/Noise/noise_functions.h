#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Math/Vec3.h>

//******************************************************************************
//! @file	noise_functions.h
//! @brief	 ノイズ関数
//******************************************************************************
/** @defgroup Noise
*　@{
*/
/**		@brief	ノイズ関数<br>
*			#include <burgess2_main/Noise.h> <br>
*/

namespace Burgess2NoiseFn {
///	cond
	template<typename T>
	T PerlinFade ( T t );

	template<typename T>
	T PerlinLerp ( T t, T a, T b );

	template<typename T>
	T PerlinGrad ( s32 hash, T x, T y, T z );
/// encond

	template<typename T>
	T Perlin ( T x, T y, T z );

	template<typename T>
	Vec3T<T> PerlinTurbulent ( T time,
		const Vec3T<T> &pos,
		const Vec3T<T> &center,
		const Vec3T<T> &amplitude,
		const Vec3T<T> &timeFrequency,
		const Vec3T<T> &spaceFrequency,
		Bool apply = false );

	template<typename T>
	Vec3T<T> Vibration ( T time, const Vec3T<T> &vibrationPow, T timeScale = T ( 1.0 ) );

}

#include "noise_functions.inl"
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************