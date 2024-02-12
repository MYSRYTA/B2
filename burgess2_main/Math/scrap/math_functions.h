#pragma once
#include "../Core/base.h"
template<typename T> class VecT;
template<typename T> class Vec2T;
template<typename T> class Vec3T;
template<typename T> class Vec4T;
template<typename T> class Mat3T;
template<typename T> class Mat4T;
template<typename T> class QuatT;
template<typename T> class Deg3T;
template<typename T> class Ray3T;
template<typename T> class XfoT;

//******************************************************************************
//! @file	math_functions.h
//! @brief 便利関数
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**	@defgroup Functions
*　	@{
*/
/**	　@brief	汎用関数<br>
*			#include <burgess2_main/Math/Math.h> <br>
*			@code
*				// リスケール
*				f32 val1 ( 0.02 );
*				f32 val2 = Math::Rescale( val1, 0.2, 3.2, 0.0, 1.0 ); // 0.2~3.2内におけるval1を0.0~1.0の範囲にリスケールする
*				
*				// グローバル・ローカル値の算出
*				Mat4 globalVal1;
*				Mat4 globalVal2;
*				Mat4 localVal = Math::GetChildLocalMat4( globalVal1, globalVal2 ); // val1を親としてval2のローカル値を算出する
*				
*				//　複数個ランダム値を生成
*				std::vector<f32> f32random = Math::RandomTArray ( 10, 4, 8, 15, -10, 10 );
*
*				//　点からラインに対する値の取得
*				Vec3 P1( 1.5, 1.0, -0.5 );
*				f32 t = 0.3;
*				Vec3 P2 = Math::GetClosestPositionOnLineSegment ( P1, Vec3 ( 2.0f, 0.0f, 2.0f ), Vec3 ( 0.0f, 0.0f, -3.0f ), t );
*
*			@endcode
*/

namespace Math {
	/*! @name min / max*/
	/* @{ */
	template<typename Type>
	Type Min ( Type val1, Type val2 );
	template<typename Type>
	Type Minimum ( const std::vector<Type> &arrayVal );
	template<typename Type>
	u32 MinimumIndex ( const std::vector<Type> &arrayVal );
	template<typename Type>
	Type Max ( Type val1, Type val2 );
	template<typename Type>
	Type Maximum ( const std::vector<Type> &arrayVal );
	template<typename Type>
	u32 MaximumIndex ( const std::vector<Type> &arrayVal );
	
	template<typename T>
	T Clamp ( T val, T min, T max );
	/* @{ */

	/*! @name resample*/
	/* @{ */
	static f32 Rescale ( f32 inVal, f32 inMin, f32 inMax, f32 outMin, f32 outMax );
	static f64 Rescale ( f64 inVal, f64 inMin, f64 inMax, f64 outMin, f64 outMax );

	static f32 Lerp ( f32 valA, f32 valB, f32 t );
	static f64 Lerp ( f64 valA, f64 valB, f64 t );
	static f32 LinearInterpolate ( f32 valA, f32 valB, f32 t );
	static f64 LinearInterpolate ( f64 valA, f64 valB, f64 t );

	static f32 ReverseLerp ( f32 val, f32 minVal, f32 maxVal );
	static f64 ReverseLerp ( f64 val, f64 minVal, f64 maxVal );

	static s32 TreeInterpolate(const std::vector<s32>& tree, s32 key, s32& remainder);
	static s32 TreeInterpolate(const std::vector<s32>& tree, s32 key);
	static u32 TreeInterpolate ( const std::vector<u32> &tree, u32 key, u32 &remainder );
	static u32 TreeInterpolate ( const std::vector<u32> &tree, u32 key );
	static u32 TreeInterpolate ( const std::vector<f32> &tree, f32 key, f32 &remainder );
	static u32 TreeInterpolate ( const std::vector<f32> &tree, f32 key );
	/* @{ */

	/*! @name test*/
	/* @{ */
	static Bool AlmostEqual ( f32 valA, f32 valB, f32 tolerance = f32(Burgess::TOLERANCE) );
	static Bool AlmostEqual ( f64 valA, f64 valB, f64 tolerance = f64(Burgess::TOLERANCE) );

	static Bool IsZero ( f32 val );
	static Bool IsZero ( f64 val );
	static Bool AlmostZero ( f32 val, f32 tolerance = f32 ( Burgess::TOLERANCE ) );
	static Bool AlmostZero ( f64 val, f64 tolerance = f64 ( Burgess::TOLERANCE ) );
	/* @{ */

	/*! @name sort*/
	/* @{ */
	template<typename Type>
	void Sort (IO std::vector<Type> &arrayVal, OUT std::vector<u32> &arrayOrigID, Bool descendingOrder = false );
	template<typename Type>
	void Sort (IO std::vector<Type> &arrayVal, Bool descendingOrder = false );
	/* @{ */

	/*! @name convert*/
	/* @{ */
	template<typename Type>
	static Type RadToDeg ( Type rad );
	template<typename Type>
	static Type DegToRad ( Type deg );
	/* @{ */

	/*! @name random*/
	/* @{ */
	static u32 RandomRollU32 ( u32 seed );

	static s32 RandomS32 ( u32 seed, u32 index, s32 min, s32 max );
	static std::vector<s32> RandomS32Array ( u32 count, u32 seed, u32 index, u32 shift, s32 min, s32 max );

	template<typename T>
	static T RandomT ( u32 seed, u32 index, T min, T max );

	template<typename T>
	static std::vector<T> RandomTArray ( u32 count, u32 seed, u32 index, u32 shift, T min, T max );

	template<typename T>
	static Vec3T<T> RandomVec3 ( u32 seed, u32 index, T min, T max );

	template<typename T>
	static std::vector<Vec3T<T>> RandomVec3Array ( u32 count, u32 seed, u32 index, u32 shift, T min, T max );

	template<typename T>
	static QuatT<T> RandomQuat ( u32 seed, u32 index, T min, T max );

	template<typename T>
	static std::vector<QuatT<T>> RandomQuatArray ( u32 count, u32 seed, u32 index, u32 shift, T min, T max );

	template<typename T>
	static Deg3T<T> RandomDeg3 ( u32 seed, u32 index, T min, T max );

	template<typename T>
	static std::vector<Deg3T<T>> RandomDeg3Array ( u32 count, u32 seed, u32 index, u32 shift, T min, T max );
	/* @{ */

	/*! @name triangle*/
	/* @{ */
	template<typename T>
	Bool TestInsideTriangle ( const Vec3T<T> &P, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, T tolerance = Burgess::TOLERANCE );

	template<typename T>
	Vec3T<T> GetTriBarycentric ( const Vec3T<T> &P, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, T tolerance = Burgess::TOLERANCE, Bool normalize = true );

	template<typename T>
	Vec3T<T> GetClosestPositionOnTriangle ( const Vec3T<T> &P, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2 );

	template<typename T>
	Vec3T<T> GetTriangleNormal ( const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2 );
	/* @{ */

	/*! @name spatial*/
	/* @{ */
	template <typename T>
	u32 GetClosestPointID ( const std::vector<Vec3T<T>> &points, const Vec3T<T> &startPosition, T maxDistance = T (Burgess::F32_INFINITE) );

	template <typename T>
	std::vector<u32> GetPointIDsInSphere ( const std::vector<Vec3T<T>> &points, const Vec3T<T> &centerPosition, T sphereRadius, Bool sort );

	template <typename T>
	Vec3T<T> GetClosestPositionOnLine ( const Vec3T<T> &P, const Vec3T<T> &lineP0, const Vec3T<T> &lineP1, OUT T& segment_t );

	template <typename T>
	Vec3T<T> GetClosestPositionOnLine ( const Vec3T<T>& P, const Vec3T<T>& lineP0, const Vec3T<T>& lineP1 );

	template <typename T>
	Vec3T<T> GetClosestPositionOnLineSegment ( const Vec3T<T> &P, const Vec3T<T> &segmentP0, const Vec3T<T> &segmentP1, OUT T &segment_t, T tolerance = Burgess::TOLERANCE );

	template <typename T>
	Vec3T<T> GetClosestPositionOnLineSegment ( const Vec3T<T>& P, const Vec3T<T>& segmentP0, const Vec3T<T>& segmentP1 );

	template <typename T>
	Vec3T<T> GetClosestPositionOnRay ( const Ray3T<T> ray, const Vec3T<T> &P, OUT T &rayParam );

	template <typename T>
	T GetDistanceFromPointToLine ( const Vec3T<T> &P, const Vec3T<T> &lineP0, const Vec3T<T> &lineP1, OUT T& segment_t, T tolerance = Burgess::TOLERANCE );

	template <typename T>
	T GetDistanceFromPointToLine ( const Vec3T<T>& P, const Vec3T<T>& lineP0, const Vec3T<T>& lineP1 );

	template<typename T>
	T GetDistanceFromPointToLineSegment ( const Vec3T<T>& P, const Vec3T<T>& segmentP0, const Vec3T<T>& segmentP1, OUT T& segment_t, T tolerance = Burgess::TOLERANCE );

	template<typename T>
	T GetDistanceFromPointToLineSegment ( const Vec3T<T> &P, const Vec3T<T> &segmentP0, const Vec3T<T> &segmentP1 );

	template<typename T>
	T GetDistanceFromLineToLine ( const Vec3T<T> &line1_p0, const Vec3T<T> &line1_p1, const Vec3T<T> &line2_p0, const Vec3T<T> &line2_p1, OUT T &line1_t, OUT T &line2_t );

	template <typename T>
	Bool TestRayIntersectPlane ( const Ray3T<T> &ray, const Vec3T<T> &planePoint, const Vec3T<T> &planeNormal, T tolerance = Burgess::TOLERANCE );

	template <typename T>
	Bool TestRayIntersectTriangle ( const Ray3T<T> &ray, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, Bool doubleSide = false, T tolerance = Burgess::TOLERANCE );

	template <typename T>
	Vec3T<T> GetRayIntersectPositionToPlane ( const Ray3T<T> &ray, const Vec3T<T> &planePoint, const Vec3T<T> &planeNormal );

	template <typename T>
	Vec3T<T> GetRayIntersectPositionToTriangle ( const Ray3T<T> &ray, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, u8 faceSide = 0, T tolerance = Burgess::TOLERANCE );

	/* @{ */

	/*! @name global / local*/
	/* @{ */
	template<typename T>
	Mat4T<T> GetChildLocalMat4 ( const Mat4T<T> &parentGlobalMat4, const Mat4T<T> &childGlobalMat4 );

	template<typename T>
	Mat4T<T> GetChildGlobalMat4 ( const Mat4T<T> &parentGlobalMat4, const Mat4T<T> &childLocalMat4 );

	template<typename T>
	XfoT<T> GetChildLocalXfo ( const XfoT<T> &parentGlobalXfo, const XfoT<T> &childGlobalXfo );

	template<typename T>
	XfoT<T> GetChildGlobalXfo ( const XfoT<T> &parentGlobalXfo, const XfoT<T> &childLocalXfo );

	template<typename T>
	std::vector<Mat4T<T>> ChainToGlobalMat4 ( const Mat4T<T> &rootGMat4, const std::vector<Mat4T<T>> &chainLocalMat4 );

	template<typename T>
	std::vector<Mat4T<T>> ChainToLocalMat4 ( const Mat4T<T>& rootGMat4, const std::vector<Mat4T<T>>& chainGlobalMat4 );

	template<typename T>
	Mat4T<T> GetChainEndJointGlobalMat4 ( const Mat4T<T> &rootGMat4, const std::vector<Mat4T<T>> &chainLocalMat4 );
	/* @{ */

	/*! @name trigonometric function*/
	/* @{ */
	template<typename T>
	T CosineFormula ( T oppositeLength, T hypotenuseLength, T adjacentLength );
	/* @{ */
}
/** @} */
/** @} */

template<typename T>
QuatT<T> operator*( T val, QuatT<T> quat );
template<typename T>
VecT<T> operator * ( T val, VecT<T> vec3 );
template<typename T>
Vec2T<T> operator * ( T val, Vec2T<T> vec3 );
template<typename T>
Vec3T<T> operator * ( T val, Vec3T<T> vec3 );
template<typename T>
Vec4T<T> operator * ( T val, Vec4T<T> vec3 );

namespace Math {
	template<typename T>
	static std::vector<T> FillInterpolated ( T a, T b, u32 count, Bool popFirst, Bool popLast );
}

#include "math_functions.inl"
#include "free_functions.inl"
#include "random_functions.inl"

//******************************************************************************
//! End of File
//******************************************************************************