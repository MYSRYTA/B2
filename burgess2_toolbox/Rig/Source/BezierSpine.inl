//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    初期化
//!	    @brief 初期化
//!		@param[in] spineLength スパインの長さ
//!		@param[in] boneSize 生成するボーンの数 (始点・終点のボーンを含めた数)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void BezierSpineT<T>::initialize ( T _neutralLength, u8 jointSize ) {
	this->neutralLength = _neutralLength;
	this->jointCnt = Math::Max ( jointSize, u8 ( 2 ) );
	this->boneLength = _neutralLength / T ( this->jointCnt - 1 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    スケールを設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void BezierSpineT<T>::setScale ( T _scale ) {
	this->scale = Math::Max ( _scale, T ( 0.01 ) );
	this->boneLength = ( this->neutralLength * this->scale ) / T ( this->jointCnt );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ベジェ曲線上に生成するボーンの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u8 BezierSpineT<T>::jointCount () const{
	return this->jointCnt;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    solve
//!	    @brief solve
//!		@param[in] root rootのGlobal-Mat4（上流モジュールとのコネクタ）
//!		@param[in] cp spineのcontrolpointのGlobal-Mat4 (配列数は3or4)
//!		@return 全てのボーンと先端ジョイントのLocal-May4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> BezierSpineT<T>::solve ( const Mat4T<T> root, const std::vector<Mat4T<T>> cp ){
	switch ( cp.size () ){
		case 3:
			return this->solveBezier2 ( root, cp );
		case 4:
			return this->solveBezier3 ( root, cp );
		default:
			std::vector<Mat4T<T>> result ( this->jointCount () );
			return result;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BezierSpineT<T>::evalBezier2 ( const L4Array<Vec3T<T>> &coeffs, T t ) const{
	return  ( t * t * coeffs[0] ) + ( T ( 2.0 ) * t * coeffs[1] ) + coeffs[2];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    solve 2 degree Bezier
//!	    @brief solve 2 degree Bezier
//!		@param[in] root rootのGlobal-Mat4（上流モジュールとのコネクタ）
//!		@param[in] cp spineのcontrolpointのGlobal-Mat4 (配列数は３)
//!		@return 全てのボーンと先端ジョイントのLocal-May4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> BezierSpineT<T>::solveBezier2 ( const Mat4T<T> root, const std::vector<Mat4T<T>> cp )
{
	std::vector<Mat4T<T>> result ( this->jointCnt );
	if ( cp.size () != 3 ){ return result; }

	Mat4T<T> rootInv = root.inverse ();
	Vec3T<T> baseHandle = ( rootInv * cp[0] ).getTranslation ();
	Vec3T<T> handle0    = ( rootInv * cp[1] ).getTranslation ();
	Vec3T<T> tipHandle  = ( rootInv * cp[2] ).getTranslation ();

	L4Array<Vec3T<T>> coeffs ( 3 );
	coeffs[0] = tipHandle - baseHandle - handle0 * T ( 2.0 );
	coeffs[1] = baseHandle * T ( -1.0 ) + handle0;
	coeffs[2] = baseHandle;

	std::vector<T> distance ( this->jointCnt * 2 );
	T c = T ( 1.0 ) / T ( this->jointCnt * 2 - 1 );
	Vec3T<T> rest;
	for ( u32 i = 0; i < this->jointCnt * 2; i++ ) {
		Vec3T<T> p = this->evalBezier2 ( coeffs, i * c );
		if ( i == 0 ) {
			distance [0] = 0.0;
		} else {
			distance [i] = distance [i-1] + p.distanceTo ( rest );
		}
		rest = p;
	}
	T totalLength = distance [distance.size () - 1];

	QuatT<T> rotOffset;
	rotOffset.setFromAxisAndAngle ( Vec3T<T> ( 0, 0, 1 ), Burgess::HPI );

	QuatT<T> baseRot = ( rootInv * cp[0] ).getRotation() * rotOffset;
	QuatT<T> tipRot = ( rootInv * cp[2] ).getRotation() * rotOffset;
	T e = T ( 1.0 ) / T ( this->jointCnt );
	T e2 = T ( 1.0 ) / ( distance.size () - 1 );
	u32 id = 1;
	for ( u32 i = 0; i<this->jointCnt; ++i ) {
		XfoT<T> resultXfo;
		resultXfo.tr = i == 0 ? 
			baseHandle :
			this->tipMat4.getTranslation() + this->tipMat4.getRotationMat3() * Vec3T<T> ( this->boneLength, 0.0, 0.0 );
		resultXfo.ori = baseRot.slerp ( tipRot, ( T ( i ) + T ( 0.5 ) ) * e );

		T param;
		if ( this->boneLength * ( i+1 ) < totalLength ) {
			for ( u32 j = id, n = distance.size (); j < n; ++j ) {
				if ( this->boneLength * ( i+1 ) < distance [j] ) {
					T ratio = ( this->boneLength * ( i+1 ) - distance [j - 1] ) / ( distance [j] - distance [j - 1] );
					id = j-1;
					param = ( T ( id ) * e2 ) + ( ratio * e2 );
					break;
				}
			}
		} else {
			param = ( this->boneLength * ( i+1 ) - totalLength ) / this->boneLength + T ( 1.0 );
		}

		Vec3T<T> targetPos = param < 1.0 ?
			this->evalBezier2 ( coeffs, param ) :
			tipHandle + ( tipHandle - handle0 ).unit () * ( ( param - 1.0 ) * this->boneLength );
		rotOffset.setFrom2Vectors ( resultXfo.ori.getXaxis (), ( targetPos - resultXfo.tr ).normal () );
		resultXfo.ori = rotOffset * resultXfo.ori;

		result [i] = this->tipMat4.inverse() * resultXfo.toMat4 ();
		this->tipMat4 = resultXfo.toMat4 ();
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BezierSpineT<T>::evalBezier3 ( const L4Array<Vec3T<T>> &coeffs, T t ) const{
	return ( coeffs[0] * t * t * t ) + ( coeffs[1] * 3.0f * t * t ) + ( coeffs[2] * 3.0f * t ) + coeffs[3];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    solve 3 degree Bezier
//!	    @brief solve 3 degree Bezier
//!		@param[in] root rootのGlobal-Mat4（上流モジュールとのコネクタ）
//!		@param[in] cp spineのcontrolpointのGlobal-Mat4 (配列数は4)
//!		@return 全てのボーンと先端ジョイントのLocal-May4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> BezierSpineT<T>::solveBezier3 ( const Mat4T<T> root, const std::vector<Mat4T<T>> cp ){
	std::vector<Mat4T<T>> result ( this->jointCnt );
	if ( cp.size () != 4 ){ return result; }

	Mat4T<T> rootInv = root.inverse ();
	Vec3T<T> baseHandle0 = ( rootInv * cp[0] ).getTranslation ();
	Vec3T<T> baseHandle1 = ( rootInv * cp[1] ).getTranslation ();
	Vec3T<T> tipHandle1 = ( rootInv * cp[2] ).getTranslation ();
	Vec3T<T> tipHandle0 = ( rootInv * cp[3] ).getTranslation ();

	L4Array<Vec3T<T>> coeffs ( 4 );
	coeffs[0] = tipHandle0 - baseHandle0 - tipHandle1 * T ( 3.0 ) + baseHandle1 * T ( 3.0 );
	coeffs[1] = baseHandle0 - baseHandle1 * T ( 2.0 ) + tipHandle1;
	coeffs[2] = baseHandle0 * T ( -1.0 ) + baseHandle1;
	coeffs[3] = baseHandle0;

	std::vector<T> distance ( this->jointCnt * 2 );
	T c = T ( 1.0 ) / T ( this->jointCnt * 2 - 1 );
	Vec3T<T> rest;
	for ( u32 i = 0; i < this->jointCnt * 2; i++ ){
		Vec3T<T> p = this->evalBezier3 ( coeffs, i * c );
		if ( i == 0 ){
			distance[0] = 0.0;
		} else{
			distance[i] = distance[i - 1] + p.distanceTo ( rest );
		}
		rest = p;
	}
	T totalLength = distance[distance.size () - 1];

	QuatT<T> rotOffset;
	rotOffset.setFromAxisAndAngle ( Vec3T<T> ( 0, 0, 1 ), Burgess::HPI );

	QuatT<T> baseRot = ( rootInv * cp[0] ).getRotation () * rotOffset;
	QuatT<T> tipRot = ( rootInv * cp[3] ).getRotation () * rotOffset;

	T e = T ( 1.0 ) / T ( this->jointCnt );
	T e2 = T ( 1.0 ) / ( distance.size () - 1 );
	this->tipMat4 = Mat4T<T> ();
	u32 id = 1;
	for ( u32 i = 0; i < this->jointCnt; ++i ){
		XfoT<T> resultXfo;
		resultXfo.tr = i == 0 ?
			baseHandle0:
			this->tipMat4.getTranslation () + this->tipMat4.getRotationMat3 () * Vec3T<T> ( this->boneLength, 0.0, 0.0 );
		resultXfo.ori = baseRot.slerp ( tipRot, ( T ( i ) + T ( 0.5 ) ) * e );

		T param;
		if ( this->boneLength * ( i + 1 ) < totalLength ){
			for ( u32 j = id, n = distance.size (); j < n; ++j ){
				if ( this->boneLength * ( i + 1 ) < distance[j] ){
					T ratio = ( this->boneLength * ( i + 1 ) - distance[j - 1] ) / ( distance[j] - distance[j - 1] );
					id = j - 1;
					param = ( T ( id ) * e2 ) + ( ratio * e2 );
					break;
				}
			}
		} else{
			param = ( this->boneLength * ( i + 1 ) - totalLength ) / this->boneLength + T ( 1.0 );
		}

		Vec3T<T> targetPos = param < 1.0 ?
			this->evalBezier3 ( coeffs, param ) :
			tipHandle0 + ( tipHandle0 - tipHandle1 ).unit () * ( ( param - 1.0 ) * this->boneLength );
		rotOffset.setFrom2Vectors ( resultXfo.ori.getXaxis (), ( targetPos - resultXfo.tr ).normal () );
		resultXfo.ori = rotOffset * resultXfo.ori;
		result[i] = this->tipMat4.inverse () * resultXfo.toMat4 ();
		this->tipMat4 = resultXfo.toMat4 ();
	}

	return result;
}