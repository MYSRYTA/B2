//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T() : Eigen::Vector<T, 3> { Eigen::Vector<T, 3>::Zero( 3 ) }
{}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T(T val, BURGESS2::RotationOrder order ) 
	: Eigen::Vector<T, 3>{ Eigen::Vector<T, 3>::Constant( val ) }
	, rotationOrder{ order }
{}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T( T _x, T _y, T _z, BURGESS2::RotationOrder order ) 
	: Eigen::Vector<T, 3>{ _x, _y, _z }
	, rotationOrder{ order }
{}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T( const std::vector<T>& arrayVal, BURGESS2::RotationOrder order ) {
	if (arrayVal.size() < 3) { 
		*this = Eigen::Vector<T, 3>::Zero( 3 ); 
	} else{
		*this = { arrayVal[0], arrayVal[1], arrayVal[2], order };
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T( const VecT<T>& vec, BURGESS2::RotationOrder order ) {
	if (vec.size() < 3) { 
		*this = Eigen::Vector<T, 3>::Zero( 3 ); 
	} else{
		*this = { vec[0], vec[1], vec[2], order };
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T( const Vec3T<T>& vec, BURGESS2::RotationOrder order )
	: Eigen::Vector<T, 3>( vec[0], vec[1], vec[2] )
	, rotationOrder{ order }
{}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T( const QuatT<T>& quat, BURGESS2::RotationOrder order ) {
	*this = quat.toRad3( order );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T( const Mat3T<T>& mat3, BURGESS2::RotationOrder order ) {
	this->setFromMat3( mat3.getRotationMat3(), order );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T( const Mat4T<T>& mat4, BURGESS2::RotationOrder order ) {
	this->setFromMat3( mat4.getRotationMat3(), order );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T( const Deg3T<T>& deg )
	: Eigen::Vector<T, 3>{ Burgess2MathFn::DegToRad( deg.x() ), Burgess2MathFn::DegToRad( deg.y() ), Burgess2MathFn::DegToRad( deg.z() ) }
	, rotationOrder {deg.getRotationOrder()}
{}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>::Rad3T( const Eigen::Vector<T, 3>& other ){
	this->Eigen::Vector<T, 3>::operator=( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T& Rad3T<T>::x() {
	return this->Eigen::Vector<T, 3>::x();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& Rad3T<T>::x()const {
	return this->Eigen::Vector<T, 3>::x();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T& Rad3T<T>::y() {
	return this->Eigen::Vector<T, 3>::y();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& Rad3T<T>::y()const {
	return this->Eigen::Vector<T, 3>::y();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T& Rad3T<T>::z() {
	return this->Eigen::Vector<T, 3>::z();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& Rad3T<T>::z()const {
	return this->Eigen::Vector<T, 3>::z();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Vec�֕ϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> Rad3T<T>::toVec()const {
	std::vector<T> v{ this->x(), this->y(), this->z() };
	return VecT<T> { v };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Type�̔z��֕ϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> Rad3T<T>::toTypeArray() const {
	return std::vector<T> { this->x(), this->y(), this->z() };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Deg3�֕ϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Deg3T<T> Rad3T<T>::toDeg3() const {
	return Deg3T<T> { Burgess2MathFn::RadToDeg( this->x() ), Burgess2MathFn::RadToDeg( this->y() ), Burgess2MathFn::RadToDeg( this->z() ), this->getRotationOrder() };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Quat�֕ϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
QuatT<T> Rad3T<T>::toQuat() const {
	return QuatT<T> { *this };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Mat3�֕ϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat3T<T> Rad3T<T>::toMat3() const {
	return QuatT<T> { *this }.toMat3();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �f�o�b�O�p�̕�����ɂ���@"�f�[�^�^�F[ �l ]"
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
String Rad3T<T>::toString( const String title ) const {
	String t = title;
	if (title.length() != 0) { t += " : "; }
	String order = "";
	switch (static_cast<u8>(this->rotationOrder)) {
		case 0://XYZ:
			order += "[XYZ]";
			break;
		case 1://YZX:
			order += "[YZX]";
			break;
		case 2://ZXY:
			order += "[ZXY]";
			break;
		case 3://XZY:
			order += "[XZY]";
			break;
		case 4://YXZ:
			order += "[YXZ]";
			break;
		case 5://ZYX:
			order += "[ZYX]";
			break;
	}
	return t + "Rad3:[" + 
		GetFloorString( this->x() ) + ", " + 
		GetFloorString( this->y() ) + ", " + 
		GetFloorString( this->z() ) + ", rotationOrder : " + order + "]";
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̃��W�A���P�ʃI�C���[�̒l���S�������ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Rad3T<T>::equal( const Rad3T& other ) const {
	return this->Eigen::Vector<T, 3>::operator==( other ) && this->rotationOrder == other.rotationOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̃��W�A���P�ʃI�C���[�̒l���덷�͈͓��œ����ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Rad3T<T>::almostEqual( const Rad3T& other, T tolerance ) const {
	return
		Burgess2MathFn::AlmostEqual( this->x(), other.x(), tolerance ) &&
		Burgess2MathFn::AlmostEqual( this->y(), other.y(), tolerance ) &&
		Burgess2MathFn::AlmostEqual( this->z(), other.z(), tolerance ) &&
		this->rotationOrder == other.rotationOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Mat3����Rad3��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Rad3T<T>::setFromMat3( const Mat3T<T>& mat3, BURGESS2::RotationOrder order ) {
	*this = QuatT<T>( mat3 ).toRad3( order );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Quat����Rad3��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Rad3T<T>::setFromQuat( const QuatT<T>& quat, BURGESS2::RotationOrder order ) {
	*this = quat.toRad3( order );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Deg3����Rad3��ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Rad3T<T>::setFromDeg3( const Deg3T<T>& deg3 ) {
	this->x() = Burgess2MathFn::DegToRad( deg3.x() );
	this->y() = Burgess2MathFn::DegToRad( deg3.y() );
	this->z() = Burgess2MathFn::DegToRad( deg3.z() );
	this->rotationOrder = deg3.getRotationOrder();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ���[�e�[�V�����I�[�_�[���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BURGESS2::RotationOrder Rad3T<T>::getRotationOrder() const {
	return this->rotationOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ���[�e�[�V�����I�[�_�[��u8�Ŏ擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u8 Rad3T<T>::getRotationOrderU8() const{
	return static_cast<u8>( this->rotationOrder );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �x�N�g�������̃��W�A���ŉ�]�����x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> Rad3T<T>::rotatVector( const Vec3T<T>& vec3 ) const {
	return QuatT<T>{*this}.rotatVector( vec3 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T>& Rad3T<T>::operator = ( const Eigen::Vector<T, 3>& other ) {
	this->Eigen::Vector<T, 3>::operator=( other );
	return *this;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T& Rad3T<T>::operator[]( u8 id ) {
	return this->Eigen::Vector<T, 3>::operator[]( id );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& Rad3T<T>::operator[]( u8 id )const {
	return this->Eigen::Vector<T, 3>::operator[]( id );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Rad3T<T>::operator == ( const Rad3T& other ) const {
	return this->Eigen::Vector<T, 3>::operator==( other ) && this->rotationOrder == other.rotationOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Rad3T<T>::operator != ( const Rad3T& other ) const {
	return this->Eigen::Vector<T, 3>::operator!=( other ) || this->rotationOrder != other.rotationOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T> Rad3T<T>::operator+( const Rad3T& other )const {
	return Rad3T<T>{this->Eigen::Vector<T, 3>::operator+( other )};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Rad3T<T>::operator+=( const Rad3T& other ) {
	this->Eigen::Vector<T, 3>::operator+=( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Rad3T<T> Rad3T<T>::operator-( const Rad3T& other )const {
	return Rad3T<T>{this->Eigen::Vector<T, 3>::operator-( other )};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Rad3T<T>::operator-=( const Rad3T& other ) {
	this->Eigen::Vector<T, 3>::operator-=( other );
}