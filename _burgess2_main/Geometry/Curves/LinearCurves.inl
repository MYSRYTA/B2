//------------------------------------------------------------------------------------------------------------------------------------------------
//!		LinearCurveを追加する
//!
//!		points: LoopCurve(formが1or2)の場合、重複頂点は内部で自動的に作成されるため引数"points"には含めない<br>
//!		@param[in] points カーブ頂点（LoopCurveでも重複頂点は含めない）
//!		@param[in] form 0=open, 1=close, 2=periodic
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 LinearCurvesT<T>::add(const std::vector<Vec3T<T>> &points, u8 form){
	assert ( form < 3 );
	assert ( 2 <= points.size () );

	u32 id = this->curveCount ();
	this->setCurveCount ( this->curveCount ()+1 );

	this->data [id].setForm ( form );

	/* 頂点位置の登録（Loopの場合は重複頂点も設定する）Attribute.v[0]=重複頂点数 */
	Vec3AttributeT<T> &v3Attr = this->getPointPositionAttribute ( id ); /* = Vec3Attribute "pointPositions" */
	v3Attr.resetValues ( points );
	u32 cnt = u32(points.size ());
	if ( form == 0 ) {
		v3Attr.v [0] = 0;
	}else{
		v3Attr.resize ( cnt + 1 );
		v3Attr [cnt] = v3Attr [0];
		v3Attr.v [0] = 1; 
	}

	/* Param⇔Lengthの変換用テーブル */
	this->getOrCreatVec3Attribute ( id, "paramLengthTable" );
	this->incrementParamLengthTableVirsion ( id );
	this->recomputeParamLengthTable ( id );

	/*NormalValueにダミーデータをセット*/
	Vec3AttributeT<T> &v3AttrD = this->getVec3Attribute ( id, "normals" );
	v3AttrD.resize ( 2 );
	v3AttrD [0] = Vec3T<T>  ( T(0.001), 0, 1 );
	v3AttrD [1] = Vec3T<T>  ( T(-0.001), 0, 1 );

	return id;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		バウンディングボックスのXYZ軸のどれかと同じ長さのリニアカーブを追加する
//!
//!		バウンディングボックスのXYZ軸のどれかと同じ長さのリニアカーブを追加する
//!		@param[in] bBox カーブ頂点（LoopCurveでも重複頂点は含めない）
//!		@param[in] axis 0=X, 1=Y, 2=Z
//!		@param[in] offsetPos リニアカーブを生成する位置のオフセット
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 LinearCurvesT<T>::add ( const BBoxT<T> &bBox, u8 axis, const Vec3T<T> offsetPos ) {
	std::vector<Vec3T<T>> points ( 2 );
	points [0] = offsetPos;
	points [1] = offsetPos;
	switch ( axis ) {
		case 0:
			points [0].x() = bBox.min.x();
			points [1].x() = bBox.max.x();
			break;
		case 1:
			points [0].y() = bBox.min.y();
			points [1].y() = bBox.max.y();
			break;
		default:
			points [0].z() = bBox.min.z();
			points [1].z() = bBox.max.z();
	}
	return this->add ( points, 0 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブを複製する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 LinearCurvesT<T>::duplicate ( u32 curveID ) {
	return this->copyCurveFrom ( *this, curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブを全てコピーする
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 LinearCurvesT<T>::copyFrom ( const LinearCurvesT<T> &src ) {
	u32 id = this->curveCount ();
	if ( src.curveCount () == 0 ) { return id -1; }
	for ( u32 i = 0; i<src.curveCount (); i++ ) {
		this->copyCurveFrom ( src, i );
	}
	return id;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブをコピーする
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 LinearCurvesT<T>::copyCurveFrom ( const LinearCurvesT<T> &src, u32 curveID ) {
	assert ( curveID < src.curveCount () );
	u32 id = this->curveCount ();
	this->setCurveCount ( this->curveCount ()+1 );

	this->data [id].setForm ( src.getForm ( curveID ) );

	Vec3AttributeT<T> & refPoiPos = src.getPointPositionAttribute ( curveID );
	this->resetAllPointPositions ( id, refPoiPos.getValues () );

	Vec3AttributeT<T> & refParamLen = src.getVec3Attribute ( curveID, "paramLengthTable" );
	Vec3AttributeT<T> & paramLenAttr = this->getOrCreatVec3Attribute ( id, "paramLengthTable" );
	paramLenAttr.resetValues ( refParamLen.getValues () );

	this->paramLengthTableCached ( id );
	return id;
}