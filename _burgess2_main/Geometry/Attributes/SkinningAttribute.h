#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	SkinningAttributeT.h
//! @brief	 �X�L���A�g���r���[�g
//******************************************************************************
/** @defgroup Geometry
*�@@{
*/
/**	@defgroup Attributes
*�@	@{
*/
/**	�@@brief SkinningAttribute�p�N���X<br>
*		#include <burgess2_main/Geometry.h> <br><br>
*		#alias <br> \b : SkinningPairData = SkinningPairDataT<f32> <br> \b : SkinningPairDatad = SkinningPairDataT<f64> <br>
*		�������Findex = ���b�V�����_�ԍ�<br>
*		�������FpartsID = �W���C���g/�{�[�����X�g�̔z��ԍ�<br>
*		�������Fweight = �X�L���E�F�C�g<br>
*		�������Fnormalize = �X�L���E�F�C�g�̃m�[�}���C�Y������ON/OFF<br>
*		�������FmaxInfluence = �C���t���G���X�̍ő吔 (����͂S�ȉ��ōœK�����Ă���)<br>
*/
template<typename T>
class SkinningPairDataT{
	L4Array<u16> partsIDs; // joint id
	L4Array<T> weights; // weight
public:
	SkinningPairDataT (){ this->partsIDs.resize ( 0 ); this->weights.resize ( 0 ); }
	~SkinningPairDataT (){}
	Bool hasParts ( u16 partsID )const;

	u32 pairCount ()const;
	void addPair ( u16 partsID, T weight, Bool normalize = false );
	void setWeight ( u16 partsID, T weight, Bool normalize = false );
	void addWeight ( u16 partsID, T weight, Bool normalize = false );
	void subWeight ( u16 partsID, T weight, Bool normalize = false );
	void normalize ( u8 maxInfluence = u8 ( 4 ) );
	void normalizeAndCleanup ( u8 maxInfluence = u8 ( 4 ) );

	void getPairValue ( u32 pairID, OUT u16 &partsID, OUT T &weight )const;
	L4Array<u16> getPartsIDs ()const;
	L4Array<T> getWeights ()const;

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( partsIDs ),
			CEREAL_NVP ( weights )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};
using SkinningPairData = SkinningPairDataT<f32>;
using SkinningPairDatad = SkinningPairDataT<f64>;

/**	�@@brief	�X�L���A�g���r���[�g <br>
*		#include <burgess2_main/Geometry.h><br>
*		#alias : \b : SkinningAttribute = SkinningAttributeT<f32> <br> \b : SkinningAttributed = SkinningAttributeT<f64> <br>
*		v[0] = �X�L���o�^�����{�[��/�W���C���g�̔z��ԍ��̍ő�l�i=�X�L���A�g���r���[�g�ɑΉ�����W���C���g/�{�[���̍ő吔�j
*/
template<typename T>
class SkinningAttributeT : public AttributeT<T> {
	std::vector<SkinningPairDataT<T>> values;
public:
	SkinningAttributeT () { this->v[0] = 0; }
	SkinningAttributeT ( const String &_name, u32 count = 0 );
	virtual ~SkinningAttributeT () {}

	u32 size () const;
	void resize ( u32 count );

	void resetValues ( const std::vector<SkinningPairDataT<T>> &data );
	void setValues ( const std::vector<SkinningPairDataT<T>> &data );
	void setValue ( u32 index, const SkinningPairDataT<T> &data );
	void setOrCreatePair ( u32 index, u16 partsID, T weight, Bool normalize = false );
	void addPairWeight ( u32 index, u16 partsID, T weight, Bool normalize = false );

	const std::vector<SkinningPairDataT<T>>& getValuesRef () const;
	std::vector<SkinningPairDataT<T>> getValues () const;
	SkinningPairDataT<T> getValue ( u32 index ) const;

	void normalize ( u32 index, Bool cleanup = false, u8 maxInfluence = u8 ( 4 ) );
	void allNormalize ( Bool cleanup = false, u8 maxInfluence = u8 ( 4 ) );

	void clear ();

	u16 partsMaxID ()const;

	/*! @name Operator*/
	/* @{ */
	SkinningPairDataT<T>& operator[]( u32 index );
	const SkinningPairDataT<T>& operator[]( u32 index ) const;
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

#include "SkinningAttribute.inl"
/** @} */
/** @} */

using SkinningAttribute = SkinningAttributeT<f32>;
using SkinningAttributed = SkinningAttributeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************