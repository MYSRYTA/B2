#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;
template<typename T> class Vec3T;

//******************************************************************************
//! @file	Vec4.h
//! @brief	4D Vector [4Dベクトル]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	4D Vector [4Dベクトル]<br>
*			alias : \b Vec4 = Vec4T<float> // \b Vec4d = Vec4T<double><br>
*			#include <burgess2_main/Math.h> <br>
*			@code
*				Vec4 a ( 1.0f );
*				Vec4 b ( 1.1f, -3.9f, -2.2f, 2.0f );
*
*				Vec4 c = a.max ( b );
*				Xfoing log = c.toString();	/// log = "Vec4T:[1.1, 1.0, 1.0, 2.0]"
*			@endcode
*/
// 4D Vector [4Dベクトル]
template<typename T>
class Vec4T
{
public:
	T x;
	T y;
	T z;
	T t;

	Vec4T ();
	Vec4T ( T _x, T _y, T _z, T _t );
	Vec4T ( T val );
	Vec4T ( const VecT<T> &val );
	Vec4T ( const QuatT<T> &val );
	Vec4T ( const std::vector<T> &val );
	Vec4T ( const Vec3T<T> &val );
	virtual  ~Vec4T () {};

	VecT<T> toVec () const;
	std::vector<T> toTypeArray () const;
	String toString ( const String title = "" )const;

	Bool equal ( const Vec4T &other ) const;
	Bool almostEqual ( const Vec4T &other, T tolerance = Burgess::TOLERANCE ) const;

	Vec4T getMin ( const Vec4T &other )const;
	Vec4T getMax ( const Vec4T &other )const;
	Vec4T clamp ( const Vec4T &limitVal0, const Vec4T &limitVal1 ) const;

	Vec4T lerp ( const Vec4T &other, f32 _t ) const;
	Vec4T linearInterpolate ( const Vec4T &other, f32 _t ) const;

	T lengthSquared () const;
	T length () const;

	Vec4T normal () const;
	void setNormal ();
	Vec4T unit () const;
	void setUnit ();

	Vec4T negate () const;
	Vec4T inverse () const;

	T dot ( const Vec4T &other ) const;

	T normalAngleTo ( const Vec4T &other ) const;
	T unitsAngleTo ( const Vec4T &other ) const;
	T angleTo ( const Vec4T &other ) const;

	T distanceTo ( const Vec4T &other ) const;
	Bool distanceLessThan ( const Vec4T &other, T dist )const;

	/*! @name Operator*/
	/* @{ */
	inline T& operator[]( u8 id );
	inline const T& operator[]( u8 id ) const;

	Bool operator == ( const Vec4T &other ) const;
	Bool operator != ( const Vec4T &other ) const;

	Vec4T operator + ( const Vec4T &other ) const;
	void operator += ( const Vec4T &other );
	Vec4T operator - ( const Vec4T &other ) const;
	void operator -= ( const Vec4T &other );

	Vec4T operator * ( const Vec4T &other ) const;
	void operator *= ( const Vec4T &other );
	Vec4T operator * ( T val ) const;
	void operator *= ( T val );
	Vec4T operator / ( const Vec4T &other ) const;
	void operator /= ( const Vec4T &other );
	Vec4T operator / ( T val ) const;
	void operator /= ( T val );
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( x ),
			CEREAL_NVP ( y ),
			CEREAL_NVP ( z ),
			CEREAL_NVP ( t )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Vec4.inl"
/** @} */

using Vec4 = Vec4T<f32>;
using Vec4d = Vec4T<f64>;

//******************************************************************************
//! End of File
//******************************************************************************