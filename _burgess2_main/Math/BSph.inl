//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �R���X�g���N�^
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BSphT<T>::BSphT( const Vec3T<T>& _position, T _radius )
	: position{ _position }
	, radius{ _radius }
{};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �f�o�b�O�p�̕�����ɂ���@"�f�[�^�^�F[ �l ]"
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
String BSphT<T>::toString( const String title ) const {
	String t = title;
	if (title.length() != 0) { t += " : "; }
	return t + "BSph:[position Vec3[" + 
		GetFloorString( this->position.x() ) + ", " + 
		GetFloorString( this->position.y() ) + ", "	+ 
		GetFloorString( this->position.z() ) + "] / radius=" + 
		GetFloorString( this->radius ) + "]";
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Ray�Ƃ̌����𔻒肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BSphT<T>::testRayIntersect( const Ray3T<T>& ray ) const {
	Vec3T<T> oc = this->position - ray.getStartPosition();
	T s = oc.dot( ray.getDirection() );
	Vec3T<T> pos = ray.getPositionOnRay( s );
	T disSQ = (this->position - pos).lengthSquared();
	T radiusSQ = SQ( this->radius );
	return disSQ <= radiusSQ || Burgess2MathFn::AlmostEqual( disSQ, radiusSQ ) ? true : false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �w�苗���ɃX�t�B�A�����݂��邩���肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BSphT<T>::testClosest( const Vec3T<T>& position, T maxDistance ) const {
	T d = (this->position - position).length();
	return (d - this->radius) <= maxDistance ? true : false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �ł��߂��ʒu���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BSphT<T>::getClosestPosition( const Vec3T<T>& position, T maxDistance ) const {
	T dist = this->position.distanceTo( position );
	if (dist - this->radius <= maxDistance) {
		return this->position.lerp( position, this->radius / dist );
	}
	else {
		return Vec3T<T> { T( BURGESS2::F32_INFINITE ) };
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BSphT<T>::operator == ( const BSphT& other ) const {
	return this->position == other.position && this->radius == other.radius;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BSphT<T>::operator != ( const BSphT& other ) const {
	return this->position != other.position || this->radius != other.radius;
}
