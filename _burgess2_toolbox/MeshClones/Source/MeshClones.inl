//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ソースメッシュ情報を登録する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void MeshClonesT<T>::set ( const PolygonMeshT<T> &sourceMesh, u32 size ) {
	if ( sourceMesh.pointCount () == 0 ) { return; }
	this->sourceMesh = sourceMesh;
	this->resize ( size );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    クローンの数を変更する (未最適化)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void MeshClonesT<T>::resize ( u32 size ) {
	std::vector<PolygonMeshT<T>> rest ( size );
	for ( u32 i = 0; i<size; ++i ) {
		rest [i] = this->sourceMesh;
	}
	this->mesh.mergeMeshes ( rest );
	this->cloneSize = size;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    クローンの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 MeshClonesT<T>::cloneCount ( )const {
	return this->cloneSize;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    クローンメッシュをクリアする
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void MeshClonesT<T>::clear () {
	this->mesh = PolygonMeshT<T> ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    クローンメッシュを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T> MeshClonesT<T>::getMesh ( Bool borderMerge ) {
	return this->mesh;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    クローンメッシュを参照で返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
const PolygonMeshT<T>& MeshClonesT<T>::getMeshRef () const{
	return this->mesh;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    クローンごとにトランスフォーム
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void MeshClonesT<T>::applayTransform ( const std::vector<XfoT<T>> &transform ) {
	if ( this->cloneCount () == 0 || transform.size () == 0 ) { return; }
	/* 頂点位置の計算 */
	Vec3Attribute &poiPos = this->mesh.getPointPositionAttribute ();
	u32 sourceMeshPoiCnt = this->sourceMesh.pointCount();
	u32 n = poiPos.size ();
#pragma omp parallel for
	for ( s32 i = 0; i<n; ++i ) {
		u32 cloneID = u32 ( i / sourceMeshPoiCnt );
		poiPos [i] = transform [cloneID] * poiPos [i];
	}

	/* 法線の計算 */
	Vec3Attribute &normAttr = this->mesh.getNormalAttribute ();
	u32 sourceMeshVerCnt = this->sourceMesh.vertexCount();
	u32 normalCnt = normAttr.size ();
#pragma omp parallel for
	for ( s32 i = 0; i<normalCnt; ++i ) {
		u32 origID = i / sourceMeshVerCnt;
		XfoT<T> nXfo ( Vec3T<T> ( T ( 1.0 ) ), transform [origID].ori, Vec3T<T> ( T ( 0.0 ) ) );
		normAttr [i] = nXfo * normAttr [i];
	}
}