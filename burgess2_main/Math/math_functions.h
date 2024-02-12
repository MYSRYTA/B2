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
//! @brief 汎用関数
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
*				f32 val2 = Burgess2MathFn::Rescale( val1, 0.2, 3.2, 0.0, 1.0 ); // 0.2~3.2内におけるval1を0.0~1.0の範囲にリスケールする
*				
*				// グローバル・ローカル値の算出
*				Mat4 globalVal1;
*				Mat4 globalVal2;
*				Mat4 localVal = Burgess2MathFn::GetChildLocalMat4( globalVal1, globalVal2 ); // val1を親としてval2のローカル値を算出する
*				
*				//　複数個ランダム値を生成
*				std::vector<f32> f32random = Burgess2MathFn::RandomTArray ( 10, 4, 8, 15, -10, 10 );
*
*				//　点からラインに対する値の取得
*				Vec3 P1( 1.5, 1.0, -0.5 );
*				f32 t = 0.3;
*				Vec3 P2 = Burgess2MathFn::GetClosestPositionOnLineSegment ( P1, Vec3 ( 2.0f, 0.0f, 2.0f ), Vec3 ( 0.0f, 0.0f, -3.0f ), t );
*
*			@endcode
*/

namespace Burgess2MathFn {
	/*! @name min / max*/
	/* @{ */
	template<typename T>
	T Min ( T val1, T val2 );
	template<typename T>
	T Minimum ( const std::vector<T> &arrayVal );
	template<typename T>
	u32 MinimumIndex ( const std::vector<T> &arrayVal );
	template<typename T>
	T Max ( T val1, T val2 );
	template<typename T>
	T Maximum ( const std::vector<T> &arrayVal );
	template<typename T>
	u32 MaximumIndex ( const std::vector<T> &arrayVal );
	
	template<typename T>
	T Clamp ( T val, T min, T max );
	/* @{ */

	/*! @name resample*/
	/* @{ */
	template<typename T>
	static T Rescale ( T inVal, T inMin, T inMax, T outMin, T outMax );

	template<typename T>
	static T Lerp ( T valA, T valB, T t );
	template<typename T>
	static T LinearInterpolate ( T valA, T valB, T t );

	template<typename T>
	static T ReverseLerp ( T val, T minVal, T maxVal );

	template<typename T>
	static s32 BinarySearchTree( const std::vector<T>& tree, T key, T& remainder );
	template<typename T>
	static s32 BinarySearchTree( const std::vector<T>& tree, T key );
	/* @{ */

	/*! @name test*/
	/* @{ */
	template<typename T>
	static Bool AlmostEqual ( T valA, T valB, T tolerance = T(BURGESS2::TOLERANCE) );
	template<typename T>
	static Bool IsZero ( T val );
	template<typename T>
	static Bool AlmostZero ( T val, T tolerance = T ( BURGESS2::TOLERANCE ) );
	/* @{ */

	/*! @name sort*/
	/* @{ */
	template<typename T>
	void Sort (IO std::vector<T> &arrayVal, OUT std::vector<u32> &arrayOrigID, Bool descendingOrder = false );
	template<typename T>
	void Sort (IO std::vector<T> &arrayVal, Bool descendingOrder = false );
	/* @{ */

	/*! @name convert*/
	/* @{ */
	template<typename T>
	static T RadToDeg ( T rad );
	template<typename T>
	static T DegToRad ( T deg );
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
	Bool TestInsideTriangle ( const Vec3T<T> &P, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, T tolerance = BURGESS2::TOLERANCE );

	template<typename T>
	Vec3T<T> GetTriBarycentric ( const Vec3T<T> &P, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, T tolerance = BURGESS2::TOLERANCE, Bool normalize = true );

	template<typename T>
	Vec3T<T> GetClosestPositionOnTriangle ( const Vec3T<T> &P, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2 );

	template<typename T>
	Vec3T<T> GetTriangleNormal ( const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2 );
	/* @{ */

	/*! @name spatial query*/
	/* @{ */
	template <typename T>
	u32 GetClosestPointID ( const std::vector<Vec3T<T>> &points, const Vec3T<T> &startPosition, T maxDistance = T (BURGESS2::F32_INFINITE) );

	template <typename T>
	std::vector<u32> GetPointIDsInSphere ( const std::vector<Vec3T<T>> &points, const Vec3T<T> &centerPosition, T sphereRadius, Bool sort );

	template <typename T>
	Vec3T<T> GetClosestPositionOnLine ( const Vec3T<T> &P, const Vec3T<T> &lineP0, const Vec3T<T> &lineP1, OUT T& segment_t );

	template <typename T>
	Vec3T<T> GetClosestPositionOnLine ( const Vec3T<T>& P, const Vec3T<T>& lineP0, const Vec3T<T>& lineP1 );

	template <typename T>
	Vec3T<T> GetClosestPositionOnLineSegment ( const Vec3T<T> &P, const Vec3T<T> &segmentP0, const Vec3T<T> &segmentP1, OUT T &segment_t, T tolerance = BURGESS2::TOLERANCE );

	template <typename T>
	Vec3T<T> GetClosestPositionOnLineSegment ( const Vec3T<T>& P, const Vec3T<T>& segmentP0, const Vec3T<T>& segmentP1 );

	template <typename T>
	Vec3T<T> GetClosestPositionOnRay ( const Ray3T<T> ray, const Vec3T<T> &P, OUT T &rayParam );

	template <typename T>
	T GetDistanceFromPointToLine ( const Vec3T<T> &P, const Vec3T<T> &lineP0, const Vec3T<T> &lineP1, OUT T& segment_t, T tolerance = BURGESS2::TOLERANCE );

	template <typename T>
	T GetDistanceFromPointToLine ( const Vec3T<T>& P, const Vec3T<T>& lineP0, const Vec3T<T>& lineP1 );

	template<typename T>
	T GetDistanceFromPointToLineSegment ( const Vec3T<T>& P, const Vec3T<T>& segmentP0, const Vec3T<T>& segmentP1, OUT T& segment_t, T tolerance = BURGESS2::TOLERANCE );

	template<typename T>
	T GetDistanceFromPointToLineSegment ( const Vec3T<T> &P, const Vec3T<T> &segmentP0, const Vec3T<T> &segmentP1 );

	template<typename T>
	T GetDistanceFromLineToLine ( const Vec3T<T> &line1_p0, const Vec3T<T> &line1_p1, const Vec3T<T> &line2_p0, const Vec3T<T> &line2_p1, OUT T &line1_t, OUT T &line2_t );

	template <typename T>
	Bool TestRayIntersectPlane ( const Ray3T<T> &ray, const Vec3T<T> &planePoint, const Vec3T<T> &planeNormal, T tolerance = BURGESS2::TOLERANCE );

	template <typename T>
	Bool TestRayIntersectTriangle ( const Ray3T<T> &ray, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, Bool doubleSide = false, T tolerance = BURGESS2::TOLERANCE );

	template <typename T>
	Vec3T<T> GetRayIntersectPositionToPlane ( const Ray3T<T> &ray, const Vec3T<T> &planePoint, const Vec3T<T> &planeNormal );

	template <typename T>
	Vec3T<T> GetRayIntersectPositionToTriangle ( const Ray3T<T> &ray, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, u8 faceSide = 0, T tolerance = BURGESS2::TOLERANCE );

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
	Mat4T<T> GetChainEndGlobalMat4 ( const Mat4T<T> &rootGMat4, const std::vector<Mat4T<T>> &chainLocalMat4 );

	template<typename T>
	std::vector<XfoT<T>> ChainToGlobalXfo( const XfoT<T>& rootGXfo, const std::vector<XfoT<T>>& chainLocalXfo );

	template<typename T>
	std::vector<XfoT<T>> ChainToLocalXfo( const XfoT<T>& rootGXfo, const std::vector<XfoT<T>>& chainGlobalXfo );

	template<typename T>
	XfoT<T> GetChainEndGlobalXfo( const XfoT<T>& rootGXfo, const std::vector<XfoT<T>>& chainLocalXfo );
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

namespace Burgess2MathFn {
	template<typename T>
	static std::vector<T> FillInterpolated ( T a, T b, u32 count, Bool popFirst, Bool popLast );
}

#include "math_functions.inl"
#include "free_functions.inl"
#include "random_functions.inl"

//******************************************************************************
//! End of File
//******************************************************************************