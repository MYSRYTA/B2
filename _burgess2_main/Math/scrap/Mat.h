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
*			alias : \b Mat = MatT<float>// \b Matd = MatT<double> <br>
*			#include <burgess2_main/Math.h> <br>
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
*/
// NxM Matrix [NxMマトリックス]
template<typename T>
class MatT {
	u32 rows;
	u32 cols;
	std::vector<T> values;

public:
	MatT ();
	MatT ( u32 n );
	MatT ( u32 rowSize, u32 colSize );
	MatT ( const std::vector<VecT<T>> &vecArray );
	MatT ( const std::vector<Vec3T<T>> &vecArray );
	MatT ( const std::vector<Vec4T<T>> &vecArray );
	MatT ( const MatT &other );
	virtual ~MatT () {}

	void initialize ( u32 rowSize, u32 colSize );

	void setIdentity ( u32 size );
	void setAllValues ( T val );
	void setValue ( u32 rowIndex, u32 colIndex, T val );
	void setValue ( u32 index, T val );
	void setRowAllValues (u32 rowIndex, T val);
	void setRowValues ( u32 rowIndex, const VecT<T> &vec );
	void setColAllValues (u32 colIndex, T val);
	void setColValues (u32 colIndex, const VecT<T> &vec);
	void setDiagonal ( u32 resize, T val );
	void setDiagonal ( const VecT<T> &vec );

	void reset ( u32 rowSize, u32 colSize );
	
	u32 getRowCount () const;
	u32 getColCount () const;
	T get ( u32 index ) const;
	T get ( u32 rowIndex, u32 colIndex ) const;
	VecT<T> getRowValues ( u32 rowIndex ) const;
	VecT<T> getColValues ( u32 colIndex ) const;
	VecT<T> getRowsMinValues () const;
	VecT<T> getRowsMaxValues () const;
	VecT<T> getColsMinValue () const;
	VecT<T> getColsMaxValue () const;

	String toString ( const String title = "" )const;

	Bool equal ( const MatT &other ) const;
	Bool almostEqual ( const MatT &other, T tolerance = Burgess::TOLERANCE ) const;

	Bool isSquare () const;

	MatT lerp ( const MatT &other, T t ) const;
	MatT linearInterpolate ( const MatT &other, T t ) const;

	void swapRows ( u32 row1, u32 row2 );
	void swapCols ( u32 col1, u32 col2 );

	MatT transpose () const;
	MatT inverse ()const;

	/*! @name Operator*/
	/* @{ */
	Bool operator == ( const MatT &other ) const;
	Bool operator != ( const MatT &other ) const;

	MatT operator + ( const MatT &other ) const;
	void operator += ( const MatT &other );
	MatT operator - ( const MatT &other ) const;
	void operator -= ( const MatT &other );
	MatT operator * ( T val ) const;
	VecT<T> operator * ( VecT<T> other ) const;
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( rows ),
			CEREAL_NVP ( cols ),
			CEREAL_NVP ( values )
		);
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