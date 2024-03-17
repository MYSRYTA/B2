#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	f63AttributeT.h
//! @brief	 double型アトリビュート
//******************************************************************************
/** @defgroup Geometry
*　@{
*/
/**	@defgroup Attributes
*　	@{
*/
/**	　@brief double型アトリビュート<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias \b : F64Attribute <br>
*			<T>はAttributesの多態性を保つためのもので、F64AttributeTそのものの精度には関係ない
*/

template<typename T>
class F64AttributeT : public AttributeT<T> {
	std::vector<T> values;
public:
	F64AttributeT () {}
	F64AttributeT ( const String &_name, u32 count = 0 );
	~F64AttributeT () {}

	u32 size () const;
	void resize ( u32 count );

	void resetValues ( const std::vector<T> &_values );
	void setValues ( const std::vector<T> &_values );
	void setValue ( u32 index, T value );

	std::vector<T>& getValuesRef ();
	std::vector<T> getValues () const;
	T getValue ( u32 index ) const;

	void clear ();

	/*! @name Operator*/
	/* @{ */
	T& operator[]( u32 index );
	const T& operator[]( u32 index ) const;
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			cereal::base_class<AttributeT<T>> ( this ),
			CEREAL_NVP ( values )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "F64Attribute.inl"
/** @} */
/** @} */

using F64Attribute = F64AttributeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************