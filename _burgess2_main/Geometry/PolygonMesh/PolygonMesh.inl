//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T>::PolygonMeshT () {
	this->attr.getOrCreatVec3Attribute ( "pointPositions", 0 );
	this->attr.getOrCreatVec3Attribute ( "normals", 0 );
	this->attr.getOrCreateL16ArrayU32Attribute ( "sharedVertexIDs", 0 );
	this->attr.getOrCreateU32Attribute ( "polygonSizes", 1 ); /* [0]=0 */
	this->attr.getOrCreateU32Attribute ( "packedPointIDs", 0 );
	this->attr.getOrCreateU32Attribute ( "vertexPolygonID", 0 );
	this->modelingMode = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    複製
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T> PolygonMeshT<T>::clone () const{
	PolygonMeshT<T> result;

	Vec3AttributeT<T> &rPoiPosAttr = result.getPointPositionAttribute ();
	Vec3AttributeT<T>& pAttr = this->attr.getVec3Attribute ( "pointPositions" );
	rPoiPosAttr.resetValues ( pAttr.getValuesRef() );

	Vec3AttributeT<T> &normalAttr = result.getNormalAttribute ();
	Vec3AttributeT<T>& nAttr = this->attr.getVec3Attribute ( "normals" );
	normalAttr.resetValues ( nAttr.getValuesRef() );

	U32AttributeT<T> &sPolySizeAttr = result.getU32Attribute ( "polygonSizes" );
	U32AttributeT<T>& psAttr = this->getU32Attribute ( "polygonSizes" );
	sPolySizeAttr.resetValues ( psAttr.getValuesRef() );

	U32AttributeT<T> &packedPoiIDAttr = result.getU32Attribute ( "packedPointIDs" );
	U32AttributeT<T> &pp = this->getU32Attribute ( "packedPointIDs" );
	packedPoiIDAttr.resetValues ( pp.getValuesRef () );

	U32AttributeT<T>& vertexPolygonIDAttr = result.getU32Attribute( "vertexPolygonID" );
	U32AttributeT<T>& vp = this->getU32Attribute( "vertexPolygonID" );
	vertexPolygonIDAttr.resetValues( vp.getValuesRef() );

	L16ArrayU32AttributeT<T> &sharedVerIDAttr = result.getL16ArrayU32Attribute ( "sharedVertexIDs" );
	L16ArrayU32AttributeT<T> &sv = this->getL16ArrayU32Attribute ( "sharedVertexIDs" );
	sharedVerIDAttr.resetValues ( sv.getValuesRef () );

	std::vector<String> uvNames = this->getAllUVNames ();
	for ( u32 i = 0; i<uvNames.size (); i++ ) {
		result.addUVMap ( uvNames [i] );
		U32AttributeT<T> &packedUVPoiIDAttr = this->uvs.getU32Attribute ( uvNames [i] + "packedUVVertexIDs" );
		result.resetAllUVValues ( uvNames [i], this->getAllUVValues ( uvNames [i] ), packedUVPoiIDAttr.getValuesRef() );
	}

	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    トポロジーのみ複製
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T> PolygonMeshT<T>::cloneTopology () const{
	PolygonMeshT<T> result;

	Vec3AttributeT<T> &rPoiPosAttr = result.getPointPositionAttribute ();
	Vec3AttributeT<T>& pAttr = this->attr.getVec3Attribute ( "pointPositions" );
	rPoiPosAttr.resetValues ( pAttr.getValuesRef () );

	U32AttributeT<T> &sPolySizeAttr = result.getU32Attribute ( "polygonSizes" );
	U32AttributeT<T>& psAttr = this->getU32Attribute ( "polygonSizes" );
	sPolySizeAttr.resetValues ( psAttr.getValuesRef () );

	U32AttributeT<T> &packedPoiIDAttr = result.getU32Attribute ( "packedPointIDs" );
	U32AttributeT<T> &pp = this->getU32Attribute ( "packedPointIDs" );
	packedPoiIDAttr.resetValues ( pp.getValuesRef () );

	U32AttributeT<T>& vertexPolygonIDAttr = result.getU32Attribute( "vertexPolygonID" );
	U32AttributeT<T>& vp = this->getU32Attribute( "vertexPolygonID" );
	vertexPolygonIDAttr.resetValues( vp.getValuesRef() );

	L16ArrayU32AttributeT<T> &sharedVerIDAttr = result.getL16ArrayU32Attribute ( "sharedVertexIDs" );
	L16ArrayU32AttributeT<T> &sv = this->getL16ArrayU32Attribute ( "sharedVertexIDs" );
	sharedVerIDAttr.resetValues ( sv.getValuesRef () );

	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    トランスフォーム
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::applyTransform ( const Mat4T<T> &transform ) {
	Vec3Attribute &poiPos = this->getPointPositionAttribute ();
	#pragma omp parallel for
	for ( s32 i = 0; i<poiPos.size (); i++ ) {
		poiPos[i] = transform * poiPos [i];
	}

	if ( this->hasNormal () ){
		Mat4T<T> nTransform = transform;
		nTransform.setTranslation ( Vec3T<T> ( T ( 0.0 ) ) );
		nTransform.setScaling ( Vec3T<T> ( T ( 1.0 ) ) );
		Vec3Attribute &normAttr = this->getNormalAttribute ();
		#pragma omp parallel for
		for ( s32 i = 0; i < normAttr.size (); i++ ){
			normAttr[i] = nTransform * normAttr[i];
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    トランスフォーム
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::applyTransform ( const XfoT<T> &transform ) {
	Vec3Attribute &poiPos = this->getPointPositionAttribute ();
	#pragma omp parallel for
	for ( s32 i = 0; i<poiPos.size (); i++ ) {
		poiPos [i] = transform * poiPos [i];
	}

	if ( this->hasNormal () ){
		XfoT<T> nTransform = transform;
		nTransform.tr = Vec3T<T> ( T ( 0.0 ) );
		nTransform.sc = Vec3T<T> ( T ( 1.0 ) );
	
		Vec3Attribute &normAttr = this->getNormalAttribute ();
		#pragma omp parallel for
		for ( s32 i = 0; i < normAttr.size (); i++ ){
			normAttr[i] = nTransform * normAttr[i];
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    トポロジーが同じ場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::sameTopology ( const PolygonMeshT<T> &other )const{
	if ( this->pointCount () != other.pointCount () ){ return false; }
	if ( this->polygonCount () != other.polygonCount () ){ return false; }
	
	U32AttributeT<T> &thisPackedPointIDs = this->getU32Attribute ( "packedPointIDs" );
	U32AttributeT<T> &otherPackedPointIDs = other.getU32Attribute ( "packedPointIDs" );
	if ( thisPackedPointIDs.size () != otherPackedPointIDs.size () ){ return false; }
	for ( u32 i = 0, n = thisPackedPointIDs.size (); i < n; ++i ){
		if ( thisPackedPointIDs[i] != otherPackedPointIDs[i] ){
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    スムース
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
void PolygonMeshT<T>::smooth ( u8 iteration, T t ){
	if ( 0 == iteration ){ return; }

	for ( u8 ite = 0; ite < iteration; ++ite ){
		std::vector < Vec3T<T> > positionsBuffer = this->getAllPointPositions ();

		s32 poiCnt = this->pointCount ();
		#pragma omp parallel for
		for(s32 index =0; index<poiCnt; ++index ){
			L16Array< u32 > poiIDs = this->getPointSurroundingPoints ( index, false );
			Vec3T<T> add{ T ( 0.0 ) };
			for ( u32 j = 0; j < poiIDs.size(); ++j ){
				add += positionsBuffer[poiIDs[j]];
			}
			add = add / f32 ( poiIDs.size() ) * t;
			this->setPointPosition ( index, positionsBuffer[index] + add );
		}
	}
}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    全てのフェースが3角形ポリゴンであればtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::isPureTriangleMesh ()const{
	U32AttributeT<T>& polySizeAttr = this->getU32Attribute ( "polygonSizes" );
	U32AttributeT<T>& poiIDsAttr = this->getU32Attribute ( "packedPointIDs" );
	u32 a = poiIDsAttr.size () / 3;
	return a == polySizeAttr.size() && a * 3 == poiIDsAttr.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    メッシュ表面上に均一なロケーションデータを生成する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<GeometryLocationT<T>> PolygonMeshT<T>::generateLocationPerFace ( u32 detail ){
	std::vector<GeometryLocationT<T>> result ( 0 );
	if ( detail == 0 || this->polygonCount () == 0 ){ return result; }

	// 各ポリゴンの三角ポリゴンの面積を元に、生成するポイント数を割り出す
	u32 count = 0;
	T pNum = std::sqrt ( T ( detail ) ); // 1平方あたりのポイント数
	u32 polyCnt = this->polygonCount ();
	for ( u32 i = 0; i < polyCnt; ++i ){
		u8 triCnt = this->triangleCount ( i ); // ポリゴンに含まれる三角ポリゴンの数
		for(u32 j=0; j<triCnt; ++j){
			L4Array< u32 > triPoiIDs = this->getTrianglePointIDs ( i, j );
			Vec3 p0 = this->getPointPosition ( triPoiIDs[0] );
			Vec3 p1 = this->getPointPosition ( triPoiIDs[1] );
			Vec3 p2 = this->getPointPosition ( triPoiIDs[2] );
			T baseLen = p0.distanceTo ( p1 );

			// 三角ポリゴンの面積を元にしてロケーションを生成していく
			T triangleArea = Burgess2MathFn::GetTriangleArea ( p0, p1, p2 );
			u32 numYa = ( triangleArea * 2 / baseLen ) * pNum;
			numYa = Burgess2MathFn::Max ( numYa, u32(1) );
			T numInv = T ( 1.0 ) / T ( numYa );
			T r = baseLen * pNum;
			for ( u32 k = 0; k < numYa; ++k ){
				T w = ( T ( k) + T ( 0.5 ) ) * numInv;
				T f = ( T ( 1.0 ) - w ) * r;
				u32 numXa = std::ceil ( f );
				numXa = Burgess2MathFn::Max ( numXa, u32(1) );
				T numXInv = T ( 1.0 ) / T ( numXa );
				for ( u32 m = 0; m < numXa; ++m ){
					T wA = ( T(m) + T(0.5) ) * numXInv;
					T w0 = ( T(1.0) - w ) * wA;
					T w1 = ( T(1.0) - w ) * ( T(1.0) - wA );
					Vec3T<T> barycentric{ w0, w1, w };
					GeometryLocationT<T> loc ( i, j, barycentric, 3 );
					result.push_back ( loc );
				}
			}
		}
	}
	return result;
}