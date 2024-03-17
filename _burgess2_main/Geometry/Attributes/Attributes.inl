//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュートの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 AttributesT<T>::attrCount () const {
	u8 result = 0;
	for ( u32 i = 0; i<u32(this->valid.size ()); i++ ) {
		if ( this->valid [i] ) { result++; }
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のアトリビュートが既に存在する場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool AttributesT<T>::has ( const String &_name ) const {
	for ( u8 i = 0; i<u8(this->valid.size ()); i++ ) {
		if ( this->valid [i] ) {
			if ( this->attrT [i]->is ( _name ) ) {
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のアトリビュートタイプを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 AttributesT<T>::getType ( const String &_name ) const {
	for ( u8 i = 0; i<u8 ( this->valid.size () ); i++ ) {
		if ( this->valid [i] ) {
			if ( this->attrT [i]->is ( _name ) ) {
				return this->attrT [i]->getType();
			}
		}
	}
	return BURGESS2::U8_INFINITE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定したIDのアトリビュートタイプを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u8 AttributesT<T>::getType ( const u8 id ) const {
	if (id < this->attrT.size ()) {
		return this->attrT[id]->getType ();
	} else {
		return BURGESS2::U8_INFINITE;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のアトリビュートが既に存在する場合はtrueと配列番号を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool AttributesT<T>::has ( const String &_name, OUT u8 &id ) const {
	for ( u8 i = 0; i<u8(this->valid.size ()); i++ ) {
		if ( this->valid [i] ) {
			if ( this->attrT [i]->is ( _name ) ) {
				id = i;
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュート名を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
String AttributesT<T>::getName ( u8 id ) const{
	if ( this->valid[id] ){
		return this->attrT[id]->getName ();
	}
	return "";
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のアトリビュートを削除する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void AttributesT<T>::deleteAttr ( const String &_name ) {
	u8 id;
	if ( this->has(_name, id) ) {
		this->attrT [id].reset ();
		this->valid [id] = false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のアトリビュート値を全て0.0にする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void AttributesT<T>::clear ( const String &_name ) {
	u8 id;
	if ( this->has ( _name, id ) ) {
		this->attr [id]->clear ( );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュートの名前を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<String> AttributesT<T>::getAllAttributeNames () const{
	std::vector<String> result;
	for ( u32 i = 0; i < this->attrT.size (); i++ ) {
		if ( this->valid [i] ) {
			result.push_back ( this->attrT [i]->getName () );
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のBoolAttributeが無ければ新規作成し、参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BoolAttributeT<T>&  AttributesT<T>::getOrCreateBoolAttribute ( const String &_name, u32 count ) {
	u8 id;
	if ( this->has ( _name, id ) == false  ) {
		id = u8(this->attrT.size ());
		this->attrT.resize ( this->attrT.size () + 1 );
		this->attrT [id].reset ( new BoolAttributeT<T> ( _name, count ) );
		this->valid.resize ( this->attrT.size () );
		this->valid [id] = true;
	}

	if ( this->getType ( _name ) != Burgess2AttrType::BoolAttr ) {
		assert ( false && "There are already different type attribute with the same name" );
	}
	BoolAttributeT<T> *att = static_cast< BoolAttributeT<T>* >( this->attrT [id].get() );
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のF32Attributeが無ければ新規作成し、参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>&  AttributesT<T>::getOrCreateF32Attribute ( const String &_name, u32 count ) {
	u8 id;
	if ( this->has ( _name, id ) == false ) {
		id = u8(this->attrT.size ());
		this->attrT.resize ( this->attrT.size () + 1 );
		this->attrT [id].reset ( new F32AttributeT<T> ( _name, count ) );
		this->valid.resize ( this->attrT.size () );
		this->valid [id] = true;
	}

	if ( this->getType ( _name ) != Burgess2AttrType::F32Attr ) {
		assert ( false && "There are already different type attribute with the same name" );
	}
	F32AttributeT<T> *att = static_cast< F32AttributeT<T>* >( this->attrT [id].get () );
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のF64Attributeが無ければ新規作成し、参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F64AttributeT<T>&  AttributesT<T>::getOrCreateF64Attribute ( const String &_name, u32 count ) {
	u8 id;
	if ( this->has ( _name, id ) == false ) {
		id = u8(this->attrT.size ());
		this->attrT.resize ( this->attrT.size () + 1 );
		this->attrT [id].reset ( new F64AttributeT<T> ( _name, count ) );
		this->valid.resize ( this->attrT.size () );
		this->valid [id] = true;
	}

	if ( this->getType ( _name ) != Burgess2AttrType::F64Attr ) {
		assert ( false && "There are already different type attribute with the same name" );
	}
	F64AttributeT<T> *att = static_cast< F64AttributeT<T>* >( this->attrT [id].get () );
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定した名前のS32Attributeが無ければ新規作成し、参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
S32AttributeT<T>&  AttributesT<T>::getOrCreateS32Attribute ( const String &_name, u32 count ) {
	u8 id;
	if ( this->has ( _name, id ) == false ) {
		id = u8(this->attrT.size ());
		this->attrT.resize ( this->attrT.size () + 1 );
		this->attrT [id].reset ( new S32AttributeT<T> ( _name, count ) );
		this->valid.resize ( this->attrT.size () );
		this->valid [id] = true;
	}

	if ( this->getType ( _name ) != Burgess2AttrType::S32Attr ) {
		assert ( false && "There are already different type attribute with the same name" );
	}
	S32AttributeT<T> *att = static_cast< S32AttributeT<T>* >( this->attrT [id].get () );
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のU32Attributeが無ければ新規作成し、参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
U32AttributeT<T>&  AttributesT<T>::getOrCreateU32Attribute ( const String &_name, u32 count ) {
	u8 id;
	if ( this->has ( _name, id ) == false ) {
		id = u8(this->attrT.size ());
		this->attrT.resize ( this->attrT.size () + 1 );
		this->attrT [id].reset ( new U32AttributeT<T> ( _name, count ) );
		this->valid.resize ( this->attrT.size () );
		this->valid [id] = true;
	}

	if ( this->getType ( _name ) != Burgess2AttrType::U32Attr ) {
		assert ( false && "There are already different type attribute with the same name" );
	}
	U32AttributeT<T> *att = static_cast< U32AttributeT<T>* >( this->attrT [id].get () );
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のL16ArrayU32Attributeが無ければ新規作成し、参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16ArrayU32AttributeT<T>&  AttributesT<T>::getOrCreateL16ArrayU32Attribute ( const String &_name, u32 count ) {
	u8 id;
	if ( this->has ( _name, id ) == false ) {
		id = u8(this->attrT.size ());
		this->attrT.resize ( this->attrT.size () + 1 );
		this->attrT [id].reset ( new L16ArrayU32AttributeT<T> ( _name, count ) );
		this->valid.resize ( this->attrT.size () );
		this->valid [id] = true;
	}

	if ( this->getType ( _name ) != Burgess2AttrType::L16U32Attr ) {
		assert ( false && "There are already different type attribute with the same name" );
	}
	L16ArrayU32AttributeT<T> *att = static_cast< L16ArrayU32AttributeT<T>* >( this->attrT [id].get () );
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のVec2Attributeが無ければ新規作成し、参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>&  AttributesT<T>::getOrCreatVec2Attribute ( const String &_name, u32 count ) {
	u8 id;
	if ( this->has ( _name, id ) == false ) {
		id = u8(this->attrT.size ());
		this->attrT.resize ( this->attrT.size () + 1 );
		this->attrT [id].reset ( new Vec2AttributeT<T> ( _name, count ) );
		this->valid.resize ( this->attrT.size () );
		this->valid [id] = true;
	}

	if ( this->getType ( _name ) != Burgess2AttrType::Vec2Attr ) {
		assert ( false && "There are already different type attribute with the same name" );
	}
	Vec2AttributeT<T> *att = static_cast< Vec2AttributeT<T>* >( this->attrT [id].get () );
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のVec3Attributeが無ければ新規作成し、参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>&  AttributesT<T>::getOrCreatVec3Attribute ( const String &_name, u32 count ) {
	u8 id;
	if ( this->has ( _name, id ) == false ) {
		id = u8(this->attrT.size ());
		this->attrT.resize ( this->attrT.size () + 1 );
		this->attrT [id].reset ( new Vec3AttributeT<T> ( _name, count ) );
		this->valid.resize ( this->attrT.size () );
		this->valid [id] = true;
	}

	if ( this->getType ( _name ) != Burgess2AttrType::Vec3Attr ) {
		assert ( false && "There are already different type attribute with the same name" );
	}
	Vec3AttributeT<T> *att = static_cast< Vec3AttributeT<T>* >( this->attrT [id].get() );
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のVec4Attributeが無ければ新規作成し、参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>&  AttributesT<T>::getOrCreatVec4Attribute ( const String &_name, u32 count ) {
	u8 id;
	if ( this->has ( _name, id ) == false ) {
		id = u8(this->attrT.size ());
		this->attrT.resize ( this->attrT.size () + 1 );
		this->attrT [id].reset ( new Vec4AttributeT<T> ( _name, count ) );
		this->valid.resize ( this->attrT.size () );
		this->valid [id] = true;
	}

	if ( this->getType ( _name ) != Burgess2AttrType::Vec4Attr ) {
		assert ( false && "There are already different type attribute with the same name" );
	}
	Vec4AttributeT<T> *att = static_cast< Vec4AttributeT<T>* >( this->attrT [id].get () );
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のSkinningAttributeが無ければ新規作成し、参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
SkinningAttributeT<T>&  AttributesT<T>::getOrCreateSkinningAttribute ( const String &_name, u32 count ){
	u8 id;
	if ( this->has ( _name, id ) == false ){
		id = u8(this->attrT.size ());
		this->attrT.resize ( this->attrT.size () + 1 );
		this->attrT[id].reset ( new SkinningAttributeT<T> ( _name, count ) );
		this->valid.resize ( this->attrT.size () );
		this->valid[id] = true;
	}

	if ( this->getType ( _name ) != Burgess2AttrType::SkinningAttr ){
		assert ( false && "There are already different type attribute with the same name" );
	}
	SkinningAttributeT<T> *att = static_cast< SkinningAttributeT<T>* >( this->attrT[id].get () );
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のBoolAttributeの参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BoolAttributeT<T>&  AttributesT<T>::getBoolAttribute ( const String &_name ) const {
	assert ( this->has ( _name ) );
	if ( this->getType ( _name ) != Burgess2AttrType::BoolAttr ) {
		assert ( false && "Attribute data type is different" );
	}

	BoolAttributeT<T>* att = NULL;
	u8 id;
	if ( this->has ( _name, id ) ) {
		if ( this->attrT [id]->getType () == Burgess2AttrType::BoolAttr ) {
			att = static_cast< BoolAttributeT<T>* >( this->attrT [id].get () );
			return *att;
		} 
	}
	assert ( false ); /* not find attribute */
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のF32Attributeの参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F32AttributeT<T>&  AttributesT<T>::getF32Attribute ( const String &_name ) const {
	assert ( this->has ( _name ) );
	if ( this->getType ( _name ) != Burgess2AttrType::F32Attr ) {
		assert ( false && "Attribute data type is different" );
	}

	F32AttributeT<T>* att = NULL;
	u8 id;
	if ( this->has ( _name, id ) ) {
		if ( this->attrT [id]->getType () == Burgess2AttrType::F32Attr ) {
			att = static_cast< F32AttributeT<T>* >( this->attrT [id].get () );
			return *att;
		} 
	}
	assert ( false ); /* not find attribute */
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のF64Attributeの参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
F64AttributeT<T>&  AttributesT<T>::getF64Attribute ( const String &_name ) const {
	assert ( this->has ( _name ) );
	if ( this->getType ( _name ) != Burgess2AttrType::F64Attr ) {
		assert ( false && "Attribute data type is different" );
	}

	F64AttributeT<T>* att = NULL;
	u8 id;
	if ( this->has ( _name, id ) ) {
		if ( this->attrT [id]->getType () == Burgess2AttrType::F64Attr ) {
			att = static_cast< F64AttributeT<T>* >( this->attrT [id].get () );
			return *att;
		}
	}
	assert ( false ); /* not find attribute */
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のU32Attributeの参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
U32AttributeT<T>&  AttributesT<T>::getU32Attribute ( const String &_name ) const {
	assert ( this->has ( _name ) );
	if ( this->getType ( _name ) != Burgess2AttrType::U32Attr ) {
		assert ( false && "Attribute data type is different" );
	}

	U32AttributeT<T>* att = NULL;
	u8 id;
	if ( this->has ( _name, id ) ) {
		if ( this->attrT [id]->getType () == Burgess2AttrType::U32Attr ) {
			att = static_cast< U32AttributeT<T>* >( this->attrT [id].get () );
			return *att;
		}
	}
	assert ( false ); /* not find attribute */
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のS32Attributeの参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
S32AttributeT<T>&  AttributesT<T>::getS32Attribute ( const String &_name ) const {
	assert ( this->has ( _name ) );
	if ( this->getType ( _name ) != Burgess2AttrType::S32Attr ) {
		assert ( false && "Attribute data type is different" );
	}

	S32AttributeT<T>* att = NULL;
	u8 id;
	if ( this->has ( _name, id ) ) {
		if ( this->attrT [id]->getType () == Burgess2AttrType::S32Attr ) {
			att = static_cast< S32AttributeT<T>* >( this->attrT [id].get () );
			return *att;
		}
	}
	assert ( false ); /* not find attribute */
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のL16ArrayU32Attributeの参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16ArrayU32AttributeT<T>&  AttributesT<T>::getL16ArrayU32Attribute ( const String &_name )const {
	assert ( this->has ( _name ) );
	if ( this->getType ( _name ) != Burgess2AttrType::L16U32Attr ) {
		assert ( false && "Attribute data type is different" );
	}

	L16ArrayU32AttributeT<T>* att = NULL;
	u8 id;
	if ( this->has ( _name, id ) ) {
		if ( this->attrT [id]->getType () == Burgess2AttrType::L16U32Attr ) {
			att = static_cast< L16ArrayU32AttributeT<T>* >( this->attrT [id].get () );
			return *att;
		}
	}
	assert ( false ); /* not find attribute */
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のVec2Attributeの参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2AttributeT<T>&  AttributesT<T>::getVec2Attribute ( const String &_name ) const {
	assert ( this->has ( _name ) );
	if ( this->getType ( _name ) != Burgess2AttrType::Vec2Attr ) {
		assert ( false && "Attribute data type is different" );
	}

	Vec2AttributeT<T>* att = NULL;
	u8 id;
	if ( this->has ( _name, id ) ) {
		if ( this->attrT [id]->getType () == Burgess2AttrType::Vec2Attr ) {
			att = static_cast< Vec2AttributeT<T>* >( this->attrT [id].get () );
			return *att;
		}
	}
	assert ( false ); /* not find attribute */
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のVec3Attributeの参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3AttributeT<T>&  AttributesT<T>::getVec3Attribute ( const String &_name ) const {
	assert ( this->has ( _name ) );
	if ( this->getType ( _name ) != Burgess2AttrType::Vec3Attr ) {
		assert ( false && "Attribute data type is different" );
	}

	Vec3AttributeT<T>* att = NULL;
	u8 id;
	if ( this->has ( _name, id ) ) {
		if ( this->attrT [id]->getType () == Burgess2AttrType::Vec3Attr ) {
			att = static_cast< Vec3AttributeT<T>* >( this->attrT [id].get () );
			return *att;
		}
	}
	assert ( false ); /* not find attribute */
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のVec4Attributeの参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4AttributeT<T>&  AttributesT<T>::getVec4Attribute ( const String &_name ) const {
	assert ( this->has ( _name ) );
	if ( this->getType ( _name ) != Burgess2AttrType::Vec4Attr ) {
		assert ( false && "Attribute data type is different" );
	}

	Vec4AttributeT<T>* att = NULL;
	u8 id;
	if ( this->has ( _name, id ) ) {
		if ( this->attrT [id]->getType () == Burgess2AttrType::Vec4Attr ) {
			att = static_cast< Vec4AttributeT<T>* >( this->attrT [id].get () );
			return *att;
		}
	}
	assert ( false ); /* not find attribute */
	return *att;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定した名前のSkinningAttributeTの参照を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
SkinningAttributeT<T>&  AttributesT<T>::getSkinningAttribute ( const String &_name ) const{
	assert ( this->has ( _name ) );
	if ( this->getType ( _name ) != Burgess2AttrType::SkinningAttr ){
		assert ( false && "Attribute data type is different" );
	}

	SkinningAttributeT<T>* att = NULL;
	u8 id;
	if ( this->has ( _name, id ) ){
		if ( this->attrT[id]->getType () == Burgess2AttrType::SkinningAttr ){
			att = static_cast< SkinningAttributeT<T>* >( this->attrT[id].get () );
			return *att;
		}
	}
	assert ( false ); /* not find attribute */
	return *att;
}
