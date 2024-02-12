//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshSpatialQueryT_Octree<T>::PolygonMeshSpatialQueryT_Octree() {
	this->attr.getOrCreatVec3Attribute ( "octreeNodePositions" );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMesh�����ԕ��͂̂��߂̍������\�����\�z����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshSpatialQueryT_Octree<T>::PolygonMeshSpatialQueryT_Octree( const PolygonMeshT<T> &mesh ) {
	assert ( 0<mesh.polygonCount () );
	this->octree.set ( mesh );
	std::vector<Vec3T<T>> allNodePos;
	this->octree.getAllNodeTreePositions ( this->octree.nodeTree, allNodePos );
	assert ( 0 < allNodePos.size () );
	Vec3AttributeT<T> &octNodePosAttr = this->attr.getOrCreatVec3Attribute ( "octreeNodePositions" );
	octNodePosAttr.resetValues ( allNodePos );
	this->initialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMesh�����ԕ��͂̂��߂̍������\�����\�z����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshSpatialQueryT_Octree<T>::PolygonMeshSpatialQueryT_Octree( const PolygonMeshT<T> &mesh, const std::vector<f32> &maskVal, Bool invertMask ) {
	assert ( 0<mesh.polygonCount () );
	this->octree.set ( mesh, maskVal, invertMask );
	std::vector<Vec3T<T>> allNodePos;
	this->octree.getAllNodeTreePositions ( this->octree.nodeTree, allNodePos );
	assert ( 0 < allNodePos.size () );
	Vec3AttributeT<T> &octNodePosAttr = this->attr.getOrCreatVec3Attribute ( "octreeNodePositions" );
	octNodePosAttr.resetValues ( allNodePos );
	this->initialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    startPosition����PolygonMeshSpatialQuery�̕\�ʏ�ւ̍ł��߂����P�[�V�������擾����
//!		@brief	startPosition����PolygonMeshSpatialQuery�̕\�ʏ�ւ̍ł��߂����P�[�V�������擾����
//!		@param[in] startPosition�@�X�^�[�g�ʒu
//!		@param[in] maxDistance�@��������
//!		@param[in] faceSide�@�����Ώۂ̃|���S���t�F�[�X�� 0=���ʁA1=�\�ʁA2=����
//!		@return GeometryLocation
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
GeometryLocationT<T> PolygonMeshSpatialQueryT_Octree<T>::getClosestLocation ( const Vec3T<T> &startPosition, T maxDistance, u8 faceSide, T tolerance ) const {
	assert ( this->isValid () );
	GeometryLocationT<T> result;
	
	// OctreeNode�̒��S���W�Q����ł��߂����W��������
	Vec3AttributeT<T> &octNodePosAttr = this->attr.getVec3Attribute ( "octreeNodePositions" );
	u32 poiID = Burgess2MathFn::GetClosestPointID ( octNodePosAttr.getValuesRef (), startPosition );
	if ( poiID == BURGESS2::U32_INFINITE ) { return result; }
	T closestDis = startPosition.distanceTo ( octNodePosAttr[poiID] );
	// �ŒZ�������ɑ��݂���S�Ă�TriPolygon��ID���擾
	std::vector<u32> triPolyIDs = this->octree.getAllClosestTriPolygonIDs ( startPosition, closestDis );
	if ( triPolyIDs.size () == 0 ) { return result; }
	
	// �eTriPolygon�ւ̍ł��߂����W���v�Z����
	std::vector<Bool> done ( this->octree.triangleCount () );
	s32 triPolyIDSize = triPolyIDs.size ();
	std::vector<T> dist2 ( triPolyIDSize );
	#pragma omp parallel for 
	for ( s32 i = 0; i<triPolyIDSize; i++ ) {
		u32 triPolyID = triPolyIDs [i];
		if ( done[triPolyID] ){
			dist2[i] = maxDistance + 1.0;// T( BURGESS2::F32_INFINITE );
			continue;
		}else{
			done[triPolyID] = true;
		}
		OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyID];
		Vec3T<T> closestPos = Burgess2MathFn::GetClosestPositionOnTriangle ( startPosition, triPoly.p0, triPoly.p1, triPoly.p2 );
		// �t�F�[�X�̗��\���m�F���ď����ɍ������ꍇ�͔z��ɓo�^���Ă���
		if ( startPosition.almostEqual( closestPos, tolerance ) ) {
			dist2[i] = T( 0.0 );
		} else {
			switch ( faceSide ) {
				case 0:// PolygonMeshSpatialQueryT::Side::bothFaces:
					dist2[i] = ( startPosition - closestPos ).length();
					break;
				case 1:// PolygonMeshSpatialQueryT::Side::frontFace:
				{
					Vec3T<T> N = Burgess2MathFn::GetTriangleNormal( triPoly.p0, triPoly.p1, triPoly.p2 );
					Vec3T<T> sToP = startPosition.almostEqual( closestPos, tolerance ) ? N : ( startPosition - closestPos ).unit();
					if ( N.dot( sToP ) < T( 0.0 ) ) {
						// �������ʂ����ʂ������ꍇ�͖����l���i�[ 
						dist2[i] = T( BURGESS2::F32_INFINITE );
					} else {
						dist2[i] = ( startPosition - closestPos ).length();
					}
					break;
				}
				case 2: //PolygonMeshSpatialQueryT::Side::backFace:
				{
					Vec3T<T> N = Burgess2MathFn::GetTriangleNormal( triPoly.p0, triPoly.p1, triPoly.p2 );
					Vec3T<T> sToP = startPosition.almostEqual( closestPos, tolerance ) ? N : ( startPosition - closestPos ).unit();
					if ( T( 0.0 ) < N.dot( sToP ) ) {
						// �������ʂ��\�ʂ������ꍇ�͖����l���i�[ 
						dist2[i] = T( BURGESS2::F32_INFINITE );
					} else {
						dist2[i] = ( startPosition - closestPos ).length();
					}
					break;
				}
			}
		}
	}
	std::vector<T> dist3 = dist2;
	// �ł������̋߂�TriPolygon�̔z��ԍ����擾
	std::vector<u32> arrayOrigID;
	Burgess2MathFn::Sort( dist2, arrayOrigID );
	u32 id = arrayOrigID[0];
	
	// �������ʂ��L���l�ł͂Ȃ������ꍇ�A�܂��͌������ʂ��ő勗���ȏ�̏ꍇ�͏I��
	if ( dist2[0] == T( BURGESS2::F32_INFINITE ) || maxDistance < dist2[0] ) { return result; }

	// GeometryLocationT�^�ŕԂ�
	OctreeTriPolygonT<T> triPoly = this->octree.triPolygons [triPolyIDs[id]];
	Vec3T<T> closestPos = Burgess2MathFn::GetClosestPositionOnTriangle ( startPosition, triPoly.p0, triPoly.p1, triPoly.p2 );
	Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric ( closestPos, triPoly.p0, triPoly.p1, triPoly.p2, tolerance );
	result = GeometryLocationT<T> ( triPoly.origPolygonID, triPoly.localTriID, weight, 3 );

	// ���������ɕ����̃��P�[�V�������������ꍇ
	// �|���S���ԍ������������A�X�Ƀg���C�A���O���ԍ��̏���������result���㏑������
	for ( u32 i = 0; i < dist2.size(); i++ ) {
		if ( Burgess2MathFn::AlmostEqual( dist2[0], dist2[i], tolerance ) == false ) {
			break;
		}
		u32 id2 = arrayOrigID[i];
		OctreeTriPolygonT<T> triPoly2 = this->octree.triPolygons[triPolyIDs[id2]];
		if ( triPoly2.origPolygonID < result.getIndex() ) {
			Vec3T<T> closestPos2 = Burgess2MathFn::GetClosestPositionOnTriangle( startPosition, triPoly2.p0, triPoly2.p1, triPoly2.p2 );
			Vec3T<T> weight2 = Burgess2MathFn::GetTriBarycentric( closestPos2, triPoly2.p0, triPoly2.p1, triPoly2.p2, tolerance );
			result = GeometryLocationT<T>( triPoly2.origPolygonID, triPoly2.localTriID, weight2, 3 );
		}
		if ( triPoly2.origPolygonID == result.getIndex() ) {
			if ( triPoly2.localTriID < result.getSubIndex() ) {
				Vec3T<T> closestPos2 = Burgess2MathFn::GetClosestPositionOnTriangle( startPosition, triPoly2.p0, triPoly2.p1, triPoly2.p2 );
				Vec3T<T> weight2 = Burgess2MathFn::GetTriBarycentric( closestPos2, triPoly2.p0, triPoly2.p1, triPoly2.p2, tolerance );
				result = GeometryLocationT<T>( triPoly2.origPolygonID, triPoly2.localTriID, weight2, 3 );
			}
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshSpatialQuery�ɑ΂���Ray���ŏ��Ɍ����������P�[�V�������擾����
//!		@brief	PolygonMeshSpatialQuery�ɑ΂���Ray���ŏ��Ɍ����������P�[�V�������擾����
//!		@param[in] ray
//!		@param[in] maxDistance�@��������
//!		@param[in] faceSide�@�����Ώۂ̃|���S���t�F�[�X�� 0=���ʁA1=�\�ʁA2=����
//!		@return GeometryLocation
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
GeometryLocationT<T> PolygonMeshSpatialQueryT_Octree<T>::raycast ( const Ray3T<T> &ray, T maxDistance, u8 faceSide, T tolerance )const {
	assert ( this->isValid () );
	GeometryLocationT<T> result;

	// Ray�ƌ�������\���̂���S�Ă�TriPolygon��ID���擾
	std::vector<u32> triPolyIDs = this->octree.getAllRayCastTriPolygonIDs ( ray );
	if ( triPolyIDs.size () == 0 ){ return result; }
	
	// �eTriPolygon�ɑ΂���Ray�ƌ���������W�܂ł̋������v�Z����
	s32 triPolyIDSize = triPolyIDs.size ();
	std::vector<T> dist ( triPolyIDSize );
	#pragma omp parallel for
	for ( s32 i = 0; i < triPolyIDSize; i++ ){
		u32 triPolyID = triPolyIDs[i];
		OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyID];
		if ( Burgess2MathFn::TestRayIntersectPlane ( ray, triPoly.p0, Burgess2MathFn::GetTriangleNormal ( triPoly.p0, triPoly.p1, triPoly.p2 ) ) ){
			Vec3T<T> raycastPos = Burgess2MathFn::GetRayIntersectPositionToTriangle ( ray, triPoly.p0, triPoly.p1, triPoly.p2, faceSide, tolerance );
			if ( raycastPos.almostEqual ( Vec3T<T> ( BURGESS2::F32_INFINITE ), tolerance ) == false ){
				dist[i] = ray.getStartPosition ().distanceTo ( raycastPos );
				continue;
			}
		}
		dist[i] = T ( BURGESS2::F32_INFINITE );
	}

// maxDistance�ȓ��̋�����Ray����������TriPolygon�����̃��X�g���쐬
std::vector<u32> triPolyIDs2;
std::vector<T> dist2;
for ( u32 i = 0; i < triPolyIDSize; i++ ) {
	if ( Burgess2MathFn::AlmostEqual( dist[i], T( BURGESS2::F32_INFINITE ), tolerance ) == false && dist[i] < maxDistance ) {
		dist2.push_back( dist[i] );
		triPolyIDs2.push_back( triPolyIDs[i] );
	}
}
if ( dist2.size() == 0 ) { return result; }

// �ł������̋߂�TriPolygon�̔z��ԍ����擾
std::vector<u32> arrayOrigID;
Burgess2MathFn::Sort( dist2, arrayOrigID );
u32 id = arrayOrigID[0];

// GeometryLocationT�^�ŕԂ�
OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyIDs2[id]];
Vec3T<T> raycastPos = Burgess2MathFn::GetRayIntersectPositionToTriangle( ray, triPoly.p0, triPoly.p1, triPoly.p2, faceSide, tolerance );
Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric( raycastPos, triPoly.p0, triPoly.p1, triPoly.p2 );
result = GeometryLocationT<T>( triPoly.origPolygonID, triPoly.localTriID, weight, 3 );

// ���������ɕ����̃��P�[�V�������������ꍇ
// �|���S���ԍ������������A�X�Ƀg���C�A���O���ԍ��̏���������result���㏑������
for ( u32 i = 0; i < dist2.size(); i++ ) {
	if ( Burgess2MathFn::AlmostEqual( dist2[0], dist2[i], tolerance ) == false ) {
		break;
	}
	u32 id2 = arrayOrigID[i];
	OctreeTriPolygonT<T> triPoly2 = this->octree.triPolygons[triPolyIDs2[id2]];
	if ( triPoly2.origPolygonID < result.getIndex() ) {
		Vec3T<T> raycastPos2 = Burgess2MathFn::GetRayIntersectPositionToTriangle( ray, triPoly2.p0, triPoly2.p1, triPoly2.p2, faceSide, tolerance );
		Vec3T<T> weight2 = Burgess2MathFn::GetTriBarycentric( raycastPos2, triPoly2.p0, triPoly2.p1, triPoly2.p2 );
		result = GeometryLocationT<T>( triPoly2.origPolygonID, triPoly2.localTriID, weight2, 3 );
	}
	if ( triPoly2.origPolygonID == result.getIndex() ) {
		if ( triPoly2.localTriID < result.getSubIndex() ) {
			Vec3T<T> raycastPos2 = Burgess2MathFn::GetRayIntersectPositionToTriangle( ray, triPoly2.p0, triPoly2.p1, triPoly2.p2, faceSide, tolerance );
			Vec3T<T> weight2 = Burgess2MathFn::GetTriBarycentric( raycastPos2, triPoly2.p0, triPoly2.p1, triPoly2.p2 );
			result = GeometryLocationT<T>( triPoly2.origPolygonID, triPoly2.localTriID, weight2, 3 );
		}
	}
}
return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Ray�����������S�Ẵ��P�[�V�������߂����Ŏ擾����
//!		@brief	Ray�����������S�Ẵ��P�[�V�������߂����Ŏ擾����
//!		@param[in] ray
//!		@param[in] maxDistance�@��������
//!		@param[in] faceSide�@�����Ώۂ̃|���S���t�F�[�X�� 0=���ʁA1=�\�ʁA2=����
//!		@param[in] omitDuplication ���P�[�V�������I���G�b�W/�I���|�C���g�̍ۂɁA�אڂ��镡���̃|���S���̒�����|���S���ԍ����ł����������P�[�V�����f�[�^�ȊO���Ȃ��ꍇ��true
//!		@return std::vector<GeometryLocation> ���������S�Ẵ��P�[�V�����i�߂����j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<GeometryLocationT<T>> PolygonMeshSpatialQueryT_Octree<T>::getAllRayIntersect( const Ray3T<T>& ray, T maxDistance, u8 faceSide, Bool omitDuplication, T tolerance )const {
	assert( this->isValid() );
	std::vector<GeometryLocationT<T>> result( 0 );

	// Ray�ƌ�������\���̂���S�Ă�TriPolygon��ID���擾
	std::vector<u32> triPolyIDs = this->octree.getAllRayCastTriPolygonIDs( ray );
	if ( triPolyIDs.size() == 0 ) { return result; }

	// �eTriPolygon�ɑ΂���Ray�ƌ���������W�܂ł̋������v�Z����
	s32 triPolyIDSize = triPolyIDs.size();
	std::vector<T> distSQ( triPolyIDSize );
	std::vector<Vec3T<T>> raycastPos( triPolyIDSize );
	#pragma omp parallel for 
	for ( s32 i = 0; i < triPolyIDSize; i++ ) {
		u32 triPolyID = triPolyIDs[i];
		OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyID];
		if ( Burgess2MathFn::TestRayIntersectPlane( ray, triPoly.p0, Burgess2MathFn::GetTriangleNormal( triPoly.p0, triPoly.p1, triPoly.p2 ) ) ) {
			raycastPos[i] = Burgess2MathFn::GetRayIntersectPositionToTriangle( ray, triPoly.p0, triPoly.p1, triPoly.p2, faceSide, tolerance );
			if ( raycastPos[i] != Vec3T<T>( BURGESS2::F32_INFINITE ) ) {
				distSQ[i] = ( ray.getStartPosition () - raycastPos[i] ).lengthSquared();
				continue;
			}
		}
		distSQ[i] = T( BURGESS2::F32_INFINITE );
	}

	// maxDistance�ȓ��̋�����Ray����������TriPolygon�����̃��X�g���쐬
	std::vector<u32> triPolyIDs2;
	std::vector<T> distSQ2;
	std::vector<Vec3T<T>> raycastPos2;
	triPolyIDs2.reserve ( triPolyIDSize );
	distSQ2.reserve ( triPolyIDSize );
	raycastPos2.reserve ( triPolyIDSize );
	for ( u32 i = 0; i < triPolyIDSize; i++ ) {
		if (distSQ[i] != T( BURGESS2::F32_INFINITE ) && distSQ[i] < SQ(maxDistance) ) {
			distSQ2.push_back( distSQ[i] );
			triPolyIDs2.push_back( triPolyIDs[i] );
			raycastPos2.push_back( raycastPos[i] );
		}
	}
	if (distSQ2.size() == 0 ) { return result; }

	// �����̋߂����Ƀ\�[�g����
	std::vector< u32 > arrayOrigID;
	Burgess2MathFn::Sort( distSQ2, arrayOrigID );
	
	// GeometryLocationT�^�ŕԂ�
	u32 resultSize = distSQ2.size();
	if ( omitDuplication == true ) {
		// ��ԕ��͂̈ʒu���������P�[�V�����̏ꍇ�A���P�[�V�����f�[�^�̃|���S���ԍ����ł����������́A�X�Ƀg���C�A���O���̔ԍ������������̂�I�ʂ���
		std::vector<Vec3T<T>> usedPos( 0 );
		for ( u32 i = 0; i < resultSize; i++ ) {
			OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyIDs2[arrayOrigID[i]]];
			Bool find = false;
			for ( u32 j = 0; j < usedPos.size(); j++ ) {
				// ����result�ɓo�^�ς݂̃��P�[�V�������ɓ����ʒu�̃��P�[�V���������邩����
				if ( raycastPos2[arrayOrigID[i]].almostEqual( usedPos[j], tolerance ) == true ) {
					// ���P�[�V�����̃|���S���ԍ����ׂāA���g�̕������������result���㏑��
					if ( triPoly.origPolygonID < result[j].getIndex() ) {
						Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric( raycastPos2[arrayOrigID[i]], triPoly.p0, triPoly.p1, triPoly.p2, tolerance );
						result[j] = GeometryLocationT<T>( triPoly.origPolygonID, triPoly.localTriID, weight, 3 );
					}
					// ���P�[�V�����̃|���S���ԍ��������ꍇ�A�g���C�A���O���̔ԍ����ׂāA���g�̕������������result���㏑��
					if ( triPoly.origPolygonID == result[j].getIndex() ) {
						if ( triPoly.localTriID < result[j].getSubIndex() ) {
							Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric( raycastPos2[arrayOrigID[i]], triPoly.p0, triPoly.p1, triPoly.p2, tolerance );
							result[j] = GeometryLocationT<T>( triPoly.origPolygonID, triPoly.localTriID, weight, 3 );
						}
					}
					find = true;
					break;
				}
			}
			// �������P�[�V���������������ꍇ�̓X�L�b�v�@�iresult�̏㏑�����K�v�ł���΁���for���ŏ����ς݁j
			if ( find == true ) { continue; }
			// result�ɐV�K���P�[�V�����f�[�^��ǉ�
			Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric( raycastPos2[arrayOrigID[i]], triPoly.p0, triPoly.p1, triPoly.p2, tolerance );
			result.push_back( GeometryLocationT<T>( triPoly.origPolygonID, triPoly.localTriID, weight, 3 ) );
			
			usedPos.push_back( raycastPos2[arrayOrigID[i]] );
		}
	} else {
		// �S�Ă̌��ʂ�Ԃ�
		// ���P�[�V�������g���C�A���O���̃I���G�b�W/�I���|�C���g�̏ꍇ�A�אڂ���S�Ẵ|���S�����Ƃ̃��P�[�V�������Ԃ����
		// �|���S���ł͂Ȃ��g���C�A���O���̃I���G�b�W�������ꍇ�͗אڂ���g���C�A���O���̃��P�[�V�������Ԃ����̂Œ���
		result.resize( resultSize );
		for ( u32 i = 0; i < resultSize; i++ ) {
			OctreeTriPolygonT<T> triPoly = this->octree.triPolygons[triPolyIDs2[arrayOrigID[i]]];
			Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric( raycastPos2[arrayOrigID[i]], triPoly.p0, triPoly.p1, triPoly.p2, tolerance );
			result[i] = GeometryLocationT<T>( triPoly.origPolygonID, triPoly.localTriID, weight, 3 );
		}
	}
	
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Line�����������S�Ẵ��P�[�V������start�ɋ߂����Ŏ擾����
//!		@brief	Line�����������S�Ẵ��P�[�V������start�ɋ߂����Ŏ擾����
//!		@param[in] start�@���C���̃X�^�[�g�ʒu
//!		@param[in] end�@���C���̃G���h�ʒu
//!		@param[in] faceSide�@�����Ώۂ̃|���S���t�F�[�X�� 0=���ʁA1=�\�ʁA2=����
//!		@param[in] omitDuplication ���P�[�V�������I���G�b�W/�I���|�C���g�̏ꍇ�A�אڂ��镡���̃|���S���̒�����|���S���ԍ����ł����������P�[�V�����f�[�^�ȊO���Ȃ��ꍇ��true
//!		@return std::vector<GeometryLocation> ���������S�Ẵ��P�[�V�����i�߂����j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<GeometryLocationT<T>> PolygonMeshSpatialQueryT_Octree<T>::getAllLineIntersect ( const Vec3T<T> &start, const Vec3T<T> &end, u8 faceSide, Bool omitDuplication, T tolerance ) const {
	Ray3T<T> ray ( start, end, false );
	T length = ( end-start ).length ();
	return this->getAllRayIntersect ( ray, length, faceSide, omitDuplication, tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshSpatialQuery���L���ȏꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshSpatialQueryT_Octree<T>::isValid () const {
	return this->initialized;
}
