#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	Vec2AttributeT.h
//! @brief	 Vec2型アトリビュート
//******************************************************************************
/** @defgroup Geometry
*　@{
*/
/**	@defgroup Attributes
*　	@{
*/
/**	　@brief Vec2型アトリビュート<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias <br> \b : Vec2Attribute = Vec2AttributeT<f32><br> \b : Vec2Attributed = Vec2AttributeT<f64><br>
*/

template<typename T>
class Vec2AttributeT : public AttributeT<T> {
	std::vector<Vec2T<T>> values;
public:
	Vec2AttributeT () {}
	Vec2AttributeT ( const String &_name, u32 count = 0 );
	~Vec2AttributeT () {}

	u32 size () const;
	void resize ( u32 count );

	void resetValues ( const std::vector<Vec2T<T>> &_values );
	void setValues ( const std::vector<Vec2T<T>> &_values );
	void setValue ( u32 index, const Vec2T<T> &value );

	std::vector<Vec2T<T>>& getValuesRef ();
	std::vector<Vec2T<T>> getValues () const;
	Vec2T<T> getValue ( u32 index ) const;

	void clear ();

	/*! @name Operator*/
	/* @{ */
	Vec2T<T>& operator[]( u32 index );
	const Vec2T<T>& operator[]( u32 index ) const;
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

#include "Vec2Attribute.inl"
/** @} */
/** @} */

using Vec2Attribute = Vec2AttributeT<f32>;
using Vec2Attributed = Vec2AttributeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************