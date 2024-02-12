//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブもしくはセクションのスタート側にセクションを作成する
//!
//!		@param[in out] section カーブセクション（上書きして返す）
//!		@param[in] valType 引数***Valのデータタイプ　0=length; 1=ratio of curveLength; 2=ratio of curveSection
//!		@param[in] onSection 引数sectionに対してセクションの計算を行う（＝既存のセクションの細分化）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BSplineCurvesT<T>::createStartSideSections ( IO CurveSectionsT<T> &section, u32 curveID, T sectionVal, u8 valType, Bool onSection )const {
	if ( sectionVal <= 0.f ) { return; }
	CurveSectionsT<T> rest;
	if ( section.isValid () && onSection ) {
		rest.copyFrom ( section );
	} else {
		rest.add ( T(0.0), T(1.0) );
	}
	section.clear ();

	T sectionRatio = sectionVal;
	if ( valType == 0 ) {  /* lengthタイプの場合はlengthRatioに変換*/
		sectionRatio = this->getLengthRatioFromLength ( curveID, sectionVal );
	}

	for ( u32 i = 0; i < rest.sectionCount (); i++ ) {
		T origStart = rest.getStart ( i );
		T origEnd = rest.getEnd ( i );
		T origRatio = origEnd - origStart;
		T val = sectionRatio;
		if ( valType == 2 ) { /* セクション区間に対するratioに変換*/
			val *= origRatio;
		}
		val = Math_min ( origStart + val, origEnd );
		section.add ( origStart, val );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブもしくはセクションのエンド側にセクションを作成する
//!
//!		@param[in out] section カーブセクション（上書きして返す）
//!		@param[in] valType 引数***Valのデータタイプ　0=length; 1=ratio of curveLength; 2=ratio of curveSection
//!		@param[in] onSection 引数sectionに対してセクションの計算を行う（＝既存のセクションの細分化）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BSplineCurvesT<T>::createEndSideSections ( IO CurveSectionsT<T> &section, u32 curveID, T sectionVal, u8 valType, Bool onSection )const {
	if ( sectionVal <= T(0.0) ) { return; }
	CurveSectionsT<T> rest;
	if ( section.isValid () && onSection ) {
		rest.copyFrom ( section );
	} else {
		rest.add ( T(0.0), T(1.0) );
	}
	section.clear ();

	T sectionRatio = sectionVal;
	if ( valType == 0 ) {  /* lengthタイプの場合はlengthRatioに変換*/
		sectionRatio = this->getLengthRatioFromLength ( curveID, sectionVal );
	}

	for ( u32 i = 0; i < rest.sectionCount (); i++ ) {
		T origStart = rest.getStart ( i );
		T origEnd = rest.getEnd ( i );
		T origRatio = origEnd - origStart;
		T val = sectionRatio;
		if ( valType == 2 ) { /* セクション区間に対するratioに変換*/
			val *= origRatio;
		}
		val = Burgess2MathFn::Max ( origEnd - val, origStart );
		section.add ( val, origEnd );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブもしくはセクションの両端にセクションを作成する
//!
//!		@param[in out] section カーブセクション（上書きして返す）
//!		@param[in] valType 引数***Valのデータタイプ　0=length; 1=ratio of curveLength; 2=ratio of curveSection
//!		@param[in] onSection 引数sectionに対してセクションの計算を行う（＝既存のセクションの細分化）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BSplineCurvesT<T>::createBothEndSections ( IO CurveSectionsT<T> &section, u32 curveID, T sectionVal, u8 valType, Bool onSection )const {
	if ( Burgess2MathFn::AlmostEqual( sectionVal, T(0.0)) ) { return; }
	CurveSectionsT<T> rest;
	if ( section.isValid () && onSection ) {
		rest.copyFrom ( section );
	} else {
		rest.add ( T(0.0), T(1.0) );
	}
	section.clear ();

	T sectionRatio = sectionVal;
	if ( valType == 0 ) {  /* lengthタイプの場合はlengthRatioに変換*/
		sectionRatio = this->getLengthRatioFromLength ( curveID, sectionVal );
	}

	for ( u32 i = 0; i < rest.sectionCount (); i++ ) {
		T origStart = rest.getStart ( i );
		T origEnd = rest.getEnd ( i );
		T origRatio = origEnd - origStart;
		T val = sectionRatio;
		T val2 = sectionRatio;
		if ( valType == 2 ) { /* セクション区間に対するratioに変換*/
			val *= origRatio;
			val2 *= origRatio;
		}
		val = Burgess2MathFn::Min ( origStart + val, origEnd );
		val2 = Burgess2MathFn::Max ( origEnd - val2, origStart );
		section.add ( origStart, val );
		section.add ( val2, origEnd );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブもしくはセクションを等分割するセクションを作成する
//!
//!		@param[in out] section カーブセクション（上書きして返す）
//!		@param[in] divisionSize 分割数
//!		@param[in] onSection 引数sectionに対してセクションの計算を行う（＝既存のセクションの細分化）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BSplineCurvesT<T>::createEvenlyDivideSections ( IO CurveSectionsT<T> &section, u32 curveID, T divisionSize, Bool onSection )const {
	if ( divisionSize == 0 ) { return; }
	CurveSectionsT<T> rest;
	if ( section.isValid () && onSection ) {
		rest.copyFrom ( section );
	} else {
		rest.add ( T(0.0), T(1.0) );
	}
	section.clear ();

	for ( u32 i = 0; i < rest.sectionCount (); i++ ) {
		T origStart = rest.getStart ( i );
		T origEnd = rest.getEnd ( i );
		T addRatio = ( origEnd - origStart ) / T ( divisionSize );
		T val, val2;
		for ( u32 j = 0; j < divisionSize; j++ ) {
			val = origStart + addRatio * T ( j );
			val2 = val + addRatio;
			section.add ( val, val2 );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブもしくはセクションを一定間隔を空けて分割するセクションを作成する
//!
//!		@param[in out] section カーブセクション（上書きして返す）
//!		@param[in] valType 引数***Valのデータタイプ　0=length; 1=ratio of curveLength; 2=ratio of curveSection
//!		@param[in] onSection 引数sectionに対してセクションの計算を行う（＝既存のセクションの細分化）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BSplineCurvesT<T>::createRegularIntervalSections ( IO CurveSectionsT<T> &section, u32 curveID, T sectionVal, T intervalVal, u8 valType, Bool onSection )const {
	if ( sectionVal < T(0.0) || intervalVal < T(0.0) ) { return; }
	CurveSectionsT<T> rest;
	if ( section.isValid ( ) && onSection ) {
		rest.copyFrom ( section );
	} else {
		rest.add ( T(0.0), T(1.0) );
	}
	section.clear ();

	T sectionRatio = sectionVal;
	T intervalRatio = intervalVal;
	if ( valType == 0 ) { /* lengthタイプの場合はlengthRatioに変換*/
		sectionRatio = this->getLengthRatioFromLength ( curveID, sectionVal );
		intervalRatio = this->getLengthRatioFromLength ( curveID, intervalVal );
	}

	for ( u32 i = 0; i < rest.sectionCount (); i++ ) {
		T origStart = rest.getStart ( i );
		T origEnd = rest.getEnd ( i );
		T origRatio = origEnd - origStart;

		T sectionRatio2 = sectionRatio;
		T intervalRatio2 = intervalRatio;
		if ( valType == 2 ) { /* セクション区間に対するratioに変換*/
			sectionRatio2 *= origRatio;
			intervalRatio2 *= origRatio;
		}

		u32 divisionCount = origRatio / ( sectionRatio2 + intervalRatio2 );
		T endpointRatio = T(0.0);
		for ( u32 k = 0; k<divisionCount; k++ ) {
			T lengthRatioS = origStart + endpointRatio;
			T lengthRatioE = origStart + endpointRatio + sectionRatio2;
			section.add ( lengthRatioS, lengthRatioE );
			endpointRatio += sectionRatio2 + intervalRatio2;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブもしくはセクションを動的な間隔を空けて分割するセクションを作成する
//!
//!		@param[in out] section カーブセクション（上書きして返す）
//!		@param[in] valType 引数***Valのデータタイプ　0=length; 1=ratio of curveLength; 2=ratio of curveSection
//!		@param[in] onSection 引数sectionに対してセクションの計算を行う（＝既存のセクションの細分化）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BSplineCurvesT<T>::createDynamicIntervalSections ( IO CurveSectionsT<T> &section, u32 curveID, T sectionVal, T intervalVal, u8 valType, Bool onSection )const {
	if ( sectionVal < T(0.0) || intervalVal < T(0.0) ) { return; }

	CurveSectionsT<T> rest;
	if ( section.isValid () && onSection ) {
		rest.copyFrom ( section );
	} else {
		rest.add ( T(0.0), T(1.0) );
	}
	section.clear ();

	T sectionRatio = sectionVal;
	T intervalRatio = intervalVal;
	if ( valType == 0 ) { /* lengthタイプの場合はlengthRatioに変換*/
		sectionRatio = this->getLengthRatioFromLength ( curveID, sectionVal );
		intervalRatio = this->getLengthRatioFromLength ( curveID, intervalVal );
	}
	T r = sectionVal / ( sectionVal + intervalVal ); /* addLengthをsectionとintervalそれぞれに配分する比率を計算*/

	for ( u32 i = 0; i < rest.sectionCount (); i++ ) {
		T origStart = rest.getStart ( i );
		T origEnd = rest.getEnd ( i );
		T origRatio = origEnd - origStart;

		T sectionRatio2 = sectionRatio;
		T intervalRatio2 = intervalRatio;
		if ( valType == 2 ) { /* セクション区間に対するratioに変換*/
			sectionRatio2 *= origRatio;
			intervalRatio2 *= origRatio;
		}
		u32 divisionCount = origRatio / ( sectionRatio2 + intervalRatio2 );
		T remainderRatio = origRatio - ( sectionRatio2 + intervalRatio2 ) * divisionCount;
		T addRatio = remainderRatio / divisionCount;

		if ( divisionCount == 0 ) {
			section.add ( origStart, origEnd );
		} else {
			T endpointRatio = T(0.0);
			for ( u32 k = 0; k < divisionCount; k++ ) {
				T lengthRatioS = origStart + endpointRatio;
				T lengthRatioE = origStart + endpointRatio + sectionRatio2 + addRatio * r;
				section.add ( lengthRatioS, lengthRatioE );
				endpointRatio += sectionRatio2 + intervalRatio2 + addRatio;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		カーブもしくはセクションを等分割した地点を中心にしてセクションを作成する
//!
//!		@param[in out] section カーブセクション（上書きして返す）
//!		@param[in] areaSize 分割数 ( =生成するセクションの数 )
//!		@param[in] valType 引数***Valのデータタイプ　0=length; 1=ratio of curveLength; 2=ratio of curveSection
//!		@param[in] onSection 引数sectionに対してセクションの計算を行う（＝既存のセクションの細分化）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BSplineCurvesT<T>::createAreaSections ( IO CurveSectionsT<T> &section, u32 curveID, u32 areaSize, T areaVal, u8 valType, Bool onSection )const {
	if ( areaSize == 0 || Burgess2MathFn::AlmostEqual( areaVal, T(0.0) ) ) { return; }
	CurveSectionsT<T> rest;
	if ( section.isValid () && onSection ) {
		rest.copyFrom ( section );
	} else {
		rest.add ( T(0.0), T(1.0) );
	}
	section.clear ();

	T areaRatio = areaVal;
	if ( valType == 0 ) { /* lengthタイプの場合はlengthRatioに変換*/
		areaRatio = this->getLengthRatioFromLength ( curveID, areaVal );
	}

	for ( u32 i = 0; i < rest.sectionCount (); i++ ) {
		T origStart = rest.getStart ( i );
		T origEnd = rest.getEnd ( i );
		T origRatio = origEnd - origStart;
		T val = areaRatio;
		if ( valType == 2 ) { /* セクション区間に対するratioに変換 */
			val *= origRatio;
		}
		T divRatio = origRatio / T ( areaSize+1 );
		for ( u32 k = 0; k < areaSize; k++ ) {
			T lengthRatioS = divRatio * ( k + 1 ) - val * T(0.5);
			T lengthRatioE = divRatio * ( k + 1 ) + val * T(0.5);
			lengthRatioS = Burgess2MathFn::Max ( origStart + lengthRatioS, origStart );
			lengthRatioE = Burgess2MathFn::Min ( origStart + lengthRatioE, origEnd );
			section.add ( lengthRatioS, lengthRatioE );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		セクションの長さを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
T BSplineCurvesT<T>::getSectionLength ( u32 curveID, const CurveSectionsT<T> &section, u32 sectionID )const {
	assart ( sectionID < section.sectionCount () );
	return this->getCurveLength ( curveID ) * section.getLengthRatio ( sectionID );
}