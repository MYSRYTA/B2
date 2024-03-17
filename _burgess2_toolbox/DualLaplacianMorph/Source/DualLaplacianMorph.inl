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
//!	    �x�[�X���b�V���ƃ^�[�Q�b�g��o�^����
//!	    @brief �x�[�X���b�V���ƃ^�[�Q�b�g��o�^����
//!		@param[in] baseMesh
//!		@param[in] targetMesh �ibaseMesh�Ɠ����g�|���W�[�̃��b�V���j
//!		@param[in] mask �����ΏۂƂ��钸�_ID�̃��X�g(�z�񐔂�0�̏ꍇ�͑S���_�������ΏۂƂ���)
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
//!	    solve (mesh�̃g�|���W�[�`�F�b�N�͎��O��sameTopology()�ōς܂��Ă���)
//!	    @brief solve�@(mesh�̃g�|���W�[�`�F�b�N�͎��O��sameTopology()�ōς܂��Ă���)
//!		@param[in] mesh �����ΏۂƂȂ郁�b�V���ibaseMesh�Ɠ����g�|���W�[�̃��b�V���j
//!		@param[in] iterateRatio �}�b�N�X�C�e���[�V�����ɑ΂��郌�V�I 0.0~1.0
//!		@param[in] weight ���̂̃C�e���[�V�����ł̃u�����h�l
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void DualLaplacianMorphT<T>::solve ( IO PolygonMeshT<T> &mesh, T iterate, T weight ){
	if ( this->initialized == false ){ return; }

	this->baseDualMesh.setAllPointPositions ( this->baseDualMeshPoiPos );
	u32 n = this->baseDualMesh.pointCount ();

	// ���_�ɗאڂ��钸�_�̃��X�g���������Ă���
	std::vector<L16Array<u32>> surroundPoiIDs ( n );
	#pragma omp parallel for
	for ( s32 i = 0; i < n; ++i ){
		surroundPoiIDs[i] = this->baseDualMesh.getPointSurroundingPoints ( i, false );
	}

	for ( u32 ite = 0; ite < iterate; ++ite ){
		// DualMesh��Dual-Laplacian-Info��V�K�쐬or�X�V
		std::vector<Vec3T<T>> restDualInfo = this->computeDualLaplacianInfo( this->baseDualMesh, surroundPoiIDs );

		#pragma omp parallel for
		for ( s32 i = 0; i < n; ++i ){
			if ( restDualInfo[i].x < T ( 0.0 ) ){ continue; }

			// DualMesh�̒��_�ʒu���X�V
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

	// DualMesh�̕ω��ʂ�mesh�̒��_�ɉ��Z����
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
//!	    Dual-Laplacian-Info���Z�o����
//!	    @brief�@Dual-Laplacian-Info���Z�o����
//!		@param[in] dualMesh DualMesh
//!		return Vec3.x=barycentric0, .y=barycentric1, .z=distance (barycentric2�́u1.0-bary0-bary1�v�ŎZ�o)
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
		// ���_�ɃG�b�W�ŗאڂ��钸�_ID�Q���擾
		//L16Array<u32> surroundPoiIDs = dualMesh.getPointSurroundingPoints ( i, false );
		if ( surroundPoiIDs[i].size () != 3 ){
			// �G�b�W�ŗאڂ��钸�_�����R�ȊO�̏ꍇ�͕s���l��ݒ肵�ďI��
			result[i] = Vec3T<T> ( T ( -1.0 ) );
			continue;
		}

		// �אڂ���R���_�ō��O�p�`�ɑ΂��āA�ł��߂��ʒu�܂ł̋�����barycentric��ۑ�����
		// Vec3(.x = barycentric0, .y = barycentric1, .z = distance)
		Vec3T<T> p = dualMesh.getPointPosition ( i );
		Vec3T<T> tP0 = dualMesh.getPointPosition ( surroundPoiIDs[i][0] );
		Vec3T<T> tP1 = dualMesh.getPointPosition ( surroundPoiIDs[i][1] );
		Vec3T<T> tP2 = dualMesh.getPointPosition ( surroundPoiIDs[i][2] );
		Vec3T<T> cPos = Math::GetClosestPositionOnTriangle ( p, tP0, tP1, tP2 );
		result[i] = Math::GetTriBarycentric ( cPos, tP0, tP1, tP2 );
		Vec3T<T> normal = Math::GetTriangleNormal ( tP0, tP1, tP2 );
		if ( p.almostEqual ( cPos ) ){
			result[i].z = T ( 0.0 ); // p��closestPoint�������ʒu�̏ꍇ�͋����O�œo�^
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