//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T>::Vec2T() : Eigen::Vector<T, 2>{ Eigen::Vector<T, 2>::Zero( 2 ) }
{}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T>::Vec2T( T val ) : Eigen::Vector<T, 2>( Eigen::Vector<T, 2>::Constant( val ) ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T>::Vec2T( T _x, T _y ) : Eigen::Vector<T, 2>( _x, _y ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T>::Vec2T( const VecT<T>& val ) : Eigen::Vector<T, 2>() {
	if ( val.size() < 2 ){
		*this = Eigen::Vector<T, 2>::Zero( 2 );
	} else{
		*this << val[0], val[1];
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T>::Vec2T( const std::vector<T>& val ) : Eigen::Vector<T, 2>(){
	if ( val.size() < 2 ){
		*this = Eigen::Vector<T, 2>::Zero( 2 );
	} else{
		*this << val[0], val[1];
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T>::Vec2T( const Eigen::Vector<T, 2>& other ) : Eigen::Vector<T, 2>( other ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     [0]�ւ̃A�N�Z�X
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T& Vec2T<T>::x(){ return this->Eigen::Vector<T, 2>::x(); }

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     [0]�ւ̃A�N�Z�X
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& Vec2T<T>::x()const{ return this->Eigen::Vector<T, 2>::x(); }

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     [1]�ւ̃A�N�Z�X
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T& Vec2T<T>::y(){ return this->Eigen::Vector<T, 2>::y(); }

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     [1]�ւ̃A�N�Z�X
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& Vec2T<T>::y()const{ return this->Eigen::Vector<T, 2>::y(); }

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Vec�֕ϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> Vec2T<T>::toVec() const{
	return VecT<T> { std::vector<T>{this->x(), this->y()} };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Type�̔z��֕ϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> Vec2T<T>::toTypeArray() const{
	return std::vector<T>{this->x(), this->y()};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �f�o�b�O�p�̕�����ɂ���@"�f�[�^�^�F[ �l ]"
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
String Vec2T<T>::toString( const String title ) const{
	String t = title;
	if ( title.length() != 0 ){ t += " : "; }
	return t + "Vec2:[" + GetFloorString( this->x() ) + ", " + GetFloorString( this->y() ) + "]";
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̃x�N�g���̒l���S�������ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::equal( const Vec2T& other ) const{
	return ( this->x() == other.x() ) && ( this->y() == other.y() );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̃x�N�g���̒l���덷�͈͓��œ����ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::almostEqual( const Vec2T& other, T tolerance ) const{
	return	( std::abs( this->x() - other.x() ) < tolerance ) &&
			( std::abs( this->y() - other.y() ) < tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �[���x�N�g���̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::isZero() const{
	return this->x() == T( 0.0 ) && this->y() == T( 0.0 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �l���덷�͈͓��Ń[���x�N�g���̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::almostZero( T tolerance ) const{
	return this->almostEqual( Vec2T<T>( T( 0.0 ) ), tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    2�̃x�N�g�������S�ɕ��s�̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::parallel( const Vec2T& other ) const{
	T dot{ 0.0 };
	if ( this->isUnit() && other.isUnit() ){
		dot = this->dot( other );
	} else{
		dot = this->normal().dot( other.normal() );
	}

	return dot == T( -1.0 ) || dot == T( 1.0 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̃x�N�g�����덷�͈͓��ŕ��s�̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::almostParallel( const Vec2T& other, T tolerance ) const{
	T dot{ 0.0 };
	if ( this->isUnit() && other.isUnit() ){
		dot = this->dot( other );
	} else{
		dot = this->normal().dot( other.normal() );
	}
	return Burgess2MathFn::AlmostEqual( dot, T( -1.0 ), tolerance ) || Burgess2MathFn::AlmostEqual( dot, T( 1.0 ), tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̃x�N�g�������S�ɓ������̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::sameDirection( const Vec2T& other ) const{
	if ( this->isZero() || other.isZero() ){ return false; }

	if ( this->equal( other ) ){ return true; }

	if ( this->isUnit() && other.isUnit() ){
		return this->angleTo( other ) == T( 0.0 );
	} else{
		return this->unit().angleTo( other.unit() ) == T( 0.0 );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̃x�N�g�����덷�͈͓��œ������̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::almostSameDirection( const Vec2T& other, T tolerance ) const{
	if ( this->almostZero( tolerance ) || other.almostZero( tolerance ) ){ return false; }

	if ( this->almostEqual( other, tolerance ) ){ return true; }

	if ( this->almostUnit( tolerance ) && other.almostUnit( tolerance ) ){
		return this->angleTo( other ) < tolerance;
	} else{
		return this->unit().angleTo( other.unit() ) < tolerance;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �P�ʃx�N�g���̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::isUnit() const{
	return this->lengthSquared() == T( 1.0 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �덷�͈͓��ŒP�ʃx�N�g���̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::almostUnit( T tolerance ) const{
	return Burgess2MathFn::AlmostEqual( this->lengthSquared(), T( 1.0 ), tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		.x .y .z���Ƃɍŏ��l�ō\�������x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::getMin( const Vec2T& other ) const{
	return Vec2T{ Burgess2MathFn::Min( this->x(), other.x() ), Burgess2MathFn::Min( this->y(), other.y() ) };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		.x .y .z���Ƃɍő�l�ō\�������x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::getMax( const Vec2T& other ) const{
	return Vec2T{ Burgess2MathFn::Max( this->x(), other.x() ), Burgess2MathFn::Max( this->y(), other.y() ) };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     .x .y .z���Ƃ�limit�͈̔͂ɐ��������x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::clamp( const Vec2T& limitVal0, const Vec2T& limitVal1 ) const{
	Vec2T min = limitVal0.getMin( limitVal1 );
	Vec2T max = limitVal0.getMax( limitVal1 );
	return	Vec2T<T> { Burgess2MathFn::Clamp( this->x(), min.x(), max.x() ),
						Burgess2MathFn::Clamp( this->y(), min.y(), max.y() ) };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     t�l(0.0~1.0)�Ɋ�Â��đΏۃx�N�g���Ɛ��`��Ԃ����x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::lerp( const Vec2T& other, T t ) const{
	return *this + ( ( other - *this ) * t );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     t�l(0.0~1.0)�Ɋ�Â��đΏۃx�N�g���Ƌ��ʕ�Ԃ����x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::slerp( const Vec2T& other, T t ) const{
	if ( this->almostEqual( other ) ){ return *this; }

	Vec2T<T> v = Burgess2MathFn::AlmostEqual( this->lengthSquared(), T( 1.0 ) ) ? *this : this->unit();
	Vec2T<T> o = Burgess2MathFn::AlmostEqual( other.lengthSquared(), T( 1.0 ) ) ? other : other.unit();
	T d = v.dot( o );
	if ( Burgess2MathFn::AlmostEqual( d, T( 1.0 ) ) ){ return *this; }

	T angle = std::acos( d );
	T sinTh = std::sin( angle );

	T s = std::sin( angle * ( T( 1.0 ) - t ) );
	T e = std::sin( angle * t );
	Vec2T<T> r = ( ( s * v ) + ( e * o ) ) / sinTh;
	return r.unit();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     t�l(0.0~1.0)�Ɋ�Â��đΏۃx�N�g���Ɛ��`��Ԃ����x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::linearInterpolate( const Vec2T& other, T t ) const{
	return this->lerp( other, t );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     t�l(0.0~1.0)�Ɋ�Â��đΏۃx�N�g���Ƌ��ʕ�Ԃ����x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::sphericalLinearInterpolate( const Vec2T& other, T t ) const{
	return this->slerp( other, t );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �x�N�g���̒�����2���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Vec2T<T>::lengthSquared() const{
	return this->Eigen::Vector<T, 2>::squaredNorm();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �x�N�g���̒�����Ԃ� (��r�̂��߂ɒ������擾����̂ł����.lengthSquared()�̒l��p��������y��)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Vec2T<T>::length() const{
	return this->Eigen::Vector<T, 2>::norm();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �x�N�g���̒�����2���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Vec2T<T>::magnitudeSquared() const{
	return this->lengthSquared();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �x�N�g���̒�����Ԃ� (��r�̂��߂ɒ������擾����̂ł����.magnitudeSquared()�̒l��p��������y��)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Vec2T<T>::magnitude() const{
	return this->length();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �P�ʃx�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::normal() const{
	if ( this->isZero() ){ return *this; }

	return this->Eigen::Vector<T, 2>::normalized();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �P�ʃx�N�g���ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::setNormal(){
	if ( this->isZero() ){ return; }

	this->Eigen::Vector<T, 2>::normalize();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �P�ʃx�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::unit() const{
	return this->normal();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �P�ʃx�N�g���ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::setUnit(){
	this->setNormal();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ���]�x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::negate() const{
	return Vec2T{ -this->x(), -this->y() };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ���]�x�N�g���ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::setNegate(){
	*this = this->negate();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �t���x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::inverse() const{
	if ( this->isZero() ){ return Vec2T<T>(); }
	T a = T( 1.0 ) / this->y();
	T b = a;
	return Vec2T{ T( 1.0 ) / this->x(), T( 1.0 ) / this->y() };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �t���x�N�g���ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::setInverse(){
	*this = this->inverse();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���˃x�N�g����Ԃ�
//!	    @brief	���˃x�N�g����Ԃ�
//!		@param[in] normal ���˖ʂ̖@�� (�P�ʃx�N�g��)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::reflect( const Vec2T& normal ) const{
	return *this - normal * T( 2.0 ) * this->dot( normal );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���˃x�N�g���ɂ���
//!	    @brief	���˃x�N�g����Ԃ�
//!		@param[in] normal ���˖ʂ̖@�� (�P�ʃx�N�g��)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::setReflect( const Vec2T& normal ){
	*this = this->reflect( normal );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ���ς�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Vec2T<T>::dot( const Vec2T& other ) const{
	return this->Eigen::Vector<T, 2>::dot( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �O�ς�Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::cross( const Vec2T& other ) const{
	return this->Eigen::Vector<T, 2>::cross( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̈ʒu�x�N�g���̋�����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Vec2T<T>::distanceTo( const Vec2T& other ) const{
	if ( this->almostEqual( other ) ){ return T( 0.0 ); }

	Vec2T<T> d = *this - other;
	return d.length();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̈ʒu�x�N�g���̋������w�肵���l�����̏ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::distanceLessThan( const Vec2T& other, T dist ) const{
	Vec2T<T> d = *this - other;
	return d.length() < dist;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2�̃x�N�g���̊Ԃ̊p�x(���W�A���P��)��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Vec2T<T>::angleTo( const Vec2T& other ) const{
	T dot{ 0.0 };
	if ( this->isUnit() && other.isUnit() ){
		dot = Burgess2MathFn::Clamp( this->dot( other ), T( -1.0 ), T( 1.0 ) );
	} else{
		dot = Burgess2MathFn::Clamp( this->unit().dot( other.unit() ), T( -1.0 ), T( 1.0 ) );
	}

	if ( dot == T( 1.0 ) ){
		return T( 0.0 );
	} else{
		return std::acos( dot );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T>& Vec2T<T>::operator = ( const Eigen::Vector<T, 2>& other ){
	this->Eigen::Vector<T, 2>::operator=( other );
	return *this;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T& Vec2T<T>::operator[]( u8 id ){
	return this->Eigen::Vector<T, 2>::operator[]( id );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& Vec2T<T>::operator[]( u8 id )const{
	return this->Eigen::Vector<T, 2>::operator[]( id );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::operator == ( const Vec2T& other ) const{
	return this->Eigen::Vector<T, 2>::operator==( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool Vec2T<T>::operator != ( const Vec2T& other ) const{
	return this->Eigen::Vector<T, 2>::operator!=( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::operator+( const Vec2T& other )const{
	return this->Eigen::Vector<T, 2>::operator+( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::operator+=( const Vec2T& other ){
	this->Eigen::Vector<T, 2>::operator+=( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::operator-( const Vec2T& other )const{
	return this->Eigen::Vector<T, 2>::operator-( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::operator-=( const Vec2T& other ){
	this->Eigen::Vector<T, 2>::operator-=( other );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::operator*( const T val ) const{
	return this->Eigen::Vector<T, 2>::operator*( val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::operator*( const Vec2T& other ) const{
	return Vec2T{ this->x() * other.x(), this->y() * other.y() };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::operator*=( const T val ){
	this->Eigen::Vector<T, 2>::operator*=( val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::operator*=( const Vec2T& other ){
	*this = *this * other;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::operator/( const T val ) const{
	return this->Eigen::Vector<T, 2>::operator/( val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec2T<T> Vec2T<T>::operator/( const Vec2T& other ) const{
	return Vec2T<T> { this->x() / other.x(), this->y() / other.y() };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::operator/=( const T val ){
	this->Eigen::Vector<T, 2>::operator/=( val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Vec2T<T>::operator/=( const Vec2T& other ){
	*this = *this / other;
}