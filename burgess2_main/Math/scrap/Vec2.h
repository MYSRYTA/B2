#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;

//******************************************************************************
//! @file	Vec2.h
//! @brief	 2D Vector [2Dベクトル]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	2D Vector [2Dベクトル]<br>
*			alias : \b Vec2 = Vec2T<float> // \b Vec2d = Vec2T<double> // \b Vec2u = Vec2T<u32> <br>
*			#include <burgess2_main/Math.h> <br>
*			@code
*				Vec2 a ( 2.0f );
*				Vec2 b ( 1.1f, -3.9f );
*
*				Vec2 c = a * b;
*				Xfoing log = c.toString();	/// log = "Vec2T:[2.2, -7.8]"
*			@endcode
*/
// 2D Vector [2Dベクトル]
template<typename T>
class Vec2T {
public:
	T x;
	T y;

	Vec2T ();
	Vec2T ( T val );
	Vec2T ( T _x, T _y );
	Vec2T ( const VecT<T> &val );
	Vec2T ( const std::vector<T> &val );
	virtual  ~Vec2T () {}

	VecT<T> toVec () const;
	std::vector<T> toTypeArray () const;
	String toString ( const String title = "" )const;

	Bool equal ( const Vec2T &other ) const;
	Bool almostEqual ( const Vec2T &other, T tolerance = Burgess::TOLERANCE ) const;

	Bool parallel ( const Vec2T &other ) const;
	Bool almostParallel ( const Vec2T &other, T tolerance = Burgess::TOLERANCE ) const;

	Vec2T getMin ( const Vec2T &other )const;
	Vec2T getMax ( const Vec2T &other )const;
	Vec2T clamp ( const Vec2T &limitVal0, const Vec2T &limitVal1 ) const;

	Vec2T lerp ( const Vec2T &other, T t ) const;
	Vec2T slerp ( const Vec2T &other, T t ) const;
	Vec2T linearInterpolate ( const Vec2T &other, T t ) const;
	Vec2T sphericalLinearInterpolate ( const Vec2T &other, T t ) const;

	T lengthSquared () const;
	T length () const;

	Vec2T normal () const;
	void setNormal ();
	Vec2T unit ()const;
	void setUnit ();

	Vec2T negate () const;
	Vec2T inverse () const;
	Vec2T reflect ( const Vec2T &_normal ) const;

	T dot ( const Vec2T &other ) const;
	T cross ( const Vec2T &other ) const;

	T normalAngleTo ( const Vec2T &other ) const;
	T unitsAngleTo ( const Vec2T &other ) const;
	T angleTo ( const Vec2T &other ) const;

	T distanceTo ( const Vec2T &other ) const;
	Bool distanceLessThan ( const Vec2T &other, T dist )const;

	/*! @name Operator*/
	/* @{ */
	T& operator[]( u8 id );
	const T& operator[]( u8 id ) const;

	Bool operator == ( const Vec2T &other ) const;
	Bool operator != ( const Vec2T &other ) const;

	Vec2T operator + ( const Vec2T &other ) const;
	void operator += ( const Vec2T &other );
	Vec2T operator - ( const Vec2T &other ) const;
	void operator -= ( const Vec2T &other );

	Vec2T operator * ( const Vec2T &other ) const;
	void operator *= ( const Vec2T &other );
	Vec2T operator * ( T val ) const;
	void operator *= ( T val );
	Vec2T operator / ( const Vec2T &other ) const;
	void operator /= ( const Vec2T &other );
	Vec2T operator / ( T val ) const;
	void operator /= ( T val );
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( x ),
			CEREAL_NVP ( y )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Vec2.inl"
/** @} */

using Vec2 = Vec2T<f32>;
using Vec2d = Vec2T<f64>;
using Vec2u = Vec2T<u32>;
using Vec2s = Vec2T<s32>;

//******************************************************************************
//! End of File
//******************************************************************************