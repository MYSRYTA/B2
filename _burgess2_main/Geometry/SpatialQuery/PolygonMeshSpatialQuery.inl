//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshSpatialQueryT<T>::PolygonMeshSpatialQueryT() {
	// �f�o�C�X�ƃV�[����������
	this->device = rtcNewDevice(NULL);
	this->scene = rtcNewScene(this->device);
	rtcCommitScene(this->scene);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMesh�����ԕ��͂̂��߂̍������\�����\�z����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshSpatialQueryT<T>::setup ( const PolygonMeshT<T> &mesh ) {
	if (0 == mesh.polygonCount()) { return false; }

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// embree�̃V�[�����\�z�ɕK�v�ȃf�[�^����������
	// 
	// ���b�V���̃|�C���g�|�W�V�����A�g���r���[�g���R�s�[
	this->pointPositions = mesh.getVec3Attribute("pointPositions").getValues();

	s32 polyCnt = mesh.polygonCount ();
	// ���b�V����packedPointIDs���R�s�[
	if (mesh.isPureTriangleMesh ()) {
		// �O�p�|���S�����b�V���̏ꍇ
		this->packedPointIDs = mesh.getU32Attribute("packedPointIDs").getValues();
		// BurgessEmbree::trianglePolygonInfo�̓o�^
		this->packedPointIDs.resize ( polyCnt );
		#pragma omp parallel for 
		for (s32 i = 0; i < polyCnt; ++i) {
			this->triangleInfo[i].origPolygonID = i;
			this->triangleInfo[i].localTriID = 0;
		}
	} else {
		// ���p�`�|���S�����b�V���̏ꍇ
		this->triangleInfo.reserve ( polyCnt * 2 ); // �S�|���S�����l�p�`���Ɖ��肵�ă��������m��
		this->packedPointIDs.reserve ( polyCnt * 2 * 3 ); // �S�|���S�����l�p�`���Ɖ��肵�ă��������m��
		// �O�p�`�|���S���̃��X�g�ɕϊ����ēo�^
		for (s32 i = 0; i < polyCnt; ++i) {
			L8Array<u32> polyPoiIDs = mesh.getPolygonPointIDs ( i );
			s8 maxItr = s8(polyPoiIDs.size () - 2);
			for (s8 j = 0; j < maxItr; ++j) {
				this->packedPointIDs.push_back ( polyPoiIDs[0] );
				this->packedPointIDs.push_back ( polyPoiIDs[j + 1] );
				this->packedPointIDs.push_back ( polyPoiIDs[j + 2] );

				// BurgessEmbree::trianglePolygonInfo�̓o�^
				BurgessEmbree::TrianglePolygonInfo<T> info;
				info.origPolygonID = i;
				info.localTriID = j;
				this->triangleInfo.push_back ( info );
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// embree�̃V�[�����\�z����

	// �f�o�C�X���쐬
	this->device = rtcNewDevice ( NULL );
	// �V�[�����쐬
	this->scene = rtcNewScene ( this->device );
	// �W�I���g��������
	RTCGeometry geom = rtcNewGeometry ( this->device, RTC_GEOMETRY_TYPE_TRIANGLE );
	// �W�I���g���ɒ��_�����Z�b�g����
	s32 poiCnt = s32(this->pointPositions.size ());
	T* vertices = (float*)rtcSetNewGeometryBuffer ( geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof ( T ), poiCnt );

	// Github�Œ�Ă���Ă������x�̃o�O�Ή�
/*	for ( s32 i = 0; i < poiCnt; ++i ){
		Vec3T<T> poiNml = mesh.getPointAverageNormal( i );
		this->pointPositions[i] += ( poiNml * T( 0.00001 ) );
	}*/
	for (s32 i = 0; i < poiCnt; ++i) {
		Vec3T<T> poiPos = this->pointPositions[ i ];
		vertices[i * 3] = poiPos.x();
		vertices[i * 3 + 1] = poiPos.y();
		vertices[i * 3 + 2] = poiPos.z();
	}
	// �W�I���g���ɎO�p�|���S���̃|�C���g�p�b�N���Z�b�g����
	u32 triPolyCnt = u32(this->packedPointIDs.size () / 3);
	s32 verCnt = s32(this->packedPointIDs.size ());
	unsigned* indices = (unsigned*)rtcSetNewGeometryBuffer ( geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof ( unsigned ), triPolyCnt );
	for (s32 i = 0; i < verCnt; ++i) {
		indices[i] = this->packedPointIDs[i];
	}

	// embree�V�[���֓o�^
	rtcSetGeometryPointQueryFunction ( geom, BurgessEmbree::closestPointFunc<T> );
	rtcCommitGeometry ( geom );
	rtcAttachGeometry ( this->scene, geom );
	rtcReleaseGeometry ( geom );
	rtcCommitScene ( this->scene );
	
	// ���o�X�g���[�h ON
	RTCSceneFlags flags = rtcGetSceneFlags( this->scene );
	rtcSetSceneFlags( this->scene, RTC_SCENE_FLAG_ROBUST | flags );
	
	this->initialized = true;
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMesh�����ԕ��͂̂��߂̍������\�����\�z����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshSpatialQueryT<T>::setup ( const PolygonMeshT<T> &mesh, const String& maskAttrName, Bool invertMask ) {
	/*�@����e�X�g�̎��Ԃ��Ȃ��̂ň�UOFF
	s32 polyCnt = mesh.polygonCount ();
	if (polyCnt == 0) { this->initialized = false; return; }

	if (mesh.hasAttribute ( maskAttrName ) == false ) {
		// �}�X�N�Ƃ��Ďg�p����A�g���r���[�g�����݂��Ȃ������ꍇ
		*this = PolygonMeshSpatialQueryT<T> ( mesh );
	} else {
		const u8 attrType = mesh.getAttrType ( maskAttrName );
		if (attrType != 9 && attrType != 10 && attrType != 12 && attrType != 13 && attrType != 14) {
			*this = PolygonMeshSpatialQueryT<T> ( mesh );
		}
		
		std::vector<Bool> en ( polyCnt );
		#pragma omp parallel for 
		for (s32 i = 0; i < polyCnt; ++i) {
			L8Array<u32> polyPoiIDs = mesh.getPolygonPointIDs ( i );
			for (u32 j = 0; j < polyPoiIDs.size (); ++j) {
				u32 id = polyPoiIDs[j];
				Bool b;
				switch (attrType) {
					case 9:
						b = Burgess2MathFn::AlmostZero ( mesh.getF32Attribute ( maskAttrName )[id], f32 ( 0.001 ) );
						break;
					case 10:
						b = Burgess2MathFn::AlmostZero ( mesh.getF64Attribute ( maskAttrName )[id], f64 ( 0.001 ) );
						break;
					case 12:
						Vec2T<T> *v = mesh.getVec2Attribute ( maskAttrName )[id];
						b = v->almostEqual ( Vec2T<T>, T ( 0.001 ) );
						break;
					case 13:
						Vec3T<T> *v = mesh.getVec3Attribute ( maskAttrName )[id];
						b = v->almostEqual ( Vec3T<T>, T ( 0.001 ) );
						break;
					case 14:
						Vec4T<T> *v = mesh.getVec4Attribute ( maskAttrName )[id];
						b = v->almostEqual ( Vec4T<T>, T ( 0.001 ) );
						break;
				}
				if(b==true){
					en[i] = true;
					break;
				}
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////
		// embree�̃V�[�����\�z�ɕK�v�ȃf�[�^����������

		// ���b�V���̃|�C���g�|�W�V�����A�g���r���[�g���R�s�[
		Vec3Attribute* posAttr = &mesh.getVec3Attribute ( "pointPositions" );
		this->pointPositions.reserve ( posAttr->size () );
		std::copy ( posAttr->getValuesRef ().begin (), posAttr->getValuesRef ().end (), this->pointPositions.begin () );

		s32 polyCnt = mesh.polygonCount ();
		// ���b�V����packedPointIDs���R�s�[
		if (mesh.isPureTriangleMesh ()) {
			// �O�p�|���S�����b�V���̏ꍇ
			U32Attribute* poiIDsAttr = &mesh.getU32Attribute ( "packedPointIDs" );
			this->packedPointIDs.reserve ( poiIDsAttr->size () );
			std::copy ( poiIDsAttr->getValuesRef ().begin (), poiIDsAttr->getValuesRef ().end (), this->packedPointIDs.begin () ); �}�X�N�̃|���S�������o�^����悤�ɕύX
			
			// embree::trianglePolygonInfo�̓o�^
			this->packedPointIDs.resize ( polyCnt );
			#pragma omp parallel for 
			for (s32 i = 0; i < polyCnt; ++i) {
				this->triangleInfo[i].origPolygonID = i;
				this->triangleInfo[i].localTriID = 0;
			}
		}
		else {
			// ���p�`�|���S�����b�V���̏ꍇ
			this->triangleInfo.reserve ( polyCnt * 2 ); // �S�|���S�����l�p�`���Ɖ��肵�ă��������m��
			this->packedPointIDs.reserve ( polyCnt * 2 * 3 ); // �S�|���S�����l�p�`���Ɖ��肵�ă��������m��
			// �O�p�`�|���S���̃��X�g�ɕϊ����ēo�^
			for (u32 i = 0; i < polyCnt; ++i) {
				if (invertMask == false) {
					if (en[i] == false) { continue; }
				} else {
					if (en[i] == true) { continue; }
				}
				L8Array<u32> polyPoiIDs = mesh.getPolygonPointIDs ( i );
				u32 maxItr = polyPoiIDs.size () - 2;
				for (u32 j = 0; j < maxItr; ++j) {
					this->packedPointIDs.push_back ( polyPoiIDs[0] );
					this->packedPointIDs.push_back ( polyPoiIDs[j + 1] );
					this->packedPointIDs.push_back ( polyPoiIDs[j + 2] );

					// embree::trianglePolygonInfo�̓o�^
					embree::TrianglePolygonInfo info;
					info.origPolygonID = i;
					info.localTriID = j;
					this->triangleInfo.push_back ( info );
				}
			}
		}
	}*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    startPosition����PolygonMeshSpatialQuery�̕\�ʏ�ւ̍ł��߂��ʒu���擾����
//!		@brief	startPosition����PolygonMeshSpatialQuery�̕\�ʏ�ւ̍ł��߂��ʒu���擾����
//!		@param[in] startPosition�@�X�^�[�g�ʒu
//!		@param[in] maxDistance�@��������
//!		@param[in] faceSide�@�����Ώۂ̃|���S���t�F�[�X�� 0=���ʁA1=�\�ʁA2=����
//!		@return �ʒu
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> PolygonMeshSpatialQueryT<T>::getClosestPosition ( const Vec3T<T>& startPosition, T maxDistance, u8 faceSide )const {
	if (this->isValid () == false) { return Vec3T<T>(T(0.0)); }

	BurgessEmbree::PointQuery<T> closestFuncData = this->getClosest_task(startPosition, maxDistance, faceSide);
	
	return closestFuncData.restClosestPosition;
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
GeometryLocationT<T> PolygonMeshSpatialQueryT<T>::getClosestLocation ( const Vec3T<T> &startPosition, T maxDistance, u8 faceSide ) const {
	GeometryLocationT<T> result;
	if (this->isValid () == false) { return result; }

	BurgessEmbree::PointQuery<T> closestFuncData = this->getClosest_task(startPosition, maxDistance, faceSide);

	Vec3T<T> locPos = closestFuncData.restClosestPosition;
	u32 locTriID = closestFuncData.restClosestTriangleID;
	u32 polyPoiIDs[3] = { this->packedPointIDs[locTriID * 3], this->packedPointIDs[locTriID * 3 + 1], this->packedPointIDs[locTriID * 3 + 2] };
	Vec3T<T> weight = Burgess2MathFn::GetTriBarycentric ( locPos, this->pointPositions[polyPoiIDs[0]], this->pointPositions[polyPoiIDs[1]], this->pointPositions[polyPoiIDs[2]] );

	BurgessEmbree::TrianglePolygonInfo<T> locTriInfo = this->triangleInfo[locTriID];
	return GeometryLocationT<T>( locTriInfo.origPolygonID, locTriInfo.localTriID, weight, 3 );	
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
GeometryLocationT<T> PolygonMeshSpatialQueryT<T>::raycast ( const Ray3T<T> &ray, T maxDistance, u8 faceSide ) const {
	GeometryLocationT<T> result;
	if (this->isValid () == false) { return result; }

	// ���C�𐶐�����
	struct RTCRayHit rayhit = BurgessEmbree::toEmbreeRay<T>(ray, maxDistance);

	// ��������
	struct RTCIntersectContext context2;
	rtcInitIntersectContext ( &context2 );
	BurgessEmbree::HitList<T> hList = BurgessEmbree::getRayIntersect_task<T> ( this->scene, rayhit, faceSide );
	
	// GeometryLocation���쐬
	u32 triID = hList.hits[hList.begin].primID;
	T uVal = Burgess2MathFn::Clamp(hList.hits[hList.begin].u, T(0.0), T(1.0));
	T vVal = Burgess2MathFn::Clamp(hList.hits[hList.begin].v, T(0.0), T(1.0));
	T wVal = Burgess2MathFn::Max(T(0.0), T(1.0) - uVal - vVal );
	return GeometryLocationT<T>(this->triangleInfo[triID].origPolygonID, this->triangleInfo[triID].localTriID, Vec3T<T>{ wVal, uVal, vVal }, 3 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Ray�����������S�Ẵ��P�[�V�������߂����Ŏ擾����
//!		@brief	Ray�����������S�Ẵ��P�[�V�������߂����Ŏ擾����
//!		@param[in] ray
//!		@param[in] maxDistance�@��������
//!		@param[in] faceSide�@�����Ώۂ̃|���S���t�F�[�X�� 0=���ʁA1=�\�ʁA2=����
//!		@param[in] tolerance�@�����n�_���I���G�b�W�E�I���|�C���g���𔻒肷�鐸�x (���͒l��BURGESS2::TOLERANCE~0.1)
//!		@return std::vector<GeometryLocation> ���������S�Ẵ��P�[�V�����i�߂����j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<GeometryLocationT<T>> PolygonMeshSpatialQueryT<T>::getAllRayIntersect( const Ray3T<T>& ray, T maxDistance, u8 faceSide, T tolerance ) const {
	std::vector<GeometryLocationT<T>> result( 0 );
	if (this->isValid () == false) { return result; }
	
	// ���C�𐶐�����
	struct RTCRayHit rayhit = BurgessEmbree::toEmbreeRay<T> ( ray, maxDistance );
	
	// ��������
	struct RTCIntersectContext context2;
	rtcInitIntersectContext ( &context2 );
	BurgessEmbree::HitList<T> hList = BurgessEmbree::getRayIntersect_task<T> ( this->scene, rayhit, faceSide);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���P�[�V�������I���G�b�W/�I���|�C���g�������ꍇ
	// �אڂ��镡���̃|���S���̒�����ł��Ⴂ�|���S���ԍ��A�X�Ƀg���C�A���O���̔ԍ������������̂�GeometryLocation��o�^����
	
	result.reserve(hList.size());

	// �o�^�ς݂̃I���|�C���g�E�I���G�b�W�𔻒肷�邽�߂̕ϐ�������
	std::vector<u32> pushedPoi;
	std::vector<Vec2u> pushdEdgePoi;
	std::vector<Vec2T<T>> pushedEdgeWei;
	pushedPoi.reserve(hList.size());
	pushdEdgePoi.reserve(hList.size());
	pushedEdgeWei.reserve(hList.size());
	T t = Burgess2MathFn::Clamp( tolerance, T(BURGESS2::TOLERANCE), T( 0.1 ) );

	for (u32 i = hList.begin; i < hList.end; ++i) {
		// ���P�[�V�����̃|���S���̔ԍ�
		u32 locTriID = hList.hits[i].primID;

		// �d�S�E�F�C�g�l���쐬
		T uVal = Burgess2MathFn::Clamp(hList.hits[i].u, T(0.0), T(1.0));
		T vVal = Burgess2MathFn::Clamp(hList.hits[i].v, T(0.0), T(1.0));
		T wVal = Burgess2MathFn::Clamp(T(1.0) - uVal - vVal, T(0.0), T(1.0));
		Vec3T<T> weight{ wVal, uVal, vVal };
		if ( Burgess2MathFn::AlmostZero( weight.x() ) ){
			weight.x() = T( 0.0 );
			weight.y() = weight.y() / ( weight.y() + weight.z() );
			weight.z() = weight.z() / ( weight.y() + weight.z() );
		} else if ( Burgess2MathFn::AlmostZero( weight.y() ) ){
			weight.x() = weight.x() / ( weight.x() + weight.z() );
			weight.y() = T( 0.0 );
			weight.z() = weight.z() / ( weight.x() + weight.z() );
		} else if ( Burgess2MathFn::AlmostZero( weight.z() ) ){
			weight.x() = weight.x() / ( weight.x() + weight.y() );
			weight.y() = weight.y() / ( weight.x() + weight.y() );
			weight.z() = T( 0.0 );
		}

		// �E�F�C�g�l��0�ł͂Ȃ����_�ԍ��𔲂��o��
		L4Array<u32> locPoiIDs;
		L4Array<T> locPoiWeight;
		for (u32 j = 0; j < 3; ++j) { 
			if (Burgess2MathFn::AlmostZero(weight[j], t ) == false) {
				locPoiIDs.push(this->packedPointIDs[locTriID *3+j]);
				locPoiWeight.push(weight[j]);
			} 
		}

		// �����o�������_�ԍ��̍��v����1or2�̏ꍇ�̓I���|�C���g�E�I���G�b�W�ł���Ɣ���
		switch (locPoiIDs.size()) {
			case 0: break;
			case 1: {// �I���|�C���g
				// ���ɃI���|�C���g���o�^�ς݂��m�F
				Bool pushed = false;
				for (u32 k = 0; k < pushedPoi.size(); ++k) {
					if (locPoiIDs[0] == pushedPoi[k]) { pushed = true; break; }
				}
				if (pushed == true) { break; } 

				// �|�C���g�ɗאڂ���ł��Ⴂ�ԍ��̃|���S�������o
				for (u32 j = 0; j < this->packedPointIDs.size(); ++j) {
					if (locPoiIDs[0] == this->packedPointIDs[j]) {
						u32 triID = j / 3; 
						if (locTriID != triID) {
							// ��ԕ��͂̌��ʂ����Ⴂ�ԍ��̃|���S�������������ꍇ�A���_�z��ԍ��ɍ��킹�ďd�S�E�F�C�g�l�̐ݒ���Đݒ肷��
							for (u32 k = 0; k < 3; ++k) {
								if (locPoiIDs[0] == this->packedPointIDs[triID * 3 + k]) {
									weight[k] = T(1.0);
								} else {
									weight[k] = T(0.0);
								}
							}
						}
						// ���P�[�V�����f�[�^�Ƃ��ēo�^����
						BurgessEmbree::TrianglePolygonInfo<T> locTriInfo = this->triangleInfo[triID];
						result.push_back(GeometryLocationT<T>{locTriInfo.origPolygonID, locTriInfo.localTriID, weight, 3});

						// �I���|�C���g���o�^�ς݂ł���ƃ}�[�L���O����
						pushedPoi.push_back(locPoiIDs[0]);
						break;
					}
				}
				break;
			}
			case 2: { // �I���G�b�W
				// ���ɃI���G�b�W���o�^�ς݂��m�F
				Bool pushed = false;
				for (u32 k = 0; k < pushdEdgePoi.size(); ++k) {
					if (locPoiIDs[0] == pushdEdgePoi[k].x() && locPoiIDs[1] == pushdEdgePoi[k].y() ) {
						if (Burgess2MathFn::AlmostEqual(locPoiWeight[0], pushedEdgeWei[k].x() ) && Burgess2MathFn::AlmostEqual(locPoiWeight[1], pushedEdgeWei[k].y() )) {
							pushed = true; break;
						}
					}
					if (locPoiIDs[1] == pushdEdgePoi[k].x() && locPoiIDs[0] == pushdEdgePoi[k].y() ) {
						if (Burgess2MathFn::AlmostEqual(locPoiWeight[1], pushedEdgeWei[k].x() ) && Burgess2MathFn::AlmostEqual(locPoiWeight[0], pushedEdgeWei[k].y() )) {
							pushed = true; break;
						}
					}
				}
				if (pushed == true) { break; }

				// �G�b�W�ɗאڂ���ł��Ⴂ�ԍ��̃|���S�������o
				for (u32 j = 0; j < this->packedPointIDs.size(); ++j) {
					if (locPoiIDs[0] == this->packedPointIDs[j]) {
						u32 triID = j / 3;
						Bool find = false;
						for (u32 k = 0; k < 3; ++k) {
							if (locPoiIDs[1] == this->packedPointIDs[triID * 3 + k]) {
								find = true; break;
							}
						}
						if (find == false) { continue; }
						
						if (locTriID != triID) {
							// ��ԕ��͂̌��ʂ����Ⴂ�ԍ��̃|���S�������������ꍇ�A���_�z��ԍ��ɍ��킹�ďd�S�E�F�C�g�l�̐ݒ���Đݒ肷��
							for (u32 k = 0; k < 3; ++k) {
								if (locPoiIDs[0] == this->packedPointIDs[triID * 3 + k]) {
									weight[k] = locPoiWeight[0];
								} else if (locPoiIDs[1] == this->packedPointIDs[triID * 3 + k]) {
									weight[k] = locPoiWeight[1];
								} else {
									weight[k] = T(0.0);
								}
							}
						}
						// ���P�[�V�����f�[�^�Ƃ��ēo�^����
						BurgessEmbree::TrianglePolygonInfo<T> locTriInfo = this->triangleInfo[triID];
						result.push_back(GeometryLocationT<T>{locTriInfo.origPolygonID, locTriInfo.localTriID, weight, 3});

						// �I���G�b�W�o�^�ς݂ł���ƃ}�[�L���O����
						pushdEdgePoi.push_back(Vec2u{ locPoiIDs[0],locPoiIDs[1] });
						pushedEdgeWei.push_back(Vec2T<T>{ locPoiWeight[0],locPoiWeight[1] });
						break;
					}
				}
				break;
			}
			case 3: { // �I���t�F�[�X
				// ���P�[�V�����f�[�^�Ƃ��ēo�^����
				BurgessEmbree::TrianglePolygonInfo<T> locTriInfo = this->triangleInfo[locTriID];
				result.push_back(GeometryLocationT<T>{locTriInfo.origPolygonID, locTriInfo.localTriID, weight, 3});
				break;
			}
		}
	}

	result.shrink_to_fit();
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Line�����������S�Ẵ��P�[�V������start�ɋ߂����Ŏ擾����
//!		@brief	Line�����������S�Ẵ��P�[�V������start�ɋ߂����Ŏ擾����
//!		@param[in] start�@���C���̃X�^�[�g�ʒu
//!		@param[in] end�@���C���̃G���h�ʒu
//!		@param[in] faceSide�@�����Ώۂ̃|���S���t�F�[�X�� 0=���ʁA1=�\�ʁA2=����
//!		@param[in] tolerance�@�����n�_���I���G�b�W�E�I���|�C���g���𔻒肷�鐸�x (���͒l��BURGESS2::TOLERANCE~0.1)
//!		@return std::vector<GeometryLocation> ���������S�Ẵ��P�[�V�����i�߂����j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<GeometryLocationT<T>> PolygonMeshSpatialQueryT<T>::getAllLineIntersect ( const Vec3T<T> &start, const Vec3T<T> &end, u8 faceSide, T tolerance ) const {
	Ray3T<T> ray ( start, (end-start).unit() );
	T length = ( end-start ).length ();
	return this->getAllRayIntersect ( ray, length, faceSide, tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    PolygonMeshSpatialQuery���L���ȏꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshSpatialQueryT<T>::isValid () const {
	return this->initialized;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
BurgessEmbree::PointQuery<T> PolygonMeshSpatialQueryT<T>::getClosest_task(const Vec3T<T>& startPosition, T maxDistance, u8 _faceSide)const {
	BurgessEmbree::PointQuery<T> result;
	if (this->isValid() == false) { return result; }

	// �R�[�h�Q�Ɛ�@https://github.com/embree/embree/blob/master/tutorials/closest_point/closest_point_device.cpp 551-563
	RTCPointQuery query;
	query.x = startPosition.x();
	query.y = startPosition.y();
	query.z = startPosition.z();
	query.radius = maxDistance;
	query.time = 0.f;

	RTCPointQueryContext context;
	rtcInitPointQueryContext(&context);

	result.pointPositionsPtr = const_cast<std::vector<Vec3T<T>>*>(&this->pointPositions);
	result.packedPointIDsPtr = const_cast<std::vector<u32>*>(&this->packedPointIDs);
	result.closestFaceSide = _faceSide;

	rtcPointQuery(this->scene, &query, &context, nullptr, (void*)&result);
	return result;
}
