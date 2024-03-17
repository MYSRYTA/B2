//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
RBFControlPointT<T>::RBFControlPointT ( const Vec3T<T> &driverLocation, const std::vector<Vec3T<T>> &drivenValues )
	: driverLocation { driverLocation.toVec () } 
{
	for ( s32 i = 0; i<drivenValues.size (); i++ ) {
		this->drivenValues.combine ( drivenValues [i].toVec () );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
RBFControlPointT<T>::RBFControlPointT ( const Vec3T<T> &driverLocation, const std::vector<Mat4T<T>> &drivenValues )
	: driverLocation { driverLocation.toVec () } 
{
	for ( s32 i = 0; i<drivenValues.size (); i++ ) {
		this->drivenValues.combine ( drivenValues [i].toVec () );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
RBFControlPointT<T>::RBFControlPointT ( const Vec4T<T> &driverLocation, const std::vector<Vec3T<T>> &drivenValues )
	: driverLocation { driverLocation.toVec () } {
	for ( s32 i = 0; i<drivenValues.size (); i++ ) {
		this->drivenValues.combine ( drivenValues [i].toVec () );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
RBFControlPointT<T>::RBFControlPointT ( const Vec4T<T> &driverLocation, const std::vector<Mat4T<T>> &drivenValues )
	: driverLocation { driverLocation.toVec () } {
	for ( s32 i = 0; i<drivenValues.size (); i++ ) {
		this->drivenValues.combine ( drivenValues [i].toVec () );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ：RBFカーネルとネットワークを構築する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
RBFT<T>::RBFT ( const std::vector<RBFControlPointT<T>> &controlPoint, u8 type, T smooth, T epsilon ){
	this->setup ( controlPoint, type, smooth, epsilon );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ：RBFカーネルとネットワークを構築する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void RBFT<T>::setup ( const std::vector<RBFControlPointT<T>> &controlPoint, u8 _type, T smooth, T _epsilon ){
	if (controlPoint.size () == 0) { return; }
	this->type = _type;
	this->epsilon = _epsilon;
	s32 check0 = controlPoint[0].driverLocation.size ();
	s32 check1 = controlPoint[0].drivenValues.size ();
	for (s32 i = 1; i < controlPoint.size (); i++) {
		if (check0 != controlPoint[i].driverLocation.size () || check1 != controlPoint[i].drivenValues.size ()) {
			return;
		}
	}

	std::vector<VecT<T>> driverLocationPack ( controlPoint.size () );
	std::vector<VecT<T>> drivenVluesPack ( controlPoint.size () );
	for (s32 i = 0; i < controlPoint.size (); i++) {
		driverLocationPack[i] = controlPoint[i].driverLocation;
		drivenVluesPack[i] = controlPoint[i].drivenValues;
	}
	MatT<T> P{ driverLocationPack };
	MatT<T> F{ drivenVluesPack };
	if (P.rowSize () != F.rowSize ()) { return; }

	MatT<T> A{ P.rowSize () + P.colSize () + 1 };

	T d = T ( 0.0 );
	#pragma omp parallel for reduction(+:d)
	for (s32 h = 0; h < P.rowSize (); h++) {
		for (s32 i = h; i < P.rowSize (); i++) {
			T dist = T ( 0.0 );
			for (s32 j = 0; j < P.colSize (); j++) {
				T v = P.get ( h, j ) - P.get ( i, j );
				dist += SQ ( v );
			}
			dist = std::sqrt ( dist );
			T rbfDistance = this->norm ( dist );
			d += dist * T ( 2.0 );
			A.setValue ( h, i, rbfDistance );
			A.setValue ( i, h, rbfDistance );
		}
	}

	// 平坦化
	if (T ( 0.0 ) < smooth) {
		d /= T ( P.rowSize () * P.rowSize () );
		T s = smooth * d * d;
		for (s32 i = 0; i < P.rowSize (); i++) {
			A.setValue ( i, i, A.get ( i, i ) + s );
		}
	}

	MatT<T> B{ P.rowSize (), P.colSize () + 1 };
	B.setColAllValues ( 0, T ( 1.0 ) );
	#pragma omp parallel for if( 1000 < P.rowSize () )
	for (s32 i = 0; i < P.rowSize (); i++) {
		for (s32 j = 0; j < P.colSize (); j++) {
			B.setValue ( i, 1 + j, P.get ( i, j ) );
		}
	}

	#pragma omp parallel for if( 1000 < P.rowSize () )
	for (s32 i = 0; i < P.rowSize (); i++) {
		for (s32 j = 0; j < P.colSize () + 1; j++) {
			A.setValue ( i, P.rowSize () + j, B.get ( i, j ) );
		}
	}

	MatT<T> C = B.transpose ();
	for (s32 i = 0; i < P.colSize () + 1; i++) {
		for (s32 j = 0; j < P.rowSize (); j++) {
			A.setValue ( P.rowSize () + i, j, C.get ( i, j ) );
		}
	}

	MatT<T> Y{ F.rowSize () + P.colSize () + 1, F.colSize () };
	#pragma omp parallel for if( 1000 < F.rowSize () )
	for (s32 i = 0; i < F.rowSize (); i++) {
		for (s32 j = 0; j < F.colSize (); j++) {
			Y.setValue ( i, j, F.get ( i, j ) );
		}
	}

	// LAPACKのLUを使用 (学習させるポーズ数が2個の場合に不具合が発生する)
	/*LUDecompositionT<T> LUD{ A };
	MatT<T> X{ Y.rowSize(), Y.colSize() };
	#pragma omp parallel for if( 1000 < Y.colSize () )
	for ( s32 i = 0; i < s32( Y.colSize() ); i++ ){
		VecT<T> b = Y.getColValue( i );
		VecT<T> x = LUD.solve ( b );
		X.setColValues( i, x );
	}*/

	// Eigen
	Eigen::FullPivLU <Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> lu( A );
	MatT<T> X{ Y.rowSize (), Y.colSize () };
	#pragma omp parallel for if( 1000 < Y.colSize () )
	for (s32 i = 0; i < Y.colSize (); i++) {
		VecT<T> b = Y.getColValue ( i );
		MatT<T> xM{ lu.solve( b ) };
		VecT<T> x = xM.getColValue(0);
		X.setColValues ( i, x );
	}

	this->W.reset ( F.rowSize (), F.colSize () );
	#pragma omp parallel for if( 1000 < F.rowSize () )
	for (s32 i = 0; i < F.rowSize (); i++) {
		for (s32 j = 0; j < F.colSize (); j++) {
			this->W.setValue ( i, j, X.get ( i, j ) );
		}
	}

	this->V.reset ( P.colSize () + 1, F.colSize () );
	for (s32 i = 0; i < P.colSize () + 1; i++) {
		for (s32 j = 0; j < F.colSize (); j++) {
			this->V.setValue ( i, j, X.get ( F.rowSize () + i, j ) );
		}
	}
	this->V = this->V.transpose ();

	this->P = P;
	this->initialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    RBF補間を実行する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> RBFT<T>::solve ( const VecT<T> &driverPose ) const {
	if ( this->initialized == false || driverPose.size () != this->P.colSize() ) { return VecT<T>(0); }
	
	VecT<T> values(this->W.colSize());

	s32 cSize = this->W.colSize();
	s32 rSize = this->P.rowSize ();
	s32 cSize2 = this->P.colSize();
	#pragma omp parallel for if( 1000 < cSize )
	for (s32 h = 0; h < cSize; h++) {
		for (s32 i = 0; i < rSize; i++) {
			T dist = T(0.0);
			for (s32 j = 0; j < cSize2; j++) {
				T v = driverPose [j] - this->P.get ( i, j );
				dist += SQ ( v );
			}
			T rbfDistance = this->norm ( std::sqrt ( dist ) );
			values[h] = values.get(h) + rbfDistance * this->W.get(i, h);
		}
	}

	VecT<T> driverVec{ driverPose };
	driverVec.push_top ( T(1.0) );
	values += this->V * driverVec;
	return values;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    RBF補間を実行する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> RBFT<T>::solve ( const Vec3T<T> &driverPose ) const {
	return this->solve ( driverPose.toVec () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    RBF補間を実行する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> RBFT<T>::solve ( const Vec4T<T> &driverPose ) const {
	return this->solve ( driverPose.toVec () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T RBFT<T>::norm ( T radius ) const {
	switch ( this->type ) {
	case 0: /// Zero
		return 0;

	case 1: /// Linear
		return radius;

	case 2:/// Cubic
		return radius * radius * radius;

	case 3:/// MultiQuadric
		return std::sqrt ( SQ ( radius * T ( 0.1 ) ) + T ( 1.0 ) );

	case 4:/// Quintic
		return radius * radius * radius * radius * radius;

	case 5:/// Gaussian
		return std::exp ( -SQ ( radius * this->epsilon ) );

	case 6:/// ThinPlate
		return radius < T ( 1.0 ) ? std::log ( std::pow (radius, radius ) ) : SQ ( radius ) * std::log ( radius );

	default:
		return radius;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     デバッグ用の文字列にする　"データ型：[ 値 ]"
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
String RBFControlPointT<T>::toString ( const String title ) const {
	String t = title;
	if ( title.length () != 0 ) { t += " : "; }
	String locString = "driverLocation = " + this->driverLocation.toString();
	String valString = "drivenValues = " + this->drivenValues.toString ();
	return t + "RBFControlPoint:[" + locString + " / " + valString + "]";
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     デバッグ用の文字列にする　"データ型：[ 値 ]"
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
String RBFT<T>::toString ( const String title ) const {
	String t = title;
	if ( title.length () != 0 ) { t += " : "; }
	String typeString = "type " + std::to_string ( this->type ) + "(";
	switch ( this->type ) {
		case 0: typeString += "Zero";	break;
		case 1: typeString += "Linear";	break;
		case 2: typeString += "Cubic";	break;
		case 3: typeString += "MultiQuadric";	break;
		case 4: typeString += "Quintic";	break;
		case 5: typeString += "Gaussian";	break;
		case 6: typeString += "ThinPlate";	break;
	}
	typeString += ")";
	return t + "RBF:[V " + this->V.toString () + " / C " + this->P.toString () + " / W " + this->W.toString () + " / " + typeString + " / epsilon " + GetFloorString ( this->epsilon ) +  "]";
}