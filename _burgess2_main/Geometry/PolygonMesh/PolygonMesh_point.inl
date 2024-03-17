//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::pointCount () const{
	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	return poiPosAttr.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点位置を全て設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setAllPointPositions ( const std::vector<Vec3T<T>> &points ) {
	assert ( this->pointCount() == points.size() );
	Vec3AttributeT<T> &poiPosAttr = this->getPointPositionAttribute ();
	poiPosAttr.setValues ( points );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点位置を設定する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setPointPosition ( u32 pointID, const Vec3T<T> &position ) {
	assert ( pointID < this->pointCount () );
	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	poiPosAttr [pointID] = position;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点位置を全て取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec3T<T>> PolygonMeshT<T>::getAllPointPositions ( )const {
	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	return poiPosAttr.getValues ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点位置を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> PolygonMeshT<T>::getPointPosition ( u32 pointID ) const {
	assert ( pointID < this->pointCount () );
	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	return poiPosAttr.getValue( pointID ) ;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    バーテックスで構成される頂点のIDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPointID ( u32 vertexID )const {
	assert ( vertexID < this->vertexCount () );
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	return pointIDAttr [vertexID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴンを構成する頂点ID群を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L8Array<u32> PolygonMeshT<T>::getPolygonPointIDs ( u32 polygonID )const {
	assert ( polygonID < this->polygonCount () );
	U32AttributeT<T>& polySizeAttr = this->getU32Attribute ( "polygonSizes" );
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	L8Array<u32> result(0);
	for ( u32 k = polySizeAttr [polygonID]; k<polySizeAttr [polygonID+1]; k++ ) {
		result.push ( pointIDAttr [k] );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    隣接する全ての頂点IDを返す (接続するポリゴン内の隣接しない頂点も含める場合は includeAllPolygonPoints=true )
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16Array<u32> PolygonMeshT<T>::getPointSurroundingPoints ( u32 pointID, Bool includeAllPolygonPoints )const {
	assert ( pointID < this->pointCount () );
	L16Array<u32> polyIDs = this->getPointPolygonIDs ( pointID ); // pointIDのポイントで構成するポリゴンID群を取得
	std::set<u32> pointIDs; // set<>にすることで、各ポリゴンのポイントIDを格納していく際に重複するポイントIDを省く
	for ( u32 i = 0; i<polyIDs.size (); i++ ) {
		if ( includeAllPolygonPoints ){
			L8Array<u32> poiIDs = this->getPolygonPointIDs ( polyIDs[i] );
			for ( u32 k = 0; k < poiIDs.size (); k++ ){
				pointIDs.insert ( poiIDs[k] );
			}
		} else{
			pointIDs.insert ( this->getNextPolygonPointID( polyIDs[i], pointID ) );
			pointIDs.insert ( this->getPreviousPolygonPointID( polyIDs[i], pointID ) );
		}
	}

	// set<>の値をvector<>へ変換
	L16Array<u32> result ( u32(pointIDs.size ()) );
	auto itr = pointIDs.begin ();
	for ( u32 i = 0; i < pointIDs.size (); i++ ) {
		result [i] = u32 ( *itr );
		itr++;
	}
	
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴン内の次(左回り)のポイントIDを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getNextPolygonPointID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );

	u32 currentLocalID = 0;
	L8Array<u32>& poiIDList = this->getPolygonPointIDs(polygonID);
	for (u32 i = 0; i < poiIDList.size(); i++) {
		if (pointID == poiIDList[i]) {
			currentLocalID = i;
			break;
		}
	}
	u32 nextLocalID = currentLocalID + 1;

	if (nextLocalID == poiIDList.size()) {
		return poiIDList[0];
	} else {
		return poiIDList[nextLocalID];
	}
	/*u32 vertexID = this->getVertexID ( polygonID, pointID );
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	U32AttributeT<T>& polygonSizeAttr = this->getU32Attribute ( "polygonSizes" );
	u32 remainder;
	u32 polyID = Burgess2MathFn::BinarySearchTree( polygonSizeAttr.getValues (), vertexID, remainder );
	if ( this->getPolygonSize ( polygonID ) - 1 == remainder ) { // vertexIDがポリゴンの最終バーテックスだった場合
		return pointIDAttr [polygonSizeAttr [polygonID]];
	} else {
		return pointIDAttr [vertexID+1];
	}*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴン内1つ前(右回り)のポイントIDを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPreviousPolygonPointID ( u32 polygonID, u32 pointID )const {
	assert ( polygonID < this->polygonCount () );
	assert ( pointID < this->pointCount () );
	
	u32 currentLocalID = 0;
	L8Array<u32>& poiIDList = this->getPolygonPointIDs(polygonID);
	for (u32 i = 0; i < poiIDList.size(); i++) {
		if (pointID == poiIDList[i]) {
			currentLocalID = i;
			break;
		}
	}

	if (currentLocalID == 0) {
		return poiIDList.back();
	} else {
		return poiIDList[currentLocalID-1];
	}

	/*u32 vertexID = this->getVertexID ( polygonID, pointID );
	U32AttributeT<T>& pointIDAttr = this->getU32Attribute ( "packedPointIDs" );
	U32AttributeT<T>& polygonSizeAttr = this->getU32Attribute ( "polygonSizes" );
	u32 remainder;
	u32 polyID = Burgess2MathFn::BinarySearchTree( polygonSizeAttr.getValues (), vertexID, remainder );
	if ( 0 == remainder ) { // vertexIDがポリゴンの最初のバーテックスだった場合
		return pointIDAttr [polygonSizeAttr[polyID + 1] -1];
	} else {
		return pointIDAttr [vertexID-1];
	}*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポイントのローカルバーテックスIDのバーテックスで構成しているポリゴンIDとポリゴン内のローカルバーテックスIDを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::getPointPolygonAndIndex ( u32 pointID, LocalIndex poiLocalID, u32 &polygonID, LocalIndex &polyLocalID )const{
	assert ( pointID < this->pointCount () );
	L16Array<u32> poiIDs = this->getSharedVertexIDs ( pointID );
	assert ( poiLocalID.id < poiIDs.size () );
	u32 vertexID = poiIDs[poiLocalID.id];
	polygonID = this->getVertexPolygonID ( vertexID );
	L8Array<u32> poiIDs2 = this->getPolygonPointIDs ( polygonID );
	for(u32 i=0; i<poiIDs2; i++ ){
		if( pointID == poiIDs2[i] ){
			polyLocalID.id = i;
			return;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点で共有しているバーテックス群に対して、次（左回り）のバーテックスIDを取得する
//!		@warning 複数のポリゴンアイランドが1つの頂点を共有しており、更に次のバーテックスが別のアイランドを指す場合は左回りとは限らない
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getNextPointVertexID ( u32 vertexID )const{
	assert ( vertexID < this->vertexCount () );
	u32 poiID = this->getPointID ( vertexID );
	L16Array<u32> poiVerIDs = this->getSharedVertexIDs ( poiID );
	if ( poiVerIDs.size () == 0 ){ return BURGESS2::U32_INFINITE; }

	LocalIndex lID;
	for(u32 i =0; i<poiVerIDs.size(); ++i ){
		if( poiVerIDs[i] == vertexID ){
			lID = i;
			break;
		}
	}
	return lID.id == poiVerIDs.size ()-1 ? poiVerIDs[0] : poiVerIDs[lID.id + 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    頂点で共有しているバーテックス群に対して、１つ前（右回り）のバーテックスIDを取得する
//!		@warning 複数のポリゴンアイランドが1つの頂点を共有しており、更に1つ前のバーテックスが別のアイランドを指す場合は右回りとは限らない
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::getPreviousPointVertexID ( u32 vertexID )const{
	assert ( vertexID < this->vertexCount () );
	u32 poiID = this->getPointID ( vertexID );
	L16Array<u32> poiVerIDs = this->getSharedVertexIDs ( poiID );
	if ( poiVerIDs.size () == 0 ){ return BURGESS2::U32_INFINITE; }

	LocalIndex lID;
	for ( u32 i = 0; i<poiVerIDs.size (); ++i ){
		if ( poiVerIDs[i] == vertexID ){
			lID = i;
			break;
		}
	}
	return lID.id == 0 ? poiVerIDs[poiVerIDs.size()-1] : poiVerIDs[lID.id - 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポイントがポリゴンボーダーであればtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::isBorderPoint ( u32 pointID )const{	
	u32 polySize = this->getPointPolygonCount ( pointID );
	if ( polySize <= 1 ){ return true; }

	// ポイントで構成しているエッジがボーダーエッジかどうかを順番に確認する
	L16Array<u32> sVerIDs = this->getSharedVertexIDs( pointID );
	for ( u32 i = 0; i < sVerIDs.size(); i++ ) {
		// ポイントをスタートにするエッジをチェック
		u32 edgeID = sVerIDs[i]; // vertex ID = half-edge ID
		if ( this->isPolygonBorder( edgeID ) ) {
			return true;
		}
		// ポイントをエンドにするエッジをチェック
		u32 pEdgeID = this->getPreviousPolygonVertexID( sVerIDs[i] );
		if ( this->isPolygonBorder( pEdgeID ) ) {
			return true;
		}
	}
	return false;
}