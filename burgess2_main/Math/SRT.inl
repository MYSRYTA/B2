//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T>::SRTT( const Vec3T<T> & _tr )
	: sc( Vec3T<T>::Ones(3) ), rot(QuatT<T>::Identity()), tr( _tr ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T>::SRTT( const QuatT<T> & _rot, const BURGESS2::RotationOrder order )
	: sc( Vec3T<T>::Ones(3) ), rot( _rot.toDeg3(order) ), tr( Vec3T<T>::Zero(3) ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T>::SRTT( const Deg3T<T> & _rot )
	: sc( Vec3T<T>::Ones(3) ), rot( _rot ), tr( Vec3T<T>::Zero(3) ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T>::SRTT( const Rad3T<T> & _rot )
	: sc( Vec3T<T>::Ones(3) ), rot( _rot ), tr( Vec3T<T>::Zero(3) ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T>::SRTT( const Mat4T<T> & mat, const BURGESS2::RotationOrder order ){
	*this = mat.toSRT( order );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T>::SRTT( const Mat3T<T> & mat, const BURGESS2::RotationOrder order )
	: sc( mat.getScaling() ), rot( Deg3T<T>{mat.getRotationMat3(), order} ), tr( Vec3T<T>::Zero(3) ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T>::SRTT( const XfoT<T> & srt, const BURGESS2::RotationOrder order )
	: sc( srt.sc ), rot( Deg3T<T>{srt.ori, order } ), tr( srt.tr ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T>::SRTT( const Vec3T<T> & _sc, const QuatT<T> & _ori, const Vec3T<T> & _tr, BURGESS2::RotationOrder order )
	: sc( _sc ), rot( Deg3T<T>{_ori, order} ), tr( _tr ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T>::SRTT( const Vec3T<T> & _sc, const Deg3T<T> & _rot, const Vec3T<T> & _tr )
	: sc( _sc ), rot( _rot ), tr( _tr ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T>::SRTT( const Vec3T<T> & _sc, const Rad3T<T> & _rot, const Vec3T<T> & _tr )
	: sc( _sc ), rot( QuatT<T>{_rot} ), tr( _tr ){}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �����l�i sc= 1,1,1 ori=0,0,0,1 tr=0,0,0�j �ɂ��� 
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void SRTT<T>::setIdentity(){
	this->sc = Vec3T<T>::Ones(3);
	this->rot = Vec3T<T>::Zero(3);
	this->tr = Vec3T<T>::Zero(3);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Mat4�^�֕ϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat4T<T> SRTT<T>::toMat4() const{
	Mat4T<T> _sc;
	_sc.setDiagonal( Vec4T<T> { this->sc.x(), this->sc.y(), this->sc.z(), T( 1.0 ) } );

	Mat4T<T> _rot;
	_rot.setUpperLeft( this->rot.toMat3() );

	Mat4T<T> _tr;
	_tr.setTranslation( this->tr );

	return _tr * _rot * _sc;
}


//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Xfo�^�֕ϊ�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> SRTT<T>::toXfo() const{
	return XfoT<T>{ this->sc, QuatT<T>{this->rot}, this->tr };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �f�o�b�O�p�̕�����ɂ���@"�f�[�^�^�F[ �l ]"
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
String SRTT<T>::toString( const String title )const{
	String t = title;
	if(title.length() != 0) { t += " : "; }
	String s = "sc Vec3[" + GetFloorString( this->sc.x() ) + ", " + GetFloorString( this->sc.y() ) + ", " + GetFloorString( this->sc.z() ) + "]";
	String r = "rot Deg3[" + GetFloorString( this->rot.x() ) + ", " + GetFloorString( this->rot.y() ) + ", " + GetFloorString( this->rot.z() ) + "]";
	String tt = "tr Vec3[" + GetFloorString( this->tr.x() ) + ", " + GetFloorString( this->tr.y() ) + ", " + GetFloorString( this->tr.z() ) + "]";
	return t + "SRT:[" + s + " / " + r + " / " + tt + "]";
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2��S/R/T�̒l���S�������ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool SRTT<T>::equal( const SRTT<T> & other ) const{
	return this->sc == other.sc && this->rot == other.rot && this->tr == other.tr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     2��S/R/T�̒l���덷�͈͓��œ����ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool SRTT<T>::almostEqual( const SRTT<T> & other, T tolerance ) const{
	return	this->sc.almostEqual( other.sc, tolerance ) &&
			this->rot.almostEqual( other.rot, tolerance ) &&
			this->tr.almostEqual( other.tr, tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     t�l(0.0~1.0)�Ɋ�Â��đΏ�SRT�ƕ�ԁi��]�������ԁA�ʒu�ƃX�P�[�������`��ԁj����SRT��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T> SRTT<T>::slerp( const SRTT<T> & other, T t ) const{
	QuatT<T> q0{ this->rot };
	QuatT<T> q1{ other.rot };
	return XfoT<T>{this->sc.lerp( other.sc, t ), Deg3T<T>{q0.slerp( q1, t )}, this->tr.lerp( other.tr, t )};
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	      t�l(0.0~1.0)�Ɋ�Â��đΏ�SRT�ƕ�ԁi��]�������ԁA�ʒu�ƃX�P�[�������`��ԁj����SRT��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T> SRTT<T>::sphericalLinearInterpolate( const SRTT<T> & other, T t ) const{
	return this->slerp( other, t );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �e�v�f���t���ɂ���Xfo��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T> SRTT<T>::inverse() const{
	SRTT<T> result = *this;
	QuatT<T> q{ result.rot };
	q.setInverse();
	result.sc.setInverse();
	result.rot = Deg3T<T>{ q };
	result.tr = q.rotatVector( this->tr.negate() * result.sc );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �e�v�f���t���ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void SRTT<T>::setInverse(){
	*this = this->toMat4().inverse();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     SRT�Ńg�����X�t�H�[�������x�N�g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> SRTT<T>::transformVector( const Vec3T<T> & val ) const{
	return this->rot.rotatVector( val * this->sc ) + this->tr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool SRTT<T>::operator==( const SRTT<T> & other ) const{
	return this->sc == other.sc && this->rot == other.rot && this->tr == other.tr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool SRTT<T>::operator !=( const SRTT<T> & other ) const{
	return this->sc != other.sc || this->rot != other.rot || this->tr != other.tr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SRTT<T> SRTT<T>::operator*( const SRTT<T> & other ) const{
	QuatT<T> thisRot = QuatT<T>{this->rot}.unit();
	SRTT<T> result;
	result.tr = this->tr + thisRot.rotatVector( other.tr * this->sc );
	result.rot = Deg3T<T>{ ( thisRot * QuatT<T>{ other.rot }.unit() ).unit() };
	result.sc = this->sc * other.sc;
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void SRTT<T>::operator*=( const SRTT<T> & other ){
	*this = *this * other;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> SRTT<T>::operator*( const Vec3T<T> & vec ) const{
	return  this->transformVector( vec );
}