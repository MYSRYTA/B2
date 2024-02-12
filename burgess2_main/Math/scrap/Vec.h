#pragma once
#include <burgess2_main/Core/base.h>

//******************************************************************************
//! @file	Vec.h
//! @brief	 ND Vector [NDベクトル]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	ND Vector [NDベクトル]<br>
*			alias : \b Vec = VecT<float>// \b Vecd = VecT<double><br>
*			#include <burgess2_main/Math.h> <br>
*			@code
*				Vec a ( 2 );
*				a.set ( 0, 1.1f );
*				a.set ( 1, 2.1f );
*
*				Vec b;
*				b.resize ( 2 );
*				b.setAll ( 1.1f );
*				b.insert ( 1, 2.1f );
*
*				a.combine ( bb );
*			@endcode
*/
// ND Vector [NDベクトル]
template<typename T>
class VecT {
public:
	std::vector<T> vals;

	VecT ();
	VecT ( u32 _size );
	VecT ( std::vector<T> _vals );
	VecT ( const VecT &source );
	VecT ( std::initializer_list<T> _vals ) : vals { _vals.begin (), _vals.end () } {}
	virtual ~VecT (){};

	void resize ( u32 newSize );

	void set ( u32 id, T val );
	void setAll ( T val );

	void insert ( u32 id, T val );
	void combine ( const VecT &other );
	void push_top ( T val );
	void push_back ( T val );

	T get ( u32 id ) const;
	std::vector<T> getValues () const;
	std::vector<T> toTypeArray () const;
	VecT toVec ()const;
	String toString ( const String title = "" )const;

	T remove ( u32 id );
	T pop_top ();
	T pop_back ();

	Bool equal ( const VecT &other ) const;
	Bool almostEqual ( const VecT &other, T TOLERANCE = Burgess::TOLERANCE ) const;

	T minimum ( OUT u32 &id ) const;
	T minimum () const;
	T maximum ( OUT u32 &id ) const;
	T maximum () const;

	VecT lerp ( const VecT &other, T t )const;
	VecT linearInterpolate ( const VecT &other, T t ) const;

	T lengthSquared () const;
	T length () const;

	VecT normal () const;
	void setNormal ();
	VecT unit () const;
	void setUnit ();

	T dot (const VecT &other ) const;

	u32 size () const;

	/*! @name Operator*/
	/* @{ */
	T& operator[]( u32 id );
	const T& operator[]( u32 id ) const;

	Bool operator == ( const VecT &other ) const;
	Bool operator != ( const VecT &other ) const;

	VecT operator + ( const VecT &other ) const;
	void operator += ( const VecT &other );
	VecT operator - ( const VecT &other ) const;
	void operator -= ( const VecT &other );
	
	VecT operator * ( T val ) const;
	VecT operator * ( const VecT &other ) const;
	void operator *= ( T val );
	void operator *= ( const VecT &other );
	VecT operator / ( T val ) const;
	VecT operator / ( const VecT &other ) const;
	void operator /= ( T val );
	void operator /= ( const VecT &other );
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( vals )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Vec.inl"
/** @} */

using Vec = VecT<f32>;
using Vecd = VecT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************