#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;
template<typename T> class Vec3T;
template<typename T> class Rad3T;
template<typename T> class QuatT;
template<typename T> class Mat4T;

//******************************************************************************
//! @file	Deg3.h
//! @brief	 Euler(degree) [オイラー（デグリー角）]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	Euler(degree) [オイラー（デグリー角）] <br>
*			alias : \b Deg3 = Deg3T<float> // \b Deg3d = Deg3T<double> <br>
*			#include <burgess2_main/Math.h> <br>
*			rotationOrder 0=xyz, 1=xzy, 2=yzx, 3=yxz, 4=zyx, 5=zxy <br>
*			@code
*				Deg3 aa ( 45, -10.3, 0.32 );
*				Deg3 bb ( 5, -180.3, 40.32 );
*				aa += bb;
*				Quat q( aa );
*			@endcode
*/
// Euler(degree) [オイラー（デグリー角）]
template<typename T>
class Deg3T {
public:
	T x;
	T y;
	T z;
	u8 rotationOrder;

	Deg3T ();
	Deg3T ( T val );
	Deg3T ( T _x, T _y, T _z, u8 order = 0 );
	Deg3T ( const VecT<T> &vec, u8 order = 0 );
	Deg3T ( const Vec3T<T> &vec, u8 order = 0 );
	Deg3T ( const Vec4T<T> &vec, u8 order = 0 );
	Deg3T ( const QuatT<T> &quat, u8 order = 0 );
	Deg3T ( const Rad3T<T> &deg );
	Deg3T ( const Mat3T<T> &quat, u8 order = 0 );
	Deg3T ( const Mat4T<T> &quat, u8 order = 0 );
	Deg3T ( const std::vector<T> &arrayVal, u8 order = 0 );
	virtual ~Deg3T () {};

	VecT<T> toVec () const;
	std::vector<T> toTypeArray () const;
	Rad3T<T> toRad3 ()const;
	QuatT<T> toQuat ()const;
	Mat3T<T> toMat3 ()const;
	String toString ( const String title = "" )const;

	void setFromMat3 ( const Mat3T<T> &mat3, u8 order = 0 );

	Bool equal ( const Deg3T &other ) const;
	Bool almostEqual ( const Deg3T &other, T tolerance = Burgess::TOLERANCE ) const;

	Vec3T<T> rotatVector ( const Vec3T<T> &vec3 ) const;

	/*! @name Operator*/
	/* @{ */
	T& operator[]( u8 id );
	const T& operator[]( u8 id ) const;

	Bool operator == ( const Deg3T &other )const;
	Bool operator != ( const Deg3T &other )const;

	Deg3T operator + ( const Deg3T &other )const;
	void operator += ( const Deg3T &other );
	Deg3T operator - ( const Deg3T &other ) const;
	void operator -= ( const Deg3T &other );
	/** @} */

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

#include "Deg3.inl"

/** @} */
using Deg3 = Deg3T<f32>;
using Deg3d = Deg3T<f64>;

//******************************************************************************
//! End of File
//******************************************************************************