
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T>::Mat3T () {
	this->setIdentity ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T>::Mat3T ( T _00, T _01, T _02, T _10, T _11, T _12, T _20, T _21, T _22 ) 
	: row { 
		Vec3T<T> { _00, _01, _02 },
		Vec3T<T> { _10, _11, _12 },
		Vec3T<T> { _20, _21, _22 }
	}
{}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T>::Mat3T ( const Vec3T<T> &row0, const Vec3T<T> &row1, const Vec3T<T> &row2 )
	: row { row0, row1, row2 } {}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T>::Mat3T ( const VecT<T> &vec ) {
	assert ( 9 <= vec.size () );
	*this = Mat3T {
		Vec3T<T> { vec.get ( 0 ), vec.get ( 1 ), vec.get ( 2 ) },
		Vec3T<T> { vec.get ( 3 ), vec.get ( 4 ), vec.get ( 5 ) },
		Vec3T<T> { vec.get ( 6 ), vec.get ( 7 ), vec.get ( 8 ) }
	};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T>::Mat3T ( const QuatT<T> &quat ) {
	*this = quat.toMat3 ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T>::Mat3T ( const Rad3T<T> &rad ) {
	*this = rad.toMat3 ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T>::Mat3T ( const Deg3T<T> &deg ) {
	*this = deg.toMat3 ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T>::Mat3T ( const std::vector<T> &arrayVal ) {
	assert ( 9 <= arrayVal.size () );
	*this = Mat3T {
		Vec3T<T> { arrayVal [0], arrayVal [1], arrayVal [2] },
		Vec3T<T> { arrayVal [3], arrayVal [4], arrayVal [5] },
		Vec3T<T> { arrayVal [6], arrayVal [7], arrayVal [8] }
	};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �P�ʃ}�g���b�N�X�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setIdentity () {
	this->row [0] = this->row [1] = this->row [2] = Vec3T<T> { 0.0 };
	this->setDiagonal ( T ( 1.0 ) );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Col�l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setColValues ( const Vec3T<T> &col0, const Vec3T<T> &col1, const Vec3T<T> &col2 ) {
	this->row [0] = Vec3T<T> { col0.x, col1.x, col2.x };
	this->row [1] = Vec3T<T> { col0.y, col1.y, col2.y };
	this->row [2] = Vec3T<T> { col0.z, col1.z, col2.z };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �}�g���b�N�X�̑Ίp������S�ē����l�ɐݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setDiagonal ( T val ) {
	this->row[0].x = this->row[1].y = this->row[2].z = val;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      �}�g���b�N�X�̑Ίp������ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setDiagonal ( const Vec3T<T> &val ) {
	this->row [0].x = val.x;
	this->row [1].y = val.y;
	this->row [2].z = val.z;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      �R���̉�]������ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setFrom3Axis ( const Vec3T<T> &xAxis, const Vec3T<T> &yAxis, const Vec3T<T> &zAxis ) {
	Vec3 ax = xAxis.normal ();
	Vec3 ay = yAxis.normal ();
	Vec3 az = zAxis.normal ();
	this->setColValues ( ax, ay, az );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �����ƃA�b�v�x�N�g�������]��ݒ肷�� (type=0�̏ꍇ�� direction=Z, upV=Y)
//!		@param[in] type 0=ZY, 1=ZX, 2=XZ, 3=XY, 4=YX, 5=YZ
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setFromDirectionAndUpvector ( const Vec3T<T> &direction, const Vec3T<T> &upvector, u8 type, Bool dirNeg, Bool upVectorNeg ) {
	Vec3T<T> zAxis, yAxis, xAxis;
	u8 axis_ = type <= 5 ? type : 0;
	switch (axis_) {
	case 0:
		zAxis = dirNeg ? direction.normal ().negate () : direction.normal ();
		yAxis = zAxis.cross ( upvector.normal () ).cross ( zAxis ).normal ();
		yAxis = upVectorNeg ? yAxis.negate () : yAxis;
		xAxis = yAxis.cross ( zAxis ).normal ();
		break;
	case 1:
		zAxis = dirNeg ? direction.normal ().negate () : direction.normal ();
		xAxis = zAxis.cross ( upvector.normal () ).cross ( zAxis ).normal ();
		xAxis = upVectorNeg ? xAxis.negate () : xAxis;
		yAxis = xAxis.cross ( zAxis ).normal ().negate ();
		break;
	case 2:
		xAxis = dirNeg ? direction.normal ().negate () : direction.normal ();
		zAxis = xAxis.cross ( upvector.normal () ).cross ( xAxis ).normal ();
		zAxis = upVectorNeg ? zAxis.negate () : zAxis;
		yAxis = zAxis.cross ( xAxis ).normal ();
		break;
	case 3:
		xAxis = dirNeg ? direction.normal ().negate () : direction.normal ();
		yAxis = xAxis.cross ( upvector.normal () ).cross ( xAxis ).normal ();
		yAxis = upVectorNeg ? yAxis.negate () : yAxis;
		zAxis = yAxis.cross ( xAxis ).normal ().negate ();
		break;
	case 4:
		yAxis = dirNeg ? direction.normal ().negate () : direction.normal ();
		xAxis = yAxis.cross ( upvector.normal () ).cross ( yAxis ).normal ();
		xAxis = upVectorNeg ? xAxis.negate () : xAxis;
		zAxis = xAxis.cross ( yAxis ).normal ();
		break;
	case 5:
		yAxis = dirNeg ? direction.normal ().negate () : direction.normal ();
		zAxis = yAxis.cross ( upvector.normal () ).cross ( yAxis ).normal ();
		zAxis = upVectorNeg ? zAxis.negate () : zAxis;
		xAxis = zAxis.cross ( yAxis ).normal ().negate ();
		break;
	}
	this->setColValues ( xAxis, yAxis, zAxis );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      ��]��X�l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setXaxis ( const Vec3T<T> &axis ) {
	Vec3 a = axis.normal ();
	this->row [0] [0] = a.x;
	this->row [1] [0] = a.y;
	this->row [2] [0] = a.z;
};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      ��]��Y�l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setYaxis ( const Vec3T<T> &axis ) {
	Vec3 a = axis.normal ();
	this->row [0] [1] = a.x;
	this->row [1] [1] = a.y;
	this->row [2] [1] = a.z;
};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      ��]��Z�l��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setZaxis ( const Vec3T<T> &axis ) {
	Vec3 a = axis.normal ();
	this->row [0] [2] = a.x;
	this->row [1] [2] = a.y;
	this->row [2] [2] = a.z;
};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      �X�P�[����ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setScaling(const Vec3T<T>& val) {
	Vec3 col0 = this->getCol(0);
	Vec3 col1 = this->getCol(1);
	Vec3 col2 = this->getCol(2);
	
	T len;
	len = col0.length();
	col0 *= (val.x / len);

	len = col1.length();
	col1 *= (val.y / len);

	len = col2.length();
	col2 *= (val.z / len);

	this->setColValues(col0, col1, col2);
};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ����ύX����
//!	    @brief ����ύX����
//!		@param[in] zyTo ZY�����ǂ̎��ɕύX���邩 0=ZY, 1=ZX, 2=XZ, 3=XY, 4=YX, 5=YZ�i0�̏ꍇ�͕ύX�Ȃ��A3�̏ꍇ��Z��X,Y��X�֕ύX�j
//!		@param[in] dirNeg 1�Ԗڂ̎��𔽓]����i1�Ԗځ�ZY��Z�j
//!		@param[in] upvNeg 2�Ԗڂ̎��𔽓]����i2�Ԗځ�ZY��Y�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::axisChange ( u8 zyTo, Bool dirNeg, Bool upvNeg ) {
	Vec3T<T> xA = Vec3 ( this->row[0].x, this->row[1].x, this->row[2].x );
	Vec3T<T> yA = Vec3 ( this->row[0].y, this->row[1].y, this->row[2].y );
	Vec3T<T> zA = Vec3 ( this->row[0].z, this->row[1].z, this->row[2].z );

	if ( dirNeg == true && upvNeg == false ) {
		zA *= -1.0;
		xA *= -1.0;
	}
	if ( dirNeg == false && upvNeg == true ) {
		xA *= -1.0;
		yA *= -1.0;
	}
	if ( dirNeg == true && upvNeg == true ) {
		zA *= -1.0;
		yA *= -1.0;
	}

	switch ( zyTo ) {
		case 0: this->setColValues ( xA, yA, zA );			break; //ZY
		case 1: this->setColValues ( yA, xA * -1.0, zA );	break; //ZX
		case 2: this->setColValues ( zA, xA, yA );			break; //XZ
		case 3: this->setColValues ( zA, yA, xA * -1.0 );	break; //XY
		case 4: this->setColValues ( yA, zA, xA );			break; //YX
		case 5: this->setColValues ( xA * -1.0, zA, yA );	break; //YZ
		default: break;
	}
};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      ���W�̒l��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Mat3T<T>::get ( u8 row, u8 col ) const {
	assert ( row < 3 && col < 3 );     
	return ( *this ) [row] [col];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      Row�̒l��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> Mat3T<T>::getRow ( u8 rowIndex ) const {
	assert ( rowIndex < 3 );
	return ( *this ) [rowIndex];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      Col�̒l��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> Mat3T<T>::getCol ( u8 colIndex ) const {
	assert ( colIndex < 3 );
	return Vec3T<T> {this->row [0] [colIndex], this->row [1] [colIndex], this->row [2] [colIndex]};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      ��]��X������Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> Mat3T<T>::getXaxis () const {
	return Vec3T<T> {this->row [0] [0], this->row [1] [0], this->row [2] [0]};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      ��]��Y������Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> Mat3T<T>::getYaxis () const {
	return Vec3T<T> {this->row [0] [1], this->row [1] [1], this->row [2] [1]};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      ��]��Z������Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> Mat3T<T>::getZaxis () const {
	return Vec3T<T> {this->row [0] [2], this->row [1] [2], this->row [2] [2]};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      ��]�l�̂݁i�X�P�[���l���Ȃ����j��Mat3���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T> Mat3T<T>::getRotationMat3()const {
	///  Matrix44GetInverseScale_C
	Vec3T<T> scalVec3 = this->getScaling();
	assert(Math::IsZero(scalVec3.x) == Math::IsZero(scalVec3.y) == Math::IsZero(scalVec3.z) == false);
	Vec3T<T> invScale = scalVec3.inverse();

	/// Matrix44GetRotationMatrix_C
	Vec3T<T> row0{ (*this)[0][0] * invScale.x, (*this)[0][1] * invScale.y, (*this)[0][2] * invScale.z };
	Vec3T<T> row1{ (*this)[1][0] * invScale.x, (*this)[1][1] * invScale.y, (*this)[1][2] * invScale.z };
	Vec3T<T> row2{ (*this)[2][0] * invScale.x, (*this)[2][1] * invScale.y, (*this)[2][2] * invScale.z };
	return Mat3T<T> { row0, row1, row2 };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      �X�P�[���l���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> Mat3T<T>::getScaling()const {
	Vec3T<T> result;
	result.x = std::sqrtf((SQ((*this)[0][0]) + SQ((*this)[1][0]) + SQ((*this)[2][0])));
	result.y = std::sqrtf((SQ((*this)[0][1]) + SQ((*this)[1][1]) + SQ((*this)[2][1])));
	result.z = std::sqrtf((SQ((*this)[0][2]) + SQ((*this)[1][2]) + SQ((*this)[2][2])));

	if (this->determinant() < T(0.0)) {
		result.setNegate();
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Vec�ɕϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> Mat3T<T>::toVec () const{
	VecT<T> result;
	for ( u8 i = 0; i<3; i++ ) {
		result.push_back ( this->row [i].x );
		result.push_back ( this->row [i].y );
		result.push_back ( this->row [i].z );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Type�̔z��ɕϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> Mat3T<T>::toTypeArray () const {
	std::vector<T> result;
	for ( u8 i = 0; i<3; i++ ) {
		result.push_back ( this->row [i].x );
		result.push_back ( this->row [i].y );
		result.push_back ( this->row [i].z );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �f�o�b�O�p�̕�����ɂ���@"�f�[�^�^�F[ �l ]"
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
String Mat3T<T>::toString ( const String title ) const {
	String t = title;
	if ( title.length () != 0 ) { t += " : "; }
	String row0 = "row[0] [" + GetFloorString ( ( *this )[0][0] ) + ", " + GetFloorString ( ( *this )[0][1] ) + ", " + GetFloorString ( ( *this )[0][2] ) + "] / ";
	String row1 = "row[1] [" + GetFloorString ( ( *this )[1][0] ) + ", " + GetFloorString ( ( *this )[1][1] ) + ", " + GetFloorString ( ( *this )[1][2] ) + "] / ";
	String row2 = "row[2] [" + GetFloorString ( ( *this )[2][0] ) + ", " + GetFloorString ( ( *this )[2][1] ) + ", " + GetFloorString ( ( *this )[2][2] ) + "]";
	return t + "Mat3:[" + row0 + row1 + row2 + "]";
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̃}�g���b�N�X�̒l���S�������ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Mat3T<T>::equal ( const Mat3T &other ) const {
	return 
		( *this )[0].equal ( other[0] ) && 
		( *this )[1].equal ( other[1] ) && 
		( *this )[2].equal ( other[2] );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̃}�g���b�N�X�̒l�����e�͈͓��œ����ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Mat3T<T>::almostEqual ( const Mat3T &other, T precision ) const{
	return 
		( *this )[0].almostEqual ( other[0], precision ) && 
		( *this )[1].almostEqual ( other[1], precision ) && 
		( *this )[2].almostEqual ( other[2], precision );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ���s�}�g���b�N�X�̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Mat3T<T>::testOrthogonal () const{
	Vec3T<T> col0 = this->getCol ( 0 );
	Vec3T<T> col1 = this->getCol ( 1 );
	Vec3T<T> col2 = this->getCol ( 2 );
	return 
		( *this)[0].almostEqual ( col0 ) &&
		( *this)[1].almostEqual ( col1 ) &&
		( *this)[2].almostEqual ( col2 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �s�񎮂�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Mat3T<T>::determinant () const {
	return
		( *this )[0][0] * ( *this )[1][1] * ( *this )[2][2] + ( *this )[0][1] * ( *this )[1][2] * ( *this )[2][0] + ( *this )[0][2] * ( *this )[1][0] * ( *this )[2][1]
		- ( *this )[0][0] * ( *this )[1][2] * ( *this )[2][1] - ( *this )[0][1] * ( *this )[1][0] * ( *this )[2][2] - ( *this )[0][2] * ( *this )[1][1] * ( *this )[2][0];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �]�u�i�Ίp���Ő�����܂�Ԃ����j�}�g���b�N�X�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setTranspose () {
	*this = this->transpose ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �]�u�i�Ίp���Ő�����܂�Ԃ����j�}�g���b�N�X��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T> Mat3T<T>::transpose () const{
	return Mat3T<T> {
		( *this ) [0] [0], ( *this ) [1] [0], ( *this ) [2] [0],
		( *this ) [0] [1], ( *this ) [1] [1], ( *this ) [2] [1],
		( *this ) [0] [2], ( *this ) [1] [2], ( *this ) [2] [2] 
	};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �t���}�g���b�N�X��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T> Mat3T<T>::inverse () const {
	T det = this->determinant ();
	assert ( Math::IsZero ( det ) == false );

	Mat3T<T> rest {
		( *this ) [1] [1] * ( *this ) [2] [2] - ( *this ) [1] [2] * ( *this ) [2] [1],
		( *this ) [0] [2] * ( *this ) [2] [1] - ( *this ) [0] [1] * ( *this ) [2] [2],
		( *this ) [0] [1] * ( *this ) [1] [2] - ( *this ) [0] [2] * ( *this ) [1] [1],
		( *this ) [1] [2] * ( *this ) [2] [0] - ( *this ) [1] [0] * ( *this ) [2] [2],
		( *this ) [0] [0] * ( *this ) [2] [2] - ( *this ) [0] [2] * ( *this ) [2] [0],
		( *this ) [0] [2] * ( *this ) [1] [0] - ( *this ) [0] [0] * ( *this ) [1] [2],
		( *this ) [1] [0] * ( *this ) [2] [1] - ( *this ) [1] [1] * ( *this ) [2] [0],
		( *this ) [0] [1] * ( *this ) [2] [0] - ( *this ) [0] [0] * ( *this ) [2] [1],
		( *this ) [0] [0] * ( *this ) [1] [1] - ( *this ) [0] [1] * ( *this ) [1] [0] 
	};
	return rest / det;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �t���}�g���b�N�X�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::setInverse () {
	*this = this->inverse();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     .x,y,z��[0~2]�Ƃ��ăA�N�Z�X����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T>& Mat3T<T>::operator[]( u8 id ) {
	assert ( id < 3 );
	return this->row [id];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const Vec3T<T>& Mat3T<T>::operator[]( u8 id ) const {
	assert ( id < 3 );
	return this->row [id];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Mat3T<T>::operator == ( const Mat3T &other ) const {
	return this->equal ( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Mat3T<T>::operator != ( const Mat3T &other ) const {
	return !this->equal ( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T> Mat3T<T>::operator + ( const Mat3T &other ) const {
	return Mat3T<T> {
		( *this )[0] + other[0],
		( *this )[1] + other[1],
		( *this )[2] + other[2]
	};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::operator += ( const Mat3T &other ) {
	*this = *this + other;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T> Mat3T<T>::operator - ( const Mat3T &other ) const {
	return Mat3T<T> {
		( *this ) [0] - other [0],
		( *this ) [1] - other [1],
		( *this ) [2] - other [2] 
	};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::operator -= ( const Mat3T &other ) {
	*this = *this - other;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T> Mat3T<T>::operator * ( const Mat3T &other ) const {
	return Mat3T<T> {
		( *this ) [0] [0] * other [0] [0] + ( *this ) [0] [1] * other [1] [0] + ( *this ) [0] [2] * other [2] [0],
		( *this ) [0] [0] * other [0] [1] + ( *this ) [0] [1] * other [1] [1] + ( *this ) [0] [2] * other [2] [1],
		( *this ) [0] [0] * other [0] [2] + ( *this ) [0] [1] * other [1] [2] + ( *this ) [0] [2] * other [2] [2],
		( *this ) [1] [0] * other [0] [0] + ( *this ) [1] [1] * other [1] [0] + ( *this ) [1] [2] * other [2] [0],
		( *this ) [1] [0] * other [0] [1] + ( *this ) [1] [1] * other [1] [1] + ( *this ) [1] [2] * other [2] [1],
		( *this ) [1] [0] * other [0] [2] + ( *this ) [1] [1] * other [1] [2] + ( *this ) [1] [2] * other [2] [2],
		( *this ) [2] [0] * other [0] [0] + ( *this ) [2] [1] * other [1] [0] + ( *this ) [2] [2] * other [2] [0],
		( *this ) [2] [0] * other [0] [1] + ( *this ) [2] [1] * other [1] [1] + ( *this ) [2] [2] * other [2] [1],
		( *this ) [2] [0] * other [0] [2] + ( *this ) [2] [1] * other [1] [2] + ( *this ) [2] [2] * other [2] [2]
	};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::operator *= ( const Mat3T &other ) {
	*this = *this * other;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> Mat3T<T>::operator * ( const Vec3T<T> &vec3 ) const {
	return Vec3T<T> {
		( *this ) [0] [0] * vec3 [0] + ( *this ) [0] [1] * vec3 [1] + ( *this ) [0] [2] * vec3 [2],
		( *this ) [1] [0] * vec3 [0] + ( *this ) [1] [1] * vec3 [1] + ( *this ) [1] [2] * vec3 [2],
		( *this ) [2] [0] * vec3 [0] + ( *this ) [2] [1] * vec3 [1] + ( *this ) [2] [2] * vec3 [2]
	};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T> Mat3T<T>::operator * ( T val ) const {
	return Mat3T { ( *this ) [0] * val, ( *this ) [1] * val, ( *this ) [2] * val };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::operator *= ( T val ) {
	*this = *this * val;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T> Mat3T<T>::operator / ( T val ) const {
	assert ( Math::IsZero ( val ) == false );
	return *this * ( T(1.0) / val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Mat3T<T>::operator /= ( T val ) {
	*this = *this / val;
}

