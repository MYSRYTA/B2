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
/**		@brief	4x4 Matrix [4x4のRowメジャーマトリックス] <br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Mat4 = Mat4T<float> <br> \b : Mat4d = Mat4T<double><br>
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
*			@date	2019.2 : オリジナル<br>
*					2021.8 : Eigen::Matrixをラッピング<br>
*/
// 4x4 Matrix [4x4マトリックス]
template<typename T>
class Mat4T : public Eigen::Matrix<T, 4, 4>{
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	Mat4T();
	Mat4T(
		T _00, T _01, T _02, T _03,
		T _10, T _11, T _12, T _13,
		T _20, T _21, T _22, T _23,
		T _30, T _31, T _32, T _33 );
	Mat4T( const Vec4T<T>& row0, const Vec4T<T>& row1, const Vec4T<T>& row2, const Vec4T<T>& row3 );
	Mat4T( const VecT<T>& data, Bool argDataIsColumnMajor = false );
	Mat4T( std::vector<T>& data, Bool argDataIsColumnMajor = false );
	Mat4T( const XfoT<T>& Xfo );
	Mat4T( const SRTT<T>& srt );
	Mat4T( const Eigen::Matrix<T, 4, 4>& m );
	~Mat4T(){};
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline VecT<T> toVec( Bool toColumnMajorVal = false ) const;
	inline std::vector<T> toTypeArray( Bool toColumnMajorVal = false )const;
	inline XfoT<T> toXfo()const;
	inline SRTT<T> toSRT( const BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ )const;
	inline String toString( const String title = "" )const;
	
	inline Bool equal( const Mat4T& other ) const;
	inline Bool almostEqual( const Mat4T& other, T tolerance = BURGESS2::TOLERANCE ) const;
	inline Bool isZero()const;
	inline Bool almostZero( T tolerance = BURGESS2::TOLERANCE )const;

	inline void setIdentity();
	inline void setValue( u8 row, u8 col, T value );
	inline void setAllValue( T val );
	inline void setRowValues( const Vec4T<T>& row0, const Vec4T<T>& row1, const Vec4T<T>& row2, const Vec4T<T>& row3 );
	inline void setColValues( const Vec4T<T>& col0, const Vec4T<T>& col1, const Vec4T<T>& col2, const Vec4T<T>& col3 );
	inline void setDiagonal( T val );
	inline void setDiagonal( const Vec4T<T>& val );
	inline void setUpperLeft( const Mat3T<T>& mat3 );
	inline void setRotationFromDirectionAndUpvector( const Vec3T<T>& direction, const Vec3T<T>& upvector, u8 axis );
	inline void setTranslation( const Vec3T<T>& val );
	inline void setScaling( const Vec3T<T>& val );
	
	inline T get( u8 row, u8 col )const;
	inline Vec4T<T> getRowValue( u8 rowIndex ) const;
	inline Vec4T<T> getColValue( u8 colIndex ) const;
	inline Mat3T<T> getUpperLeft() const;
	inline Mat3T<T> getMat3() const;
	inline Mat3T<T> getRotationMat3() const;
	inline Vec3T<T> getTranslation() const;
	inline QuatT<T> getRotation() const;
	inline Vec3T<T> getScaling() const;

	inline void axisChange( u8 type, Bool ZNeg, Bool YNeg, Bool keepScaleAxis = false );
	inline void axisChangeXZ( u8 type, Bool XNeg, Bool ZNeg, Bool keepScaleAxis = false );
	
	inline Mat4T transpose() const;
	inline void setTranspose();
	inline Mat4T inverse() const;
	inline void setInverse();
	inline Mat4T conjugate() const;
	inline void setConjugate();

	inline void swapRows( u8 row1, u8 row2 );
	inline void swapCols( u8 col1, u8 col2 );

	inline T determinant() const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline Mat4T& operator = ( const Eigen::Matrix<T, 4, 4>& other );

	inline T& operator()( u8 row, u8 col );
	inline const T& operator()( u8 row, u8 col ) const;

	inline Bool operator == ( const Mat4T& other ) const;
	inline Bool operator != ( const Mat4T& other ) const;

	inline Mat4T operator + ( const Mat4T& other ) const;
	inline void operator += ( const Mat4T& other );
	inline Mat4T operator - ( const Mat4T& other ) const;
	inline void operator -= ( const Mat4T& other );

	inline Mat4T operator * ( const Mat4T& other ) const;
	inline void operator *= ( const Mat4T& other );
	inline Mat4T operator * ( T val ) const;
	inline void operator *= ( T val );
	inline Vec4T<T> operator * ( const Vec4T<T>& vec4 ) const;
	inline Vec3T<T> operator * ( const Vec3T<T>& vec3 ) const;
	inline Mat4T operator / ( T val ) const;
	inline void operator /= ( T val );
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/	
	template<class cerealArchive>
	void save( cerealArchive& archive )const{
		// 旧Mathに合わせるために、保存対象のデータを一旦変数に格納する
		Vec4T<T> row[4];
		row[0] = this->getRowValue( 0 );
		row[1] = this->getRowValue( 1 );
		row[2] = this->getRowValue( 2 );
		row[3] = this->getRowValue( 3 );
		archive(
			CEREAL_NVP( row )
		);
	}

	template<class cerealArchive>
	void load( cerealArchive& archive ){
		Vec4T<T> row[4];
		archive(
			CEREAL_NVP( row )
		);
		*this = Mat4T<T>( row[0], row[1], row[2], row[3] );
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