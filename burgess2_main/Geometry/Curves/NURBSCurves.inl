//------------------------------------------------------------------------------------------------------------------------------------------------
//!		NURBSCurveを追加する
//!
//!		points: LoopCurve(formが1or2)の場合、重複頂点は内部で自動的に作成されるため引数"points"には含めない<br>
//!		knots: 例：points.size() = 4, degree = 2<br>
//!			open : knots = { 0, 0, 0, 1, 2, 2, 2 } /// points.size + degree + 1<br>
//!			close : knots = { 0, 0, 0, 1, 2, 3, 3, 3 } /// points.size + degree + 2<br>
//!			periodic : knots = { 0, 1, 2, 3, 4, 5, 6, 7, 8 } /// point.size + (degree * 2) + 1 <br>
//!		@param[in] points カーブ頂点（LoopCurveでも重複頂点は含めない）
//!		@param[in] degree 次数
//!		@param[in] form 0=open, 1=close, 2=periodic
//!		@param[in] knots ノットベクトル
//!		@param[in] weights 頂点へのウェイト値（0.0~1.0）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 NURBSCurvesT<T>::add(const std::vector<Vec3T<T>> &points, u8 degree, u8 form, const std::vector<T> &knots, const std::vector<f32> &weights) {
	assert ( 2 == degree || 3 == degree );
	assert ( form < 3 );
	assert ( 3 <= points.size () );
	assert ( points.size () == weights.size () );
	switch ( form ) {
		case 0:	assert ( knots.size () == points.size () + degree + 1 );	break;
		case 1:	assert ( knots.size () == points.size () + degree + 2 );	break;
		case 2:	assert ( knots.size () == points.size () + degree*2 + 1 );	break;
	}

	u32 id = this->curveCount ();
	this->setCurveCount ( this->curveCount ()+1 );
	this->data [id].setForm ( form );
	this->data [id].v [0] = degree;

	/* ウェイトの登録 （Loopの場合は重複頂点も設定する）*/
	F32AttributeT<T> & weightAttr = this->getOrCreateF32Attribute ( id, "rationalWeight" );
	weightAttr.resetValues ( weights );
	/* 頂点位置の登録（Loopの場合は重複頂点も設定する）Attribute.v[0]=重複頂点数 */
	Vec3AttributeT<T> &v3Attr = this->getPointPositionAttribute ( id ); /* = Vec3Attribute "pointPositions" */
	v3Attr.resetValues ( points );
	u32 cnt = u32(points.size ());
	switch ( form ) {
	case 0:
		v3Attr.v [0] = 0; break;
	case 1:
		weightAttr.resize ( cnt + 1 );
		weightAttr [cnt] = weightAttr [0];
		v3Attr.resize ( cnt + 1 );
		v3Attr [cnt] = v3Attr [0];
		v3Attr.v [0] = 1; break;
	case 2:
		weightAttr.resize ( cnt + degree + 1 );
		for ( u8 i = 0; i<degree+1; i++ ) {
			weightAttr [cnt + i] = weightAttr [i];
		}
		v3Attr.resize ( cnt + degree + 1 );
		for ( u8 i = 0; i<degree+1; i++ ) {
			v3Attr [cnt + i] = v3Attr [i];
		}
		v3Attr.v [0] = degree + 1; break;
	}
	
	/* ノットベクトルの登録 */
	F32AttributeT<T> & knotAttr = this->getOrCreateF32Attribute ( id, "knotVector" );
	knotAttr.resetValues ( knots );

	/* Param⇔Lengthの変換用テーブル */
	this->getOrCreatVec3Attribute ( id, "paramLengthTable" );
	this->incrementParamLengthTableVirsion ( id );
	this->recomputeParamLengthTable ( id );
	return id;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		NURBSCurveを追加する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 NURBSCurvesT<T>::add ( const std::vector<Vec3T<T>> &points, u8 degree, u8 form, const std::vector<f32> &weights ) {
	std::vector<T> knot = this->createBasicKnotVector ( degree, points.size(), form );
	return this->add ( points, degree, form, knot, weights );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Weightを設定する（引数のVec3配列にはLoopの重複頂点は含めない）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void NURBSCurvesT<T>::setAllRationalWeight ( u32 curveID, const std::vector<f32> &weights ) {
	assert ( curveID < this->curveCount () );
	assert ( weights.size () == this->pointCount ( curveID ) );
	F32AttributeT<T> & weightAttr = this->createF32Attribute ( curveID, "rationalWeight" );
	for ( u32 i = 0; i<weights.size (); i++ ) {
		weightAttr.setValue ( i, weights [i] );
	}

	/// 重複頂点を更新する
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	u32 c = this->pointCount ( curveID );
	for ( u8 i = 0; i<v3Attr.v [0]; i++ ) {
		weightAttr [c+i] = weightAttr [i];
	}
	this->incrementParamLengthTableVirsion ( curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Weightを設定する（引数のpointIDでLoopの重複頂点IDは指定できない）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void NURBSCurvesT<T>::setRationalWeight ( u32 curveID, u32 pointID, f32 weight ) {
	assert ( curveID < this->curveCount () );
	assert ( pointID < this->pointCount ( curveID ) );
	F32AttributeT<T> & weightAttr = this->createF32Attribute ( curveID, "rationalWeight" );
	weightAttr.setValue ( pointID, weight );

	/// 重複頂点を更新する
	Vec3AttributeT<T>& v3Attr = this->getPointPositionAttribute ( curveID );
	if ( pointID < v3Attr.v [0] ) {
		u32 c = this->pointCount ( curveID );
		weightAttr [c+pointID] = weightAttr [pointID];
	}
	this->incrementParamLengthTableVirsion ( curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Weightを取得する（戻り値にはLoopの重複頂点は含まれない）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<f32> NURBSCurvesT<T>::getAllRationalWeight ( u32 curveID )const {
	F32AttributeT<T> & weightAttr = this->createF32Attribute ( curveID, "rationalWeight" );
	std::vector<f32> result = weightAttr.getValues();
	if ( this->getForm ( curveID ) != 0 ) {
		u32 cnt = this->pointCount ( curveID ); /// 重複頂点数を省いた頂点数を取得
		result.resize ( cnt );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Weightを取得する（引数のpointIDでLoopの重複頂点IDは指定できない）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
f32 NURBSCurvesT<T>::getRationalWeight ( u32 curveID, u32 pointID )const {
	assert ( curveID < this->curveCount () );
	assert ( pointID < this->pointCount ( curveID ) );
	F32AttributeT<T> & weightAttr = this->createF32Attribute ( curveID, "rationalWeight" );
	return weightAttr.getValue ( pointID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Param値の位置を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> NURBSCurvesT<T>::getPositionAtParam ( u32 curveID, T _param )const {
	Vec3T<T> result;
	assert ( curveID < this->curveCount () );

	u8 degree = this->getDegree ( curveID );
	u32 poiCount = this->pointCount ( curveID );
	assert ( 2 == degree || 3 == degree );
	assert ( 3 <= poiCount );

	T periodicOffset = this->isPeriodic ( curveID ) ? T ( this->getDegree ( curveID ) ) : 0;
	T param = _param + periodicOffset;
	Vec3AttributeT<T> &refPoiAttr = this->getPointPositionAttribute ( curveID );
	F32AttributeT<T> &refKnotAttr = this->getF32Attribute ( curveID, "knotVector" );
	F32AttributeT<T> &refWeightAttr = this->getF32Attribute ( curveID, "rationalWeight" );

	u8 domainA = degree;
	u8 domainB = refKnotAttr.size () - 1 - degree;

	if ( param < refKnotAttr [domainA] || refKnotAttr [domainB] < param ) { return result; }

	/// s = segmentIndex
	u8 s;
	for ( s = domainA; s < domainB; s++ ) {
		if ( refKnotAttr [s] <= param && param <= refKnotAttr [s + 1] ) {
			break;
		}
	}

	std::vector<Vec3T<T>> V = refPoiAttr.getValues ();
	std::vector<f32> W = refWeightAttr.getValues ();
	for ( u8 h = 1; h <= degree + 1; h++ ) {
		for ( u8 i = s; i > s - degree - 1 + h; i-- ) {
			T alpha = ( param - refKnotAttr [i] ) / ( refKnotAttr [i + degree + 1 - h] - refKnotAttr [i] );
			V [i] = ( T ( 1.0 ) - alpha ) * V [i - 1] + alpha * V [i];
			W [i] = ( T ( 1.0 ) - alpha ) * W [i-1] + alpha * W [i];
		}
	}
	return V [s] * W [s];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブをコピーする
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 NURBSCurvesT<T>::copyCurveFrom ( const NURBSCurvesT<T> &src, u32 curveID ) {
	u32 id = BSplineCurves::copyCurveFrom ( src, curveID );

	F32AttributeT<T> & refWeight = src.getVec3Attribute ( curveID, "rationalWeight" );
	F32AttributeT<T> & weightAttr = this->createF32Attribute ( id, "rationalWeight" );
	weightAttr.resetValues ( refWeight.getValues () );

	return id;
}