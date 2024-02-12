//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�̐���Ԃ�
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
//!	     �w�肵�����O�̃A�g���r���[�g�����ɑ��݂���ꍇ��true��Ԃ�
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
//!	     �w�肵�����O�̃A�g���r���[�g�^�C�v��Ԃ�
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
//!	     �w�肵��ID�̃A�g���r���[�g�^�C�v��Ԃ�
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
//!	     �w�肵�����O�̃A�g���r���[�g�����ɑ��݂���ꍇ��true�Ɣz��ԍ���Ԃ�
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
//!	     �A�g���r���[�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
String AttributesT<T>::getName ( u8 id ) const{
	if ( this->valid[id] ){
		return this->attrT[id]->getName ();
	}
	return "";
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �w�肵�����O�̃A�g���r���[�g���폜����
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
//!	     �w�肵�����O�̃A�g���r���[�g�l��S��0.0�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void AttributesT<T>::clear ( const String &_name ) {
	u8 id;
	if ( this->has ( _name, id ) ) {
		this->attr [id]->clear ( );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�̖��O��Ԃ�
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
//!	     �w�肵�����O��BoolAttribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��F32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��F64Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
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
//!	    �w�肵�����O��S32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��U32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��L16ArrayU32Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��Vec2Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��Vec3Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��Vec4Attribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��SkinningAttribute��������ΐV�K�쐬���A�Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��BoolAttribute�̎Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��F32Attribute�̎Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��F64Attribute�̎Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��U32Attribute�̎Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��S32Attribute�̎Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��L16ArrayU32Attribute�̎Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��Vec2Attribute�̎Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��Vec3Attribute�̎Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��Vec4Attribute�̎Q�Ƃ�Ԃ�
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
//!	     �w�肵�����O��SkinningAttributeT�̎Q�Ƃ�Ԃ�
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
