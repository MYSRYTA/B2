#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	f32AttributeT.h
//! @brief	 float�^�A�g���r���[�g
//******************************************************************************
/** @defgroup Geometry
*�@@{
*/
/**	@defgroup Attributes
*�@	@{
*/
/**	�@@brief	 float�^�A�g���r���[�g<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias \b : F32Attribute <br>
*			<T>��Attributes�̑��Ԑ���ۂ��߂̂��̂ŁAF32AttributeT���̂��̂̐��x�ɂ͊֌W�Ȃ�
*/

template<typename T>
class F32AttributeT : public AttributeT<T> {
	std::vector<T> values;
public:
	F32AttributeT () {}
	F32AttributeT ( const String &_name, u32 count = 0 );
	~F32AttributeT () {}

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
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			cereal::base_class<AttributeT<T>> ( this ),
			CEREAL_NVP ( this->values )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

#include "F32Attribute.inl"
/** @} */
/** @} */

using F32Attribute = F32AttributeT<f32>;

//******************************************************************************
//! End of File
//******************************************************************************