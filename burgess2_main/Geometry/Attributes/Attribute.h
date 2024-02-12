#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Math.h>
#include <burgess2_main/Containers/LArray.h>

//******************************************************************************
//! @file	Attribute.h
//! @brief	 アトリビュート基底クラス
//******************************************************************************
/** @defgroup Geometry
*　@{
*/
/**	@defgroup Attributes
*　	@{
*/
/**			@defgroup Macro
*				@{
*/
namespace Burgess2AttrType {
	const u8 BoolAttr = u8 ( 0 );
	//const u8 S8Attr = u8 ( 1 );
	//const u8 S16Attr = u8 ( 2 );
	const u8 S32Attr = u8 ( 3 );
	//const u8 S64Attr = u8 ( 4 );
	//const u8 U8Attr = u8 ( 5 );
	//const u8 U16Attr = u8 ( 6 );
	const u8 U32Attr = u8 ( 7 );
	//const u8 U64Attr = u8 ( 8 );
	const u8 F32Attr = u8 ( 9 );
	const u8 F64Attr = u8 ( 10 );
	//const u8 VecAttr = u8 ( 11 );
	const u8 Vec2Attr = u8 ( 12 );
	const u8 Vec3Attr = u8 ( 13 );
	const u8 Vec4Attr = u8 ( 14 );
	const u8 L16U32Attr = u8 ( 15 );
	const u8 SkinningAttr = u8 ( 16 );
}
/** @} */

/**		@brief	アトリビュート基底クラス<br>
*			#include <burgess2_main/Geometry.h><br>
*/

template<typename T>
class AttributeT {
	String name = "";
	String subName = "";
	Bool enable = false;
	Bool dirty = true;
	u8 type;
public:
	u16 v [5];

	AttributeT () {}
	virtual ~AttributeT () {}

	void setName ( const String &_name );
	void setSubName ( const String &_name );
	void setType ( u8 _type );
	void setValid ( Bool _enable );
	void setDirty ( Bool dirt );

	String getName () const;
	String getSubName () const;
	u8 getType ()const;
	Bool isValid ()const;
	Bool isDirty ()const;
	Bool is ( const String &_name )const;
	Bool is ( const String &_name, const String &_subName )const;

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( name ),
			CEREAL_NVP ( subName ),
			CEREAL_NVP ( enable ),
			CEREAL_NVP ( dirty ),
			CEREAL_NVP ( type ),
			CEREAL_NVP ( v )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Attribute.inl"
	/** @} */
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************