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
//!		空間分析用のLinesに対して最も近い場所のGeometryLocationを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
GeometryLocationT<T> LinesSpatialQueryT<T>::getClosestLocation ( const Vec3T<T> &position, T maxDistance ) const {
	/*各ラインへの最短距離を計算*/
	std::vector<T> dis ( this->lineCount () );
	std::vector<T> ratio ( this->lineCount () );
	for ( s32 i = 0; i<dis.size (); i++ ) {
		Vec3T<T> v = Burgess2MathFn::GetClosestPositionOnLineSegment ( position, this->getLineP0Position ( i ), this->getLineP1Position ( i ), ratio [i] );
		dis [i] = position.distanceTo ( v );
	}
	/*最も近いラインを見つける*/
	std::vector<u32> arrayOrigID;
	Burgess2MathFn::Sort ( dis, arrayOrigID );

	/*最も近いラインのIDとライン上のレシオ*/
	u32 closestLineID = arrayOrigID [0];
	T lineRatio = ratio [arrayOrigID [0]];

	/*最も近い位置*/
	Vec3T<T> p0 = this->getLineP0Position ( closestLineID );
	Vec3T<T> p1 = this->getLineP1Position ( closestLineID );
	Vec3T<T> closestPos = p0.lerp ( p1, lineRatio );

	/*検索の範囲より外の場合は終了*/
	T dist = position.distanceTo ( closestPos );
	if ( maxDistance < dist ) {
		return GeometryLocationT<T>();
	}

	/*最も近い場所のLengthRatioを算出*/
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
//!		Lineの始点の位置を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinesSpatialQueryT<T>::getLineP0Position ( u32 lineID )const {
	assert ( lineID < this->lineCount () );
	return this->pointPositions [lineID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Lineの終点の位置を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> LinesSpatialQueryT<T>::getLineP1Position ( u32 lineID )const {
	assert ( lineID < this->lineCount () );
	return this->pointPositions [lineID + 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Lineの長さを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T LinesSpatialQueryT<T>::getLineLength ( u32 lineID )const {
	return lineID == 0 ? this->lineLength[0] : this->lineLength [lineID] - this->lineLength [lineID-1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Lineの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 LinesSpatialQueryT<T>::lineCount ()const {
	if ( this->isValid () == false ) { return BURGESS2::U32_INFINITE; }
	return u32(this->pointPositions.size ()) - 1;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		LinesSpatialQueryTが有効な場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool LinesSpatialQueryT<T>::isValid ( )const {
	return 2 <= this->pointPositions.size () ? true : false;
}