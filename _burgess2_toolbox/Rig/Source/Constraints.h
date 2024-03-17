#pragma once
#include <burgess2_main/Math.h>
#include <burgess2_main/Geometry.h>

//******************************************************************************
//! @file	Constraint.h
//! @brief	 コンストレイン
//******************************************************************************
/** @defgroup Rig
*　@{
*/
/**		@brief	Constraint <br>
*			#include <burgess2_main/Rig/Rig.h> <br>
*/

namespace Constraint {
	template<typename T>
	Vec3T<T> Position2 ( const Vec3T<T> &pos0, const Vec3T<T> &pos1, T weight );

	template<typename T>
	Vec3T<T> Position3 ( const Vec3T<T> &pos0, const Vec3T<T> &pos1, const Vec3T<T> &pos2, const Vec3T<T> &weight );

	template<typename T>
	Vec3T<T> PositionN ( const std::vector<Vec3T<T>> &positions, const std::vector<T> &weights );

	template<typename T>
	Vec3T<T> Distance ( const Mat4T<T> &parent, const Vec3T<T> &delta, T weigth = T(1.0) );

	template<typename T>
	QuatT<T> Orientation ( const QuatT<T> &ori0, const QuatT<T> &ori1, T weight, const QuatT<T> &offset = QuatT<T> () );

	template<typename T>
	QuatT<T> Direction ( const Vec3T<T> &position, const Vec3T<T> &target, const Vec3T<T> &upVector, u8 axisType, const QuatT<T> &offset = QuatT<T> () );

	template<typename T>
	Vec3T<T> LimitAngle (const Vec3T<T> &inVector, const Vec3T<T> &baseVector, T maxDegryAngle );

	template<typename T>
	Mat4T<T> Mirroring ( const Mat4T<T> &inMat4, u8 mirrorNormal );

	template<typename T>
	Vec3T<T> Curve ( const CurvesBaseT<T> &curve, u32 curveID, T val, u8 valType );

	template<typename T>
	Mat4T<T> Curve ( const CurvesBaseT<T> &curve, u32 curveID, T val, u8 valType, Vec3T<T> &upVector, u8 axisType );
}

#include "Constraints.inl"

/** @} */
//******************************************************************************
//! End of File
//******************************************************************************