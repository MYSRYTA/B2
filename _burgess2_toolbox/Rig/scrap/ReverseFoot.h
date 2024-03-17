#pragma once
#include <burgess2_main/Math.h>

//******************************************************************************
//! @file	SlidingPivot.h
//! @brief	 Sliding Pivot [動的ピボット]
//******************************************************************************
/** @defgroup Rig
*　@{
*/
/**		@brief	Sliding Pivot [動的ピボット] <br>
*			alias : \b SlidingPivot = SlidingPivotT<float> // \b SlidingPivotd = SlidingPivotT<double> <br>
*			#include <burgess2_main/Rig/Rig.h> <br><br>
*/
// Sliding Pivot [動的ピボット]
template<typename T>
class SlidingPivotT {
	u8 type;
	Mat4T<T> pivots[5];
	Bool mirror = false;
public:
	String name;

	SlidingPivotT () {}
	virtual ~SlidingPivotT () {}

	void setMirror ( Bool mirror );

	void initialize(
		const Mat4T<T>& pivotBase,
		const Mat4T<T>& backPivot,
		const Mat4T<T>& frontPivot,
		const Mat4T<T>& leftPivot,
		const Mat4T<T>& rightPivot
	);

	Mat4T<T> solve (T pitchAngle, T rollAngle)const;
};

#include "SlidingPivot.inl"
/** @} */
using SlidingPivot = SlidingPivotT<f32>;
using SlidingPivotd = SlidingPivotT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************