#pragma once
#include <burgess2_main/Core/base.h>
extern "C"
{
	#include <CLAPACK/include/f2c.h>
	#include <CLAPACK/include/clapack.h>
}

template<typename T> class XfoT;
template<typename T> class SRTT;
template<typename T> class VecT;
template<typename T> class Vec3T;
template<typename T> class Vec4T;
template<typename T> class QuatT;
template<typename T> class Deg3T;
template<typename T> class Mat3T;

//******************************************************************************
//! @file	Mat4.h
//! @brief	  4x4 Matrix [4x4マトリックス]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	4x4 Matrix [4x4マトリックス] <br>
*			alias : \b Mat4 = Mat4T<float>// \b Mat4d = Mat4T<double><br>
*			#include <burgess2_main/Math.h> <br>
*			@code
*				Mat4 ff;
*				ff.setDiagonal ( 5.0f );
*				Vec4 r0 ( 5, 0, 0, 0 );
*				Vec4 r1 ( 0, 5, 0, 0 );
*				Vec4 r2 ( 0, 0, 5, 0 );
*				Vec4 r3 ( 0, 0, 0, 5 );
*				Mat4 aa;
*				aa.setRowValues ( r0, r1, r2, r3 );
*				Bool e = aa.almostEqual( ff );
*			@endcode
*/
// 4x4 Matrix [4x4マトリックス]
template<typename T>
class Mat4T {
public:
	Vec4T<T> row [4];

	Mat4T ();
	Mat4T (
		T _00, T _01, T _02, T _03,
		T _10, T _11, T _12, T _13,
		T _20, T _21, T _22, T _23,
		T _30, T _31, T _32, T _33 );
	Mat4T ( const Mat3T<T> &m );
	Mat4T ( const Vec4T<T> &row0, const Vec4T<T> &row1, const Vec4T<T> &row2, const Vec4T<T> &row3 );
	Mat4T ( const VecT<T> &vec );
	Mat4T ( const std::vector<T> &typeArray );
	Mat4T ( const XfoT<T> &Xfo );
	Mat4T ( const SRTT<T> &srt );
	Mat4T ( const Mat4T<T> &m );
	virtual ~Mat4T () {};

	void setIdentity ();
	void setValue ( u8 row, u8 col, T value );
	void setAllVal ( T val );
	void setRowValues ( const Vec4T<T> &row0, const Vec4T<T> &row1, const Vec4T<T> &row2, const Vec4T<T> &row3 );
	void setColValues ( const Vec4T<T> &col0, const Vec4T<T> &col1, const Vec4T<T> &col2, const Vec4T<T> &col3 );
	void setDiagonal ( T val );
	void setDiagonal ( const Vec4T<T> &val );
	void setUpperLeft ( const Mat3T<T> &mat3 );
	void setRotationFromDirectionAndUpvector ( const Vec3T<T> &direction, const Vec3T<T> &upvector, u8 axis );
	void setTranslation ( const Vec3T<T> &val ) ;
	void setScaling (const Vec3T<T> &val);
	void axisChangeZY(u8 type, Bool ZNeg, Bool YNeg, Bool keepScaleAxis = false);
	void axisChangeXZ(u8 type, Bool XNeg, Bool ZNeg, Bool keepScaleAxis = false);

	inline T get ( u8 row, u8 col )const;
	Vec4T<T> getRowValues ( u8 rowIndex ) const;
	Vec4T<T> getColValues ( u8 colIndex ) const;
	Mat3T<T> getUpperLeft () const;
	Mat3T<T> getMat3 () const;
	Mat3T<T> getRotationMat3 () const;
	inline Vec3T<T> getTranslation () const;
	QuatT<T> getRotation () const;
	Vec3T<T> getScaling () const;

	VecT<T> toVec () const;
	std::vector<T> toTypeArray ()const;
	XfoT<T> toXfo ()const;
	SRTT<T> toSRT ( u8 order = 0 )const;
	String toString ( const String title = "" )const;

	Bool equal ( const Mat4T &other ) const;
	Bool almostEqual ( const Mat4T &other, T tolerance = Burgess::TOLERANCE ) const;
	Bool isZero ()const;

	Mat4T transpose () const;
	void setTranspose ();
	Mat4T inverse () const;
	void setInverse ();

	void swapRows ( u8 row1, u8 row2 );
	void swapCols ( u8 col1, u8 col2 );

	/*! @name Operator*/
	/* @{ */
	Vec4T<T>& operator[]( u8 id );
	const Vec4T<T>& operator[]( u8 id ) const;

	Bool operator == ( const Mat4T &other ) const;
	Bool operator != ( const Mat4T &other ) const;

	Mat4T operator + ( const Mat4T &other ) const;
	void operator += ( const Mat4T &other );
	Mat4T operator - ( const Mat4T &other ) const;
	void operator -= ( const Mat4T &other );

	Mat4T operator * ( const Mat4T &other ) const;
	void operator *= ( const Mat4T &other );
	Mat4T operator * ( T val ) const;
	void operator *= ( T val );
	Vec4T<T> operator * ( const Vec4T<T> &vec4 ) const;
	Vec3T<T> operator * ( const Vec3T<T> &vec3 ) const;
	Mat4T operator / ( T val ) const;
	void operator /= ( T val );
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( row )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Mat4.inl"

/** @} */

using Mat4 = Mat4T<f32>;
using Mat4d = Mat4T<f64>;

//******************************************************************************
//! End of File
//******************************************************************************