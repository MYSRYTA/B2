#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	Vec4AttributeT.h
//! @brief	 Vec4型アトリビュート
//******************************************************************************
/** @defgroup Geometry
*　@{
*/
/**	@defgroup Attributes
*　	@{
*/
/**	　@brief Vec4型アトリビュート<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias <br> \b : Vec4Attribute = Vec4AttributeT<f32> <br> \b : Vec4Attributed = Vec4AttributeT<f64> <br>
*/

template<typename T>
class Vec4AttributeT : public AttributeT<T> {
	std::vector<Vec4T<T>> values;
public:
	Vec4AttributeT () {}
	Vec4AttributeT ( const String &_name, u32 count = 0 );
	~Vec4AttributeT () {}

	u32 size () const;
	void resize ( u32 count );

	void resetValues ( const std::vector<Vec4T<T>> &_values );
	void setValues ( const std::vector<Vec4T<T>> &_values );
	void setValue ( u32 index, const Vec4T<T> &value );

	std::vector<Vec4T<T>>& getValuesRef ();
	std::vector<Vec4T<T>> getValues () const;
	Vec4T<T> getValue ( u32 index ) const;

	void clear ();

	/*! @name Operator*/
	/* @{ */
	Vec4T<T>& operator[]( u32 index );
	const Vec4T<T>& operator[]( u32 index ) const;
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

#include "Vec4Attribute.inl"
/** @} */
/** @} */

using Vec4Attribute = Vec4AttributeT<f32>;
using Vec4Attributed = Vec4AttributeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************