#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	BoolAttribute.h
//! @brief	 Bool�^�A�g���r���[�g
//******************************************************************************
/** @defgroup Geometry
*�@@{
*/
/**	@defgroup Attributes
*�@	@{
*/
/**	�@@brief	 Bool�^�A�g���r���[�g<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias \b : BoolAttribute <br>
*			<T>��Attributes�̑��Ԑ���ۂ��߂̂��̂ŁABoolAttributeT���̂��̂̐��x�ɂ͊֌W�Ȃ�
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

#include "BoolAttribute.inl"
/** @} */
/** @} */

using BoolAttribute = BoolAttributeT<f32>;

//******************************************************************************
//! End of File
//******************************************************************************