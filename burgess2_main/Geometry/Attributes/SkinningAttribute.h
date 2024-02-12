#pragma once
#include "Attribute.h"

//******************************************************************************
//! @file	SkinningAttributeT.h
//! @brief	 スキンアトリビュート
//******************************************************************************
/** @defgroup Geometry
*　@{
*/
/**	@defgroup Attributes
*　	@{
*/
/**	　@brief SkinningAttribute用クラス<br>
*		#include <burgess2_main/Geometry.h> <br><br>
*		#alias <br> \b : SkinningPairData = SkinningPairDataT<f32> <br> \b : SkinningPairDatad = SkinningPairDataT<f64> <br>
*		引数名：index = メッシュ頂点番号<br>
*		引数名：partsID = ジョイント/ボーンリストの配列番号<br>
*		引数名：weight = スキンウェイト<br>
*		引数名：normalize = スキンウェイトのノーマライズ処理のON/OFF<br>
*		引数名：maxInfluence = インフルエンスの最大数 (現状は４個以下で最適化している)<br>
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
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( partsIDs ),
			CEREAL_NVP ( weights )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};
using SkinningPairData = SkinningPairDataT<f32>;
using SkinningPairDatad = SkinningPairDataT<f64>;

/**	　@brief	スキンアトリビュート <br>
*		#include <burgess2_main/Geometry.h><br>
*		#alias : \b : SkinningAttribute = SkinningAttributeT<f32> <br> \b : SkinningAttributed = SkinningAttributeT<f64> <br>
*		v[0] = スキン登録したボーン/ジョイントの配列番号の最大値（=スキンアトリビュートに対応するジョイント/ボーンの最大数）
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

#include "SkinningAttribute.inl"
/** @} */
/** @} */

using SkinningAttribute = SkinningAttributeT<f32>;
using SkinningAttributed = SkinningAttributeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************