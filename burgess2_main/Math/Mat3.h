#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;
template<typename T> class Vec3T;
template<typename T> class QuatT;
template<typename T> class Rad3T;
template<typename T> class Deg3T;

//******************************************************************************
//! @file	Mat3.h
//! @brief	 3x3 Matrix [3x3マトリックス]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	 3x3 Matrix [3x3のRowメジャーマトリックス] <br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Mat3 = Mat3T<float>　<br> \b : Mat3d = Mat3T<double><br>
*			@code
*				Mat3 aa;
*				aa.setDiagonal ( Vec3 ( 2, 4, 8 ) );
*				Mat3 cc ( 2, 0, 0, 0, 4, 0, 0, 0, 8 );
*				Bool e = aa.almostEqual( cc );
*			@endcode
*			@date	2019.2 : オリジナル<br>
*					2021.8 : Eigen::Matrixをラッピング<br>
*/
//  3x3 Matrix [3x3マトリックス]
template<typename T>
class Mat3T : public Eigen::Matrix<T, 3, 3>{
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	Mat3T();
	Mat3T(	T _00, T _01, T _02,
			T _10, T _11, T _12,
			T _20, T _21, T _22 );
	Mat3T( const Vec3T<T>& row0, const Vec3T<T>& row1, const Vec3T<T>& row2 );
	Mat3T( const VecT<T>& data, Bool argDataIsColumnMajor = false );
	Mat3T( std::vector<T>& data, Bool argDataIsColumnMajor = false );
	Mat3T( const QuatT<T>& quat );
	Mat3T( const Rad3T<T>& rad );
	Mat3T( const Deg3T<T>& deg );
	Mat3T( const Eigen::Matrix<T, 3, 3>& m );
	~Mat3T(){}
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline VecT<T> toVec( Bool toColumnMajorVal = false ) const;
	inline std::vector<T> toTypeArray( Bool toColumnMajorVal = false )const;
	inline String toString( const String title = "" )const;
	 
	inline Bool equal( const Mat3T<T>& other ) const;
	inline Bool almostEqual( const Mat3T<T>& other, T tolerance = BURGESS2::TOLERANCE ) const;
	inline Bool isZero()const;
	inline Bool almostZero( T tolerance = BURGESS2::TOLERANCE )const;

	inline void setIdentity();
	inline void setValue( u8 row, u8 col, T value );
	inline void setAllValue( T val );
	inline void setRowValues( const Vec3T<T>& row0, const Vec3T<T>& row1, const Vec3T<T>& row2 );
	inline void setColValues( const Vec3T<T>& col0, const Vec3T<T>& col1, const Vec3T<T>& col2 );
	inline void setAxis( const Vec3T<T> & xAxis, const Vec3T<T> & yAxis, const Vec3T<T> & zAxis );
	inline void setDiagonal( T val );
	inline void setDiagonal( const Vec3T<T>& val );
	inline void setFromDirectionAndUpvector( const Vec3T<T>& direction, const Vec3T<T>& upvector, u8 type = 0, Bool dirNeg = false, Bool upVectorNeg = false );
	inline void setScaling( const Vec3T<T>& val );

	inline T get( u8 row, u8 col )const;
	inline Vec3T<T> getRowValue( u8 rowIndex ) const;
	inline Vec3T<T> getColValue( u8 colIndex ) const;
	inline Vec3T<T> getXaxis()const;
	inline Vec3T<T> getYaxis()const;
	inline Vec3T<T> getZaxis()const;
	inline Mat3T<T> getRotationMat3()const;
	inline Vec3T<T> getScaling()const;

	inline Mat3T transpose() const;
	inline void setTranspose();
	inline Mat3T inverse() const;
	inline void setInverse();
	inline void axisChange( u8 zyTo, Bool dirNeg, Bool upvNeg );

	inline void swapRows( u8 row1, u8 row2 );
	inline void swapCols( u8 col1, u8 col2 );

	inline T determinant() const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline Mat3T<T>& operator = ( const Eigen::Matrix<T, 3, 3>& other );

	inline T& operator()( u8 row, u8 col );
	inline const T& operator()( u8 row, u8 col ) const;

	inline Bool operator == ( const Mat3T<T>& other ) const;
	inline Bool operator != ( const Mat3T<T>& other ) const;

	inline Mat3T<T> operator + ( const Mat3T<T>& other ) const;
	inline void operator += ( const Mat3T<T>& other );
	inline Mat3T<T> operator - ( const Mat3T<T>& other ) const;
	inline void operator -= ( const Mat3T<T>& other );

	inline Mat3T<T> operator * ( const Mat3T<T>& other ) const;
	inline void operator *= ( const Mat3T<T>& other );
	inline Mat3T<T> operator * ( T val ) const;
	inline void operator *= ( T val );
	inline Vec3T<T> operator * ( const Vec3T<T>& vec3 ) const;
	inline Mat3T<T> operator / ( T val ) const;
	inline void operator /= ( T val );
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void save( cerealArchive& archive )const{
		// 旧Mathに合わせるために、保存対象のデータを一旦変数に格納する
		Vec3T<T> row[3];
		row[0] = this->getRowValue( 0 );
		row[1] = this->getRowValue( 1 );
		row[2] = this->getRowValue( 2 );
		archive(
			CEREAL_NVP( row )
		);
	}

	template<class cerealArchive>
	void load( cerealArchive& archive ){
		Vec3T<T> row[3];
		archive(
			CEREAL_NVP( row )
		);
		*this = Mat3T<T>( row[0], row[1], row[2] );
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