//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
OctreeT<T>::OctreeT (){
	this->branchOffset.resize ( 8 );
	this->branchOffset[0] = Vec3T<T>{ -1,-1,-1 };
	this->branchOffset[1] = Vec3T<T>{ -1,-1, 1 };
	this->branchOffset[2] = Vec3T<T>{ -1, 1,-1 };
	this->branchOffset[3] = Vec3T<T>{  1,-1,-1 };
	this->branchOffset[4] = Vec3T<T>{  1, 1,-1 };
	this->branchOffset[5] = Vec3T<T>{  1,-1, 1 };
	this->branchOffset[6] = Vec3T<T>{ -1, 1, 1 };
	this->branchOffset[7] = Vec3T<T>{  1, 1, 1 };
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�Ẵm�[�h��Xfo���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::getAllNodeTreeXfos ( const OctreeNodeT<T> &node, IO std::vector<XfoT<T>> &result, Bool includeEmptyNode )const {
	if ( node.isLeafNode () ) {
		if ( includeEmptyNode || node.polyCount () != 0 ) {
			result.push_back ( XfoT<T>( Vec3T<T>( node.sideLength ), QuatT<T>(), node.position ));
		}
	} else {
		for ( u32 i = 0; i<8; i++ ) {
			this->getAllNodeTreeXfos ( node.branches [i], result );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�Ẵm�[�h�̃|�W�V�������擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::getAllNodeTreePositions ( const OctreeNodeT<T> &node, IO std::vector<Vec3T<T>> &result, Bool includeEmptyNode )const {
	if ( node.isLeafNode () ) {
		if ( includeEmptyNode || node.polyCount () != 0 ) {
			result.push_back ( node.position );
		}
	} else {
		for ( u32 i = 0; i<8; i++ ) {
			this->getAllNodeTreePositions ( node.branches [i], result, includeEmptyNode );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    maxDist�������̑��݂���S�Ă�TriPolygon��ID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<u32> OctreeT<T>::getAllClosestTriPolygonIDs ( const Vec3T<T> &startPosition, T maxDist )const {
	std::vector<u32> result;
	this->getTriPolyIDsShortestDistance ( this->nodeTree, startPosition, maxDist, result );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Ray�ƌ�������S�Ă�TriPolygon��ID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<u32> OctreeT<T>::getAllRayCastTriPolygonIDs ( const Ray3T<T> &ray )const {
	std::vector<u32> result;
	this->getTriPolyIDsRayIntersect ( this->nodeTree, ray, result );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMesh����Octree���\�z����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::set ( const PolygonMeshT<T> &mesh ){
	std::vector<f32> dummy;
	this->set ( mesh, dummy, false );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMesh����Octree���\�z����
//!	    @brief	PolygonMesh����Octree���\�z����
//!		@param[in] mask VertexValue�@
//!		@param[in] invertMask�@�}�X�N�𔽓]
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::set ( const PolygonMeshT<T> &mesh, const std::vector<f32> &mask, Bool invertMask ) {
	assert ( 0<mesh.polygonCount () );

	PolygonMeshT<T> triMesh = mesh.createPureTriangleMesh ();
	Vec3AttributeT<T> &meshPointPosAttr = triMesh.getVec3Attribute ( "pointPositions" );
	U32AttributeT<T> &origPolyIDAttr = triMesh.getU32Attribute ( "OriginalPolygonIDs" );
	this->triPolygons.resize ( triMesh.polygonCount () );
	u32 cnt = triMesh.polygonCount ();
	#pragma omp parallel for
	for ( s32 i = 0; i<cnt; i++ ) {
		/* mesh���I�N�g���[�g���C�A���O���|���S���ɕϊ� */
		L8Array<u32> polyPoiIDs = triMesh.getPolygonPointIDs ( i );
		OctreeTriPolygonT<T> triPoly;
		{
			triPoly.p0 = meshPointPosAttr [polyPoiIDs [0]];
			triPoly.p1 = meshPointPosAttr [polyPoiIDs [1]];
			triPoly.p2 = meshPointPosAttr [polyPoiIDs [2]];
			triPoly.bBox = BBoxT<T> ( triPoly.p0, triPoly.p1 );
			triPoly.bBox.merge ( triPoly.p2 );
			triPoly.triPolygonID = u32 ( i );
			u32 loTriID;
			triPoly.origPolygonID = Burgess2MathFn::BinarySearchTree( origPolyIDAttr.getValuesRef (), i, loTriID );
			triPoly.localTriID = loTriID;
		}
		this->triPolygons [i] = triPoly;
	}

	/* octreeNode�c���[�̃��[�g���쐬���� */
	BBoxT<T> rootBBox ( meshPointPosAttr.getValuesRef () );
	this->nodeTree = OctreeNodeT<T> ( u8 ( 1 ), rootBBox.getCenter (), rootBBox.getLongestSideLength () );

	/* octreeNode�c���[���쐬���� (�O�p�`�|���S�����i�[���Ă���) */
	u32 triPolyCount = this->triPolygons.size ();
	if ( mask.size () == mesh.vertexCount () ) {
		/* Mask���g���ꍇ��VertexAttribute��PolygonAttribute�֕ϊ� */
		u32 polyCount = mesh.polygonCount ();
		std::vector<Bool> origPolyMask ( polyCount );
		#pragma omp parallel for if(1000 < polyCount)
		for ( s32 i = 0; i<polyCount; i++ ) {
			L8Array<u32> verIDs = mesh.getPolygonVertexIDs ( i );
			u32 verSize = verIDs.size ();
			u8 sum = 0;
			for ( u32 k = 0; k <verSize; k++ ) {
				if ( 0.0f < mask[ verIDs [k] ] ) {
					sum++;
				}
			}
			/* �|���S�����\������S�Ẵo�[�e�b�N�X(mask)�ɒl���ݒ肳��Ă���ꍇ��false, �l���O�̃o�[�e�b�N�X��1�ł��������ꍇ��true */
			origPolyMask [i] = verSize == sum ? false : true;
		}
		/* origPolyMask�ɑ�����triPolygon��Octree�Ɋi�[���� */
		for ( u32 i = 0; i<triPolyCount; i++ ) {
			u32 polyID = this->triPolygons[i].origPolygonID;
			if ( origPolyMask [polyID] == invertMask ) {
				this->buildNodeTreeTask ( this->nodeTree, this->triPolygons [i] );
			}
		}
	} else {
		/* �S�Ă�TriPolygon���g�p */
		for ( u32 i = 0; i<triPolyCount; i++ ) {
			this->buildNodeTreeTask ( this->nodeTree, this->triPolygons [i] );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �O�p�`�|���S��ID��OctreeNode�c���[�ɓo�^���� (�K�v�ɉ����Ďq�K�w�̍쐬���s��)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::buildNodeTreeTask ( IO OctreeNodeT<T> &node, const OctreeTriPolygonT<T> &triPoly ){
	if (node.isLeafNode ()) { /* node�����[�m�[�h�̏ꍇ */
		if (node.testIntersect ( triPoly.bBox )) { /* �O�p�|���S����BBox��node�ƐڐGor�����ꍇ */
			if (node.polyCount () < BurgessOctree::MAX_TRIPOLYGON_COUNT ) { /* node�ɓo�^���Ă���|���S���������E�ɒB���Ă��Ȃ����`�F�b�N */
				node.addTriPolyID ( triPoly.triPolygonID ); /* �|���S��ID��node�ɓo�^���ďI�� */
			} else {
				node.addTriPolyID ( triPoly.triPolygonID ); /* ��U���̃m�[�h�Ƀ|���S��ID��o�^ */
				if (node.level < BurgessOctree::MAX_OCTREE_LEVEL) {
					/* �q���K�w���܂�����(�c���[�̊K�w�������E�ɒB���Ă��Ȃ��ꍇ) */
					this->addBranchAndReallocate ( node ); /* �q���K�w�������node�ɓo�^����Ă����|���S�����q�K�w�ɍĊ��蓖�Ă��� */
				}
			}
		}
	} else {
		/* ���[�m�[�h�܂ŒH�� */
		for (u32 i = 0; i<8; i++) {
			this->buildNodeTreeTask ( node.branches[i], triPoly );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    node�̎q���K�w���쐬���Anode�Ɋi�[���Ă����O�p�`�|���S���̏����q�K�w�֍Ċ��蓖�Ă���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::addBranchAndReallocate ( IO OctreeNodeT<T> &node ) {
	/* node��8�������Ďq�K�w�ɂ��� */
	Vec3T<T> childNodePos;
	for ( u32 i = 0; i < 8; i++ ) {
		childNodePos = node.position + node.sideLength * T ( 0.25 ) * this->branchOffset [i];
		node.addNode ( node.level + 1, childNodePos, node.sideLength * T ( 0.5 ) );
	}

	/* node�ɓo�^����Ă����|���S��ID�Q���q���K�w�ɍĊ��蓖�Ă��� */
	for ( u32 i = 0; i<8; i++ ) {
		for ( u32 k = 0; k<node.polyCount (); k++ ) { 
			u32 triPolyID = node.triPolyIDs [k];
			if ( this->triPolygons [triPolyID].bBox.testIntersect ( node.branches [i] ) ) { /* �O�p�|���S����BBox��node�̎q���m�[�h�ƐڐGor�����ꍇ */
				if ( node.branches [i].polyCount () < BurgessOctree::MAX_TRIPOLYGON_COUNT ) {/* node�ɓo�^�ł���O�p�`�|���S���̐������E�l�𒴂��Ă��Ȃ����`�F�b�N */
					node.branches [i].addTriPolyID ( triPolyID ); /* �q�m�[�h�Ƀ|���S��ID��o�^���ďI�� */
				} else {
					if ( node.branches [i].level < BurgessOctree::MAX_OCTREE_LEVEL ) {
						this->addBranchAndReallocate ( node.branches [i] ); /* �X�Ɏq���K�w�̍쐬�֐i�� */
					}
				}
			}
		}
	}
	node.triPolyIDs.resize ( 0 ); /* node�ɓo�^����Ă����|���S��ID�Q���N���A */
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ��苗�����ɑ��݂���OctreeTriPolygon��ID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::getTriPolyIDsShortestDistance ( const OctreeNodeT<T> &node, const Vec3T<T> &startPosition, T maxDist, OUT std::vector<u32> &result )const {
	BSphT<T> bs ( node.position, node.radius );
	if ( bs.testClosest ( startPosition, maxDist ) ) { // �o�E���f�B���O�X�t�B�A�ŊȈՂȋߖT�������s��
		if ( node.testClosest ( startPosition, maxDist ) ) { // OctreeNode(�o�E���f�B���O�{�b�N�X)�ɑ΂��Đ��m�ȋߖT�������s��
			if ( node.isLeafNode () ) { // node�����[�m�[�h���`�F�b�N
				// node�Ɋi�[����Ă���S�Ă̎O�p�|���S��ID�𔲂��o��
				for ( u32 i = 0; i<node.polyCount (); i++ ) {
					result.push_back( node.triPolyIDs [i] );
				}
			} else {
				// ���[�̃m�[�h�܂ŒH��
				for ( u32 i = 0; i<8; i++ ) {
					this->getTriPolyIDsShortestDistance ( node.branches [i], startPosition, maxDist, result );
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Ray�ƌ�������OctreeTriPolygon��ID���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void OctreeT<T>::getTriPolyIDsRayIntersect ( const OctreeNodeT<T> &node, const Ray3T<T> &ray, OUT std::vector<u32> &result )const {
	BSphT<T> bs ( node.position, node.radius );
	if (bs.testRayIntersect ( ray )) { // �o�E���f�B���O�X�t�B�A�ŊȈՂȌ���������s��
		if (node.testRayIntersect ( ray )) { // OctreeNode(�o�E���f�B���O�{�b�N�X)�ɑ΂��Đ��m�Ȍ���������s��
			if (node.isLeafNode ()) { // node�����[�m�[�h���`�F�b�N
				//node�Ɋi�[����Ă���S�Ă̎O�p�|���S��ID�𔲂��o��
				for (u32 i = 0; i<node.polyCount (); i++) {
					result.push_back ( node.triPolyIDs[i] );
				}
			} else {
				// ���[�̃m�[�h�܂ŒH��
				for (u32 i = 0; i<8; i++) {
					this->getTriPolyIDsRayIntersect ( node.branches[i], ray, result );
				}
			}
		}
	}
}

template<typename T>
void OctreeT<T>::test_getTriPolyIDsRayIntersect ( const OctreeNodeT<T>& node, const Ray3T<T>& ray, OUT std::vector<u32>& result )const {
	result.resize ( 0 );
	BSphT<T> bs ( node.position, node.radius );
	if (bs.testRayIntersect ( ray )) { /* �o�E���f�B���O�X�t�B�A�ŊȈՂȌ���������s�� */
		if (node.testRayIntersect ( ray )) { /* OctreeNode(�o�E���f�B���O�{�b�N�X)�ɑ΂��Đ��m�Ȍ���������s�� */
			if (node.isLeafNode ()) { /* node�����[�m�[�h���`�F�b�N */
				/* node�Ɋi�[����Ă���S�Ă̎O�p�|���S��ID�𔲂��o�� */
				for (u32 i = 0; i < node.polyCount (); i++) {
					result.push_back ( node.triPolyIDs[i] );
				}
			}
			else {
				/* ���[�̃m�[�h�܂ŒH�� */
				for (u32 i = 0; i < 8; i++) {
					this->getTriPolyIDsRayIntersect ( node.branches[i], ray, result );
				}
			}
		}
	}
}