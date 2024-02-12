//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    カーブセクションとインターバルを反転する　例：[0.0, 0.2][0.8, 0.9]　→　[0.2, 0.8][0.9, 1.0]
//!		反転できる範囲がない場合は処理を実行しない
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool CurveSectionsT<T>::flip (){
	if ( this->isValid () == false ) { return false; }
	
	CurveSectionsT<T> rest = *this;
	this->resize (0);

	/* sectionの情報は前から順番に保存されていない可能性があるため、 0.0,1.0以外の全ての値を一旦restに格納していく*/
	/* restをソートした後に前から順にVec2にしていく */
	std::vector<f32> restXY;
	for ( u32 j = 0; j<rest.sectionCount (); j++ ) {
		Vec2T<T> v = rest.getValue ( j );
		if ( v.almostEqual ( Vec2T<T> ( 0.0, 1.0 ) ) ) { break; }
		if ( j == 0 && v.x != T(0.0) ) { restXY.push_back ( T(0.0) ); }
		if ( v.x != T ( 0.0 ) ) { restXY.push_back ( v.x ); }
		if ( v.y != T ( 1.0 ) ) { restXY.push_back ( v.y ); }
		if ( j == rest.sectionCount () - 1 && v.y != T(1.0) ) { restXY.push_back ( T(1.0) ); }
		Burgess2MathFn::Sort ( restXY );
	}
	for ( u32 j = 0; j < restXY.size () / 2; j++ ) {
		if ( restXY [j * 2]<T(0.0) || restXY [j * 2 + 1]<T(0.0) || 1.0<restXY [j * 2] || 1.0<restXY [j * 2 + 1] 
			|| restXY [j * 2 + 1] < restXY [j * 2] || abs ( restXY [j * 2]-restXY [j * 2 + 1] )<BURGESS2::TOLERANCE ) {
			continue;
		}
		this->add ( restXY [j * 2], restXY [j * 2 + 1] );
	}
	if ( this->sectionCount () == 0 ) { return false; }
	this->setDirty ();
	return this->isValid ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    @brief カーブセクションを拡縮する
//!		@param[in] scale sectionの拡縮値
//!		@param[in] clamp sectionの値を0.0~1.0にクランプする場合はtrue
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurveSectionsT<T>::scaling ( u32 sectionID, T scale, Bool clamp ) {
	assert ( sectionID < this->sectionCount () );
	f32 val = this->values [sectionID].y - this->values [sectionID].x;
	f32 addVal = val * ( scale - T(1.0) );
	if ( clamp ) {
		this->values [sectionID].x = Burgess2MathFn::Clamp ( this->values [sectionID].x - addVal * T ( 0.5 ), T ( 0.0 ), T ( 1.0 ) );
		this->values [sectionID].y = Burgess2MathFn::Clamp ( this->values [sectionID].y + addVal * T ( 0.5 ), T ( 0.0 ), T ( 1.0 ) );
	} else {
		this->values [sectionID].x -= addVal * T ( 0.5 );
		this->values [sectionID].y += addVal * T ( 0.5 );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    @brief 全てのカーブセクションを拡縮する
//!		@param[in] scale sectionの拡縮値
//!		@param[in] clamp sectionの値を0.0~1.0にクランプする場合はtrue
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurveSectionsT<T>::allScaling ( T scale, Bool clamp ) {
	for ( int i = 0; i < this->sectionCount (); i++ ) {
		this->scaling ( i, scale, clamp );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    @brief カーブセクションに値を加算する
//!		@param[in] startSide sectionのスタートに加算する値
//!		@param[in] endSide sectionのエンドに加算する値
//!		@param[in] clamp sectionの値を0.0~1.0にクランプする場合はtrue
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurveSectionsT<T>::addStartEndValue ( u32 sectionID, T startSide, T endSide, Bool clamp ) {
	if ( clamp ) {
		this->values [sectionID].x = Burgess2MathFn::Clamp ( this->values [sectionID].x - startSide, T ( 0.0 ), T ( 1.0 ) );
		this->values [sectionID].y = Burgess2MathFn::Clamp ( this->values [sectionID].y + endSide, T ( 0.0 ), T ( 1.0 ) );
	} else {
		this->values [sectionID].x -= startSide;
		this->values [sectionID].y += endSide;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    @brief 全てのカーブセクションに値を加算する
//!		@param[in] startSide sectionのスタートに加算する値
//!		@param[in] endSide sectionのエンドに加算する値
//!		@param[in] clamp sectionの値を0.0~1.0にクランプする場合はtrue
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CurveSectionsT<T>::addAllStartEndValues ( T startSide, T endSide, Bool clamp ) {
	for ( u32 i = 0; i < this->sectionCount (); i++ ) {
		this->addValue ( i, startSide, endSide, clamp );
	}
}