#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;
template<typename T> class QuatT;
template<typename T> class Deg3T;
template<typename T> class Rad3T;

//******************************************************************************
//! @file	Vec3.h
//! @brief	3D Vector [3Dベクトル]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	3D Vector [3Dベクトル]<br>
*			alias : \b Vec3 = Vec3T<float> // \b Vec3d = Vec3T<double> // \b Vec3s = Vec3T<int> // \b Vec3u = Vec3T<unsigned int><br>
*			#include <burgess2_main/Math.h> <br>
*			@code
*				Vec3 a;
*				a.x = 2.0f;
*				a.y = 5.9f;
*				Vec3 b ( 1.0f, 3.9f, -2.2f );
*			
*				Vec3 c = a.lerp ( b, 0.5f );
*			@endcode
*/
// 3D Vector [3Dベクトル]
template<typename T>
class Vec3T{
public:
	T x;
	T y;
	T z;

	Vec3T ();
	Vec3T ( T val );
	Vec3T ( T _x, T _y, T _z );
	Vec3T ( const VecT<T> &val );
	Vec3T ( const Rad3T<T> &val );
	Vec3T ( const Deg3T<T> &val );
	Vec3T ( const std::vector<T> &val );
	virtual  ~Vec3T (){}

	VecT<T> toVec () const;
	std::vector<T> toTypeArray () const;
	String toString ( const String title = "" )const;

	Bool equal ( const Vec3T &other ) const;
	Bool almostEqual ( const Vec3T &other, T tolerance = Burgess::TOLERANCE ) const;
	Bool isZero ()const;

	Bool parallel ( const Vec3T &other ) const;
	Bool almostParallel ( const Vec3T &other, T tolerance = Burgess::TOLERANCE ) const;

	Bool sameDirection ( const Vec3T &other, Bool unitVector = false )const;
	Bool almostSameDirection ( const Vec3T &other, T tolerance = Burgess::TOLERANCE, Bool unitVector = false ) const;

	Vec3T getMin ( const Vec3T &other )const;
	Vec3T getMax ( const Vec3T &other )const;
	Vec3T clamp ( const Vec3T &limitVal0, const Vec3T &limitVal1 ) const;

	Vec3T lerp ( const Vec3T &other, T t ) const;
	Vec3T slerp ( const Vec3T &other, T t ) const;
	Vec3T linearInterpolate ( const Vec3T &other, T t ) const;
	Vec3T sphericalLinearInterpolate ( const Vec3T &other, T t ) const;

	T lengthSquared () const;
	T length () const;
	T magnitudeSquared ()const;
	T magnitude ()const;

	Bool isNotZeroLength ()const;
	Vec3T normal () const;
	void setNormal ();
	Vec3T unit ()const;
	void setUnit ();

	Vec3T negate () const;
	void setNegate ();
	Vec3T inverse () const;
	void setInverse ();
	Vec3T reflect ( const Vec3T &_normal ) const;
	void setReflect ( const Vec3T &_normal );

	T dot ( const Vec3T &other ) const;
	static T dotProduct ( Vec3T &v, Vec3T &other );
	Vec3T cross ( const Vec3T &other ) const;

	T normalAngleTo ( const Vec3T &other ) const;
	T unitsAngleTo ( const Vec3T &other ) const;
	T angleTo ( const Vec3T &other ) const;

	T distanceTo ( const Vec3T &other ) const;
	Bool distanceLessThan ( const Vec3T &other, T dist )const;

	Vec3T<T> rotate ( const QuatT<T> &rot ) const;
	Vec3T<T> rotate ( const Rad3T<T> &rot ) const;
	Vec3T<T> rotate ( const Deg3T<T> &rot ) const;
	QuatT<T> rotateTo ( const Vec3T<T> &other ) const;

	/*! @name Operator*/
	/* @{ */
	T& operator[]( u8 id );
	const T& operator[]( u8 id ) const;

	Bool operator == ( const Vec3T &other ) const;
	Bool operator != ( const Vec3T &other ) const;

	Vec3T operator + ( const Vec3T &other ) const;
	void operator += ( const Vec3T &other );
	Vec3T operator - ( const Vec3T &other ) const;
	void operator -= ( const Vec3T &other );

	Vec3T operator * ( const Vec3T &other ) const;
	void operator *= ( const Vec3T &other );
	Vec3T operator * ( T val ) const;
	void operator *= ( T val );
	Vec3T operator / ( const Vec3T &other ) const;
	void operator /= ( const Vec3T &other );
	Vec3T operator / ( T val ) const;
	void operator /= ( T val );
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( x ),
			CEREAL_NVP ( y ),
			CEREAL_NVP ( z )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Vec3.inl"
/** @} */

using Vec3 = Vec3T<f32>;
using Vec3d = Vec3T<f64>;
using Vec3s = Vec3T<s32>;
using Vec3u = Vec3T<u32>;

//******************************************************************************
//! End of File
//******************************************************************************