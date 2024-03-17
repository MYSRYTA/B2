#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	Vec3AttributeT.h
//! @brief	 Vec3型アトリビュート
//******************************************************************************
/** @defgroup Geometry
*　@{
*/
/**	@defgroup Attributes
*　	@{
*/
/**	　@brief Vec3型アトリビュート<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias <br> \b : Vec3Attribute = Vec3AttributeT<f32> <br> \b :Vec3Attributed = Vec3AttributeT<f64> <br>
*/

template<typename T>
class Vec3AttributeT : public AttributeT<T> {
	
public:
	std::vector<Vec3T<T>> values;
	Vec3AttributeT () {}
	Vec3AttributeT ( const String &_name, u32 count = 0 );
	~Vec3AttributeT () {}

	inline u32 size () const;
	inline void resize ( u32 count );

	inline void resetValues ( const std::vector<Vec3T<T>> &_values );
	inline void setValues ( const std::vector<Vec3T<T>> &_values );
	inline void setValue ( u32 index, const Vec3T<T> &value );

	inline std::vector<Vec3T<T>>& getValuesRef ();
	inline std::vector<Vec3T<T>> getValues () const;
	inline Vec3T<T> getValue ( u32 index ) const;

	inline void clear ();

	/*! @name Operator*/
	/* @{ */
	inline Vec3T<T>& operator[]( u32 index );
	inline const Vec3T<T>& operator[]( u32 index ) const;
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

#include "Vec3Attribute.inl"
/** @} */
/** @} */

using Vec3Attribute = Vec3AttributeT<f32>;
using Vec3Attributed = Vec3AttributeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************