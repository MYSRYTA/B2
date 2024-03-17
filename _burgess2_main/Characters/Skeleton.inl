
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
	// まず各ボーンのグローバル値と親ボーンIDのみを登録していく
	for ( u32 i = 0; i<boneCnt; ++i ){
		String boneName = "Bone" + std::to_string ( i );
		this->bones[i].set ( boneName, XfoT<T> ( boneGMat4[i] ), parentBoneID[i] );
	}
	// 各ボーンのローカル値の計算と親子階層リストの構築
	this->resetHierarchy ();
};

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   シンメトリーマップを作る
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void SkeletonT<T>::resetSymmetryID ( u8 normal ) {
	/* ミラーIDをリセット */
	for ( u32 i = 0, n = this->boneCount (); i<n; ++i ) {
		this->bones [i].setMirrorBoneID ( s32 ( -1 ) );
	}

	for ( u32 i = 0, n = this->boneCount (); i<n; ++i ) {
		/* 既にミラーIDが見つかっている場合はスルー */
		if ( this->bones [i].hasMirrorBone() ) { continue; }

		/* 頂点がミラー軸上に存在する場合 */
		switch ( normal ) {
			case 0:/* X軸 */
				if ( Burgess2MathFn::AlmostEqual ( this->bones [i].tr.x, T ( 0.0 ) ) ) { this->bones [i].setMirrorBoneID(i); continue; } break;
			case 1:/* Y軸 */
				if ( Burgess2MathFn::AlmostEqual ( this->bones [i].tr.y, T ( 0.0 ) ) ) { this->bones [i].setMirrorBoneID ( i ); continue; } break;
			default:/* Z軸 */
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
		Vec3T<T> mirrorPos = this->bones [i].tr * nor; /* 反転したポジションを作成 */
		for ( u32 h = 0, m = this->boneCount (); h<m; ++h ) { /* スケルトンのボーンを全検索 */
			if ( i==h ) { continue; } /* 対象が自身の場合はスルー */
			if ( this->bones [h].hasMirrorBone() ) { continue; } /* ミラーIDを設定済みのボーンは検索対象外 */
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
//!	   Boneの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 SkeletonT<T>::boneCount ( )const{
	return this->bones.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Boneを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
BoneT<T> SkeletonT<T>::getBone (u32 id)const {
	assert ( id < this->boneCount () );
	return this->bones [id];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   全てのBoneを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<BoneT<T>> SkeletonT<T>::getAllBones ()const {
	return this->bones;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Boneのニュートラルポーズ(ローカル値)Xfoを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> SkeletonT<T>::getNeutralPose ( u32 id )const {
	assert ( id < this->boneCount () );
	return this->bones [id].getLocalNeutralPose ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Boneのニュートラルポーズ(グローバル値)Xfoを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
XfoT<T> SkeletonT<T>::getGlobalNeutralPose ( u32 id )const {
	assert ( id < this->boneCount () );
	return this->bones [id].getGlobalNeutralPose();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Boneの配列IDを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 SkeletonT<T>::getBoneIdByName ( const String &boneName )const {
	for ( u32 i = 0, n = this->boneCount (); i<n; ++i ) {
		if ( this->bones [i].name == boneName ) { return i; }
	}
	return BURGESS2::U32_INFINITE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   Boneの親子関係を各Boneに記録する
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