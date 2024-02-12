
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Vec3[]からRBF空間をセットアップする　(リターゲットの値はVec3型)
//!	    @brief Vec3[]からRBF空間をセットアップする　(リターゲットの値はVec3型)
//!		@param[in] referencePoint RBF空間を作るVec3[]
//!		@param[in] refarenceValue RBF空間に設定するリターゲット値 (referencePointと同じ配列数が必要)
//!		@param[in] rbfType \ref RBFT "RBFT<T>"を参照
//!		@param[in] rbfSmooth \ref RBFT "RBFT<T>"を参照
//!		@param[in] rbfEpsilon \ref RBFT "RBFT<T>"を参照
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void RBFRetargeterT<T>::setup ( const std::vector<Vec3T<T>> &referencePoint, const std::vector<Vec3T<T>> &refarenceValue, u8 type, T smooth, T epsilon ){
	assert ( referencePoint.size () == refarenceValue.size () );

	std::vector<RBFControlPointT<T>> ctrlPoints ( referencePoint.size () );
	#pragma omp parallel for
	for ( s32 i = 0; i<referencePoint.size (); ++i ){
		ctrlPoints[i] = RBFControlPointT<T> ( referencePoint[i], refarenceValue[i] );
	}
	this->rbf.setup( ctrlPoints, type, smooth, epsilon );
	this->size = 3;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Vec3[]からRBF空間をセットアップする　(リターゲットの値はMat4型)
//!	    @brief Vec3[]からRBF空間をセットアップする　(リターゲットの値はMat4型)
//!		@param[in] referencePoint RBF空間を作るVec3[]
//!		@param[in] refarenceValue RBF空間に設定するリターゲット値 (referencePointと同じ配列数が必要)
//!		@param[in] rbfType \ref RBFT "RBFT<T>"を参照
//!		@param[in] rbfSmooth \ref RBFT "RBFT<T>"を参照
//!		@param[in] rbfEpsilon \ref RBFT "RBFT<T>"を参照
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void RBFRetargeterT<T>::setup ( const std::vector<Vec3T<T>> &referencePoint, const std::vector<Mat4T<T>> &refarenceValue, u8 type, T smooth, T epsilon ){
	assert ( referencePoint.size () == refarenceValue.size () );

	std::vector<RBFControlPointT<T>> ctrlPoints ( referencePoint.size () );
	#pragma omp parallel for
	for ( s32 i = 0; i<referencePoint.size (); ++i ){
		ctrlPoints[i] = RBFControlPointT<T> ( referencePoint[i], refarenceValue[i] );
	}
	this->rbf.setup ( ctrlPoints, type, smooth, epsilon );
	this->size = 3;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    refarenceMeshの頂点からRBF空間をセットアップする　(リターゲットの値はretargetMeshの頂点位置)
//!	    @brief refarenceMeshの頂点からRBF空間をセットアップする　(リターゲットの値はretargetMeshの頂点位置)
//!		@param[in] refarenceMesh RBF空間を作るVec3[]
//!		@param[in] retargetMesh RBF空間に設定するリターゲット値 (refarenceMeshと同じトポロジーが必要)
//!		@param[in] rbfType \ref RBFT "RBFT<T>"を参照
//!		@param[in] rbfSmooth \ref RBFT "RBFT<T>"を参照
//!		@param[in] rbfEpsilon \ref RBFT "RBFT<T>"を参照
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void RBFRetargeterT<T>::setup ( const PolygonMeshT<T> &referenceMesh, const PolygonMeshT<T> &retargetMesh, u8 rbfType, T rbfSmooth, T rbfEpsilon ){
	assert ( referenceMesh.sameTopology ( retargetMesh ) );
	this->setup ( referenceMesh.getPointPositionAttribute ().getValuesRef (),
				  retargetMesh.getPointPositionAttribute ().getValuesRef (),
				  rbfType,
				  rbfSmooth,
				  rbfEpsilon );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    solve (リターゲットの値をVec3型でセットアップしたRBFRetargeterTにのみ有効)
//!	    @brief solve (リターゲットの値をVec3型でセットアップしたRBFRetargeterTにのみ有効)
//!		@param[io] result RBF空間に対してsourcePointで返されるリターゲット値
//!		@param[in] sourcePoint セットアップされたRBF空間に対する算出座標
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void RBFRetargeterT<T>::solve ( OUT std::vector<Vec3T<T>> &result, const std::vector<Vec3T<T>> &sourcePoint )const{
	result.resize ( sourcePoint.size () );
	if ( this->rbf.initialized == false || this->size != 3 ){ return; }

	#pragma omp parallel for
	for ( s32 i = 0; i<sourcePoint.size (); ++i ){
		result[i] = this->rbf.solve ( sourcePoint[i] );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    solve (リターゲットの値をMat4T型でセットアップしたRBFRetargeterTにのみ有効)
//!	    @brief solve (リターゲットの値をMat4T型でセットアップしたRBFRetargeterTにのみ有効)
//!		@param[io] result RBF空間に対してsourcePointで返されるリターゲット値
//!		@param[in] sourcePoint セットアップされたRBF空間に対する算出座標
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void RBFRetargeterT<T>::solve ( OUT std::vector<Mat4T<T>> &result, const std::vector<Vec3T<T>> &sourcePoint )const{
	result.resize ( sourcePoint.size () );
	if ( this->rbf.initialized == false || this->size != 3 ){ return; }

	#pragma omp parallel for
	for ( s32 i = 0; i<sourcePoint.size (); ++i ){
		result[i] = this->rbf.solve ( sourcePoint[i] );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    solve (リターゲットの値をPolygonMesh型でセットアップしたRBFRetargeterTにのみ有効)
//!	    @brief solve (リターゲットの値をPolygonMesh型でセットアップしたRBFRetargeterTにのみ有効)
//!		@param[io] result PSRetargeterの結果
//!		@param[in] rbf PSRetargeter::Setup()で作成したRBF2T<T>
//!		@param[in] sourceMesh PSRetargeterを実行する対象
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void RBFRetargeterT<T>::solve ( IO PolygonMeshT<T> &result, const PolygonMeshT<T> &sourceMesh )const{
	if ( this->rbf.initialized == false || this->size != 3 ){ return; }
	std::vector<Vec3T<T>> resultPoiPos;
	this->solve ( resultPoiPos, sourceMesh.getPointPositionAttribute ().getValuesRef () );
	assert ( resultPoiPos.size () == result.pointCount () );
	result.setAllPointPositions ( resultPoiPos );
}
