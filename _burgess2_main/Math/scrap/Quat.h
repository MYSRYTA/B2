#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class Vec3T;
template<typename T> class VecT;
template<typename T> class Mat4T;
template<typename T> class Mat3T;
template<typename T> class Deg3T;
template<typename T> class Rad3T;

//******************************************************************************
//! @file	Quat.h
//! @brief	 Quaternion [クォータニオン]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	Quaternion [クォータニオン] <br>
*			alias : \b Quat = QuatT<float>// \b Quatd = QuatT<double><br>
*			#include <burgess2_main/Math.h> <br>
*			@code
*				Vec3 v ( 1.0f, 0.3f, -0.2f );
*				v.setNormal ();
*
*				Quat q;
*				q.setFromAxisAndAngle ( v, 0.1f );
*				Xfoing log = q.toString();	/// log = "QuatT:[0.047, 0.014, -0.009, 0.999]"
*			@endcode
*/
// Quaternion [クォータニオン]
template<typename T>
class QuatT{
public:
	Vec3T<T> v;
	T w;

	QuatT ();
	QuatT ( T _x, T _y, T _z, T _w );
	QuatT ( const std::vector<T> &arrayVal );
	QuatT ( const VecT<T> &vec );
	QuatT ( const Vec3T<T> &_v, T _w );
	QuatT ( const Vec4T<T> &_v );
	QuatT ( const Deg3T<T> &deg );
	QuatT ( const Rad3T<T> &rad );
	QuatT ( const Mat3T<T> &mat );
	QuatT ( const Mat4T<T> &mat );
	QuatT ( const QuatT &other );
	virtual  ~QuatT () {}

	u32 size () const { return 4; }

	void setIdentity ();
	void set ( const Vec3T<T> &_v, T _w );
	void setFromMat3 ( const Mat3T<T> &mat );
	void setFromRad3 ( const Rad3T<T> &rad );
	void setFromDeg3 ( const Deg3T<T> &deg );
	void setFromAxisAndAngle ( const Vec3T<T> &axis, T angle );
	void setFromDirectionAndUpvector ( const Vec3T<T> &direction, const Vec3T<T> &upvector, u8 type = 0, Bool dirNeg = false, Bool upVectorNeg = false );
	void setFrom2Vectors ( const Vec3T<T> &sourcVec, const Vec3T<T> &targetVec );
	void setFromExpmap ( const Vec3T<T> &_expmap );

	Vec3T<T> getXaxis () const;
	Vec3T<T> getYaxis () const;
	Vec3T<T> getZaxis () const;
	T getAngle () const;
	void getAxisAndAngle ( OUT Vec3T<T> &axis, OUT T &angle ) const;

	VecT<T> toVec () const;
	std::vector<T> toTypeArray ()const;
	Mat3T<T> toMat3 () const;
	Rad3T<T> toRad3 ( u8 order = 0 ) const; 
	Deg3T<T> toDeg3 ( u8 order = 0) const; 
	String toString ( const String title = "" )const;

	Bool equal ( const QuatT &other ) const;
	Bool almostEqual ( const QuatT &other, T tolerance = Burgess::TOLERANCE ) const;
	Bool isZero ()const;

	QuatT lerp ( const QuatT &other, T t ) const;
	QuatT slerp ( const QuatT &other, T t )const;
	QuatT linearInterpolate ( const QuatT &other, T t )const;
	QuatT sphericalLinearInterpolate ( const QuatT &other, T t )const;

	T length () const;
	T lengthSquared () const;

	T dist ( const QuatT &other )const;

	QuatT normal () const;
	void setNormal ();
	QuatT unit () const;
	void setUnit ();

	QuatT inverse () const;
	void setInverse () ;
	QuatT mirror ( u32 mirrorNorml )const ;
	void setMirror ( u32 mirrorNorml );
	QuatT mirror2 ( u32 mirrorNorml )const;
	void setMirror2 ( u32 mirrorNorml );

	QuatT conjugate () const;
	Vec3T<T> expmap ()const;

	T dot ( const QuatT &other ) const;
	
	Vec3T<T> rotatVector ( const Vec3T<T> &val ) const;

	QuatT<T> addAngle( const Deg3T<T> &deg ) const;
	QuatT<T> addAngle ( const Rad3T<T> &rad ) const;

	/*! @name Operator*/
	/* @{ */
	T& operator[]( u8 id );
	const T& operator[]( u8 id ) const;

	Bool operator == ( const QuatT &other ) const;
	Bool operator != ( const QuatT &other ) const;

	QuatT operator + ( const QuatT &other ) const;
	void operator += ( const QuatT &other );

	QuatT operator * ( const QuatT &other ) const;
	QuatT operator * ( T val ) const;
	void operator *= ( const QuatT &other );
	void operator *= ( T val );
	QuatT operator / ( const QuatT &other ) const;
	QuatT operator / ( T  val ) const;
	void operator /= ( const QuatT &other );	
	void operator /= ( T val );
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( v ),
			CEREAL_NVP ( w )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Quat.inl"

/** @} */

using Quat = QuatT<f32>;
using Quatd = QuatT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************