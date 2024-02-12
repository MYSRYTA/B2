#pragma once
#include "Attribute.h"
#include "BoolAttribute.h"
#include "S32Attribute.h"
#include "U32Attribute.h"
#include "F32Attribute.h"
#include "F64Attribute.h"
#include "L16ArrayU32Attribute.h"
#include "Vec2Attribute.h"
#include "Vec3Attribute.h"
#include "Vec4Attribute.h"
#include "SkinningAttribute.h"

// std::shared_ptr
#include <iostream>
#include <memory>

//******************************************************************************
//! @file	Attributes.h
//! @brief	 様々なタイプのアトリビュートを管理するコンテナ
//******************************************************************************
/** @defgroup Geometry
*　@{
*/
/**	@defgroup Attributes
*　	@{
*/
/**	　@brief アトリビュートコンテナ<br>
*			#include <burgess2_main/Geometry.h> <br><br>
*			#alias <br> \b : Attributes <br> \b : Attributesd <br>
*			@code
*				Attributes a;
*				a.getOrCreatVec2Attribute ( "vec2Attr" ); // create attr
*				a.getOrCreatVec3Attribute ( "vec3Attr" ); // create attr
*				Bool e = a.has ( "vec4Attr" ); // e = false
*	
*				Vec2Attribute & v2Attr = a.getOrCreatVec2Attribute ( "vec2Attr" ); // get attr ref
*				std::vector<Vec2> b ( 3 );
*				b [0] = Vec2(1.0f, 2.0f);
*				b [1] = Vec2(1.0f, 3.0f);
*				b [2] = Vec2(2.0f, 3.0f);
*				v2Attr.resetValues ( b );
*				Vec2 v2 = v2Attr.getValue ( 1 ); // v2 = Vec2(1.0f, 3.0f) );
*	
*				Vec4Attribute & v4Attr = a.getOrCreatVec4Attribute ( "vec4Attr", 3 ); // create attr
*				v4Attr [0] = Vec4 ( 1.0f, 2.0f, 4.0f, 5.0f);
*				v4Attr [1] = Vec4 ( 1.0f, 3.0f, 4.0f, 5.0f);
*				v4Attr [2] = Vec4 ( 2.0f, 3.0f, 4.0f, 5.0f);
*				Vec4 v4 = v4Attr.getValue ( 1 ) ; // v4 = Vec4 ( 1.0f, 3.0f, 4.0f, 5.0f )
*			@endcode
*/

template<typename T>
class AttributesT {
	std::vector<std::shared_ptr<AttributeT<T>>> attrT;
	std::vector<Bool> valid;
	Bool has ( const String &_name, OUT u8 &id )const;
public:
	AttributesT () {}
	virtual ~AttributesT () {}

	u8 attrCount ()const;
	String getName ( u8 id )const;
	Bool has ( const String &_name )const;
	u8 getType( const String &_name )const;
	u8 getType ( const u8 id )const;
	void deleteAttr ( const String &_name );
	void clear ( const String &_name );

	std::vector<String> getAllAttributeNames ()const;
	String toString ( const String title = "" )const;

	BoolAttributeT<T>& getOrCreateBoolAttribute ( const String &_name, u32 count = 0 );
	F32AttributeT<T>& getOrCreateF32Attribute ( const String &_name, u32 count = 0 );
	F64AttributeT<T>& getOrCreateF64Attribute ( const String &_name, u32 count = 0 );
	S32AttributeT<T>& getOrCreateS32Attribute ( const String&_name, u32 count = 0 );
	U32AttributeT<T>& getOrCreateU32Attribute (const String &_name, u32 count = 0 );
	L16ArrayU32AttributeT<T>& getOrCreateL16ArrayU32Attribute ( const String &_name, u32 count = 0 );
	Vec2AttributeT<T>& getOrCreatVec2Attribute ( const String &_name, u32 count = 0 );
	Vec3AttributeT<T>& getOrCreatVec3Attribute ( const String &_name, u32 count = 0 );
	Vec4AttributeT<T>& getOrCreatVec4Attribute ( const String &_name, u32 count = 0 );
	SkinningAttributeT<T>& getOrCreateSkinningAttribute ( const String &_name, u32 count = 0 );

	BoolAttributeT<T>& getBoolAttribute ( const String &_name )const ;
	F32AttributeT<T>& getF32Attribute (const String &_name )const;
	F64AttributeT<T>& getF64Attribute (const String &_name )const ;
	S32AttributeT<T>& getS32Attribute (const String &_name )const ;
	U32AttributeT<T>& getU32Attribute (const String &_name )const ;
	L16ArrayU32AttributeT<T>& getL16ArrayU32Attribute ( const String &_name ) const;
	Vec2AttributeT<T>& getVec2Attribute ( const String &_name )const ;
	Vec3AttributeT<T>& getVec3Attribute ( const String &_name )const ;
	Vec4AttributeT<T>& getVec4Attribute ( const String &_name )const ;
	SkinningAttributeT<T>& getSkinningAttribute ( const String &_name )const;

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void save ( cerealArchive &archive )const{
		std::vector<BoolAttributeT<T>> boolAttr;
		std::vector<F32AttributeT<T>> f32Attr;
		std::vector<F64AttributeT<T>> f64Attr;
		std::vector<S32AttributeT<T>> s32Attr;
		std::vector<U32AttributeT<T>> u32Attr;
		std::vector<L16ArrayU32AttributeT<T>> l16Attr;
		std::vector<Vec2AttributeT<T>> vec2Attr;
		std::vector<Vec3AttributeT<T>> vec3Attr;
		std::vector<Vec4AttributeT<T>> vec4Attr;
		std::vector<SkinningAttributeT<T>> skinAttr;

		for ( u32 i = 0; i < this->attrCount (); ++i ){
			String n = this->getName ( i );
			u8 type = this->getType ( n );
			switch ( type ){
				case  0:	boolAttr.push_back( this->getBoolAttribute ( n )); break;
				//case 1:	att.push_back( this->getF32Attribute ( n )); break;
				//case 2:	att.push_back( this->getF32Attribute ( n )); break;
				case  3:	s32Attr.push_back( this->getS32Attribute ( n )); break;
				//case 4:	att.push_back( this->getF32Attribute ( n )); break;
				//case 5:	att.push_back( this->getF32Attribute ( n )); break;
				//case 6:	att.push_back( this->getF32Attribute ( n )); break;
				case  7:	u32Attr.push_back( this->getU32Attribute ( n )); break;
				//case 8:	att.push_back( this->getF32Attribute ( n )); break;
				case  9:	f32Attr.push_back( this->getF32Attribute ( n )); break;
				case 10:	f64Attr.push_back( this->getF64Attribute ( n )); break;
				//case 11:	att.push_back( this->getF32Attribute ( n )); break;
				case 12:	vec2Attr.push_back( this->getVec2Attribute ( n )); break;
				case 13:	vec3Attr.push_back( this->getVec3Attribute ( n )); break;
				case 14:	vec4Attr.push_back( this->getVec4Attribute ( n )); break;
				case 15:	l16Attr.push_back( this->getL16ArrayU32Attribute ( n )); break;
				case 16:	skinAttr.push_back( this->getSkinningAttribute ( n )); break;
			}
		}

		archive (
			CEREAL_NVP ( boolAttr ),
			CEREAL_NVP ( f32Attr ),
			CEREAL_NVP ( f64Attr ),
			CEREAL_NVP ( s32Attr ),
			CEREAL_NVP ( u32Attr ),
			CEREAL_NVP ( l16Attr ),
			CEREAL_NVP ( vec2Attr ),
			CEREAL_NVP ( vec3Attr ),
			CEREAL_NVP ( vec4Attr ),
			CEREAL_NVP ( skinAttr ),
			CEREAL_NVP ( valid )
		);
	}

	template<class cerealArchive>
	void load ( cerealArchive &archive ){
		std::vector<BoolAttributeT<T>> boolAttr;
		std::vector<F32AttributeT<T>> f32Attr;
		std::vector<F64AttributeT<T>> f64Attr;
		std::vector<S32AttributeT<T>> s32Attr;
		std::vector<U32AttributeT<T>> u32Attr;
		std::vector<L16ArrayU32AttributeT<T>> l16Attr;
		std::vector<Vec2AttributeT<T>> vec2Attr;
		std::vector<Vec3AttributeT<T>> vec3Attr;
		std::vector<Vec4AttributeT<T>> vec4Attr;
		std::vector<SkinningAttributeT<T>> skinAttr;		

		archive (
			CEREAL_NVP ( boolAttr ),
			CEREAL_NVP ( f32Attr ),
			CEREAL_NVP ( f64Attr ),
			CEREAL_NVP ( s32Attr ),
			CEREAL_NVP ( u32Attr ),
			CEREAL_NVP ( l16Attr ),
			CEREAL_NVP ( vec2Attr ),
			CEREAL_NVP ( vec3Attr ),
			CEREAL_NVP ( vec4Attr ),
			CEREAL_NVP ( skinAttr ),
			CEREAL_NVP ( valid )
		);

		*this = AttributesT<T> ();
		for ( u32 i = 0; i < boolAttr.size (); ++i ){
			BoolAttributeT<T> &attr = this->getOrCreateBoolAttribute ( boolAttr[i].getName (), boolAttr[i].size () );
			attr.resetValues ( boolAttr[i].getValues () );
		}
		for ( u32 i = 0; i < f32Attr.size (); ++i ){
			F32AttributeT<T> &attr = this->getOrCreateF32Attribute ( f32Attr[i].getName (), f32Attr[i].size () );
			attr.resetValues ( f32Attr[i].getValues () );
		}
		for ( u32 i = 0; i < f64Attr.size (); ++i ){
			F64AttributeT<T> &attr = this->getOrCreateF64Attribute ( f64Attr[i].getName (), f64Attr[i].size () );
			attr.resetValues ( f64Attr[i].getValues () );
		}
		for ( u32 i = 0; i < s32Attr.size (); ++i ){
			S32AttributeT<T> &attr = this->getOrCreateS32Attribute ( s32Attr[i].getName (), s32Attr[i].size () );
			attr.resetValues ( s32Attr[i].getValues () );
		}
		for ( u32 i = 0; i < u32Attr.size (); ++i ){
			U32AttributeT<T> &attr = this->getOrCreateU32Attribute ( u32Attr[i].getName (), u32Attr[i].size () );
			attr.resetValues ( u32Attr[i].getValues () );
		}
		for ( u32 i = 0; i < l16Attr.size (); ++i ){
			L16ArrayU32AttributeT<T> &attr = this->getOrCreateL16ArrayU32Attribute ( l16Attr[i].getName (), l16Attr[i].size () );
			attr.resetValues ( l16Attr[i].getValues () );
		}
		for ( u32 i = 0; i < vec2Attr.size (); ++i ){
			Vec2AttributeT<T> &attr = this->getOrCreatVec2Attribute ( vec2Attr[i].getName (), vec2Attr[i].size () );
			attr.resetValues ( vec2Attr[i].getValues () );
		}
		for ( u32 i = 0; i < vec3Attr.size (); ++i ){
			Vec3AttributeT<T> &attr = this->getOrCreatVec3Attribute ( vec3Attr[i].getName (), vec3Attr[i].size () );
			attr.resetValues ( vec3Attr[i].getValues () );
		}
		for ( u32 i = 0; i < vec4Attr.size (); ++i ){
			Vec4AttributeT<T> &attr = this->getOrCreatVec4Attribute ( vec4Attr[i].getName (), vec4Attr[i].size () );
			attr.resetValues ( vec4Attr[i].getValues () );
		}
		for ( u32 i = 0; i < skinAttr.size (); ++i ){
			SkinningAttributeT<T> &attr = this->getOrCreateSkinningAttribute ( skinAttr[i].getName (), skinAttr[i].size () );
			attr.resetValues ( skinAttr[i].getValues () );
		}
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Attributes.inl"
/** @} */
/** @} */

using Attributes = AttributesT<f32>;
using Attributesd = AttributesT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************