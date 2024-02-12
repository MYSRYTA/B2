#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	U32AttributeT.h
//! @brief	 unsined int型アトリビュート
//******************************************************************************
/** @defgroup Geometry
*　@{
*/
/**	@defgroup Attributes
*　	@{
*/
/**	　@brief unsined int型アトリビュート<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias : \b U32Attribute <br>
*			<T>はAttributesの多態性を保つためのもので、U32AttributeTそのものの精度には関係ない
*/

template<typename T>
class U32AttributeT : public AttributeT<T> {
	std::vector<u32> values;
public:
	U32AttributeT () {}
	U32AttributeT ( const String &_name, u32 count = 0 );
	~U32AttributeT () {}

	inline u32 size () const;
	inline void resize ( u32 count );

	inline void resetValues ( const std::vector<u32> &_values );
	inline void setValues ( const std::vector<u32> &_values );
	inline void setValue ( u32 index, u32 value );

	inline std::vector<u32>& getValuesRef ();
	inline std::vector<u32> getValues () const;
	inline u32 getValue ( u32 index ) const;

	inline void clear ();

	/*! @name Operator*/
	/* @{ */
	inline u32& operator[]( u32 index );
	inline const u32& operator[]( u32 index ) const;
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

#include "U32Attribute.inl"
/** @} */
/** @} */

using U32Attribute = U32AttributeT<f32>;

//******************************************************************************
//! End of File
//******************************************************************************