//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
SkinningAttributeT<T>::SkinningAttributeT ( const String &_name, u32 count ){
	if ( _name == "" ) {
		return;
	}
	this->setValid ( true );
	this->setName ( _name );
	this->setType ( Burgess2AttrType::SkinningAttr );
	this->values.resize ( count );
	this->v[0] = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�̔z�񐔂�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 SkinningAttributeT<T>::size () const {
	return u32(this->values.size ());
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�̔z�񐔂�ύX����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::resize ( u32 count ) {
	this->values.resize ( count );
	this->values.shrink_to_fit ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�l��V�����z��ɐݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::resetValues ( const std::vector<SkinningPairDataT<T>> &_data ) {
	this->values = _data;
	for(u32 i=0; i<_data.size(); ++i){
		L4Array<u16> parts = _data[i].getPartsIDs ();
		for ( u32 j = 0; j<parts.size (); ++j ){
			this->v[0] = Burgess2MathFn::Max ( this->v[0], u16 ( parts[j] ) );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �S�ẴA�g���r���[�g�l��ݒ�i�z�񐔂̕ύX�͖����j����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::setValues ( const std::vector<SkinningPairDataT<T>> &_data ) {
	assert ( this->size () == _data.size () );
	this->values = _data;
	for ( u32 i = 0; i<_data.size (); ++i ){
		L4Array<u16> parts = _data[i].getPartsIDs ();
		for ( u32 j = 0; j<parts.size (); ++j ){
			this->v[0] = Burgess2MathFn::Max ( this->v[0], u8(parts[j]) );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::setValue ( u32 index, const SkinningPairDataT<T> &_data ) {
	assert ( index < this->size () );
	this->values [index] = _data;
	L4Array<u16> parts = _data.getPartsIDs ();
	for(u32 i=0; i<parts.size(); ++i ){
		this->v[0] = Burgess2MathFn::Max ( this->v[0], u16(parts[i]) );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �Y���̃p�[�cID�̃X�L���E�F�C�g�l��ݒ肷�� (�p�[�c�����o�^�̏ꍇ�͐V�K�y�A�Ƃ��Ēǉ��o�^����)
//!	    @brief �Y���̃p�[�cID�̃X�L���E�F�C�g�l��ݒ肷�� (�p�[�c�����o�^�̏ꍇ�͐V�K�y�A�Ƃ��Ēǉ��o�^����)
//!		@param[in] index �A�g���r���[�gID
//!		@param[in] partsID �p�[�cID
//!		@param[in] weight �X�L���E�F�C�g�l
//!		@param[in] normalize�@�ݒ肵���E�F�C�g�l��ێ������܂ܑ��̃y�A�̃E�F�C�g�l���m�[�}���C�Y����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::setOrCreatePair ( u32 index, u16 partsID, T weight, Bool normalize ){
	assert ( index < this->size () );
	if ( this->values[index].hasParts ( partsID ) ){
		this->values[index].setWeight ( partsID, weight, normalize );
	} else{
		this->values[index].addPair ( partsID, weight, normalize );
	}
	this->v[0] = Burgess2MathFn::Max ( this->v[0], partsID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �Y���̃p�[�cID�̃X�L���E�F�C�g�l�����Z�E���Z���� (�p�[�c�����o�^�̏ꍇ�͏I��)
//!	    @brief �Y���̃p�[�cID�̃X�L���E�F�C�g�l�����Z�E���Z���� (�p�[�c�����o�^�̏ꍇ�͏I��)
//!		@param[in] index �A�g���r���[�gID
//!		@param[in] partsID �p�[�cID
//!		@param[in] weight �X�L���E�F�C�g�̉��Z�E���Z�l
//!		@param[in] normalize�@�ҏW�����E�F�C�g�l��ێ������܂ܑ��̃y�A�̃E�F�C�g�l���m�[�}���C�Y����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::addPairWeight ( u32 index, u16 partsID, T weight, Bool normalize ){
	assert ( index < this->size () );
	if ( this->values[index].hasParts ( partsID ) ){
		if ( T ( 0.0 ) < weight ){
			this->values[index].addWeight ( partsID, weight, normalize );
		} else{
			this->values[index].subWeight ( partsID, weight, normalize );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�l���Q�ƂŕԂ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
const std::vector<SkinningPairDataT<T>>& SkinningAttributeT<T>::getValuesRef () const {
	return this->values;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�l��S�Ď擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<SkinningPairDataT<T>> SkinningAttributeT<T>::getValues () const {
	return this->values;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�l���P�擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
SkinningPairDataT<T> SkinningAttributeT<T>::getValue (u32 index) const{
	assert ( index < this->size () );
	return this->values [index];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �w�肵���A�g���r���[�g�ɓo�^����Ă���y�A�̃E�F�C�g�l�̍��v��1.0�ɂȂ�悤�ɒl�𒲐�����
//!	    @brief �w�肵���A�g���r���[�g�ɓo�^����Ă���y�A�̃E�F�C�g�l�̍��v��1.0�ɂȂ�悤�ɒl�𒲐�����
//!		@param[in] index �A�g���r���[�gID
//!		@param[in] cleanup ���g�p�̃y�A���폜����
//!		@param[in] maxInfluence�@�}�b�N�X�C���t���G���X��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::normalize ( u32 index, Bool cleanup, u8 maxInfluence ){
	assert ( index < this->size () );
	this->values[index].normalize( maxInfluence );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �E�F�C�g�l�̍��v��1.0�ɂȂ�悤�ɒl�𒲐�����
//!	    @brief �E�F�C�g�l�̍��v��1.0�ɂȂ�悤�ɒl�𒲐�����
//!		@param[in] cleanup ���g�p�̃y�A���폜����
//!		@param[in] maxInfluence�@�}�b�N�X�C���t���G���X��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::allNormalize ( Bool cleanup, u8 maxInfluence ){
	#pragma omp parallel for if(500 < this->values.size())
	for ( u32 i = 0, n = this->values.size (); i < n; ++i ){
		this->normalize ( i, cleanup, maxInfluence );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �A�g���r���[�g�l��[ ]�ŃA�N�Z�X����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SkinningPairDataT<T>& SkinningAttributeT<T>::operator[]( u32 index ) {
	assert ( index < this->size () );
	return this->values [index];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�l��[ ]�ŃA�N�Z�X����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const SkinningPairDataT<T>& SkinningAttributeT<T>::operator[]( u32 index ) const {
	assert ( index < this->size () );
	return this->values [index];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �l��S��0�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::clear () {
	u32 cnt = this->values.size ();
	for ( u32 i = 0; i<cnt; i++ ) {
		this->values [i] = SkinningAttributeT<T> ();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �A�g���r���[�g�ɓo�^���Ă���p�[�cID�̍ő�ID��Ԃ��i=�X�L���A�g���r���[�g�ɑΉ�����W���C���g/�{�[���̍ő吔�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u16 SkinningAttributeT<T>::partsMaxID ()const{
	return this->v[0];
}

/* SkinningPairDataT<T> */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �p�[�cID���y�A�ɓo�^����Ă���ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool SkinningPairDataT<T>::hasParts ( u16 partsID )const{
	for ( u32 i = 0, n = this->pairCount (); i < n; ++i ){
		if ( this->partsIDs[i] == partsID ){ return true; }
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �o�^����Ă���y�A�̐���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 SkinningPairDataT<T>::pairCount ()const{
	return this->partsIDs.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �y�A��V�K�ǉ�����i�p�[�cID���y�A���X�g�ɑ��݂��Ă��Ȃ����Ƃ����O��.hasPart()�Ŋm�F���Ă�����s���邱�Ɓj
//!	    @brief �y�A��V�K�ǉ�����i�p�[�cID���y�A���X�g�ɑ��݂��Ă��Ȃ����Ƃ����O��.hasPart()�Ŋm�F���Ă�����s���邱�Ɓj
//!		@param[in] partsID �p�[�cID
//!		@param[in] weight �E�F�C�g�l
//!		@param[in] normalize �ݒ肵���E�F�C�g�l��ێ������܂ܑ��̃y�A�̃E�F�C�g�l���m�[�}���C�Y����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::addPair ( u16 partsID, T weight, Bool normalize ){
	assert ( this->hasParts ( partsID ) == false );
	u16 s = this->partsIDs.size ();
	this->partsIDs.resize ( s + 1 );
	this->weights.resize ( s + 1 );
	this->partsIDs[s] = partsID;
	this->weights[s] = weight;

	// �ǉ������E�F�C�g��ێ����āA���̃y�A�̃E�F�C�g���m�[�}���C�Y
	if ( normalize ){
		T total{ 0.0 };
		for ( u32 i = 0; i < s; ++i ){ 
			total += this->weights[i]; 
		}
		T remainder = T ( 1.0 ) - weight;
		for ( u32 i = 0; i < s; ++i ){
			this->weights[i] = remainder * ( this->weights[i] / total );
		}
	}

}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵���p�[�cID�̃E�F�C�g��ݒ肷��i�p�[�cID�����Ƀy�A���X�g�ɑ��݂��邱�Ƃ����O��.hasPart()�Ŋm�F���Ă�����s���邱�Ɓj
//!	    @brief �w�肵���p�[�cID�̃E�F�C�g��ݒ肷��i�p�[�cID�����Ƀy�A���X�g�ɑ��݂��邱�Ƃ����O��.hasPart()�Ŋm�F���Ă�����s���邱�Ɓj
//!		@param[in] partsID �p�[�cID
//!		@param[in] weight �E�F�C�g�l
//!		@param[in] normalize �ݒ肵���E�F�C�g�l��ێ������܂ܑ��̃y�A�̃E�F�C�g�l���m�[�}���C�Y����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::setWeight ( u16 partsID, T weight, Bool normalize ){
	assert ( this->hasParts ( partsID ) );
	if ( weight <= T ( 0.0 ) ){ return; } // �E�F�C�g�l��0.0�ȉ��̏ꍇ�͏I��

	Bool fined = false;
	T total{0.0}; // �m�[�}���C�Y�������s�����߂̃f�[�^���������Ă���
	for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
		if ( this->partsIDs[i] == partsID ){
			this->weights[i] = weight;
			fined = true;
		} else{
			total += this->weights[i];
		}
	}

	// �w��̃p�[�cID�����݂��A���m�[�}���C�Y���L���ȏꍇ
	if ( fined && normalize ){ 
		T remainder = T ( 1.0 ) - weight;
		for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
			if ( partsIDs[i] != partsID ){
				this->weights[i] = remainder * ( this->weights[i] / total );
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �w�肵���p�[�cID�̃E�F�C�g�l�����Z����i�p�[�cID�����Ƀy�A���X�g�ɑ��݂��邱�Ƃ����O��.hasPart()�Ŋm�F���Ă�����s���邱�Ɓj
//!	    @brief �w�肵���p�[�cID�̃E�F�C�g�l�����Z����i�p�[�cID�����Ƀy�A���X�g�ɑ��݂��邱�Ƃ����O��.hasPart()�Ŋm�F���Ă�����s���邱�Ɓj
//!		@param[in] partsID �p�[�cID
//!		@param[in] weight �E�F�C�g�l
//!		@param[in] normalize �ҏW�����E�F�C�g�l��ێ������܂ܑ��̃y�A�̃E�F�C�g�l���m�[�}���C�Y����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::addWeight ( u16 partsID, T weight, Bool normalize){
	assert ( this->hasParts ( partsID ) );
	if ( weight <= T ( 0.0 ) ){ return; } // �E�F�C�g�l��0.0�ȉ��̏ꍇ�͏I��

	Bool fined = false;
	T setW;
	T total{0.0}; // �m�[�}���C�Y�������s�����߂̃f�[�^���������Ă���
	for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
		if ( this->partsIDs[i] == partsID ){
			setW = Burgess2MathFn::Min ( this->weights[i] + weight, T ( 1.0 ) );
			this->weights[i] = setW;
			fined = true;
		} else{
			total += this->weights[i];
		}
	}

	// �w��̃p�[�cID�����݂��A���m�[�}���C�Y���L���ȏꍇ
	if ( fined && normalize ){
		T remainder = T ( 1.0 ) - setW;
		for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
			if ( partsIDs[i] != partsID ){
				this->weights[i] = remainder * ( this->weights[i] / total );
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �w�肵���p�[�cID�̃E�F�C�g�l�����Z����i�p�[�cID�����Ƀy�A���X�g�ɑ��݂��邱�Ƃ����O��.hasPart()�Ŋm�F���Ă�����s���邱�Ɓj
//!	    @brief �w�肵���p�[�cID�̃E�F�C�g�l�����Z����i�p�[�cID�����Ƀy�A���X�g�ɑ��݂��邱�Ƃ����O��.hasPart()�Ŋm�F���Ă�����s���邱�Ɓj
//!		@param[in] partsID �p�[�cID
//!		@param[in] weight �E�F�C�g�l
//!		@param[in] normalize �ҏW�����E�F�C�g�l��ێ������܂ܑ��̃y�A�̃E�F�C�g�l���m�[�}���C�Y����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::subWeight ( u16 partsID, T weight, Bool normalize ){
	assert ( this->hasParts ( partsID ) );
	if ( weight <= T ( 0.0 ) ){ return; } // �E�F�C�g�l��0.0�ȉ��̏ꍇ�͏I��

	Bool fined = false;
	T setW;
	T total{0.0}; // �m�[�}���C�Y�������s�����߂̃f�[�^���������Ă���
	for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
		if ( this->partsIDs[i] == partsID ){
			setW = Burgess2MathFn::Max ( this->weights[i] - weight, T ( 0.0 ) );
			this->weights[i] = setW;
			fined = true;
		} else{
			total += this->weights[i];
		}
	}

	// �w��̃p�[�cID�����݂��A���m�[�}���C�Y���L���ȏꍇ
	if ( fined && normalize ){
		T remainder = T ( 1.0 ) - setW;
		for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
			if ( partsIDs[i] != partsID ){
				this->weights[i] = remainder * ( this->weights[i] / total );
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �E�F�C�g�l�̍��v��1.0�ɂȂ�悤�Ɋe�y�A�̃E�F�C�g�l�𒲐�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::normalize ( u8 maxInfluence ){
	u32 s = this->pairCount ();
	if ( s == 0 ){ return; } // �o�^�̃y�A��0�̏ꍇ�͏I��
	if ( s == 1 ){ this->weights[0] = T ( 1.0 ); } // �o�^�̃y�A���P�̏ꍇ�̓E�F�C�g�l��1.0�ɂ��ďI��

	T total{0.0};
	if ( s < maxInfluence ){ 
		// �o�^�̃y�A�����}�b�N�X�C���t���G���X�ȉ��̏ꍇ�̓E�F�C�g���m�[�}���C�Y����
		for ( u8 i = 0; i < s; ++i ){ total += this->weights[i]; }
		for ( u8 i = 0; i < s; ++i ){ this->weights[i] = this->weights[i] / total; }
	} else{
		// �E�F�C�g�l���傫�ȏ��Ƀ}�b�N�X�C���t���G���X���Ńm�[�}���C�Y����i���̑��̃y�A�̃E�F�C�g�l��0.0�ɂ���j
		
		// �E�F�C�g�l���傫�ȏ��ɕ��ׂ�
		std::vector<T> restW ( s );
		std::vector<u32> origID ( s );
		for ( u8 i = 0; i < s; ++i ){ restW[i] = this->weights[i]; origID[i] = i; }
		Burgess2MathFn::Sort ( restW, origID, true ); 

		// �S�Ẵy�A�̃E�F�C�g�l��0.0�ɂ���
		for ( u8 i = 0; i < s; ++i ){ this->weights[i] = T ( 0.0 ); }

		// �}�b�N�X�C���t���G���X���̃y�A�ɃE�F�C�g��ݒ肵����
		for ( u8 i = 0; i < maxInfluence; ++i ){ total += restW[i]; }
		for ( u8 i = 0; i < maxInfluence; ++i ){ this->weights[origID[i]] = restW[i] / total; }
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �E�F�C�g�l�̍��v��1.0�ɂȂ�悤�ɒ������A���g�p�̃y�A���폜����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::normalizeAndCleanup ( u8 maxInfluence ){
	u32 s = this->pairCount ();
	if ( s == 0 ){ return; } // �o�^�̃y�A��0�̏ꍇ�͏I��
	if ( s == 1 ){ this->weights[0] = T ( 1.0 ); } // �o�^�̃y�A���P�̏ꍇ�̓E�F�C�g�l��1.0�ɂ��ďI��

	T total{0.0};
	if ( s < maxInfluence ){
		// �o�^�̃y�A�����}�b�N�X�C���t���G���X�ȉ��̏ꍇ�̓E�F�C�g���m�[�}���C�Y����
		for ( u8 i = 0; i < s; ++i ){ total += this->weights[i]; }
		L4Array<u16> restP = this->partsIDs;
		L4Array<T> restW = this->weights;
		
		// �o�^���Ă����S�Ẵy�A���폜����
		this->partsIDs.resize ( 0 );
		this->weights.resize ( 0 );

		// �E�F�C�g�l��0.0���傫���y�A���ēo�^���ďI��
		for ( u8 i = 0; i < s; ++i ){
			if ( T ( 0.0 ) < restW[i] ){
				this->partsIDs.push ( restP[i] );
				this->weights.push ( restW[i] / total );
			}
		}
	} else{
		// �E�F�C�g�l���傫�ȏ��Ƀ}�b�N�X�C���t���G���X���Ńm�[�}���C�Y����i�E�F�C�g�l��0.0�̃y�A�͍폜����j

		// �E�F�C�g�l���傫�ȏ��ɕ��ׂ�
		std::vector<T> restW ( s );
		std::vector<u32> origID ( s );
		for ( u8 i = 0; i < s; ++i ){ restW[i] = this->weights[i]; origID[i] = i; }
		Burgess2MathFn::Sort ( restW, origID, true );

		L4Array<u16> restP = this->partsIDs; // ���̃p�[�cID���X�g��ێ����Ă���

		// �o�^���Ă����S�Ẵy�A���폜����
		this->partsIDs.resize ( 0 );
		this->weights.resize ( 0 );

		// �}�b�N�X�C���t���G���X���̃y�A��o�^������(�E�F�C�g��0.0�̂��͓̂o�^���Ȃ�)
		for ( u8 i = 0; i < maxInfluence; ++i ){ total += restW[i]; } // �E�F�C�g�̑傫�ȏ��ɉ��Z���Ă���
		for ( u8 i = 0; i < maxInfluence; ++i ){
			if ( T ( 0.0 ) < restW[i] ){
				this->partsIDs.push ( restP[origID[i]] ); // �p�[�cID��o�^
				this->weights.push( restW[i] / total ); // �V�����E�F�C�g�l��ݒ�
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �y�A�̒l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::getPairValue ( u32 pairID, OUT u16 &partsID, OUT T &weight )const{
	if ( this->pairCount () <= pairID ){ return; }
	partsID = this->partsIDs[pairID];
	weight = this->weights[pairID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �p�[�cID���X�g���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L4Array<u16> SkinningPairDataT<T>::getPartsIDs ()const{
	return this->partsIDs;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �X�L���E�F�C�g���X�g���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L4Array<T> SkinningPairDataT<T>::getWeights ()const{
	return this->weights;
}