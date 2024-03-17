//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     デフォルトコンストラクタ
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BBoxT<T>::BBoxT () 
	: min { 0 }
	, max { 0 } 
{};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BBoxT<T>::BBoxT ( const Vec3T<T> &val0, const Vec3T<T> &val1 ) {
	assert ( Math::AlmostEqual ( val0.x, val1.x ) == false || 
		Math::AlmostEqual ( val0.y, val1.y ) == false || 
		Math::AlmostEqual ( val0.z, val1.z ) == false );
	this->min = val0.getMin ( val1 );
	this->max = val0.getMax ( val1 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BBoxT<T>::BBoxT ( const BBoxT<T> &other ) 
	: min { other.min }
	, max { other.max } 
{};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BBoxT<T>::BBoxT ( const std::vector<Vec3T<T>> &points ) {
	assert ( 0 != points.size () ); 
	this->min = points [0];
	this->max = points [0];
	for ( int i = 1; i < points.size (); i++ ) {
		this->merge ( points [i] );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ 正六面体のBBoxを作成
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BBoxT<T>::BBoxT ( const Vec3T<T> &centerPosition, T sideLength ) {
	this->min = centerPosition - Vec3T<T> ( sideLength * T ( 0.5 ) );
	this->max = centerPosition + Vec3T<T> ( sideLength * T ( 0.5 ) );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     空のボックスにする
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void BBoxT<T>::setEmpty () {
	this->min = Vec3T<T> { T ( 0.0 ) };
	this->max = Vec3T<T> { T ( 0.0 ) };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     デバッグ用の文字列にする　"データ型：[ 値 ]"
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
String BBoxT<T>::toString ( const String title ) const {
	String t = title;
	if ( title.length () != 0 ) { t += " : "; }
	String mi = "min Vec3[" + GetFloorString ( this->min.x ) + ", " + GetFloorString ( this->min.y ) + ", " + GetFloorString ( this->min.z ) + "] / ";
	String ma =  "max Vec3["+ GetFloorString ( this->max.x ) + ", " + GetFloorString ( this->max.y ) + ", " + GetFloorString ( this->max.z ) + "]";
	return t + "BBox:[" + mi + " / " + ma +  "]";
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ポジションをマージする
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void BBoxT<T>::merge ( const Vec3T<T> &position ) {
	this->min = this->min.getMin ( position );
	this->max = this->max.getMax ( position );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     他のBBoxをマージする
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void BBoxT<T>::merge ( const BBoxT &other ) {
	this->min = this->min.getMin ( other.min );
	this->max = this->max.getMax ( other.max );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     BBoxの中心位置を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BBoxT<T>::getCenter () const {
	return ( this->min + this->max ) * T ( 0.5 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     BBoxの対角線を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BBoxT<T>::getDiagonal () const{
	return ( this->max - this->min );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     BBoxの対角距離を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BBoxT<T>::getDiameter () const {
	return this->getDiagonal ().length ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     BBOxの体積を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BBoxT<T>::getVolume () const {
	return T(
		( this->max.x - this->min.x ) * 
		( this->max.y - this->min.y ) * 
		( this->max.z - this->min.z ) );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     BBOxの最も長い辺の長さを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T BBoxT<T>::getLongestSideLength () const {
	Vec3T<T> diagonal = this->getDiagonal ();
	T result = diagonal.x;
	if (result < diagonal.y) { result = diagonal.y; }
	if (result < diagonal.z) { result = diagonal.z; }
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     空のボックスの場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BBoxT<T>::isEmpty () const {
	return 
		this->max.x <= this->min.x && 
		this->max.y <= this->min.y && 
		this->max.z <= this->min.z;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     pがボックス内に含まれる場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BBoxT<T>::testInside ( const Vec3T<T> &p )const {
	return
		this->min.x < p.x &&
		this->min.y < p.y &&
		this->min.z < p.z &&
		p.x < this->max.x &&
		p.y < this->max.y &&
		p.z < this->max.z;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     他のバウンディングボックスと交差する場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BBoxT<T>::testIntersect ( const BBoxT<T> &other )const {
	if (Math::AlmostEqual(this->max.x, other.min.x)) return true;
	if (Math::AlmostEqual(this->max.y, other.min.y)) return true;
	if (Math::AlmostEqual(this->max.z, other.min.z)) return true;
	if (Math::AlmostEqual(other.max.x, this->min.x)) return true;
	if (Math::AlmostEqual(other.max.y, this->min.y)) return true;
	if (Math::AlmostEqual(other.max.z, this->min.z)) return true;
	if (this->max.x < other.min.x) return false;
	if (this->max.y < other.min.y) return false;
	if (this->max.z < other.min.z) return false;
	if (other.max.x < this->min.x) return false;
	if (other.max.y < this->min.y) return false;
	if (other.max.z < this->min.z) return false;
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Rayとの交差を判定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BBoxT<T>::testRayIntersect ( const Ray3T<T> &ray ) const {
	Vec3T<T> intarsect = this->getRayIntersectPosition ( ray );
	return intarsect.almostEqual( Vec3T<T> { T(Burgess::F32_INFINITE) } ) == false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     Rayと交差する位置を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BBoxT<T>::getRayIntersectPosition ( const Ray3T<T> &ray ) const {
	Vec3T<T> result;
	std::vector<T> low = this->min.toTypeArray ();
	std::vector<T> upp = this->max.toTypeArray ();
	T rayParam = T(Burgess::F32_INFINITE);
	for ( u32 faceId = 1; faceId<=6; faceId++ ) {
		Vec3T<T> normal { T ( 0.0 ) };
		T D;
		switch ( faceId ) {
			case 1:/// -x face
				D = this->min.x * T(-1.0); 
				normal.x = T(-1.0);
				break;
			case 2:/// -y face
				D = this->min.y * T(-1.0);
				normal.y = T(-1.0);
				break;
			case 3:/// -z face
				D = this->min.z * T(-1.0);
				normal.z = T(-1.0);
				break;
			case 4:/// +x face
				D = this->max.x; 
				normal.x = T(1.0);
				break;
			case 5:/// +y face
				D = this->max.y; 
				normal.y = T(1.0);
				break;
			case 6:/// +z face
				D = this->max.z; 
				normal.z = T(1.0);
		}

		T sDen = ray.getDirection().dot (normal);
		if ( Burgess::TOLERANCE < std::abs ( sDen ) ) {
			T sNum = D - ray.getStartPosition ().dot (normal);
			T s = sNum / sDen;
			result = ray.getPositionOnRay ( s );
			std::vector<T> p = result.toTypeArray ();

			int j, k;
			if ( faceId==1 || faceId==4 ) { k = 1; j = 2; } // -x,+x
			else if ( faceId==2 || faceId==5 ) { k = 0; j = 2; } // -y,+y
			else if ( faceId==3 || faceId==6 ) { k = 0; j = 1; } // -z,+z
			if ( ( p [k]>=low [k] && p [k]<=upp [k] ) && ( p [j]>=low [j] && p [j]<=upp [j] ) ) {
				if(s < rayParam){
					rayParam = s;
				}
			}
		}
	}

	if (Math::AlmostEqual( rayParam, T(Burgess::F32_INFINITE) ) ) {
		return Vec3T<T> { T(Burgess::F32_INFINITE) };
	}else{
		return ray.getPositionOnRay ( rayParam );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定距離内にボックスが存在するか判定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BBoxT<T>::testClosest ( const Vec3T<T> &position, T maxDistance ) const {
	if ( this->testInside ( position ) ) { return true; }
	Vec3T<T> closest = this->getClosestPosition ( position, maxDistance );
	return closest != Vec3T<T> { T(Burgess::F32_INFINITE) };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     指定距離内でボックス表面上への最も近い座標を検索する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Vec3T<T> BBoxT<T>::getClosestPosition ( const Vec3T<T> &position, T maxDistance ) const {
	Vec3T<T> result { T(Burgess::F32_INFINITE) };
	Vec3T<T> boxPos = this->getCenter ();
	T radiusSQ = ( Vec3T<T> { boxPos.x, 0, 0 } - Vec3T<T> { this->max.x, 0, 0 } ).length();
	if ( ( position - boxPos ).length() < radiusSQ ) { return result; }
	Vec3T<T> dirPosToBox = ( position - boxPos ).unit ();

	f32 restDis = T(Burgess::F32_INFINITE);
	for ( u8 i = 0; i < 12; i++ ) {
		Vec3T<T> p0;
		Vec3T<T> p1;
		Vec3T<T> p2;
		switch ( i ) {
			case 0:
				if ( dirPosToBox.dot ( Vec3T<T> { 0, 0, 1 } ) < T ( 0.0 ) ) { continue; }
				p0 = max;
				p1 = Vec3T<T> { min.x, max.y, max.z };
				p2 = Vec3T<T> { min.x, min.y, max.z };
				break;
			case 1:
				if ( dirPosToBox.dot ( Vec3T<T> { 0, 0, 1 } ) < T ( 0.0 ) ) { continue; }
				p0 = max;
				p1 = Vec3T<T> { min.x, min.y, max.z };
				p2 = Vec3T<T> { max.x, min.y, max.z };
				break;
			case 2:
				if ( dirPosToBox.dot ( Vec3T<T> { 1, 0, 0 } ) < T ( 0.0 ) ) { continue; }
				p0 = Vec3T<T> { max.x, max.y, min.z };
				p1 = max;
				p2 = Vec3T<T> { max.x, min.y, max.z };
				break;
			case 3:
				if ( dirPosToBox.dot ( Vec3T<T> { 1, 0, 0 } ) < T ( 0.0 ) ) { continue; }
				p0 = Vec3T<T> { max.x, max.y, min.z };
				p1 = Vec3T<T> { max.x, min.y, max.z };
				p2 = Vec3T<T> { max.x, min.y, min.z };
				break;
			case 4:
				if ( dirPosToBox.dot ( Vec3T<T> ( 0, 0, -1 ) ) < T( 0.0 ) ) { continue; }
				p0 = Vec3T<T> { min.x, max.y, min.z };
				p1 = Vec3T<T> { max.x, max.y, min.z};
				p2 = Vec3T<T> { max.x, min.y, min.z };
				break;
			case 5:
				if ( dirPosToBox.dot ( Vec3T<T> ( 0, 0, -1 ) ) < T( 0.0 ) ) { continue; }
				p0 = Vec3T<T> { min.x, max.y, min.z };
				p1 = Vec3T<T> { max.x, min.y, min.z };
				p2 = min;
				break;
			case 6:
				if ( dirPosToBox.dot ( Vec3T<T> ( -1, 0, 0 ) ) < T( 0.0 ) ) { continue; }
				p0 = Vec3T<T> { min.x, max.y, max.z };
				p1 = Vec3T<T> { min.x, max.y, min.z };
				p2 = min;
				break;
			case 7:
				if ( dirPosToBox.dot ( Vec3T<T> ( -1, 0, 0 ) ) < T( 0.0 ) ) { continue; }
				p0 = Vec3T<T> { min.x, max.y, max.z };
				p1 = min;
				p2 = Vec3T<T> { min.x, min.y, max.z };
				break;
			case 8:
				if ( dirPosToBox.dot ( Vec3T<T> ( 0, 1, 0 ) ) < T( 0.0 ) ) { continue; }
				p0 = Vec3T<T> { max.x, max.y, min.z };
				p1 = Vec3T<T> { min.x, max.y, min.z };
				p2 = Vec3T<T> { min.x, max.y, max.z };
				break;
			case 9:
				if ( dirPosToBox.dot ( Vec3T<T> ( 0, 1, 0 ) ) < T( 0.0 ) ) { continue; }
				p0 = Vec3T<T> { max.x, max.y, min.z };
				p1 = Vec3T<T> { min.x, max.y, max.z };
				p2 = max;
				break;
			case 10:
				if ( dirPosToBox.dot ( Vec3T<T> ( 0, -1, 0 ) ) < T( 0.0 ) ) { continue; }
				p0 = Vec3T<T> { max.x, min.y, min.z };
				p1 = min;
				p2 = Vec3T<T> { min.x, min.y, max.z };
				break;
			case 11:
				if ( dirPosToBox.dot ( Vec3T<T> ( 0, -1, 0 ) ) < T( 0.0 ) ) { continue; }
				p0 = Vec3T<T> { max.x, min.y, min.z };
				p1 = Vec3T<T> { min.x, min.y, max.z };
				p2 = Vec3T<T> { max.x, min.y, max.z};
				break;
		}
		Vec3T<T> closestPos = Math::GetClosestPositionOnTriangle ( position, p0, p1, p2 );
		T disSQ = ( position - closestPos ).length();
		if ( disSQ < maxDistance && disSQ < restDis) {
			result = closestPos;
			restDis = disSQ;
		}
	}
	//report ( result, "22222222222222" );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BBoxT<T>::operator == ( const BBoxT &other ) const {
	return this->min == other.min && this->max == other.max;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool BBoxT<T>::operator != ( const BBoxT &other ) const{
	return this->min != other.min || this->max != other.max;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BBoxT<T> BBoxT<T>::operator * ( const Mat4T<T> &mat4 ) const {
	BBoxT<T> result;
	BBoxT<T> box = *this;

	if ( !box.isEmpty () ) {
		result += ( mat4 * Vec3 { box.min.x, box.min.y, box.min.z });
		result += ( mat4 * Vec3 { box.max.x, box.min.y, box.min.z} );
		result += ( mat4 * Vec3 { box.min.x, box.max.y, box.min.z} );
		result += ( mat4 * Vec3 { box.min.x, box.min.y, box.max.z} );
		result += ( mat4 * Vec3 { box.min.x, box.max.y, box.max.z} );
		result += ( mat4 * Vec3 { box.max.x, box.min.y, box.max.z} );
		result += ( mat4 * Vec3 { box.max.x, box.max.y, box.min.z} );
		result += ( mat4 * Vec3 { box.max.x, box.max.y, box.max. z} );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BBoxT<T> BBoxT<T>::operator * ( const XfoT<T> &Xfo ) const {
	Mat4T<T> m4 = Xfo.toMat4 ();
	return this * m4;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BBoxT<T> BBoxT<T>::operator * ( const Vec3T<T> &scale ) const {
	Mat4T<T> m4;
	m4.setTranslation ( scale );
	return this * m4;
}
