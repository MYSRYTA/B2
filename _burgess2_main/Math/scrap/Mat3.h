#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;
template<typename T> class Vec3T;

//******************************************************************************
//! @file	Mat3.h
//! @brief	 3x3 Matrix [3x3マトリックス]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	 3x3 Matrix [3x3マトリックス] <br>
*			alias : \b Mat3 = Mat3T<float>// \b Mat3d = Mat3T<double><br>
*			#include <burgess2_main/Math.h> <br>
*			@code
*				Mat3 aa;
*				aa.setDiagonal ( Vec3 ( 2, 4, 8 ) );
*				Mat3 cc ( 2, 0, 0, 0, 4, 0, 0, 0, 8 );
*				Bool e = aa.almostEqual( cc );
*			@endcode
*/
//  3x3 Matrix [3x3マトリックス]
template<typename T>
class Mat3T {
public:
	Vec3T<T> row[3];

	Mat3T ();
	Mat3T ( T _00, T _01, T _02,	T _10, T _11, T _12, T _20, T _21, T _22 );
	Mat3T ( const Vec3T<T> &row0, const Vec3T<T> &row1, const Vec3T<T> &row2 );
	Mat3T ( const VecT<T> &vec );
	Mat3T ( const QuatT<T> &quat );
	Mat3T ( const Rad3T<T> &rad );
	Mat3T ( const Deg3T<T> &deg );
	Mat3T ( const std::vector<T> &arrayVal );
	virtual ~Mat3T () {}

	void setIdentity ();
	void setColValues ( const Vec3T<T> &col0, const Vec3T<T> &col1, const Vec3T<T> &col2 );
	void setDiagonal ( T val );
	void setDiagonal ( const Vec3T<T> &v );
	void setFrom3Axis ( const Vec3T<T> &xAxis, const Vec3T<T> &yAxis, const Vec3T<T> &zAxis );
	void setFromDirectionAndUpvector ( const Vec3T<T> &direction, const Vec3T<T> &upvector, u8 type = 0, Bool dirNeg = false, Bool upVectorNeg = false );
	void setXaxis ( const Vec3T<T> &axis );
	void setYaxis ( const Vec3T<T> &axis );
	void setZaxis ( const Vec3T<T> &axis );
	void setScaling(const Vec3T<T>& val);

	void axisChange (u8 zyTo, Bool dirNeg, Bool upvNeg );

	T get ( u8 rowIndex, u8 colIndex ) const;
	Vec3T<T> getRow ( u8 rowIndex ) const;
	Vec3T<T> getCol ( u8 colIndex ) const;
	Vec3T<T> getXaxis ()const;
	Vec3T<T> getYaxis ()const;
	Vec3T<T> getZaxis ()const;
	Mat3T<T> getRotationMat3()const;
	Vec3T<T> getScaling()const;

	VecT<T> toVec () const;
	std::vector<T> toTypeArray () const;
	String toString ( const String title = "" )const;

	Bool equal ( const Mat3T &other ) const;
	Bool almostEqual ( const Mat3T &other, T tolerance = Burgess::TOLERANCE ) const;
	Bool testOrthogonal () const;

	Mat3T inverse () const;
	void setInverse ();
	Mat3T transpose () const;
	void setTranspose ();

	T determinant () const;

	/*! @name Operator*/
	/* @{ */
	Vec3T<T>& operator[]( u8 id );
	const Vec3T<T>& operator[]( u8 id ) const;

	Bool operator == ( const Mat3T &other ) const;
	Bool operator != ( const Mat3T &other ) const;

	Mat3T operator + ( const Mat3T &other ) const;
	void operator += ( const Mat3T &other );
	Mat3T operator - ( const Mat3T &other ) const;
	void operator -= ( const Mat3T &other );

	Mat3T operator * ( const Mat3T &other ) const;
	void operator *= ( const Mat3T &other );
	Mat3T operator * ( T val ) const;
	void operator *= ( T val );
	Vec3T<T> operator * ( const Vec3T<T> &vec3 ) const;
	Mat3T operator / ( T val ) const;
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

#include "Mat3.inl"

/** @} */

using Mat3 = Mat3T<f32>;
using Mat3d = Mat3T<f64>;

//******************************************************************************
//! End of File
//******************************************************************************