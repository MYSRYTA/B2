#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	L16ArrayU32Attribute.h
//! @brief	 L16ArrayT<u32>�^�A�g���r���[�g
//******************************************************************************
/** @defgroup Geometry
*�@@{
*/
/**	@defgroup Attributes
*�@	@{
*/
/**	�@@brief L16ArrayT<u32>�^�A�g���r���[�g<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias : \b L16ArrayU32Attribute <br>
*			<T>��Attributes�̑��Ԑ���ۂ��߂̂��̂ŁAL16ArrayU32AttributeT���̂��̂̐��x�ɂ͊֌W�Ȃ�
*/

template<typename T>
class L16ArrayU32AttributeT : public AttributeT<T> {
	std::vector<L16Array<u32>> values;
public:
	L16ArrayU32AttributeT () {}
	L16ArrayU32AttributeT ( const String &_name, u32 count = 0 );
	~L16ArrayU32AttributeT () {}

	u32 size () const;
	void resize ( u32 count );

	void resetValues ( const std::vector<L16Array<u32>> &_values );
	void setValues ( const std::vector<L16Array<u32>> &_values );
	void setValue ( u32 index, const L16Array<u32> &value );

	std::vector<L16Array<u32>>& getValuesRef ();
	std::vector<L16Array<u32>> getValues () const;
	L16Array<u32> getValue ( u32 index ) const;

	void clear ();

	/*! @name Operator*/
	/* @{ */
	L16Array<u32>& operator[]( u32 index );
	const L16Array<u32>& operator[]( u32 index ) const;
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

#include "L16ArrayU32Attribute.inl"
/** @} */
/** @} */

using L16ArrayU32Attribute = L16ArrayU32AttributeT<f32>;

//******************************************************************************
//! End of File
//******************************************************************************