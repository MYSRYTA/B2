
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Vec3[]����RBF��Ԃ��Z�b�g�A�b�v����@(���^�[�Q�b�g�̒l��Vec3�^)
//!	    @brief Vec3[]����RBF��Ԃ��Z�b�g�A�b�v����@(���^�[�Q�b�g�̒l��Vec3�^)
//!		@param[in] referencePoint RBF��Ԃ����Vec3[]
//!		@param[in] refarenceValue RBF��Ԃɐݒ肷�郊�^�[�Q�b�g�l (referencePoint�Ɠ����z�񐔂��K�v)
//!		@param[in] rbfType \ref RBFT "RBFT<T>"���Q��
//!		@param[in] rbfSmooth \ref RBFT "RBFT<T>"���Q��
//!		@param[in] rbfEpsilon \ref RBFT "RBFT<T>"���Q��
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
//!	    Vec3[]����RBF��Ԃ��Z�b�g�A�b�v����@(���^�[�Q�b�g�̒l��Mat4�^)
//!	    @brief Vec3[]����RBF��Ԃ��Z�b�g�A�b�v����@(���^�[�Q�b�g�̒l��Mat4�^)
//!		@param[in] referencePoint RBF��Ԃ����Vec3[]
//!		@param[in] refarenceValue RBF��Ԃɐݒ肷�郊�^�[�Q�b�g�l (referencePoint�Ɠ����z�񐔂��K�v)
//!		@param[in] rbfType \ref RBFT "RBFT<T>"���Q��
//!		@param[in] rbfSmooth \ref RBFT "RBFT<T>"���Q��
//!		@param[in] rbfEpsilon \ref RBFT "RBFT<T>"���Q��
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
//!	    refarenceMesh�̒��_����RBF��Ԃ��Z�b�g�A�b�v����@(���^�[�Q�b�g�̒l��retargetMesh�̒��_�ʒu)
//!	    @brief refarenceMesh�̒��_����RBF��Ԃ��Z�b�g�A�b�v����@(���^�[�Q�b�g�̒l��retargetMesh�̒��_�ʒu)
//!		@param[in] refarenceMesh RBF��Ԃ����Vec3[]
//!		@param[in] retargetMesh RBF��Ԃɐݒ肷�郊�^�[�Q�b�g�l (refarenceMesh�Ɠ����g�|���W�[���K�v)
//!		@param[in] rbfType \ref RBFT "RBFT<T>"���Q��
//!		@param[in] rbfSmooth \ref RBFT "RBFT<T>"���Q��
//!		@param[in] rbfEpsilon \ref RBFT "RBFT<T>"���Q��
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
//!	    solve (���^�[�Q�b�g�̒l��Vec3�^�ŃZ�b�g�A�b�v����RBFRetargeterT�ɂ̂ݗL��)
//!	    @brief solve (���^�[�Q�b�g�̒l��Vec3�^�ŃZ�b�g�A�b�v����RBFRetargeterT�ɂ̂ݗL��)
//!		@param[io] result RBF��Ԃɑ΂���sourcePoint�ŕԂ���郊�^�[�Q�b�g�l
//!		@param[in] sourcePoint �Z�b�g�A�b�v���ꂽRBF��Ԃɑ΂���Z�o���W
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
//!	    solve (���^�[�Q�b�g�̒l��Mat4T�^�ŃZ�b�g�A�b�v����RBFRetargeterT�ɂ̂ݗL��)
//!	    @brief solve (���^�[�Q�b�g�̒l��Mat4T�^�ŃZ�b�g�A�b�v����RBFRetargeterT�ɂ̂ݗL��)
//!		@param[io] result RBF��Ԃɑ΂���sourcePoint�ŕԂ���郊�^�[�Q�b�g�l
//!		@param[in] sourcePoint �Z�b�g�A�b�v���ꂽRBF��Ԃɑ΂���Z�o���W
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
//!	    solve (���^�[�Q�b�g�̒l��PolygonMesh�^�ŃZ�b�g�A�b�v����RBFRetargeterT�ɂ̂ݗL��)
//!	    @brief solve (���^�[�Q�b�g�̒l��PolygonMesh�^�ŃZ�b�g�A�b�v����RBFRetargeterT�ɂ̂ݗL��)
//!		@param[io] result PSRetargeter�̌���
//!		@param[in] rbf PSRetargeter::Setup()�ō쐬����RBF2T<T>
//!		@param[in] sourceMesh PSRetargeter�����s����Ώ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void RBFRetargeterT<T>::solve ( IO PolygonMeshT<T> &result, const PolygonMeshT<T> &sourceMesh )const{
	if ( this->rbf.initialized == false || this->size != 3 ){ return; }
	std::vector<Vec3T<T>> resultPoiPos;
	this->solve ( resultPoiPos, sourceMesh.getPointPositionAttribute ().getValuesRef () );
	assert ( resultPoiPos.size () == result.pointCount () );
	result.setAllPointPositions ( resultPoiPos );
}
