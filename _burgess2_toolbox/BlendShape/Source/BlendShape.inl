//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�^����Ă���V�F�C�v�^�[�Q�b�g�̐���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 BlendShapeT<T>::targetCount ()const{
	return this->targets.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �x�[�X���b�V���̓o�^�ƒ��_�~���[�}�b�v�𐶐�����
//!	    @brief �x�[�X���b�V���̓o�^�ƒ��_�~���[�}�b�v�𐶐�����
//!		@param[in] _baseMesh �x�[�X���b�V��
//!		@param[in] mirroringAxis �~���[�̎�:0=X,1=Y,2=Z
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setBaseMeshAndMirroringMap ( const PolygonMeshT<T> &_baseMesh, u8 mirroringAxis, T tolerance ) {
	if ( _baseMesh.pointCount () == 0  || this->initialized == true ) { return; }

	this->baseMesh = _baseMesh;

	Vec3T<T> mirVec ( T ( 1.0 ) );
	switch ( mirroringAxis ){
		case 0: mirVec.x = T ( -1.0 ); break;
		case 1: mirVec.y = T ( -1.0 ); break;
		case 2: mirVec.z = T ( -1.0 ); break;
		default: mirVec.x = T ( -1.0 );
	}
	u32 n = _baseMesh.pointCount ();
	this->mirroringPointIDMap.resize ( n );
	for ( s32 i = 0; i<n; i++ ){
		Vec3T<T> mirP = _baseMesh.getPointPosition ( i ) * mirVec;

		// ���_�����]����ɑ��݂���ꍇ�͎������g�̒��_ID��o�^���ďI��
		if ( mirP.almostEqual ( _baseMesh.getPointPosition ( i ), tolerance ) ){
			this->mirroringPointIDMap [i] = i;
			continue;
		}

		// �����ȊO�̒��_�ɑ΂��đΏے��_��T��
		s32 id = -1; // �Ώے��_�������ꍇ��-1
		for ( s32 j = 0; j<n; ++j ){
			if ( j == i ){ continue; }
			if ( mirP.almostEqual ( _baseMesh.getPointPosition ( j ), tolerance ) ){ id = j; break; }
		}
		this->mirroringPointIDMap [i] = id;
	}
	this->initialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �x�[�X���b�V���̓o�^�ƒ��_�~���[�}�b�v�̐����������I�ɂ�蒼��
//!	    @brief �x�[�X���b�V���̓o�^�ƒ��_�~���[�}�b�v�̐����������I�ɂ�蒼��
//!		@param[in] _baseMesh �x�[�X���b�V��
//!		@param[in] mirroringAxis �~���[�̎�:0=X,1=Y,2=Z
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::resetBaseMeshAndMirroringMap ( const PolygonMeshT<T> &_baseMesh, u8 mirroringAxis, T tolerance ){
	this->initialized = false;
	this->setBaseMeshAndMirroringMap ( _baseMesh, mirroringAxis, tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �V�F�C�v�^�[�Q�b�g�̐���ݒ肷��
//!	    @brief �V�F�C�v�^�[�Q�b�g�̐���ݒ肷��
//!		@param[in] size �V�F�C�v�^�[�Q�b�g�̐�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::resetTargetSize ( u32 size ){
	if ( size == this->targets.size () ){ return; } // �o�^�ς݂̃^�[�Q�b�g�Ɛ��������ł���ΏI��
	this->targets.resize ( size );
	this->mirroringTargets.resize ( size );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �V�F�C�v�^�[�Q�b�g��o�^����
//!	    @brief �V�F�C�v�^�[�Q�b�g��o�^����
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] targetMesh �V�F�C�v�^�[�Q�b�g���b�V���@�i�g�|���W�[��BlendShapeT��baseMesh�Ɠ����łȂ���Γo�^�ł����ɏI���j
//!		@param[in] normal �@�����u�����h�V�F�C�v�^�[�Q�b�g�Ɋ܂߂�ꍇ��true
//!		@param[in] threshold �^�[�Q�b�g��o�^����ۂɃx�[�X���b�V������ω������Ɣ��f����Œ�ω���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetMesh ( u32 targetID, const PolygonMeshT<T> &targetMesh, Bool normal, T threshold ){
	if ( this->targetCount () <= targetID ){ return; }
	std::vector<T> weight ( targetMesh.pointCount (), T ( 1.0 ) );
	this->setTargetMesh ( targetID, this->baseMesh, targetMesh, weight, normal, threshold );
	this->hasNormals = normal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �V�F�C�v�^�[�Q�b�g��o�^����
//!	    @brief �V�F�C�v�^�[�Q�b�g��o�^����
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] targetMesh �V�F�C�v�^�[�Q�b�g���b�V���@�i�g�|���W�[��BlendShapeT��baseMesh�Ɠ����łȂ���Γo�^�ł����ɏI���j
//!		@param[in] weight �e���_���Ƃ̃^�[�Q�b�g�̓o�^�̋������X�g�@0.0~1.0 (�z�񐔂�targetMesh�̒��_���ƈقȂ�ꍇ�͏I��)
//!		@param[in] normal �@�����u�����h�V�F�C�v�^�[�Q�b�g�Ɋ܂߂�ꍇ��true
//!		@param[in] threshold �^�[�Q�b�g��o�^����ۂɃx�[�X���b�V������ω������Ɣ��f����Œ�ω���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetMesh ( u32 targetID, const PolygonMeshT<T> &targetMesh, std::vector<T> &weight, Bool normal, T threshold ){
	this->setTargetMesh ( targetID, this->baseMesh, targetMesh, weight, normal, threshold );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    refMesh�Ƃ̍������^�[�Q�b�g�Ƃ��ēo�^���� 
//!	    @brief refMesh�Ƃ̍������^�[�Q�b�g�Ƃ��ēo�^����
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] refMesh �x�[�X���b�V���@�i�g�|���W�[��BlendShapeT��baseMesh�Ɠ����łȂ���Γo�^�ł����ɏI���j
//!		@param[in] targetMesh �V�F�C�v�^�[�Q�b�g���b�V���@�i�g�|���W�[��BlendShapeT��baseMesh�Ɠ����łȂ���Γo�^�ł����ɏI���j
//!		@param[in] normal �@�����u�����h�V�F�C�v�^�[�Q�b�g�Ɋ܂߂�ꍇ��true
//!		@param[in] threshold �^�[�Q�b�g��o�^����ۂɃx�[�X���b�V������ω������Ɣ��f����Œ�ω���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetMesh ( u32 targetID, const PolygonMeshT<T> &refMesh, const PolygonMeshT<T> &targetMesh, Bool normal, T threshold ){
	if ( this->targetCount () <= targetID ){ return; }
	std::vector<T> weight ( targetMesh.pointCount (), T ( 1.0 ) );
	this->setTargetMesh ( targetID, refMesh, targetMesh, weight, normal, threshold );
	this->hasNormals = normal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    refMesh�Ƃ̍������^�[�Q�b�g�Ƃ��ēo�^����
//!	    @brief refMesh�Ƃ̍������^�[�Q�b�g�Ƃ��ēo�^����
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] refMesh �x�[�X���b�V���@�i�g�|���W�[��BlendShapeT��baseMesh�Ɠ����łȂ���Γo�^�ł����ɏI���j
//!		@param[in] targetMesh �V�F�C�v�^�[�Q�b�g���b�V���@�i�g�|���W�[��BlendShapeT��baseMesh�Ɠ����łȂ���Γo�^�ł����ɏI���j
//!		@param[in] weight �e���_���Ƃ̃^�[�Q�b�g�̓o�^�̋������X�g�@0.0~1.0 (�z�񐔂�targetMesh�̒��_���ƈقȂ�ꍇ�͏I��)
//!		@param[in] normal �@�����u�����h�V�F�C�v�^�[�Q�b�g�Ɋ܂߂�ꍇ��true
//!		@param[in] threshold �^�[�Q�b�g��o�^����ۂɃx�[�X���b�V������ω������Ɣ��f����Œ�ω���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetMesh ( u32 targetID, const PolygonMeshT<T> &refMesh, const PolygonMeshT<T> &targetMesh, std::vector<T> &weight, Bool normal, T threshold ){
	if ( this->targetCount () <= targetID ){ return; }
	// �x�[�X���b�V���ƃ^�[�Q�b�g���b�V���̒��_�����قȂ�ꍇ�͏I��
	u32 poiCnt = targetMesh.pointCount ();
	if ( poiCnt == 0 || poiCnt != refMesh.pointCount() || poiCnt != weight.size() || poiCnt != this->baseMesh.pointCount() ){ return; }
	
	BlendShapeTargetT<T> targetData;
	Vec3AttributeT<T> &basePosAttribute = refMesh.getPointPositionAttribute ();
	Vec3AttributeT<T> &targetMeshPosAttribute = targetMesh.getPointPositionAttribute ();
	// ���_���Ƃɍ������Z�o���Ă���
	for ( u32 i = 0; i< poiCnt; ++i ){
		// �E�F�C�g�l���O�̒��_�̏ꍇ�͏I��
		if ( Math::IsZero ( weight [i] ) ){ continue; }
		// �ʒu�̍������������_�̏ꍇ�͏I��
		if ( basePosAttribute [i].almostEqual ( targetMeshPosAttribute [i], threshold ) ){ continue; }

		// ��������o�^����
		Vec3T<T> local = targetMeshPosAttribute [i] - basePosAttribute [i];
		GeometryDeltaT<T> delta;
		if ( normal ){
			// �@���̍��������܂߂�ꍇ
			L16Array<Vec3T<T>> baseNormals = refMesh.getPointNormal ( i );// �x�[�X���b�V���̃o�[�e�b�N�X�@��
			L16Array<Vec3T<T>> targetNormals = targetMesh.getPointNormal ( i );// �^�[�Q�b�g���b�V���̃o�[�e�b�N�X�@��
			if ( baseNormals.size () != targetNormals.size () ){ break; }// �x�[�X���b�V���ƃ^�[�Q�b�g���b�V���̃g�|���W�[�������ł��邱�Ƃ��m�F�i�O�����j
			std::vector<Vec3T<T>> nDelta ( baseNormals.size () );// �@���̍������̈ꎞ�ۑ��ꏊ
			// ���_���\������o�[�e�b�N�X���Ƃɖ@���̍������i�[���Ă���
			for ( u32 j = 0, m = baseNormals.size (); j<m; ++j ){
				nDelta [j] = targetNormals [j] - baseNormals [j];
			}
			delta.set ( GeometryLocationT<T> ( i, T ( 1.0 ), 1 ), local.normal(), local.length (), nDelta );
		} else{
			// �@���̍������͊܂߂Ȃ��ꍇ�i���_�ʒu�̍����̂݁j
			delta.set ( GeometryLocationT<T> ( i, T ( 1.0 ), 1 ), local.normal(), local.length () );
		}
		targetData.delta.push_back ( delta );
	}
	targetData.delta.shrink_to_fit (); // �z��̃��������œK��
	this->targets [targetID] = targetData;
	this->hasNormals = normal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �����̃^�[�Q�b�g���b�V������V�F�C�v�^�[�Q�b�g��o�^����i�㏑���j
//!	    @brief �����̃^�[�Q�b�g���b�V������V�F�C�v�^�[�Q�b�g��o�^����i�㏑���j
//!		@param[in] targetMesh �V�F�C�v�^�[�Q�b�g���b�V���Q�@�i�g�|���W�[���x�[�X���b�V���Ɠ����łȂ���Γo�^�ł����ɏI���j
//!		@param[in] normal �@�����u�����h�V�F�C�v�^�[�Q�b�g�Ɋ܂߂�ꍇ��true
//!		@param[in] threshold �^�[�Q�b�g��o�^����ۂɃx�[�X���b�V������ω������Ɣ��f����Œ�ω���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setAllTargetMesh ( const std::vector<PolygonMeshT<T>> &targetMesh, Bool normal, T threshold ){
	this->resetTargetSize ( targetMesh.size () );
	for ( u32 i = 0, n = targetMesh.size (); i < n; ++i ){
		this->setTargetMesh ( i, targetMesh[i], normal, threshold );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �V�F�C�v�^�[�Q�b�g��o�^����
//!	    @brief �V�F�C�v�^�[�Q�b�g��o�^����
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] targetPoints �V�F�C�v�^�[�Q�b�g�@(�z�񐔂�baseMesh�̒��_���ƈقȂ�ꍇ�͏I��)
//!		@param[in] threshold �^�[�Q�b�g��o�^����ۂɃx�[�X����ω������Ɣ��f����Œ�ω���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetPoints ( u32 targetID, const std::vector<Vec3T<T>> &targetPoints, T threshold ){
	std::vector<T> weight ( targetPoints.size (), T ( 1.0 ) );
	this->setTargetPoints ( targetID, this->baseMesh.getPointPositionAttribute ().getValuesRef (), targetPoints, weight, threshold );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    refPoints�Ƃ̍������^�[�Q�b�g�Ƃ��ēo�^����
//!	    @brief refPoints�Ƃ̍������^�[�Q�b�g�Ƃ��ēo�^����
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] targetPoints �V�F�C�v�^�[�Q�b�g�@(�z�񐔂�baseMesh�̒��_���ƈقȂ�ꍇ�͏I��)
//!		@param[in] weight �e���_���Ƃ̃^�[�Q�b�g�̓o�^�̋������X�g�@0.0~1.0 (�z�񐔂�targetPoints�̔z�񐔂ƈقȂ�ꍇ�͏I��)
//!		@param[in] threshold �^�[�Q�b�g��o�^����ۂɃx�[�X����ω������Ɣ��f����Œ�ω���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetPoints ( u32 targetID,
									   const std::vector<Vec3T<T>> &targetPoints,
									   const std::vector<T> &weight,
									   T threshold ){
	this->setTargetPoints ( targetID, this->baseMesh.getPointPositionAttribute ().getValuesRef(), targetPoints, weight, threshold );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �V�F�C�v�^�[�Q�b�g��o�^����
//!	    @brief �V�F�C�v�^�[�Q�b�g��o�^����
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] targetPoints �V�F�C�v�^�[�Q�b�g�@(�z�񐔂�baseMesh�̒��_���ƈقȂ�ꍇ�͏I��)
//!		@param[in] weight �e���_���Ƃ̃^�[�Q�b�g�̓o�^�̋������X�g�@0.0~1.0 (�z�񐔂�targetPoints�̔z�񐔂ƈقȂ�ꍇ�͏I��)
//!		@param[in] threshold �^�[�Q�b�g��o�^����ۂɃx�[�X����ω������Ɣ��f����Œ�ω���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetPoints ( u32 targetID,
									   const std::vector<Vec3T<T>> &refPoints,
									   const std::vector<Vec3T<T>> &targetPoints,
									   T threshold )
{
	std::vector<T> weight ( targetPoints.size (), T ( 1.0 ) );
	this->setTargetPoints ( targetID, refPoints, targetPoints, weight, threshold );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    refPoints�Ƃ̍������^�[�Q�b�g�Ƃ��ēo�^����
//!	    @brief refPoints�Ƃ̍������^�[�Q�b�g�Ƃ��ēo�^����
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] refPoints �x�[�X�@(�z�񐔂�baseMesh�̒��_���ƈقȂ�ꍇ�͏I��)
//!		@param[in] targetPoints �V�F�C�v�^�[�Q�b�g�@(�z�񐔂�baseMesh�̒��_���ƈقȂ�ꍇ�͏I��)
//!		@param[in] weight �e���_���Ƃ̃^�[�Q�b�g�̓o�^�̋������X�g�@0.0~1.0 (�z�񐔂�targetPoints�̔z�񐔂ƈقȂ�ꍇ�͏I��)
//!		@param[in] threshold �^�[�Q�b�g��o�^����ۂɃx�[�X����ω������Ɣ��f����Œ�ω���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetPoints ( u32 targetID,
									   const std::vector<Vec3T<T>> &refPoints,
									   const std::vector<Vec3T<T>> &targetPoints,
									   const std::vector<T> &weight,
									   T threshold )
{
	if ( this->targetCount () <= targetID ){ return; }
	// �x�[�X���b�V���ƃ^�[�Q�b�g���b�V���̒��_�����قȂ�ꍇ�͏I��
	u32 poiCnt = targetPoints.size ();
	if ( poiCnt == 0 || poiCnt != refPoints.size () || poiCnt != weight.size () || poiCnt != this->baseMesh.pointCount () ){ return; }

	BlendShapeTargetT<T> targetData;
	// ���_���Ƃɍ������Z�o���Ă���
	for ( u32 i = 0; i < poiCnt; ++i ){
		// �E�F�C�g�l���O�̒��_�̏ꍇ�͏I��
		if ( Math::IsZero ( weight[i] ) ){ continue; }
		// �ʒu�̍������������_�̏ꍇ�͏I��
		if ( refPoints[i].almostEqual ( targetPoints[i], threshold ) ){ continue; }

		// ��������o�^����
		Vec3T<T> local = targetPoints[i] - refPoints[i];
		GeometryDeltaT<T> delta;
		delta.set ( GeometryLocationT<T> ( i, T ( 1.0 ), 1 ), local.normal (), local.length () );
		targetData.delta.push_back ( delta );
	}
	targetData.delta.shrink_to_fit (); // �z��̃��������œK��
	this->targets[targetID] = targetData;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�^���Ă���V�F�C�v�^�[�Q�b�g�����E�Ώ̂ɂ���i�㏑���j
//!	    @brief ���ɓo�^���Ă���V�F�C�v�^�[�Q�b�g�����E�Ώ̂ɂ���i�㏑���j
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] symmetryAxis �Ώۂɂ��鎲:0=X,1=Y,2=Z
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setSymmetry ( u32 targetID, u8 mirroringAxis ){
	if ( this->targetCount () <= targetID ){ return; }
	if (this->targets[targetID].delta.size() == 0 ){ return; }

	BlendShapeTargetT<T> symmetryTarget;
	u32 origSize = this->targets[targetID].delta.size ();
	for ( u32 i = 0; i < origSize; ++i ){
		u32 id = this->targets[targetID].delta[i].getIndex();
		u32 mID = this->mirroringPointIDMap[id];
		if( mID == id || mID == -1){continue;} // ���_�����]����A�������͑Ώۂ������ꍇ�̓X���[
		Bool conflict;
		for ( u32 j = 0; j<origSize; ++j ){
			if ( j != i && this->targets[targetID].delta[j].getIndex () == mID ){ conflict = true; }
		}
		if ( conflict ){ continue; } // �Ώۂ̒��_�����Ƀ^�[�Q�b�g�ɓo�^����Ă����ꍇ���X���[

		GeometryLocationT<T> newLoc ( mID, this->targets[targetID].delta[i].getWeight(), 1 );
		Vec3T<T> mirror ( T ( 1.0 ) );
		mirror[mirroringAxis] = T ( -1.0 );
		GeometryDeltaT<T> newDelta;
		newDelta.set ( newLoc, this->targets[targetID].delta[i].getDirection () * mirror, this->targets[targetID].delta[i].getDistance () );
		
		this->targets[targetID].delta.push_back( newDelta );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�^���Ă���V�F�C�v�^�[�Q�b�g�𔽓]����i�㏑���j
//!	    @brief �o�^���Ă���V�F�C�v�^�[�Q�b�g�𔽓]����i�㏑���j
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] mirroringAxis ���]��:0=X,1=Y,2=Z
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::mirroring ( u32 targetID, u8 mirroringAxis ){
	if ( this->targetCount () <= targetID ){ return; }

	Vec3T<T> mirVec ( T ( 1.0 ) );
	switch ( mirroringAxis ){
		case 0: mirVec.x = T ( -1.0 ); break;
		case 1: mirVec.y = T ( -1.0 ); break;
		case 2: mirVec.z = T ( -1.0 ); break;
		default: mirVec.x = T ( -1.0 );
	}
	
	BlendShapeTargetT<T> rest = this->targets[targetID];
	// �^�[�Q�b�g�ɓo�^����Ă��鍷���������Ԃɔ��]���Ă���
	for ( u32 j = 0, m = rest.delta.size (); j < m; ++j ){
		u32 origPoiID = rest.delta[j].getIndex (); // �\�[�X�̒��_�ԍ����擾
		u32 mirPoiID;
		if ( this->mirroringPointIDMap[origPoiID] != -1 ){ // ���]�o�^���钸�_������ꍇ
			mirPoiID = this->mirroringPointIDMap[origPoiID]; // ���]��̒��_�ԍ����擾
		} else{ // ���]�o�^���钸�_���Ȃ��ꍇ�̓\�[�X�̔ԍ����g�p
			mirPoiID = origPoiID;
		}
		GeometryLocationT<T> loc ( mirPoiID, rest.delta[j].getWeight (), 1 );// ���_�����쐬
		Vec3T<T> dir = ( rest.delta[j].getDirection () * mirVec ).normal (); // ���]�������������쐬
		T dis = rest.delta[j].getDistance ();

		// �o�^
		if ( this->hasNormals ){
			// �^�[�Q�b�g�ɖ@����񂪊܂܂��ꍇ
			L16Array<Vec3T<T>> nDelta = rest.delta[j].getNormalDeltas ();
			std::vector<Vec3T<T>> n ( nDelta.size () );
			for ( u32 h = 0, l = nDelta.size (); h < l; ++h ){
				n[h] = nDelta[h] * mirVec;
			}
			this->targets[targetID].delta[j].set ( loc, dir, dis, n );
		} else{
			this->targets[targetID].delta[j].set ( loc, dir, dis );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�^���Ă���V�F�C�v�^�[�Q�b�g�𕡐�����i�㏑���j
//!	    @brief �o�^���Ă���V�F�C�v�^�[�Q�b�g�𕡐�����i�㏑���j
//!		@param[in] targetID �V�F�C�v�^�[�Q�b�g��ID
//!		@param[in] from �������̃V�F�C�v�^�[�Q�b�gID
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::copyTarget ( u32 targetID, u32 from ){
	if ( this->targetCount () <= targetID || this->targetCount () <= from ){ return; }
	if ( this->targets[from].delta.size () == 0 ){ return; }

	this->targets[targetID] = this->targets[from];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �o�^�ς݂̑S�Ẵ^�[�Q�b�g�𔽓]������mirroringTargets[]�Ɋi�[����
//!	    @brief �o�^�ς݂̑S�Ẵ^�[�Q�b�g�𔽓]������mirroringTargets[]�Ɋi�[����
//!		@param[in] mirroringAxis ���]��:0=X,1=Y,2=Z
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::createMirroringTargets ( u8 mirroringAxis ){
	if ( this->initialized == false ){ return; }
	if ( this->targetCount () == 0 ){ return; }

	Vec3T<T> mirVec ( T ( 1.0 ) );
	switch ( mirroringAxis ){
		case 0: mirVec.x = T ( -1.0 ); break;
		case 1: mirVec.y = T ( -1.0 ); break;
		case 2: mirVec.z = T ( -1.0 ); break;
		default: mirVec.x = T ( -1.0 );
	}
	// �^�[�Q�b�g�����Ԃɔ��]����mirroringTargets�Ɋi�[���Ă���
	for ( u32 i=0, n=this->targetCount(); i<n; ++i ){
		this->mirroringTargets[i].delta.resize ( this->targets [i].delta.size () );
		// �^�[�Q�b�g�ɓo�^����Ă��鍷���������Ԃɔ��]���Ă���
		for ( u32 j = 0, m = this->targets [i].delta.size (); j<m; ++j ){
			// ���]�����쐬
			u32 origPoiID = this->targets [i].delta [j].getIndex(); // �\�[�X�̒��_�ԍ����擾
			u32 mirPoiID;
			if ( this->mirroringPointIDMap [origPoiID] != -1 ){ // ���]�o�^���钸�_������ꍇ
				mirPoiID = this->mirroringPointIDMap [origPoiID]; // ���]��̒��_�ԍ����擾
			} else{ // ���]�o�^���钸�_���Ȃ��ꍇ�̓\�[�X�̔ԍ����g�p
				mirPoiID = origPoiID;
			}
			GeometryLocationT<T> loc ( mirPoiID, this->targets [i].delta [j].getWeight (), 1 );// ���_�����쐬
			Vec3T<T> dir = ( this->targets [i].delta [j].getDirection () * mirVec ).normal (); // ���]�������������쐬
			T dis = this->targets [i].delta [j].getDistance ();
			
			// �o�^
			if ( this->hasNormals ){
				// �^�[�Q�b�g�ɖ@����񂪊܂܂��ꍇ
				L16Array<Vec3T<T>> nDelta = this->targets [i].delta [j].getNormalDeltas ();
				std::vector<Vec3T<T>> n ( nDelta.size () );
				for ( u32 h = 0, l = nDelta.size (); h<l; ++h ){
					n [h] = nDelta [h] * mirVec;
				}
				this->mirroringTargets [i].delta [j].set ( loc, dir, dis, n );
			} else{
				this->mirroringTargets [i].delta [j].set ( loc, dir, dis );
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �u�����h�V�F�C�v�����s����
//!	    @brief �u�����h�V�F�C�v�����s����
//!		@param[io] mesh �����̑Ώ�
//!		@param[in] blendWeights �u�����h�^�[�Q�b�g�̃u�����h�l
//!		@param[in] mirrorWeights �~���[�u�����h�^�[�Q�b�g�̃u�����h�l
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::evaluate ( IO PolygonMeshT<T> &mesh, const std::vector<T> &blendWeights, const std::vector<T> &mirrorWeights )const{
	if ( this->initialized == false ){ return; }
	if ( this->targetCount () != blendWeights.size () ||
		 this->baseMesh.pointCount() != mesh.pointCount() ||
		 this->baseMesh.polygonCount() != mesh.polygonCount () ||
		 this->baseMesh.vertexCount() != mesh.vertexCount() ){ return; }
	Bool m = false;
	if( mirrorWeights.size () != 0 ){
		if ( mirrorWeights.size () != blendWeights.size () ||
			 mirrorWeights.size () != this->targetCount () ) { return; }
		m = true;
	}
	Vec3AttributeT<T> &refPoiPos = mesh.getPointPositionAttribute ();
	Vec3AttributeT<T> &refNomPos = mesh.getNormalAttribute ();
	// �u�����h�V�F�C�v�^�[�Q�b�g�����Ɋm�F���Ă���
	for ( u32 i = 0, n = this->targetCount (); i<n; ++i ){
		// �^�[�Q�b�g�̃u�����h�E�F�C�g�����͂���Ă���ꍇ�̓u�����h�������s��
		if ( T ( 0.0 ) < blendWeights [i] ){
			// �^�[�Q�b�g�ɓo�^����Ă��鍷�����Q�����Ƀu�����h�������Ă���
			for ( u32 j = 0, m = this->targets [i].delta.size (); j<m; ++j ){
				GeometryDelta delta = this->targets [i].delta [j];
				u32 poiID = delta.getIndex ();// �u�����h�������s�����_�ԍ����擾
				Vec3T<T> d = delta.getDelta () * delta.getWeight().x;// ���_�ʒu�̍������ = ���_�̍��� * �K����
				refPoiPos [poiID] += ( d * blendWeights [i] );// (���_�ʒu�̍������ * �u�����h��)�𒸓_�ʒu�ɉ��Z����
				// �@���̃u�����h�����s����̂��m�F
				if ( this->hasNormals ){ 
					L16Array<u32> verIDs = mesh.getSharedVertexIDs ( poiID );// ���_���\������o�[�e�b�N�X��ID���X�g���擾
					if ( delta.normalCount () != verIDs.size () ){ continue; }// �x�[�X���b�V���ƃ^�[�Q�b�g���b�V���̃g�|���W�[�������ł��邱�Ƃ��m�F�i�O�����j
					// ���_���\������o�[�e�b�N�X�̖@�������ɏ�������
					for ( u32 h = 0, k = verIDs.size (); h<k; ++h ){
						d = delta.getNormalDelta(h) * delta.getWeight ().x;//�@�o�[�e�b�N�X�@���̍������ = �@���̍��� * �K����
						refNomPos [verIDs [h]] += ( d * blendWeights [i] ); //  (�o�[�e�b�N�X�@���̍������ * �u�����h��)���o�[�e�b�N�X�@���ɉ��Z����
						refNomPos [verIDs [h]].setNormal ();//�@�P�ʃx�N�g����
					}
				}
			}
		}
		if ( m == false ){ continue; }

		// ���]�^�[�Q�b�g�ɑ΂��ď�L�Ɠ����������s��
		if ( T ( 0.0 ) < mirrorWeights [i] ){
			for ( u32 j = 0, m = this->mirroringTargets [i].delta.size (); j<m; ++j ){
				u32 poiID = this->mirroringTargets [i].delta [j].getIndex ();
				GeometryDelta delta = this->mirroringTargets [i].delta [j];
				Vec3T<T> d = delta.getDelta () * delta.getWeight ().x;
				refPoiPos [poiID] += ( d * mirrorWeights [i] );
				if ( this->hasNormals ){
					L16Array<u32> verIDs = mesh.getSharedVertexIDs ( poiID );
					if ( delta.normalCount () != verIDs.size () ){ continue; }
					for ( u32 h = 0, k = verIDs.size (); h<k; ++h ){
						d = delta.getNormalDelta ( h ) * delta.getWeight ().x;
						refNomPos [verIDs [h]] += ( d * mirrorWeights [i] );
						refNomPos [verIDs [h]].setNormal ();
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �x�[�X���b�V�����擾����
//!	    @brief �x�[�X���b�V�����擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T> BlendShapeT<T>::getBaseMesh ()const{
	return this->baseMesh;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �V�F�C�v�^�[�Q�b�g���擾����
//!	    @brief �V�F�C�v�^�[�Q�b�g���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T> BlendShapeT<T>::getTargetMesh ( u32 targetID )const{
	assert ( targetID < this->targetCount () );
	u32 cnt = this->targets[targetID].delta.size ();
	PolygonMeshT<T> result = this->baseMesh.cloneTopology();
	for(u32 i=0; i<cnt; ++i ){
		GeometryDeltaT<T> val = this->targets[targetID].delta[i];
		u32 poiID = val.getIndex ();
		assert ( poiID < result.pointCount () );
		Vec3T<T> basePos = result.getPointPosition ( poiID );
		Vec3T<T> delta = val.getDelta ();
		result.setPointPosition ( poiID, basePos + delta );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �V�F�C�v�^�[�Q�b�g�̃~���[���擾����
//!	    @brief �V�F�C�v�^�[�Q�b�g�̃~���[���擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T> BlendShapeT<T>::getMirrorTargetMesh ( u32 targetID )const{
	assert ( targetID < this->targetCount () );
	u32 cnt = this->mirroringTargets[targetID].delta.size ();
	PolygonMeshT<T> result = this->baseMesh.cloneTopology ();
	for ( u32 i = 0; i<cnt; ++i ){
		GeometryDeltaT<T> val = this->mirroringTargets[targetID].delta[i];
		u32 poiID = val.getIndex ();
		assert ( poiID < result.pointCount () );
		Vec3T<T> basePos = result.getPointPosition ( poiID );
		Vec3T<T> delta = val.getDelta ();
		result.setPointPosition ( poiID, basePos + delta );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �V�F�C�v�^�[�Q�b�g�̏����擾����
//!	    @brief �V�F�C�v�^�[�Q�b�g�̏����擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BlendShapeTargetT<T> BlendShapeT<T>::getTarget ( u32 targetID )const{
	assert ( targetID < this->targetCount () );
	return this->targets[targetID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �V�F�C�v�^�[�Q�b�g�̃~���[�̏����擾����
//!	    @brief �V�F�C�v�^�[�Q�b�g�̃~���[�̏����擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BlendShapeTargetT<T> BlendShapeT<T>::getMirrorTarget ( u32 targetID )const{
	assert ( targetID < this->targetCount () );
	return this->mirroringTargets[targetID];
}