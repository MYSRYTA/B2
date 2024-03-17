//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
DualLaplacianMorphT<T>::DualLaplacianMorphT() {
	this->initialized = false;
	this->baseMeshPoiIDs.resize(0);
	this->baseDualMeshPoiPos.resize(0);
	this->targetDualInfo.resize(0);
	this->baseDualMesh = PolygonMeshT<T>();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ベースメッシュとターゲットを登録する
//!	    @brief ベースメッシュとターゲットを登録する
//!		@param[in] baseMesh
//!		@param[in] targetMesh （baseMeshと同じトポロジーのメッシュ）
//!		@param[in] mask 処理対象とする頂点IDのリスト(配列数が0の場合は全頂点を処理対象とする)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void DualLaplacianMorphT<T>::setDualLaplacianInfo ( const PolygonMeshT<T> &baseMesh,
													const PolygonMeshT<T> &targetMesh,
													const std::vector<u32> &mask ){
	this->initialized = false;

	if ( baseMesh.sameTopology ( targetMesh ) == false ){ return; }

	this->baseDualMesh = baseMesh.createDualMesh ( T ( 1.0 ), mask, this->baseMeshPoiIDs );
	this->baseDualMeshPoiPos = this->baseDualMesh.getAllPointPositions ();

	PolygonMeshT<T> targetDualMesh;
	targetDualMesh = targetMesh.createDualMesh ( T ( 1.0 ), mask );
	if (targetDualMesh.polygonCount() < 3) { return; }

	u32 n = targetDualMesh.pointCount ();
	std::vector<L16Array<u32>> surroundPoiIDs ( n );
	#pragma omp parallel for
	for ( s32 i = 0; i < n; ++i ){
		surroundPoiIDs[i] = targetDualMesh.getPointSurroundingPoints ( i, false );
	}
	this->targetDualInfo = this->computeDualLaplacianInfo ( targetDualMesh, surroundPoiIDs );

	this->initialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    solve (meshのトポロジーチェックは事前にsameTopology()で済ませておく)
//!	    @brief solve　(meshのトポロジーチェックは事前にsameTopology()で済ませておく)
//!		@param[in] mesh 処理対象となるメッシュ（baseMeshと同じトポロジーのメッシュ）
//!		@param[in] iterateRatio マックスイテレーションに対するレシオ 0.0~1.0
//!		@param[in] weight 一回ののイテレーションでのブレンド値
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void DualLaplacianMorphT<T>::solve ( IO PolygonMeshT<T> &mesh, T iterate, T weight ){
	if ( this->initialized == false ){ return; }

	this->baseDualMesh.setAllPointPositions ( this->baseDualMeshPoiPos );
	u32 n = this->baseDualMesh.pointCount ();

	// 頂点に隣接する頂点のリストを準備しておく
	std::vector<L16Array<u32>> surroundPoiIDs ( n );
	#pragma omp parallel for
	for ( s32 i = 0; i < n; ++i ){
		surroundPoiIDs[i] = this->baseDualMesh.getPointSurroundingPoints ( i, false );
	}

	for ( u32 ite = 0; ite < iterate; ++ite ){
		// DualMeshのDual-Laplacian-Infoを新規作成or更新
		std::vector<Vec3T<T>> restDualInfo = this->computeDualLaplacianInfo( this->baseDualMesh, surroundPoiIDs );

		#pragma omp parallel for
		for ( s32 i = 0; i < n; ++i ){
			if ( restDualInfo[i].x < T ( 0.0 ) ){ continue; }

			// DualMeshの頂点位置を更新
			if ( surroundPoiIDs[i].size () == 3 ){
				Vec3T<T> DLCoordinate = restDualInfo[i].lerp ( this->targetDualInfo[i], weight );

				Vec3T<T> p0 = this->baseDualMesh.getPointPosition ( surroundPoiIDs[i][0] );
				Vec3T<T> p1 = this->baseDualMesh.getPointPosition ( surroundPoiIDs[i][1] );
				Vec3T<T> p2 = this->baseDualMesh.getPointPosition ( surroundPoiIDs[i][2] );
				Vec3T<T> q = ( p0 * DLCoordinate.x ) + ( p1 * DLCoordinate.y ) + ( p2 * ( T ( 1.0 ) - DLCoordinate.x - DLCoordinate.y ) );
				Vec3T<T> h = Math::GetTriangleNormal ( p0, p1, p2 ) * DLCoordinate.z;
				this->baseDualMesh.setPointPosition ( i, q + h );
			}
		}
	}

	// DualMeshの変化量をmeshの頂点に加算する
	u32 polyCnt = this->baseMeshPoiIDs.size ();
	#pragma omp parallel for
	for (s32 i = 0; i < polyCnt; ++i) {
		if (this->baseMeshPoiIDs[i] < 0) { continue; }

		L8Array<u32> polPoiIDs = this->baseDualMesh.getPolygonPointIDs ( i );
		Vec3T<T> coordinate ( T ( 0.0 ) );
		for (u32 j = 0; j < polPoiIDs.size (); ++j) {
			u32 dPoiID = polPoiIDs[j];
			coordinate += this->baseDualMesh.getPointPosition ( dPoiID ) - this->baseDualMeshPoiPos[dPoiID];
		}
		Vec3T<T> origPos = mesh.getPointPosition ( this->baseMeshPoiIDs[i] );
		mesh.setPointPosition ( this->baseMeshPoiIDs[i], origPos + coordinate / T ( polPoiIDs.size () ) );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Dual-Laplacian-Infoを算出する
//!	    @brief　Dual-Laplacian-Infoを算出する
//!		@param[in] dualMesh DualMesh
//!		return Vec3.x=barycentric0, .y=barycentric1, .z=distance (barycentric2は「1.0-bary0-bary1」で算出)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec3T<T>> DualLaplacianMorphT<T>::computeDualLaplacianInfo (	
	const PolygonMeshT<T> &dualMesh,
	const std::vector<L16Array<u32>> &surroundPoiIDs )const
{
	u32 dMPoiSize = dualMesh.pointCount ();
	std::vector<Vec3T<T>> result ( dMPoiSize );

	#pragma omp parallel for
	for ( s32 i = 0; i < dMPoiSize; ++i ){
		// 頂点にエッジで隣接する頂点ID群を取得
		//L16Array<u32> surroundPoiIDs = dualMesh.getPointSurroundingPoints ( i, false );
		if ( surroundPoiIDs[i].size () != 3 ){
			// エッジで隣接する頂点数が３以外の場合は不正値を設定して終了
			result[i] = Vec3T<T> ( T ( -1.0 ) );
			continue;
		}

		// 隣接する３頂点で作る三角形に対して、最も近い位置までの距離とbarycentricを保存する
		// Vec3(.x = barycentric0, .y = barycentric1, .z = distance)
		Vec3T<T> p = dualMesh.getPointPosition ( i );
		Vec3T<T> tP0 = dualMesh.getPointPosition ( surroundPoiIDs[i][0] );
		Vec3T<T> tP1 = dualMesh.getPointPosition ( surroundPoiIDs[i][1] );
		Vec3T<T> tP2 = dualMesh.getPointPosition ( surroundPoiIDs[i][2] );
		Vec3T<T> cPos = Math::GetClosestPositionOnTriangle ( p, tP0, tP1, tP2 );
		result[i] = Math::GetTriBarycentric ( cPos, tP0, tP1, tP2 );
		Vec3T<T> normal = Math::GetTriangleNormal ( tP0, tP1, tP2 );
		if ( p.almostEqual ( cPos ) ){
			result[i].z = T ( 0.0 ); // pとclosestPointが同じ位置の場合は距離０で登録
		} else{
			Vec3T<T> dir = ( p - cPos ).unit ();
			if ( dir.dot ( normal ) < 0 ){
				result[i].z = cPos.distanceTo ( p ) * T ( -1.0 );
			} else{
				result[i].z = cPos.distanceTo ( p );
			}
		}
	}
	return result;
}