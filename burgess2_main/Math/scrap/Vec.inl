
template <typename T>
u32 TestVecArraySize (const VecT<T> &vec0, const VecT<T> &vec1 ) {
	return vec0.size () == vec1.size () ? vec0.size () : 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T>::VecT ( ) {
	this->vals.resize ( 0 );
};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T>::VecT ( u32 size ) {
	this->vals.resize ( size );
};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T>::VecT ( std::vector<T> vals )
	: vals { vals }
{}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T>::VecT ( const VecT& source )
	: vals { source.vals }
{}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �z�񐔂�ύX����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::resize ( u32 newSize ) {
	this->vals.resize ( newSize );
	this->vals.shrink_to_fit ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �z��v�f��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::set ( u32 id, T val ) {
	assert ( id < this->vals.size () );
	this->vals [id] = val;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �S�Ă̔z��v�f�𓯂��l�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::setAll ( T val ) {
	u32 size = this->size ();
	for ( u32 id = 0; id < size; ++id ) {
		(*this)[id] = val;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �z��v�f��}������
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::insert ( u32 id, T val ) {
	this->vals.resize ( id > this->size () ? id + 1 : this->size () + 1 );
	for ( u32 i = this->size () - 1; i>id; i-- ) {
		this->vals [i] = this->vals [i - 1];
	}
	this->vals [id] = val;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Vec��z��̍Ō�Ɍ�������
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::combine ( const VecT<T> &other ) {
	u32 origSize = this->size ();
	this->resize ( this->size () + other.size () );
	for ( u32 i = 0; i < other.size (); i++ ) {
		this->set ( origSize + i, other.get ( i ) );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �z��̍ŏ��ɗv�f��ǉ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::push_top ( T val ) {
	this->insert ( 0, val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �z��̍Ō�ɗv�f��ǉ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::push_back ( T val ) {
	this->insert ( this->size (), val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �z��v�f���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::get ( u32 id ) const {
	assert ( id < this->vals.size () );
	return this->vals [id];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �S�Ă̔z��v�f���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> VecT<T>::getValues () const {
	return this->vals;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Type�z��֕ϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> VecT<T>::toTypeArray () const {
	return this->getValues();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Vec��Ԃ��i�|�����[�t�B�Y���̂��߂ɏ����j
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::toVec () const {
	return *this;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �f�o�b�O�p�̕�����ɂ���@"�f�[�^�^�F[ �l ]"
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
String VecT<T>::toString ( const String title ) const {
	String s = "Vec :size=" + std::to_string(this->size()) + ", [";
	for ( u32 i = 0; i < this->size () - 1; i++ ) {
		s += GetFloorString ( this->get(i) ) + ", ";
	}
	s += GetFloorString ( this->get(this->size () - 1) ) + "]";
	String t = title;
	if ( title.length () != 0 ) { t += " : "; }
	return t + s;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �z��v�f���폜����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::remove ( u32 id ) {
	assert ( id < this->vals.size () );
	T temp = this->vals [id];
	for ( u32 i = id; i<this->size () - 1; i++ ) {
		this->vals [i] = this->vals [i + 1];
	}
	this->vals.resize ( this->size () - 1 );
	return temp;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �ŏ��̔z��v�f���폜����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::pop_top () {
	return this->remove ( 0 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �Ō�̔z��v�f���폜����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::pop_back () {
	return this->remove ( this->size () - 1 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �S�Ă̔z��v�f�������ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool VecT<T>::equal ( const VecT &other ) const {
	for ( u32 i = 0; i < this->vals.size (); ++i ) {
		if ( this->vals [i] != other.vals [i] ) {
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �S�Ă̔z��v�f���덷�͈͓��œ����ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool VecT<T>::almostEqual ( const VecT &other, T TOLERANCE ) const {
	for ( u32 i = 0; i < this->vals.size (); ++i ) {
		if ( TOLERANCE <= std::abs ( this->vals [i] - other.vals [i] )  ) {
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �ŏ��l�̔z��v�f��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::minimum ( OUT u32 &id ) const {
	assert ( 1 < this->vals.size () );
	id = 0;
	T val = this->get ( 0 );
	for ( u32 i = 1; i < this->size (); i++ ) {
		if ( this->get ( i ) < val ) {
			val = this->get ( i );
			id = i;
		}
	}
	return val;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �ŏ��l�̔z��v�f��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::minimum ( ) const {
	u32 id;
	return this->minimum(id);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �ő�l�̔z��v�f��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::maximum ( OUT u32 &id ) const {
	assert ( 1 < this->vals.size () );
	id = 0;
	T val = this->get ( 0 );
	for ( u32 i = 1; i < this->size (); i++ ) {
		if ( this->get ( i ) > val ) {
			val = this->get ( i );
			id = i;
		}
	}
	return val;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �ő�l�̔z��v�f��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::maximum () const {
	u32 id;
	return this->maximum ( id );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     t�l(0.0~1.0)�Ɋ�Â��đΏۃx�N�g���Ɛ��`��Ԃ����x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::lerp ( const VecT &other, T t )const {
	return *this + ( ( other - *this ) * t );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     t�l(0.0~1.0)�Ɋ�Â��đΏۃx�N�g���Ɛ��`��Ԃ����x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::linearInterpolate ( const VecT &other, T t ) const {
	return this->lerp ( other, t );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::lengthSquared () const {
	T result = 0;
	for ( u32 i = 0; i < this->size (); i++ ) {
		T val = this->get ( i );
		result += val * val;
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �x�N�g���̒����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::length () const {
	return sqrt ( this->lengthSquared () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �P�ʃx�N�g���ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::setNormal () {
	*this = *this / this->length ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �P�ʃx�N�g���ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::setUnit () {
	this->setNormal ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �P�ʃx�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::normal () const {
	return this->unit ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �P�ʃx�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::unit () const {
	return *this / this->length ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      ���ς�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T VecT<T>::dot ( const VecT &other ) const {
	u32 size = TestVecArraySize ( *this, other );
	T result = 0;
	for ( u32 i = 0; i < size; i++ ) {
		result += this->get ( i ) * other.get ( i );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �z��v�f�̐���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 VecT<T>::size () const {
	return int ( this->vals.size () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     [ ]�ŃA�N�Z�X����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T& VecT<T>::operator[]( u32 id ) {
	assert ( id < this->vals.size () );
	u32 nID = this->size () <= id ? this->size () : id;
	return this->vals [nID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& VecT<T>::operator[]( u32 id ) const {
	assert ( id < this->vals.size () );
	u32 nID = this->size () <= id ? this->size () : id;
	return this->vals [nID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool VecT<T>::operator == ( const VecT &other ) const {
	return this->equal ( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool VecT<T>::operator != ( const VecT &other ) const {
	return !this->equal ( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::operator + ( const VecT &other ) const {
	u32 size = TestVecArraySize ( *this, other );
	VecT resultult ( size );
	for ( u32 i = 0; i < size; ++i ) {
		resultult.vals [i] = this->get ( i ) + other.get ( i );
	}
	return resultult;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::operator += ( const VecT &other ) {
	u32 size = TestVecArraySize ( *this, other );
	for ( u32 i = 0; i < size; ++i ) {
		this -> vals[i] = this->get ( i ) + other.get ( i );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::operator - ( const VecT &other ) const {
	u32 size = TestVecArraySize ( *this, other );
	VecT resultult ( size );
	for ( u32 i = 0; i < size; ++i ) {
		resultult.vals [i] = this->get ( i ) - other.get ( i );
	}
	return resultult;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::operator -= ( const VecT &other ) {
	u32 size = TestVecArraySize ( *this, other );
	for ( u32 i = 0; i < size; ++i ) {
		this -> vals [i] = this->get ( i ) - other.get ( i );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::operator * ( T val ) const {
	u32 size = this->size ();
	VecT result ( size );
	for ( u32 i = 0; i < size; i++ ) {
		result .vals[i] = this->get ( i ) * val;
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::operator * ( const VecT &other ) const {
	u32 size = TestVecArraySize ( *this, other );
	VecT result ( size );
	for ( u32 i = 0; i < size; ++i ) {
		result.vals [i] = this->get ( i ) * other.get ( i );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::operator *= ( T val ) {
	u32 size = this->size ();
	for ( u32 i = 0; i < size; i++ ) {
		this->vals [i] = this->get ( i ) * val;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::operator *= ( const VecT &other ) {
	u32 size = TestVecArraySize ( *this, other );
	for ( u32 i = 0; i < size; ++i ) {
		this->vals [i] = this->get ( i ) * other.get ( i );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::operator / ( T val ) const {
	u32 size = this->size ();
	VecT result ( size );
	for ( u32 i = 0; i < size; i++ ) {
		result.vals [i] = this->get ( i ) / val;
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> VecT<T>::operator / ( const VecT &other ) const {
	u32 size = TestVecArraySize ( *this, other );
	VecT result ( size );
	for ( u32 i = 0; i < size; ++i ) {
		result.vals [i] = this->get ( i ) / other.get ( i );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::operator /= ( T val ) {
	for ( u32 id = 0; id < this->size (); ++id ) {
		this->vals [id] = this->get ( id ) / val;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void VecT<T>::operator /= ( const VecT &other ) {
	u32 size = TestVecArraySize ( *this, other );
	for ( u32 i = 0; i < size; ++i ) {
		this->vals [i] = this->get ( i ) / other.get ( i );
	}
}






