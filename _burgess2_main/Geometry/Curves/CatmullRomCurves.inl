//------------------------------------------------------------------------------------------------------------------------------------------------
//!		CatmullRomCurvesを追加する
//!		
//!		points: LoopCurve(formが1or2)の場合、重複頂点は内部で自動的に作成されるため引数"points"には含めない<br>
//!		@param[in] points カーブ頂点（LoopCurveでも重複頂点は含めない）
//!		@param[in] form 0=open, 1=close, 2=periodic
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 CatmullRomCurvesT<T>::add ( const std::vector<Vec3T<T>> &points, u8 form, T _parameterization, u8 sample ) {
	assert ( form < 3 );
	assert ( 3 <= points.size () );
	T parameterization = Burgess2MathFn::Clamp ( _parameterization, T ( 0.0 ), T ( 1.0 ) );

	std::vector<std::vector<Vec3T<T>>> initArray = this->initializeArray ( points, form );
	
	s32 c = s32(initArray.size());
	std::vector<std::vector<Vec3T<T>>> segments ( c );
	#pragma omp parallel for
	for (s32 i = 0; i < c; ++i) {
		T t[4];
		t[0] = T(0.0);
		t[1] = this->calculate_t(initArray[i][0], initArray[i][1], parameterization, T(0.0));
		t[2] = this->calculate_t(initArray[i][1], initArray[i][2], parameterization, t[1]);
		t[3] = this->calculate_t(initArray[i][2], initArray[i][3], parameterization, t[2]);

		//Bool popLast = ( i == initArray.size()-1 ) && ( form == 0 );
		std::vector<T> fillT = Burgess2MathFn::FillInterpolated(t[1], t[2], sample, false, true);
		std::vector<Vec3T<T>> res(fillT.size());
		for (u32 j = 0; j < fillT.size(); j++) {
			res[j] = this->core(fillT[j], initArray[i], t);
		}
		segments[i] = res;
	}

	std::vector<Vec3T<T>> catmullRomPoints = this->segmentsToSpline ( segments );
	if(form == 0){ 
		catmullRomPoints.push_back ( points [points.size ()-1] );
	}
	// BSplineCurve degree=2, sample=2
	u32 id = this->BSplineCurvesT<T>::add (catmullRomPoints, 2, form );
	this->data [id].setSample ( 2 );
	return id;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<std::vector<Vec3T<T>>> CatmullRomCurvesT<T>::initializeArray ( const std::vector<Vec3T<T>> &ctrlPoints, u8 form ) {
	std::vector<std::vector<Vec3T<T>>>  result ( ctrlPoints.size ()-1 );
	for ( u32 i = 0; i<ctrlPoints.size ()-1; i++ ) {
		result [i].resize ( 4 );
		//  カーブタイプが閉じ/開きでindexの取得方法を切り替える
		u32 index [4];
		if ( form != 0 && i == 0 ) {
			index [0] = u32(ctrlPoints.size () - 1);
			index [1] = 0;
		} else {
			if ( i==0 ) {
				index [0] = i;
				index [1] = index [0];
			} else {
				index [0] = i - 1;
				index [1] = index [0] + 1;
			}
		}
		index [2] = index [1] + 1;
		if ( form != 0 && i == ctrlPoints.size ()-2 ) {
			index [3] = 0;
		} else {
			if ( i == ctrlPoints.size ()-2 ) {
				index [3] = index [2];
			} else {
				index [3] = index [2] + 1;
			}
		}

		Vec3T<T> p0 = ctrlPoints [index [0]];
		Vec3T<T> p1 = ctrlPoints [index [1]];
		Vec3T<T> p2 = ctrlPoints [index [2]];
		Vec3T<T> p3 = ctrlPoints [index [3]];

		if ( form == 0 && p0 == p1 ) {
			result [i] [0] = p0 + ( p0 - p2 );
		} else {
			result [i] [0] = p0;
		}
		result [i] [1] = p1;
		result [i] [2] = p2;
		if ( form == 0 && p2 == p3 ) {
			result [i] [3] = p3 + ( p3 - p1 );
		} else {
			result [i] [3] = p3;
		}

		if ( ctrlPoints.size ()-2 <= i ) { break; }
	}

	if ( form != 0 ) {
		result.resize ( result.size () + 1 );
		result[result.size()-1].resize ( 4 );
		result[result.size()-1] [0] = ctrlPoints [ctrlPoints.size ()-2];
		result[result.size()-1] [1] = ctrlPoints [ctrlPoints.size ()-1];
		result[result.size()-1] [2] = ctrlPoints [0];
		result[result.size()-1] [3] = ctrlPoints [1];
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
T CatmullRomCurvesT<T>::calculate_t ( const Vec3T<T> &p0, const Vec3T<T> &p1, T alpha, T t0 ) {
	if (p0.almostEqual(p1)) {
		return t0 + T(0.00001);
	}
	T s = SQ ( p1.x()-p0.x() ) + SQ ( p1.y()-p0.y() ) + SQ ( p1.z()-p0.z() );
	return std::pow ( std::sqrt ( s ), alpha ) + t0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> CatmullRomCurvesT<T>::core ( T fillT, const std::vector<Vec3T<T>> &p, T t [4] ) {
	Vec3T<T> mulVal0 = Vec3T<T> ( ( t [1]-fillT ) / ( t [1]-t [0] ) );
	Vec3T<T> mulVal1 = Vec3T<T> ( ( fillT-t [0] ) / ( t [1]-t [0] ) );
	Vec3T<T> mulVal2 = Vec3T<T> ( ( t [2]-fillT ) / ( t [2]-t [1] ) );
	Vec3T<T> mulVal3 = Vec3T<T> ( ( fillT-t [1] ) / ( t [2]-t [1] ) );
	Vec3T<T> mulVal4 = Vec3T<T> ( ( t [3]-fillT ) / ( t [3]-t [2] ) );
	Vec3T<T> mulVal5 = Vec3T<T> ( ( fillT-t [2] ) / ( t [3]-t [2] ) );

	Vec3T<T> a1 = p [0]*mulVal0 + p [1]*mulVal1;
	Vec3T<T> a2 = p [1]*mulVal2 + p [2]*mulVal3;
	Vec3T<T> a3 = p [2]*mulVal4 + p [3]*mulVal5;

	Vec3T<T> mulVal6 = Vec3T<T> ( ( t [2]-fillT ) / ( t [2]-t [0] ) );
	Vec3T<T> mulVal7 = Vec3T<T> ( ( fillT-t [0] ) / ( t [2]-t [0] ) );
	Vec3T<T> mulVal8 = Vec3T<T> ( ( t [3]-fillT ) / ( t [3]-t [1] ) );
	Vec3T<T> mulVal9 = Vec3T<T> ( ( fillT-t [1] ) / ( t [3]-t [1] ) );

	Vec3T<T> b1 = a1*mulVal6 + a2*mulVal7;
	Vec3T<T> b2 = a2*mulVal8 + a3*mulVal9;

	Vec3T<T> c = b1*mulVal2 + b2*mulVal3;
	return c;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec3T<T>> CatmullRomCurvesT<T>::segmentsToSpline ( const std::vector<std::vector<Vec3T<T>>> &segments ) {
	std::vector<Vec3T<T>> result;
	result.reserve(segments.size() * segments[0].size());
	for ( u32 i = 0; i < segments.size (); i++ ) {
		for ( u32 j = 0; j < segments [i].size (); j++ ) {
			result.push_back ( segments [i] [j] );
		}
	}
	return result;
}
