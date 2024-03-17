//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
SkinningAttributeT<T>::SkinningAttributeT ( const String &_name, u32 count ){
	if ( _name == "" ) {
		return;
	}
	this->setValid ( true );
	this->setName ( _name );
	this->setType ( Burgess2AttrType::SkinningAttr );
	this->values.resize ( count );
	this->v[0] = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュートの配列数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 SkinningAttributeT<T>::size () const {
	return u32(this->values.size ());
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュートの配列数を変更する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::resize ( u32 count ) {
	this->values.resize ( count );
	this->values.shrink_to_fit ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュート値を新しい配列に設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::resetValues ( const std::vector<SkinningPairDataT<T>> &_data ) {
	this->values = _data;
	for(u32 i=0; i<_data.size(); ++i){
		L4Array<u16> parts = _data[i].getPartsIDs ();
		for ( u32 j = 0; j<parts.size (); ++j ){
			this->v[0] = Burgess2MathFn::Max ( this->v[0], u16 ( parts[j] ) );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     全てのアトリビュート値を設定（配列数の変更は無し）する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::setValues ( const std::vector<SkinningPairDataT<T>> &_data ) {
	assert ( this->size () == _data.size () );
	this->values = _data;
	for ( u32 i = 0; i<_data.size (); ++i ){
		L4Array<u16> parts = _data[i].getPartsIDs ();
		for ( u32 j = 0; j<parts.size (); ++j ){
			this->v[0] = Burgess2MathFn::Max ( this->v[0], u8(parts[j]) );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュート値を設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::setValue ( u32 index, const SkinningPairDataT<T> &_data ) {
	assert ( index < this->size () );
	this->values [index] = _data;
	L4Array<u16> parts = _data.getPartsIDs ();
	for(u32 i=0; i<parts.size(); ++i ){
		this->v[0] = Burgess2MathFn::Max ( this->v[0], u16(parts[i]) );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     該当のパーツIDのスキンウェイト値を設定する (パーツが未登録の場合は新規ペアとして追加登録する)
//!	    @brief 該当のパーツIDのスキンウェイト値を設定する (パーツが未登録の場合は新規ペアとして追加登録する)
//!		@param[in] index アトリビュートID
//!		@param[in] partsID パーツID
//!		@param[in] weight スキンウェイト値
//!		@param[in] normalize　設定したウェイト値を保持したまま他のペアのウェイト値をノーマライズする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::setOrCreatePair ( u32 index, u16 partsID, T weight, Bool normalize ){
	assert ( index < this->size () );
	if ( this->values[index].hasParts ( partsID ) ){
		this->values[index].setWeight ( partsID, weight, normalize );
	} else{
		this->values[index].addPair ( partsID, weight, normalize );
	}
	this->v[0] = Burgess2MathFn::Max ( this->v[0], partsID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    該当のパーツIDのスキンウェイト値を加算・減算する (パーツが未登録の場合は終了)
//!	    @brief 該当のパーツIDのスキンウェイト値を加算・減算する (パーツが未登録の場合は終了)
//!		@param[in] index アトリビュートID
//!		@param[in] partsID パーツID
//!		@param[in] weight スキンウェイトの加算・減算値
//!		@param[in] normalize　編集したウェイト値を保持したまま他のペアのウェイト値をノーマライズする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::addPairWeight ( u32 index, u16 partsID, T weight, Bool normalize ){
	assert ( index < this->size () );
	if ( this->values[index].hasParts ( partsID ) ){
		if ( T ( 0.0 ) < weight ){
			this->values[index].addWeight ( partsID, weight, normalize );
		} else{
			this->values[index].subWeight ( partsID, weight, normalize );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュート値を参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
const std::vector<SkinningPairDataT<T>>& SkinningAttributeT<T>::getValuesRef () const {
	return this->values;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュート値を全て取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<SkinningPairDataT<T>> SkinningAttributeT<T>::getValues () const {
	return this->values;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュート値を１つ取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
SkinningPairDataT<T> SkinningAttributeT<T>::getValue (u32 index) const{
	assert ( index < this->size () );
	return this->values [index];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定したアトリビュートに登録されているペアのウェイト値の合計が1.0になるように値を調整する
//!	    @brief 指定したアトリビュートに登録されているペアのウェイト値の合計が1.0になるように値を調整する
//!		@param[in] index アトリビュートID
//!		@param[in] cleanup 未使用のペアを削除する
//!		@param[in] maxInfluence　マックスインフルエンス数
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::normalize ( u32 index, Bool cleanup, u8 maxInfluence ){
	assert ( index < this->size () );
	this->values[index].normalize( maxInfluence );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ウェイト値の合計が1.0になるように値を調整する
//!	    @brief ウェイト値の合計が1.0になるように値を調整する
//!		@param[in] cleanup 未使用のペアを削除する
//!		@param[in] maxInfluence　マックスインフルエンス数
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::allNormalize ( Bool cleanup, u8 maxInfluence ){
	#pragma omp parallel for if(500 < this->values.size())
	for ( u32 i = 0, n = this->values.size (); i < n; ++i ){
		this->normalize ( i, cleanup, maxInfluence );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    アトリビュート値へ[ ]でアクセスする
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SkinningPairDataT<T>& SkinningAttributeT<T>::operator[]( u32 index ) {
	assert ( index < this->size () );
	return this->values [index];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュート値へ[ ]でアクセスする
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const SkinningPairDataT<T>& SkinningAttributeT<T>::operator[]( u32 index ) const {
	assert ( index < this->size () );
	return this->values [index];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     値を全て0にする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningAttributeT<T>::clear () {
	u32 cnt = this->values.size ();
	for ( u32 i = 0; i<cnt; i++ ) {
		this->values [i] = SkinningAttributeT<T> ();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     アトリビュートに登録しているパーツIDの最大IDを返す（=スキンアトリビュートに対応するジョイント/ボーンの最大数）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u16 SkinningAttributeT<T>::partsMaxID ()const{
	return this->v[0];
}

/* SkinningPairDataT<T> */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     パーツIDがペアに登録されている場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool SkinningPairDataT<T>::hasParts ( u16 partsID )const{
	for ( u32 i = 0, n = this->pairCount (); i < n; ++i ){
		if ( this->partsIDs[i] == partsID ){ return true; }
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     登録されているペアの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 SkinningPairDataT<T>::pairCount ()const{
	return this->partsIDs.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ペアを新規追加する（パーツIDがペアリストに存在していないことを事前に.hasPart()で確認してから実行すること）
//!	    @brief ペアを新規追加する（パーツIDがペアリストに存在していないことを事前に.hasPart()で確認してから実行すること）
//!		@param[in] partsID パーツID
//!		@param[in] weight ウェイト値
//!		@param[in] normalize 設定したウェイト値を保持したまま他のペアのウェイト値をノーマライズする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::addPair ( u16 partsID, T weight, Bool normalize ){
	assert ( this->hasParts ( partsID ) == false );
	u16 s = this->partsIDs.size ();
	this->partsIDs.resize ( s + 1 );
	this->weights.resize ( s + 1 );
	this->partsIDs[s] = partsID;
	this->weights[s] = weight;

	// 追加したウェイトを保持して、他のペアのウェイトをノーマライズ
	if ( normalize ){
		T total{ 0.0 };
		for ( u32 i = 0; i < s; ++i ){ 
			total += this->weights[i]; 
		}
		T remainder = T ( 1.0 ) - weight;
		for ( u32 i = 0; i < s; ++i ){
			this->weights[i] = remainder * ( this->weights[i] / total );
		}
	}

}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定したパーツIDのウェイトを設定する（パーツIDが既にペアリストに存在することを事前に.hasPart()で確認してから実行すること）
//!	    @brief 指定したパーツIDのウェイトを設定する（パーツIDが既にペアリストに存在することを事前に.hasPart()で確認してから実行すること）
//!		@param[in] partsID パーツID
//!		@param[in] weight ウェイト値
//!		@param[in] normalize 設定したウェイト値を保持したまま他のペアのウェイト値をノーマライズする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::setWeight ( u16 partsID, T weight, Bool normalize ){
	assert ( this->hasParts ( partsID ) );
	if ( weight <= T ( 0.0 ) ){ return; } // ウェイト値が0.0以下の場合は終了

	Bool fined = false;
	T total{0.0}; // ノーマライズ処理を行うためのデータを準備しておく
	for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
		if ( this->partsIDs[i] == partsID ){
			this->weights[i] = weight;
			fined = true;
		} else{
			total += this->weights[i];
		}
	}

	// 指定のパーツIDが存在し、かつノーマライズが有効な場合
	if ( fined && normalize ){ 
		T remainder = T ( 1.0 ) - weight;
		for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
			if ( partsIDs[i] != partsID ){
				this->weights[i] = remainder * ( this->weights[i] / total );
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    指定したパーツIDのウェイト値を加算する（パーツIDが既にペアリストに存在することを事前に.hasPart()で確認してから実行すること）
//!	    @brief 指定したパーツIDのウェイト値を減算する（パーツIDが既にペアリストに存在することを事前に.hasPart()で確認してから実行すること）
//!		@param[in] partsID パーツID
//!		@param[in] weight ウェイト値
//!		@param[in] normalize 編集したウェイト値を保持したまま他のペアのウェイト値をノーマライズする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::addWeight ( u16 partsID, T weight, Bool normalize){
	assert ( this->hasParts ( partsID ) );
	if ( weight <= T ( 0.0 ) ){ return; } // ウェイト値が0.0以下の場合は終了

	Bool fined = false;
	T setW;
	T total{0.0}; // ノーマライズ処理を行うためのデータを準備しておく
	for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
		if ( this->partsIDs[i] == partsID ){
			setW = Burgess2MathFn::Min ( this->weights[i] + weight, T ( 1.0 ) );
			this->weights[i] = setW;
			fined = true;
		} else{
			total += this->weights[i];
		}
	}

	// 指定のパーツIDが存在し、かつノーマライズが有効な場合
	if ( fined && normalize ){
		T remainder = T ( 1.0 ) - setW;
		for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
			if ( partsIDs[i] != partsID ){
				this->weights[i] = remainder * ( this->weights[i] / total );
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定したパーツIDのウェイト値を減算する（パーツIDが既にペアリストに存在することを事前に.hasPart()で確認してから実行すること）
//!	    @brief 指定したパーツIDのウェイト値を減算する（パーツIDが既にペアリストに存在することを事前に.hasPart()で確認してから実行すること）
//!		@param[in] partsID パーツID
//!		@param[in] weight ウェイト値
//!		@param[in] normalize 編集したウェイト値を保持したまま他のペアのウェイト値をノーマライズする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::subWeight ( u16 partsID, T weight, Bool normalize ){
	assert ( this->hasParts ( partsID ) );
	if ( weight <= T ( 0.0 ) ){ return; } // ウェイト値が0.0以下の場合は終了

	Bool fined = false;
	T setW;
	T total{0.0}; // ノーマライズ処理を行うためのデータを準備しておく
	for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
		if ( this->partsIDs[i] == partsID ){
			setW = Burgess2MathFn::Max ( this->weights[i] - weight, T ( 0.0 ) );
			this->weights[i] = setW;
			fined = true;
		} else{
			total += this->weights[i];
		}
	}

	// 指定のパーツIDが存在し、かつノーマライズが有効な場合
	if ( fined && normalize ){
		T remainder = T ( 1.0 ) - setW;
		for ( u32 i = 0, n = this->partsIDs.size (); i < n; ++i ){
			if ( partsIDs[i] != partsID ){
				this->weights[i] = remainder * ( this->weights[i] / total );
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ウェイト値の合計が1.0になるように各ペアのウェイト値を調整する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::normalize ( u8 maxInfluence ){
	u32 s = this->pairCount ();
	if ( s == 0 ){ return; } // 登録のペアが0の場合は終了
	if ( s == 1 ){ this->weights[0] = T ( 1.0 ); } // 登録のペアが１つの場合はウェイト値を1.0にして終了

	T total{0.0};
	if ( s < maxInfluence ){ 
		// 登録のペア数がマックスインフルエンス以下の場合はウェイトをノーマライズする
		for ( u8 i = 0; i < s; ++i ){ total += this->weights[i]; }
		for ( u8 i = 0; i < s; ++i ){ this->weights[i] = this->weights[i] / total; }
	} else{
		// ウェイト値が大きな順にマックスインフルエンス数でノーマライズする（その他のペアのウェイト値は0.0にする）
		
		// ウェイト値が大きな順に並べる
		std::vector<T> restW ( s );
		std::vector<u32> origID ( s );
		for ( u8 i = 0; i < s; ++i ){ restW[i] = this->weights[i]; origID[i] = i; }
		Burgess2MathFn::Sort ( restW, origID, true ); 

		// 全てのペアのウェイト値を0.0にする
		for ( u8 i = 0; i < s; ++i ){ this->weights[i] = T ( 0.0 ); }

		// マックスインフルエンス数のペアにウェイトを設定し直す
		for ( u8 i = 0; i < maxInfluence; ++i ){ total += restW[i]; }
		for ( u8 i = 0; i < maxInfluence; ++i ){ this->weights[origID[i]] = restW[i] / total; }
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ウェイト値の合計が1.0になるように調整し、未使用のペアを削除する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::normalizeAndCleanup ( u8 maxInfluence ){
	u32 s = this->pairCount ();
	if ( s == 0 ){ return; } // 登録のペアが0の場合は終了
	if ( s == 1 ){ this->weights[0] = T ( 1.0 ); } // 登録のペアが１つの場合はウェイト値を1.0にして終了

	T total{0.0};
	if ( s < maxInfluence ){
		// 登録のペア数がマックスインフルエンス以下の場合はウェイトをノーマライズする
		for ( u8 i = 0; i < s; ++i ){ total += this->weights[i]; }
		L4Array<u16> restP = this->partsIDs;
		L4Array<T> restW = this->weights;
		
		// 登録していた全てのペアを削除する
		this->partsIDs.resize ( 0 );
		this->weights.resize ( 0 );

		// ウェイト値が0.0より大きいペアを再登録して終了
		for ( u8 i = 0; i < s; ++i ){
			if ( T ( 0.0 ) < restW[i] ){
				this->partsIDs.push ( restP[i] );
				this->weights.push ( restW[i] / total );
			}
		}
	} else{
		// ウェイト値が大きな順にマックスインフルエンス数でノーマライズする（ウェイト値が0.0のペアは削除する）

		// ウェイト値が大きな順に並べる
		std::vector<T> restW ( s );
		std::vector<u32> origID ( s );
		for ( u8 i = 0; i < s; ++i ){ restW[i] = this->weights[i]; origID[i] = i; }
		Burgess2MathFn::Sort ( restW, origID, true );

		L4Array<u16> restP = this->partsIDs; // 元のパーツIDリストを保持しておく

		// 登録していた全てのペアを削除する
		this->partsIDs.resize ( 0 );
		this->weights.resize ( 0 );

		// マックスインフルエンス数のペアを登録し直す(ウェイトが0.0のものは登録しない)
		for ( u8 i = 0; i < maxInfluence; ++i ){ total += restW[i]; } // ウェイトの大きな順に加算していく
		for ( u8 i = 0; i < maxInfluence; ++i ){
			if ( T ( 0.0 ) < restW[i] ){
				this->partsIDs.push ( restP[origID[i]] ); // パーツIDを登録
				this->weights.push( restW[i] / total ); // 新しいウェイト値を設定
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ペアの値を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void SkinningPairDataT<T>::getPairValue ( u32 pairID, OUT u16 &partsID, OUT T &weight )const{
	if ( this->pairCount () <= pairID ){ return; }
	partsID = this->partsIDs[pairID];
	weight = this->weights[pairID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     パーツIDリストを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L4Array<u16> SkinningPairDataT<T>::getPartsIDs ()const{
	return this->partsIDs;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     スキンウェイトリストを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L4Array<T> SkinningPairDataT<T>::getWeights ()const{
	return this->weights;
}