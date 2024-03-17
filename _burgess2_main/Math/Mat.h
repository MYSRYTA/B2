#pragma once
#include <burgess2_main/Core/base.h>
template<typename T> class VecT;
template<typename T> class Vec4T;
template<typename T> class LUDecompositionT;

//******************************************************************************
//! @file	Mat.h
//! @brief	 NxM Matrix [NxMマトリックス]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	NxM Matrix [NxMマトリックス] <br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Mat = MatT<float> <br> \b : Matd = MatT<double> <br>
*			@code
*				Mat a ( 2 );
*				a.setValue ( 0, 0, 5 );
*				a.setValue ( 0, 1, 50 );
*				a.setValue ( 1, 0, 6 );
*				a.setValue ( 1, 1, 60 );
*		
*				Vec vec ( 2 );
*				vec.set ( 0, 5 );
*				vec.set ( 1, 6 );
*				Vec ff = a.getRowsMinValues ();
*				Bool e = vec.almostEqual( ff );
*			@endcode
*			@date	2019.2 : オリジナル<br>
*					2021.8 : Eigen::Matrixをラッピング<br>
*/
// NxM Matrix [NxMマトリックス]
template<typename T>
class MatT : public Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>{
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	MatT();
	MatT( s32 n );
	MatT( s32 rowSize, s32 colSize );
	MatT( const std::vector<VecT<T>>& vecArray );
	MatT( const std::vector<Vec3T<T>>& vecArray );
	MatT( const std::vector<Vec4T<T>>& vecArray );
	MatT( const VecT<T>& data, const s32 rowSize, const s32 colSize, Bool argDataIsColumnMajor = false);
	MatT(  std::vector<T>& data, const s32 rowSize, const s32 colSize, Bool argDataIsColumnMajor = false );
	MatT( const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& other );
	~MatT(){}
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline s32 size()const;
	inline s32 rowSize()const;
	inline s32 colSize()const;

	inline VecT<T> toVec( Bool toColumnMajorVal = false ) const;
	inline std::vector<T> toTypeArray( Bool toColumnMajorVal = false )const;
	inline String toString( const String title = "" )const;

	inline Bool equal( const MatT& other ) const;
	inline Bool almostEqual( const MatT& other, T tolerance = BURGESS2::TOLERANCE ) const;
	inline Bool isSquare() const;

	inline void resize( s32 rowSize, s32 colSize );
	inline void reset( s32 rowSize, s32 colSize );
	inline void setZero();
	inline void setValue( s32 index, T val );
	inline void setValue( s32 row, s32 col, T val );
	inline void setAllValue( T val );
	inline void setRowAllValues( s32 rowIndex, T val );
	inline void setRowValues( s32 rowIndex, const VecT<T>& vec );
	inline void setColAllValues( s32 colIndex, T val );
	inline void setColValues( s32 colIndex, const VecT<T>& vec );

	inline T get( s32 row, s32 col )const;
	inline VecT<T> getRowValue( s32 rowIndex ) const;
	inline VecT<T> getColValue( s32 colIndex ) const;
	inline Vec2u getMinIndex() const;
	inline Vec2u getMaxIndex() const;

	inline MatT transpose() const;
	inline void setTranspose();
	inline MatT inverse() const;
	inline void setInverse();

	inline void swapRows( s32 row1, s32 row2 );
	inline void swapCols( s32 col1, s32 col2 );
	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline MatT& operator = ( const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& other );

	inline T& operator()( s32 row, s32 col );
	inline const T& operator()( s32 row, s32 col ) const;

	inline Bool operator == ( const MatT& other ) const;
	inline Bool operator != ( const MatT& other ) const;

	inline MatT operator + ( const MatT& other ) const;
	inline void operator += ( const MatT& other );
	inline MatT operator - ( const MatT& other ) const;
	inline void operator -= ( const MatT& other );

	inline VecT<T> operator * ( const VecT<T>& other ) const;
	inline MatT operator * ( T val ) const;
	inline void operator *= ( T val );
	inline MatT operator / ( T val ) const;
	inline void operator /= ( T val );
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void save( cerealArchive& archive )const{
		u32 rows = this->rowSize();
		u32 cols = this->colSize();
		std::vector<T> values = this->toTypeArray();

		archive(
			CEREAL_NVP( rows ),
			CEREAL_NVP( cols ),
			CEREAL_NVP( values )
		);
	}

	template<class cerealArchive>
	void load( cerealArchive& archive ){
		u32 rows = this->rowSize();
		u32 cols = this->colSize();
		std::vector<T> values;

		archive(
			CEREAL_NVP( rows ),
			CEREAL_NVP( cols ),
			CEREAL_NVP( values )
		);
		*this = MatT<T>( values, rows, cols );
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};
#include "Mat.inl"

/** @} */
using Mat = MatT<f32>;
using Matd = MatT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************