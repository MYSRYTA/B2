//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
LinesSpatialQueryT<T>::LinesSpatialQueryT( const std::vector<Vec3T<T>> &_pointPositions ) {
	if ( _pointPositions.size () < 2 ) {
		return;
	}
	this->pointPositions = _pointPositions;
	
	this->lineLength.resize ( this->lineCount () );
	this->lineLength [0] = this->getLineP0Position ( 0 ).distanceTo ( this->getLineP1Position ( 0 ) );
	for ( u32 i = 1; i<this->lineLength.size (); i++ ) {
		this->lineLength [i] = this->lineLength [i-1] + this->getLineP0Position ( i ).distanceTo ( this->getLineP1Position ( i ) );
	}
	this->invTotalLengthRatio = T ( 1.0 ) / this->lineLength [this->lineLength.size()-1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		��ԕ��͗p��Lines�ɑ΂��čł��߂��ꏊ��GeometryLocation��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
GeometryLocationT<T> LinesSpatialQueryT<T>::getClosestLocation ( const Vec3T<T> &position, T maxDistance ) const {
	/*�e���C���ւ̍ŒZ�������v�Z*/
	std::vector<T> dis ( this->lineCount () );
	std::vector<T> ratio ( this->lineCount () );
	for ( s32 i = 0; i<dis.size (); i++ ) {
		Vec3T<T> v = Burgess2MathFn::GetClosestPositionOnLineSegment ( position, this->getLineP0Position ( i ), this->getLineP1Position ( i ), ratio [i] );
		dis [i] = position.distanceTo ( v );
	}
	/*�ł��߂����C����������*/
	std::vector<u32> arrayOrigID;
	Burgess2MathFn::Sort ( dis, arrayOrigID );

	/*�ł��߂����C����ID�ƃ��C����̃��V�I*/
	u32 closestLineID = arrayOrigID [0];
	T lineRatio = ratio [arrayOrigID [0]];

	/*�ł��߂��ʒu*/
	Vec3T<T> p0 = this->getLineP0Position ( closestLineID );
	Vec3T<T> p1 = this->getLineP1Position ( closestLineID );
	Vec3T<T> closestPos = p0.lerp ( p1, lineRatio );

	/*�����͈̔͂��O�̏ꍇ�͏I��*/
	T dist = position.distanceTo ( closestPos );
	if ( maxDistance < dist ) {
		return GeometryLocationT<T>();
	}

	/*�ł��߂��ꏊ��LengthRatio���Z�o*/
	T len = 0;
	if ( 0 < closestLineID ) {
		len = this->lineLength [closestLineID-1];
	}
	len += this->getLineLength ( closestLineID ) * lineRatio;
	T lengthRatio = len * this->invTotalLengthRatio;

	Vec3T<T> weight ( lengthRatio, 0, 0 );
	return GeometryLocationT<T> ( this->v [0], weight, 2 ); /* 2 = curves type*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Line�̎n�_�̈ʒu��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinesSpatialQueryT<T>::getLineP0Position ( u32 lineID )const {
	assert ( lineID < this->lineCount () );
	return this->pointPositions [lineID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Line�̏I�_�̈ʒu��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinesSpatialQueryT<T>::getLineP1Position ( u32 lineID )const {
	assert ( lineID < this->lineCount () );
	return this->pointPositions [lineID + 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Line�̒�����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinesSpatialQueryT<T>::getLineLength ( u32 lineID )const {
	return lineID == 0 ? this->lineLength[0] : this->lineLength [lineID] - this->lineLength [lineID-1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Line�̐���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 LinesSpatialQueryT<T>::lineCount ()const {
	if ( this->isValid () == false ) { return BURGESS2::U32_INFINITE; }
	return u32(this->pointPositions.size ()) - 1;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		LinesSpatialQueryT���L���ȏꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool LinesSpatialQueryT<T>::isValid ( )const {
	return 2 <= this->pointPositions.size () ? true : false;
}