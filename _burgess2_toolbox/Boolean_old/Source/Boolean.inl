
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BooleanCrossDataT<T>::push( const u32 id, const Vec3T<T>& pos, const Vec2s& poly, const s32 edgeStartPoiID, const T ratio ) {
	Bool find = false;
	for ( u32 i = 0; i < this->crossPos[id].size(); i++ ) {
		if (this->crossPos[id][i].almostEqual(pos, T( 0.001 ) ) &&
			this->crossPoly[id][i] == poly //&&
			//this->splitEdgeSPoiID[id][i] == edgeID &&
			//Burgess2MathFn::AlmostEqual( this->splitRatio[id][i], r, T(0.001) )
		) {
			find = true;
			break;
		}
	}
	
	if ( find ) {
		return;
	} else {
		this->crossPos[id].push_back( pos );
		this->crossPoly[id].push_back( poly );
		this->splitEdgeSPoiID[id].push_back( edgeStartPoiID );
		this->splitRatio[id].push_back( ratio );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �G�b�W�ɗאڂ���|���S���ԍ����r���ď��������̃|���S���ԍ���L����n�[�t�G�b�W�̎n�_�̃o�[�e�b�N�XID��Ԃ�
//!	    @brief �G�b�W�ɗאڂ���|���S���ԍ����r���ď��������̃|���S���ԍ���L����n�[�t�G�b�W�̎n�_�̃o�[�e�b�N�XID��Ԃ�
//!		@param[in] mesh
//!		@return �n�[�t�G�b�W�̎n�_�̃o�[�e�b�N�XID
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<u32> BooleanT<T>::createEdgeList( const PolygonMeshT<T>& mesh )const {
	std::vector<u32> result;
	for ( u32 i = 0; i < mesh.polygonCount(); i++ ) {
		// �|���S�����\������o�[�e�b�N�XID(�n�[�t�G�b�WID)���擾
		L8Array<u32> edgeIDs = mesh.getPolygonVertexIDs( i );
 
		// �G�b�W�𑖍�
		for ( u32 j = 0; j < edgeIDs.size(); j++ ) {
			if ( mesh.isPolygonBorder( edgeIDs[j] ) ) {
				// �אڂ���|���S���������ꍇ�̓G�b�WID���i�[����
				result.push_back( edgeIDs[j] );
			} else {
				// �G�b�W�ɗאڂ���|���S���̔ԍ����擾
				u32 adjPolyID = mesh.getPolygonVertexAdjacentPolygonID( edgeIDs[j] );
				// �|���S��ID���r���A���݃��[�v�������Ă���|���S��ID�̕����������ꍇ�̓G�b�WID���i�[����
				if ( i < adjPolyID ) {
					result.push_back( edgeIDs[j] );
				}
			}
		}
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] mesh
//!		@return 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BooleanT<T>::CrossDataSet(
	IO BooleanCrossDataT<T>& crossData0,
	IO BooleanCrossDataT<T>& crossData1,
	const std::vector<u32>& edgeList0,
	const PolygonMeshT<T>& mesh0,
	const PolygonMeshT<T>& mesh1,
	const PolygonMeshSpatialQueryT<T>& sq1,
	const T tolerance
) {
	// �G�b�W�̌������P�[�V�������擾����
	std::vector < std::vector<GeometryLocationT<T>>> allLocList( edgeList0.size () );
	#pragma omp parallel for 
	for (s32 i = 0; i < edgeList0.size (); i++) {
		Vec2u edgePoiIDs = mesh0.getEdgePointIDs ( edgeList0[i] );
		Vec3T<T> startPos = mesh0.getPointPosition ( edgePoiIDs[0] );
		Vec3T<T> endpos = mesh0.getPointPosition ( edgePoiIDs[1] );
		allLocList[i] = sq1.getAllLineIntersect ( startPos, endpos, 0 ); // ���P�[�V�������I���|�C���g/�I���G�b�W�������ꍇ�A�אڂ���|���S������|���S���ԍ�����ԎႢ���P�[�V����������Ԃ�
	}

	// �G�b�W�ƃ|���S���̌�������BooleanCrossData�Ɋi�[���Ă���
	for ( u32 i = 0; i < edgeList0.size(); i++ ) {
		std::vector<GeometryLocationT<T>>& locList1 = allLocList[i];
		// �����������ꍇ�̓X�L�b�v
		if ( locList1.size() == 0 ) { continue; }

		// �G�b�W�̏����擾����
		Vec2u edgePoiIDs = mesh0.getEdgePointIDs ( edgeList0[i] );
		Vec3T<T> startPos = mesh0.getPointPosition ( edgePoiIDs[0] );
		Vec3T<T> endpos = mesh0.getPointPosition ( edgePoiIDs[1] );

		// �G�b�W�ɗאڂ���|���S��ID���擾����
		std::vector<u32> edgeNeighberPoly0;
		edgeNeighberPoly0.push_back( mesh0.getEdgeLeftPolygonID( edgeList0[i] ) );
		u32 rPoly = mesh0.getEdgeRightPolygonID( edgeList0[i] );
		if ( rPoly < mesh0.polygonCount() ) {
			// �|���S���{�[�_�[�łȂ���΃G�b�W�̉E���̃|���S��ID���i�[����
			edgeNeighberPoly0.push_back( rPoly );
		}
		
		// CrossData.setData()
		for ( u32 j = 0; j < locList1.size(); j++ ) {
			GeometryLocationT<T>& loc1 = locList1[j]; // mesh0�̃G�b�W��mesh1�Ɍ����������P�[�V����
			Vec3T<T> edgeCrossPos1 = mesh1.getPositionAtLocation( loc1 ); // mesh0�̃G�b�W��mesh1�Ɍ��������ʒu
			T ratio0;
			if (startPos.almostEqual ( edgeCrossPos1, tolerance )) {
				ratio0 = T ( 0.0 );
			} else if (endpos.almostEqual ( edgeCrossPos1, tolerance )) {
				ratio0 = T ( 1.0 );
			} else {
				T edgeLen = startPos.distanceTo ( endpos );
				T val = startPos.distanceTo ( edgeCrossPos1 ) / edgeLen;
				ratio0 = Burgess2MathFn::Clamp( val, T( 0.0 ), T( 1.0 ) );
			}

			///////////////////////////////////////////////////////////////////////////////////////////////
			// ���P�[�V�����f�[�^���A���P�[�V���������_�� / �G�b�W�� / �|���S���G���A��̂ǂ�ɓ����邩��������

			Vec3u locPoiIDs1;  // ���P�[�V�����̃g���C�A���O���̒��_�h�c�R��
			Vec3T<T> locWeights1;  // ���P�[�V�����̃g���C�A���O���̃E�F�C�g�l
			mesh1.getLocationPointsAndWeights( loc1, locPoiIDs1, locWeights1 ); // ���P�[�V�����̃g���C�A���O���̒��_�h�c�R�Ƃ��ꂼ��ւ̃E�F�C�g�l
			u32 crossPolyID1 = loc1.getIndex(); // ���P�[�V�����̃|���S��ID

			// �E�F�C�g��0.0�̒��_���Ȃ������X�g�����
			std::vector<u32> poiIDs1;
			Bool notZero[3];
			for ( u32 k = 0; k < 3; k++ ) {
				if ( locWeights1[k] < T(0.0001) ) {
					notZero[k] = false;
				} else {
					poiIDs1.push_back( locPoiIDs1[k] );
					notZero[k] = true;
				}
			}
			///////////////////////////////
			// poiIDs�̐���2�̏ꍇ
			// ���P�[�V�������O�p�`�̍Ō�̃G�b�W�ゾ�����ꍇ�A�G�b�W�̃X�^�[�g�E�G���h�̏���ID�����Ԃ悤�ɐ������Ă���
			if ( poiIDs1.size() == 2 ) {
				if ( notZero[0] && notZero[2] ) {
					u32 lastPoiID = mesh1.getPolygonPoint ( crossPolyID1, mesh1.getPolygonSize ( crossPolyID1 ) - 1 );
					if (poiIDs1[1] == lastPoiID) {
						u32 rest = poiIDs1[0];
						poiIDs1[0] = poiIDs1[1];
						poiIDs1[1] = rest;
					}
					
				}
			}
			// �T�u�g���C�A���O���̃G�b�W�ォ�|���S���̃G�b�W�ォ�����O�ɔ��肵�Ă���
			Bool onPolygonEdge = false;
			if ( poiIDs1.size() == 2 ) {
				u32 nextPoiID = mesh1.getNextPolygonPointID( crossPolyID1, poiIDs1[0] );
				onPolygonEdge = poiIDs1[1] == nextPoiID;
			}

			if ( poiIDs1.size() == 1 ) { // ���P�[�V���������b�V�����_��̏ꍇ
				for ( u32 k = 0; k < edgeNeighberPoly0.size(); k++ ) {
					L16Array<u32> polyIDs = mesh1.getPointPolygonIDs( poiIDs1[0] );
					s32 edgePoiID;
					T r;
					if (k == 0) {
						edgePoiID = edgePoiIDs[0];
						r = ratio0;
					} else if (k == 1) {
						edgePoiID = edgePoiIDs[1];
						r = T ( 1.0 ) - ratio0;
					} else {
						break;
					}
					for ( u32 l = 0; l < polyIDs.size(); l++ ) {
						crossData0.push( edgeNeighberPoly0[k], edgeCrossPos1, Vec2s{ s32( polyIDs[l] ), -1 }, edgePoiID, r );
					}
				}
			} else if ( poiIDs1.size() == 2 && onPolygonEdge ) { // ���P�[�V���������b�V���G�b�W��̏ꍇ
				// �G�b�W�ɗאڂ���|���S��ID���i�[����
				for ( u32 k = 0; k < edgeNeighberPoly0.size(); k++ ) {
					s32 edgePoiID;
					T r;
					if (k == 0) {
						edgePoiID = edgePoiIDs[0];
						r = ratio0;
					} else if (k == 1) {
						edgePoiID = edgePoiIDs[1];
						r = T ( 1.0 ) - ratio0;
					} else {
						break;
					}
					u32 edgeID1 = mesh1.getPolygonPointEdgeID ( crossPolyID1, poiIDs1[0] );
					crossData0.push( edgeNeighberPoly0[k], edgeCrossPos1, Vec2s{ s32( mesh1.getEdgeLeftPolygonID( edgeID1 ) ), -1 }, edgePoiID, r );
					crossData0.push( edgeNeighberPoly0[k], edgeCrossPos1, Vec2s{ s32( mesh1.getEdgeRightPolygonID( edgeID1 ) ), -1 }, edgePoiID, r );
				}
			} else { // ���P�[�V�������|���S���G���A��̏ꍇ
				for ( u32 k = 0; k < edgeNeighberPoly0.size(); k++ ) {
					if ( k == 0 ) {
						// edgeNeighberPoly0[0](=�G�b�W�̍���)��o�^
						crossData0.push( edgeNeighberPoly0[k], edgeCrossPos1, Vec2s{ s32( crossPolyID1 ), -1 }, edgePoiIDs[0], ratio0 );
					} else if ( k == 1 ) {
						// edgeNeighberPoly0[1](=�G�b�W�̉E��)��o�^
						crossData0.push( edgeNeighberPoly0[k], edgeCrossPos1, Vec2s{ s32( crossPolyID1 ), -1 }, edgePoiIDs[1], T( 1.0 ) - ratio0 );
					}
				}
				if ( edgeNeighberPoly0.size() == 1 ) {
					crossData1.push( crossPolyID1, edgeCrossPos1, Vec2s{ s32( edgeNeighberPoly0[0] ), -1 }, -1, T( -1.0 ) );
				} else {
					crossData1.push( crossPolyID1, edgeCrossPos1, Vec2s{ s32( edgeNeighberPoly0[0] ), s32( edgeNeighberPoly0[1]) }, -1, T( -1.0 ) );
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] mesh
//!		@return 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BooleanT<T>::MakeSplitPolygonsList(
	OUT std::vector<u32>& polygonIDs,
	OUT std::vector<u32>& pointIDs1,
	OUT std::vector<f32>& ratio1,
	OUT std::vector<u32>& pointIDs2,
	OUT std::vector<f32>& ratio2,
	OUT std::vector<std::vector<Vec3T<T>>>& spritPositionsList,
	const PolygonMeshT<T>& mesh0,
	const BooleanCrossDataT<T>& crossData0,
	const Bool hole
) {
	// crossPoly0��Vec2.x/.y�Ɋi�[���ꂽ�|���S��ID���q���ł����A�|���S�������p�̃X�^�[�g�E�G���h�E�ǉ����_�̃��X�g�����
	for ( u32 j = 0; j < mesh0.polygonCount(); j++ ) {
		std::vector<Bool> used( crossData0.crossPoly[j].size(), false );
		while ( true ) {
			std::vector<Vec3T<T>> rSplitPositions( 0 );
			s32 chainPoly = -2, endPoly = -1;

			// �|�C���gID���������l���m�F���邽�߂ɁA��U���̕ϐ��ɕۑ�����
			s32 restIDs[2] = { -1,-1 };
			T restRatios[2];

			// �I���G�b�W/�I���|�C���g(Vec2.y=-1)�������Ďg�p�ς݂̃t���O���n�m�ɂ���
			for ( u32 k = 0; k < crossData0.crossPoly[j].size(); k++ ) {
				if ( used[k] == false && crossData0.crossPoly[j][k].y() == -1 ) {
					restIDs[0] = crossData0.splitEdgeSPoiID[j][k];
					restRatios[0] = crossData0.splitRatio[j][k];
					//rSplitPositions.push_back( crossPos0[j][k] );
					chainPoly = crossData0.crossPoly[j][k].x();
					used[k] = true;
					break;
				}
			}

			// �I���G�b�W/�I���|�C���g(Vec2.y=-1)�������炸�A�X�ɂ܂����g�p�̗v�f���c���Ă���ꍇ
			// PolyHole�̏����p�̃��X�g�����
			if ( hole && chainPoly == -2 ) {
				for ( u32 k = 0; k < crossData0.crossPoly[j].size(); k++ ) {
					if ( used[k] == false ) {
						rSplitPositions.push_back( crossData0.crossPos[j][k] );
						chainPoly = crossData0.crossPoly[j][k].x();
						endPoly = crossData0.crossPoly[j][k].y();
						used[k] = true;
						break;
					}
				}
			}

			while ( true ) {
				// crossPoly0��Vec2.x/.y�Ɋi�[���ꂽ�|���S��ID���q���ł���
				s32 rID = -1;
				for ( u32 k = 0; k < crossData0.crossPoly[j].size(); k++ ) {
					if ( used[k] == true ) { continue; }

					if ( chainPoly == crossData0.crossPoly[j][k].x() || chainPoly == crossData0.crossPoly[j][k].y() ) {
						rID = k;
						if ( crossData0.crossPoly[j][k].x() + crossData0.crossPoly[j][k].y() - chainPoly != -1 ) {
							break;
						}
					}
				}
				if ( rID != -1 ) {
					chainPoly = crossData0.crossPoly[j][rID].x() + crossData0.crossPoly[j][rID].y() - chainPoly;
					if ( chainPoly == -1 ) {
						restIDs[1] = crossData0.splitEdgeSPoiID[j][rID];
						restRatios[1] = crossData0.splitRatio[j][rID];
					} else {
						rSplitPositions.push_back( crossData0.crossPos[j][rID] );
					}
					used[rID] = true;
				} else {
					break;
				}

				if ( chainPoly == endPoly ) {
					// �X�^�[�g�E�G���h�݂̂ŁA�X�ɕ����ʒu�������ꍇ�̓X�L�b�v�@
					// SplitEdgesAndPolygons()�ŏ����ΏۊO�ɕ��ނ��Ă����̂ŁA�����Ő��������͍s��Ȃ��ł���
					/*if ( rSplitPositions.size() == 2 ) {
						if ( rSplitPositions[0].almostEqual( rSplitPositions[1] ) ) {
							break;
						}
					}*/

					// �|�C���gID���s���l�̏ꍇ�̓X�L�b�v
					if ( restIDs[0] == -1 || restIDs[1] == -1 ) {
						break;
					}
					
					pointIDs1.push_back( restIDs[0] );
					pointIDs2.push_back( restIDs[1] );
					ratio1.push_back( Burgess2MathFn::Clamp ( restRatios[0], T ( 0.0 ), T ( 1.0 ) ) );
					ratio2.push_back( Burgess2MathFn::Clamp ( restRatios[1], T ( 0.0 ), T ( 1.0 ) ) );
					polygonIDs.push_back( j );
					spritPositionsList.push_back( rSplitPositions );
					break;
				}
			}
			// �S�Ă̗v�f���g�p�ς݂ɂȂ�����I��
			if ( chainPoly == -2 ) {
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] mesh
//!		@return 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool BooleanT<T>::initalize ( const PolygonMeshT<T>& meshA, const PolygonMeshT<T>& meshB ) {
	if (this->initalized == true || meshA.polygonCount () == 0 || meshB.polygonCount () == 0) { return false; }

	this->edgeListA = this->createEdgeList ( meshA );
	this->pointSizeA = meshA.pointCount ();
	this->polygonSizeA = meshA.polygonCount ();
	this->sqA.setup( meshA );
	
	this->edgeListB = this->createEdgeList ( meshB );
	this->pointSizeB = meshB.pointCount ();
	this->polygonSizeB = meshB.polygonCount ();
	this->sqB.setup( meshB );
	
	this->initalized = true;
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] mesh
//!		@return 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool BooleanT<T>::meshReset ( const PolygonMeshT<T>& mesh, Bool isMeshA ){
	if (this->initalized == false || mesh.polygonCount () == 0 ) { return false; }

	if (isMeshA == true) {
		this->sqA.setup( mesh );
		if (this->pointSizeA != mesh.pointCount () || this->polygonSizeA != mesh.polygonCount ()) {
			this->edgeListA = this->createEdgeList ( mesh );
			this->pointSizeA = mesh.pointCount ();
			this->polygonSizeA = mesh.polygonCount ();
		}
	} else {
		this->sqB.setup( mesh );
		if (this->pointSizeB != mesh.pointCount () || this->polygonSizeB != mesh.polygonCount ()) {
			this->edgeListB = this->createEdgeList ( mesh );
			this->pointSizeB = mesh.pointCount ();
			this->polygonSizeB = mesh.polygonCount ();			
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//!	    @brief 
//!		@param[in] mesh
//!		@return 
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool BooleanT<T>::core( OUT std::vector<PolygonMeshT<T>>& result, const PolygonMeshT<T>& meshA, const PolygonMeshT<T>& meshB, const T tolerance ) {
	if (this->initalized == false || this->pointSizeA == 0 || this->pointSizeB == 0 || this->polygonSizeA == 0 || this->polygonSizeB == 0) { return false; }

	// Boolean_CrossData
	BooleanCrossDataT<T> crossDataA ( meshA.polygonCount () );
	BooleanCrossDataT<T> crossDataB ( meshB.polygonCount () );
	this->CrossDataSet ( crossDataA, crossDataB, edgeListA, meshA, meshB, this->sqB, tolerance );
	this->CrossDataSet ( crossDataB, crossDataA, edgeListB, meshB, meshA, this->sqA, tolerance );

	result.resize ( 2 );
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			result[0] = meshA.clone ();
			std::vector<u32> polygonIDs;
			std::vector<u32> pointIDs1;
			std::vector<f32> ratio1;
			std::vector<u32> pointIDs2;
			std::vector<f32> ratio2;
			std::vector<std::vector<Vec3>> addPointPositions;
			this->MakeSplitPolygonsList ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, meshA, crossDataA, false );
			result[0].beginStructureChanges ();
			result[0].splitEdgesAndPolygons ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, tolerance );
			if (0 < polygonIDs.size () && result[0].polygonCount () == meshA.polygonCount ()) {
				result[0].splitEdgesAndPolygons ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, tolerance );
			}
			result[0].endStructureChanges ();
		}

		#pragma omp section
		{
			result[1] = meshB.clone ();
			std::vector<u32> polygonIDs;
			std::vector<u32> pointIDs1;
			std::vector<f32> ratio1;
			std::vector<u32> pointIDs2;
			std::vector<f32> ratio2;
			std::vector<std::vector<Vec3>> addPointPositions;
			this->MakeSplitPolygonsList ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, meshB, crossDataB, false );
			result[1].beginStructureChanges ();
			result[1].splitEdgesAndPolygons ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, tolerance );
			if (0 < polygonIDs.size () && result[1].polygonCount () == meshB.polygonCount ()) {
				result[1].splitEdgesAndPolygons ( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositions, tolerance );
			}
			result[1].endStructureChanges ();
		}
	}
	
	return meshA.pointCount () < result[0].pointCount () || meshB.pointCount () < result[1].pointCount ();
}