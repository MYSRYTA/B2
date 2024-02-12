#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;
template<typename T> class QuatT;
template<typename T> class Vec3T;
template<typename T> class Deg3T;

//******************************************************************************
//! @file	Rad3.h
//! @brief	 Euler (radian) [オイラー(ラジアン角)]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	Euler (radian) [オイラー(ラジアン角)]<br>
*			alias : \b Rad3 = Rad3T<float>// \b Rad3d = Rad3T<double><br>
*			#include <burgess2_main/Math.h> <br>
*			rotationOrder 0=xyz, 1=xzy, 2=yzx, 3=yxz, 4=zyx, 5=zxy <br>
*			@code
*				Mat3 m3 { 0.813797f, 0.094492f, 0.573414f, 0.469846f, -0.687671f, -0.55349f, 0.34202f, 0.719846f, -0.604022f };
*				Rad3 r1;
*				r1.setFromMat3 ( m3, 0 );
*				Rad r2 = Rad3 { 2.268928f,  -0.349065f, 0.523598f, 0 };
*				Bool e = r1.almostEqual( r2 );
*			@endcode
*/
// Euler (radian) [オイラー(ラジアン角)]
template<typename T>
class Rad3T {
public:
	T x;
	T y;
	T z;
	u8 rotationOrder;

	Rad3T ();
	Rad3T ( T _x, T _y, T _z, u8 order = 0 );
	Rad3T ( const VecT<T> &vec, u8 order = 0 );
	Rad3T ( const Vec3T<T> &quat, u8 order = 0 );
	Rad3T ( const Vec4T<T> &quat, u8 order = 0 );
	Rad3T ( const QuatT<T> &quat, u8 order = 0 );
	Rad3T ( const Mat3T<T> &mat3, u8 order = 0 );
	Rad3T ( const Mat4T<T> &mat4, u8 order = 0 );
	Rad3T ( const std::vector<T> &arrayVal, u8 order = 0 );
	Rad3T ( const Deg3T<T> &deg );
	virtual ~Rad3T () {}

	VecT<T> toVec () const;
	std::vector<T> toTypeArray () const;
	Deg3T<T> toDeg3 ()const;
	QuatT<T> toQuat ()const;
	Mat3T<T> toMat3 ()const;
	String toString ( const String title = "" )const;

	void setFromMat3 ( const Mat3T<T> &mat3, u8 order = 0 );

	Bool equal ( const Rad3T &other ) const;
	Bool almostEqual ( const Rad3T &other, T tolerance = Burgess::TOLERANCE ) const;

	Vec3T<T> rotatVector ( const Vec3T<T> &val ) const;

	/*! @name Operator*/
	/* @{ */
	T& operator[]( u8 id );
	const T& operator[]( u8 id ) const;

	Bool operator == ( const Rad3T &other )const;
	Bool operator != ( const Rad3T &other )const;

	Rad3T operator + ( const Rad3T &other )const;
	void operator += ( const Rad3T &other );
	Rad3T operator - ( const Rad3T &other ) const;
	void operator -= ( const Rad3T &other );
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( x ),
			CEREAL_NVP ( y ),
			CEREAL_NVP ( z ),
			CEREAL_NVP ( rotationOrder )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Rad3.inl"

/** @} */

using Rad3 = Rad3T<f32>;
using Rad3d = Rad3T<f64>;

//******************************************************************************
//! End of File
//******************************************************************************