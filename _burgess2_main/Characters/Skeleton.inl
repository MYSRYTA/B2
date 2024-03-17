
//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SkeletonT<T>::SkeletonT ( const String &_name, const std::vector<BoneT<T>> &_bones )
	: name { _name }
	, bones { _bones }
{
	this->resetHierarchy ();
};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
SkeletonT<T>::SkeletonT ( const String &_name, const std::vector<Mat4T<T>> &boneGMat4, std::vector<s32> parentBoneID )
	: name{ _name }
{
	u32 boneCnt = boneGMat4.size ();
	if ( boneCnt != parentBoneID.size() ){ return; }

	this->bones.resize ( boneCnt );
	// �܂��e�{�[���̃O���[�o���l�Ɛe�{�[��ID�݂̂�o�^���Ă���
	for ( u32 i = 0; i<boneCnt; ++i ){
		String boneName = "Bone" + std::to_string ( i );
		this->bones[i].set ( boneName, XfoT<T> ( boneGMat4[i] ), parentBoneID[i] );
	}
	// �e�{�[���̃��[�J���l�̌v�Z�Ɛe�q�K�w���X�g�̍\�z
	this->resetHierarchy ();
};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   �V�����g���[�}�b�v�����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void SkeletonT<T>::resetSymmetryID ( u8 normal ) {
	/* �~���[ID�����Z�b�g */
	for ( u32 i = 0, n = this->boneCount (); i<n; ++i ) {
		this->bones [i].setMirrorBoneID ( s32 ( -1 ) );
	}

	for ( u32 i = 0, n = this->boneCount (); i<n; ++i ) {
		/* ���Ƀ~���[ID���������Ă���ꍇ�̓X���[ */
		if ( this->bones [i].hasMirrorBone() ) { continue; }

		/* ���_���~���[����ɑ��݂���ꍇ */
		switch ( normal ) {
			case 0:/* X�� */
				if ( Burgess2MathFn::AlmostEqual ( this->bones [i].tr.x, T ( 0.0 ) ) ) { this->bones [i].setMirrorBoneID(i); continue; } break;
			case 1:/* Y�� */
				if ( Burgess2MathFn::AlmostEqual ( this->bones [i].tr.y, T ( 0.0 ) ) ) { this->bones [i].setMirrorBoneID ( i ); continue; } break;
			default:/* Z�� */
				if ( Burgess2MathFn::AlmostEqual ( this->bones [i].tr.z, T ( 0.0 ) ) ) { this->bones [i].setMirrorBoneID ( i ); continue; } break;
		}

		s32 restID = s32 ( -1 );
		T restDist = T(BURGESS2::F32_INFINITE);
		Vec3T<T> nor ( T(1.0) );
		switch ( normal ) {
			case 0:
				nor.x = T ( -1.0 ); break;
			case 1:
				nor.y = T ( -1.0 ); break;
			default:
				nor.z = T ( -1.0 ); break;
		}
		Vec3T<T> mirrorPos = this->bones [i].tr * nor; /* ���]�����|�W�V�������쐬 */
		for ( u32 h = 0, m = this->boneCount (); h<m; ++h ) { /* �X�P���g���̃{�[����S���� */
			if ( i==h ) { continue; } /* �Ώۂ����g�̏ꍇ�̓X���[ */
			if ( this->bones [h].hasMirrorBone() ) { continue; } /* �~���[ID��ݒ�ς݂̃{�[���͌����ΏۊO */
			T dist = this->bones [i].tr.distanceTo ( this->bones [h].tr );
			if ( dist < restDist ) {
				restID = h;
				restDist = dist;
			}
		}
		if ( restDist < T ( 0.001 ) ) {
			this->bones [i].setMirrorBoneID( restID );
			this->bones [restID].setMirrorBoneID( i );
		} else {
			this->bones [i].setMirrorBoneID( s32 ( -1 ) );
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Bone�̐���Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 SkeletonT<T>::boneCount ( )const{
	return this->bones.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Bone��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BoneT<T> SkeletonT<T>::getBone (u32 id)const {
	assert ( id < this->boneCount () );
	return this->bones [id];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   �S�Ă�Bone��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<BoneT<T>> SkeletonT<T>::getAllBones ()const {
	return this->bones;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Bone�̃j���[�g�����|�[�Y(���[�J���l)Xfo��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> SkeletonT<T>::getNeutralPose ( u32 id )const {
	assert ( id < this->boneCount () );
	return this->bones [id].getLocalNeutralPose ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Bone�̃j���[�g�����|�[�Y(�O���[�o���l)Xfo��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> SkeletonT<T>::getGlobalNeutralPose ( u32 id )const {
	assert ( id < this->boneCount () );
	return this->bones [id].getGlobalNeutralPose();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Bone�̔z��ID��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 SkeletonT<T>::getBoneIdByName ( const String &boneName )const {
	for ( u32 i = 0, n = this->boneCount (); i<n; ++i ) {
		if ( this->bones [i].name == boneName ) { return i; }
	}
	return BURGESS2::U32_INFINITE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Bone�̐e�q�֌W���eBone�ɋL�^����
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void SkeletonT<T>::resetHierarchy ( ) {
	for ( u32 i = 0, n = this->boneCount (); i<n; ++i ) {
		s32 pID = this->bones [i].getParentBoneID();
		if ( pID == s32(-1) ) {
			this->bones[i].setLocalNeutralPose ( this->bones[i].getGlobalNeutralPose () );
		}else{
			assert ( pID < n );
			Mat4T<T> localMat = this->bones[pID].getInvGlobalNeutralPose () * Mat4T<T> ( this->bones[i].getGlobalNeutralPose () );
			this->bones[i].setLocalNeutralPose ( XfoT<T> ( localMat ) );
			this->bones[pID].pushChildBoneID ( i );
		}
	}
}