#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	BoolAttribute.h
//! @brief	 Bool型アトリビュート
//******************************************************************************
/** @defgroup Geometry
*　@{
*/
/**	@defgroup Attributes
*　	@{
*/
/**	　@brief	 Bool型アトリビュート<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias \b : BoolAttribute <br>
*			<T>はAttributesの多態性を保つためのもので、BoolAttributeTそのものの精度には関係ない
*/

template<typename T>
class BoolAttributeT : public AttributeT<T> {
	std::vector<Bool> values;
public:
	BoolAttributeT () {}
	BoolAttributeT ( const String &_name, u32 count = 0 );
	~BoolAttributeT () {}

	u32 size () const;
	void resize ( u32 count );

	void resetValues ( const std::vector<Bool> &_values );
	void setValues ( const std::vector<Bool> &_values );
	void setValue ( u32 index, Bool value );

	std::vector<Bool>& getValuesRef ();
	std::vector<Bool> getValues () const;
	Bool getValue ( u32 index ) const;

	void clear ();

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

#include "BoolAttribute.inl"
/** @} */
/** @} */

using BoolAttribute = BoolAttributeT<f32>;

//******************************************************************************
//! End of File
//******************************************************************************