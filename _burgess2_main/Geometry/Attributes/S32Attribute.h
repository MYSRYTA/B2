#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	S32AttributeT.h
//! @brief	 int�^�A�g���r���[�g
//******************************************************************************
/** @defgroup Geometry
*�@@{
*/
/**	@defgroup Attributes
*�@	@{
*/
/**	�@@brief int�^�A�g���r���[�g<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias : \b S32Attribute <br>
*			<T>��Attributes�̑��Ԑ���ۂ��߂̂��̂ŁAS32AttributeT���̂��̂̐��x�ɂ͊֌W�Ȃ�
*/

template<typename T>
class S32AttributeT : public AttributeT<T> {
	std::vector<s32> values;
public:
	S32AttributeT () {}
	S32AttributeT ( const String &_name, u32 count = 0 );
	~S32AttributeT () {}

	u32 size () const;
	void resize ( u32 count );

	void resetValues ( const std::vector<s32> &_values );
	void setValues ( const std::vector<s32> &_values );
	void setValue ( u32 index, s32 value );

	std::vector<s32>& getValuesRef ();
	std::vector<s32> getValues () const;
	s32 getValue ( u32 index ) const;

	void clear ();

	/*! @name Operator*/
	/* @{ */
	s32& operator[]( u32 index );
	const s32& operator[]( u32 index ) const;
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			cereal::base_class<AttributeT<T>> ( this ),
			CEREAL_NVP ( values )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

#include "S32Attribute.inl"
/** @} */
/** @} */

using S32Attribute = S32AttributeT<f32>;

//******************************************************************************
//! End of File
//******************************************************************************