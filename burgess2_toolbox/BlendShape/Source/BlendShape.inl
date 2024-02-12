//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    登録されているシェイプターゲットの数を返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 BlendShapeT<T>::targetCount ()const{
	return this->targets.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ベースメッシュの登録と頂点ミラーマップを生成する
//!	    @brief ベースメッシュの登録と頂点ミラーマップを生成する
//!		@param[in] _baseMesh ベースメッシュ
//!		@param[in] mirroringAxis ミラーの軸:0=X,1=Y,2=Z
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

		// 頂点が反転軸上に存在する場合は自分自身の頂点IDを登録して終了
		if ( mirP.almostEqual ( _baseMesh.getPointPosition ( i ), tolerance ) ){
			this->mirroringPointIDMap [i] = i;
			continue;
		}

		// 自分以外の頂点に対して対象頂点を探す
		s32 id = -1; // 対象頂点が無い場合は-1
		for ( s32 j = 0; j<n; ++j ){
			if ( j == i ){ continue; }
			if ( mirP.almostEqual ( _baseMesh.getPointPosition ( j ), tolerance ) ){ id = j; break; }
		}
		this->mirroringPointIDMap [i] = id;
	}
	this->initialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ベースメッシュの登録と頂点ミラーマップの生成を強制的にやり直す
//!	    @brief ベースメッシュの登録と頂点ミラーマップの生成を強制的にやり直す
//!		@param[in] _baseMesh ベースメッシュ
//!		@param[in] mirroringAxis ミラーの軸:0=X,1=Y,2=Z
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::resetBaseMeshAndMirroringMap ( const PolygonMeshT<T> &_baseMesh, u8 mirroringAxis, T tolerance ){
	this->initialized = false;
	this->setBaseMeshAndMirroringMap ( _baseMesh, mirroringAxis, tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    シェイプターゲットの数を設定する
//!	    @brief シェイプターゲットの数を設定する
//!		@param[in] size シェイプターゲットの数
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::resetTargetSize ( u32 size ){
	if ( size == this->targets.size () ){ return; } // 登録済みのターゲットと数が同じであれば終了
	this->targets.resize ( size );
	this->mirroringTargets.resize ( size );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    シェイプターゲットを登録する
//!	    @brief シェイプターゲットを登録する
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] targetMesh シェイプターゲットメッシュ　（トポロジーがBlendShapeTのbaseMeshと同じでなければ登録できずに終了）
//!		@param[in] normal 法線をブレンドシェイプターゲットに含める場合はtrue
//!		@param[in] threshold ターゲットを登録する際にベースメッシュから変化したと判断する最低変化量
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetMesh ( u32 targetID, const PolygonMeshT<T> &targetMesh, Bool normal, T threshold ){
	if ( this->targetCount () <= targetID ){ return; }
	std::vector<T> weight ( targetMesh.pointCount (), T ( 1.0 ) );
	this->setTargetMesh ( targetID, this->baseMesh, targetMesh, weight, normal, threshold );
	this->hasNormals = normal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    シェイプターゲットを登録する
//!	    @brief シェイプターゲットを登録する
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] targetMesh シェイプターゲットメッシュ　（トポロジーがBlendShapeTのbaseMeshと同じでなければ登録できずに終了）
//!		@param[in] weight 各頂点ごとのターゲットの登録の強さリスト　0.0~1.0 (配列数がtargetMeshの頂点数と異なる場合は終了)
//!		@param[in] normal 法線をブレンドシェイプターゲットに含める場合はtrue
//!		@param[in] threshold ターゲットを登録する際にベースメッシュから変化したと判断する最低変化量
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetMesh ( u32 targetID, const PolygonMeshT<T> &targetMesh, std::vector<T> &weight, Bool normal, T threshold ){
	this->setTargetMesh ( targetID, this->baseMesh, targetMesh, weight, normal, threshold );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    refMeshとの差分をターゲットとして登録する 
//!	    @brief refMeshとの差分をターゲットとして登録する
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] refMesh ベースメッシュ　（トポロジーがBlendShapeTのbaseMeshと同じでなければ登録できずに終了）
//!		@param[in] targetMesh シェイプターゲットメッシュ　（トポロジーがBlendShapeTのbaseMeshと同じでなければ登録できずに終了）
//!		@param[in] normal 法線をブレンドシェイプターゲットに含める場合はtrue
//!		@param[in] threshold ターゲットを登録する際にベースメッシュから変化したと判断する最低変化量
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetMesh ( u32 targetID, const PolygonMeshT<T> &refMesh, const PolygonMeshT<T> &targetMesh, Bool normal, T threshold ){
	if ( this->targetCount () <= targetID ){ return; }
	std::vector<T> weight ( targetMesh.pointCount (), T ( 1.0 ) );
	this->setTargetMesh ( targetID, refMesh, targetMesh, weight, normal, threshold );
	this->hasNormals = normal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    refMeshとの差分をターゲットとして登録する
//!	    @brief refMeshとの差分をターゲットとして登録する
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] refMesh ベースメッシュ　（トポロジーがBlendShapeTのbaseMeshと同じでなければ登録できずに終了）
//!		@param[in] targetMesh シェイプターゲットメッシュ　（トポロジーがBlendShapeTのbaseMeshと同じでなければ登録できずに終了）
//!		@param[in] weight 各頂点ごとのターゲットの登録の強さリスト　0.0~1.0 (配列数がtargetMeshの頂点数と異なる場合は終了)
//!		@param[in] normal 法線をブレンドシェイプターゲットに含める場合はtrue
//!		@param[in] threshold ターゲットを登録する際にベースメッシュから変化したと判断する最低変化量
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetMesh ( u32 targetID, const PolygonMeshT<T> &refMesh, const PolygonMeshT<T> &targetMesh, std::vector<T> &weight, Bool normal, T threshold ){
	if ( this->targetCount () <= targetID ){ return; }
	// ベースメッシュとターゲットメッシュの頂点数が異なる場合は終了
	u32 poiCnt = targetMesh.pointCount ();
	if ( poiCnt == 0 || poiCnt != refMesh.pointCount() || poiCnt != weight.size() || poiCnt != this->baseMesh.pointCount() ){ return; }
	
	BlendShapeTargetT<T> targetData;
	Vec3AttributeT<T> &basePosAttribute = refMesh.getPointPositionAttribute ();
	Vec3AttributeT<T> &targetMeshPosAttribute = targetMesh.getPointPositionAttribute ();
	// 頂点ごとに差分を算出していく
	for ( u32 i = 0; i< poiCnt; ++i ){
		// ウェイト値が０の頂点の場合は終了
		if ( Math::IsZero ( weight [i] ) ){ continue; }
		// 位置の差分が無い頂点の場合は終了
		if ( basePosAttribute [i].almostEqual ( targetMeshPosAttribute [i], threshold ) ){ continue; }

		// 差分情報を登録する
		Vec3T<T> local = targetMeshPosAttribute [i] - basePosAttribute [i];
		GeometryDeltaT<T> delta;
		if ( normal ){
			// 法線の差分情報も含める場合
			L16Array<Vec3T<T>> baseNormals = refMesh.getPointNormal ( i );// ベースメッシュのバーテックス法線
			L16Array<Vec3T<T>> targetNormals = targetMesh.getPointNormal ( i );// ターゲットメッシュのバーテックス法線
			if ( baseNormals.size () != targetNormals.size () ){ break; }// ベースメッシュとターゲットメッシュのトポロジーが同じであることを確認（念押し）
			std::vector<Vec3T<T>> nDelta ( baseNormals.size () );// 法線の差分情報の一時保存場所
			// 頂点を構成するバーテックスごとに法線の差分を格納していく
			for ( u32 j = 0, m = baseNormals.size (); j<m; ++j ){
				nDelta [j] = targetNormals [j] - baseNormals [j];
			}
			delta.set ( GeometryLocationT<T> ( i, T ( 1.0 ), 1 ), local.normal(), local.length (), nDelta );
		} else{
			// 法線の差分情報は含めない場合（頂点位置の差分のみ）
			delta.set ( GeometryLocationT<T> ( i, T ( 1.0 ), 1 ), local.normal(), local.length () );
		}
		targetData.delta.push_back ( delta );
	}
	targetData.delta.shrink_to_fit (); // 配列のメモリを最適化
	this->targets [targetID] = targetData;
	this->hasNormals = normal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    複数のターゲットメッシュからシェイプターゲットを登録する（上書き）
//!	    @brief 複数のターゲットメッシュからシェイプターゲットを登録する（上書き）
//!		@param[in] targetMesh シェイプターゲットメッシュ群　（トポロジーがベースメッシュと同じでなければ登録できずに終了）
//!		@param[in] normal 法線をブレンドシェイプターゲットに含める場合はtrue
//!		@param[in] threshold ターゲットを登録する際にベースメッシュから変化したと判断する最低変化量
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setAllTargetMesh ( const std::vector<PolygonMeshT<T>> &targetMesh, Bool normal, T threshold ){
	this->resetTargetSize ( targetMesh.size () );
	for ( u32 i = 0, n = targetMesh.size (); i < n; ++i ){
		this->setTargetMesh ( i, targetMesh[i], normal, threshold );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    シェイプターゲットを登録する
//!	    @brief シェイプターゲットを登録する
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] targetPoints シェイプターゲット　(配列数がbaseMeshの頂点数と異なる場合は終了)
//!		@param[in] threshold ターゲットを登録する際にベースから変化したと判断する最低変化量
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetPoints ( u32 targetID, const std::vector<Vec3T<T>> &targetPoints, T threshold ){
	std::vector<T> weight ( targetPoints.size (), T ( 1.0 ) );
	this->setTargetPoints ( targetID, this->baseMesh.getPointPositionAttribute ().getValuesRef (), targetPoints, weight, threshold );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    refPointsとの差分をターゲットとして登録する
//!	    @brief refPointsとの差分をターゲットとして登録する
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] targetPoints シェイプターゲット　(配列数がbaseMeshの頂点数と異なる場合は終了)
//!		@param[in] weight 各頂点ごとのターゲットの登録の強さリスト　0.0~1.0 (配列数がtargetPointsの配列数と異なる場合は終了)
//!		@param[in] threshold ターゲットを登録する際にベースから変化したと判断する最低変化量
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetPoints ( u32 targetID,
									   const std::vector<Vec3T<T>> &targetPoints,
									   const std::vector<T> &weight,
									   T threshold ){
	this->setTargetPoints ( targetID, this->baseMesh.getPointPositionAttribute ().getValuesRef(), targetPoints, weight, threshold );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    シェイプターゲットを登録する
//!	    @brief シェイプターゲットを登録する
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] targetPoints シェイプターゲット　(配列数がbaseMeshの頂点数と異なる場合は終了)
//!		@param[in] weight 各頂点ごとのターゲットの登録の強さリスト　0.0~1.0 (配列数がtargetPointsの配列数と異なる場合は終了)
//!		@param[in] threshold ターゲットを登録する際にベースから変化したと判断する最低変化量
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
//!	    refPointsとの差分をターゲットとして登録する
//!	    @brief refPointsとの差分をターゲットとして登録する
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] refPoints ベース　(配列数がbaseMeshの頂点数と異なる場合は終了)
//!		@param[in] targetPoints シェイプターゲット　(配列数がbaseMeshの頂点数と異なる場合は終了)
//!		@param[in] weight 各頂点ごとのターゲットの登録の強さリスト　0.0~1.0 (配列数がtargetPointsの配列数と異なる場合は終了)
//!		@param[in] threshold ターゲットを登録する際にベースから変化したと判断する最低変化量
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::setTargetPoints ( u32 targetID,
									   const std::vector<Vec3T<T>> &refPoints,
									   const std::vector<Vec3T<T>> &targetPoints,
									   const std::vector<T> &weight,
									   T threshold )
{
	if ( this->targetCount () <= targetID ){ return; }
	// ベースメッシュとターゲットメッシュの頂点数が異なる場合は終了
	u32 poiCnt = targetPoints.size ();
	if ( poiCnt == 0 || poiCnt != refPoints.size () || poiCnt != weight.size () || poiCnt != this->baseMesh.pointCount () ){ return; }

	BlendShapeTargetT<T> targetData;
	// 頂点ごとに差分を算出していく
	for ( u32 i = 0; i < poiCnt; ++i ){
		// ウェイト値が０の頂点の場合は終了
		if ( Math::IsZero ( weight[i] ) ){ continue; }
		// 位置の差分が無い頂点の場合は終了
		if ( refPoints[i].almostEqual ( targetPoints[i], threshold ) ){ continue; }

		// 差分情報を登録する
		Vec3T<T> local = targetPoints[i] - refPoints[i];
		GeometryDeltaT<T> delta;
		delta.set ( GeometryLocationT<T> ( i, T ( 1.0 ), 1 ), local.normal (), local.length () );
		targetData.delta.push_back ( delta );
	}
	targetData.delta.shrink_to_fit (); // 配列のメモリを最適化
	this->targets[targetID] = targetData;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    登録しているシェイプターゲットを左右対称にする（上書き）
//!	    @brief 既に登録しているシェイプターゲットを左右対称にする（上書き）
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] symmetryAxis 対象にする軸:0=X,1=Y,2=Z
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
		if( mID == id || mID == -1){continue;} // 頂点が反転軸上、もしくは対象が無い場合はスルー
		Bool conflict;
		for ( u32 j = 0; j<origSize; ++j ){
			if ( j != i && this->targets[targetID].delta[j].getIndex () == mID ){ conflict = true; }
		}
		if ( conflict ){ continue; } // 対象の頂点が既にターゲットに登録されていた場合もスルー

		GeometryLocationT<T> newLoc ( mID, this->targets[targetID].delta[i].getWeight(), 1 );
		Vec3T<T> mirror ( T ( 1.0 ) );
		mirror[mirroringAxis] = T ( -1.0 );
		GeometryDeltaT<T> newDelta;
		newDelta.set ( newLoc, this->targets[targetID].delta[i].getDirection () * mirror, this->targets[targetID].delta[i].getDistance () );
		
		this->targets[targetID].delta.push_back( newDelta );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    登録しているシェイプターゲットを反転する（上書き）
//!	    @brief 登録しているシェイプターゲットを反転する（上書き）
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] mirroringAxis 反転軸:0=X,1=Y,2=Z
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
	// ターゲットに登録されている差分情報を順番に反転していく
	for ( u32 j = 0, m = rest.delta.size (); j < m; ++j ){
		u32 origPoiID = rest.delta[j].getIndex (); // ソースの頂点番号を取得
		u32 mirPoiID;
		if ( this->mirroringPointIDMap[origPoiID] != -1 ){ // 反転登録する頂点がある場合
			mirPoiID = this->mirroringPointIDMap[origPoiID]; // 反転先の頂点番号を取得
		} else{ // 反転登録する頂点がない場合はソースの番号を使用
			mirPoiID = origPoiID;
		}
		GeometryLocationT<T> loc ( mirPoiID, rest.delta[j].getWeight (), 1 );// 頂点情報を作成
		Vec3T<T> dir = ( rest.delta[j].getDirection () * mirVec ).normal (); // 反転した差分情報を作成
		T dis = rest.delta[j].getDistance ();

		// 登録
		if ( this->hasNormals ){
			// ターゲットに法線情報が含まれる場合
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
//!	    登録しているシェイプターゲットを複製する（上書き）
//!	    @brief 登録しているシェイプターゲットを複製する（上書き）
//!		@param[in] targetID シェイプターゲットのID
//!		@param[in] from 複製元のシェイプターゲットID
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void BlendShapeT<T>::copyTarget ( u32 targetID, u32 from ){
	if ( this->targetCount () <= targetID || this->targetCount () <= from ){ return; }
	if ( this->targets[from].delta.size () == 0 ){ return; }

	this->targets[targetID] = this->targets[from];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    登録済みの全てのターゲットを反転させてmirroringTargets[]に格納する
//!	    @brief 登録済みの全てのターゲットを反転させてmirroringTargets[]に格納する
//!		@param[in] mirroringAxis 反転軸:0=X,1=Y,2=Z
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
	// ターゲットを順番に反転してmirroringTargetsに格納していく
	for ( u32 i=0, n=this->targetCount(); i<n; ++i ){
		this->mirroringTargets[i].delta.resize ( this->targets [i].delta.size () );
		// ターゲットに登録されている差分情報を順番に反転していく
		for ( u32 j = 0, m = this->targets [i].delta.size (); j<m; ++j ){
			// 反転情報を作成
			u32 origPoiID = this->targets [i].delta [j].getIndex(); // ソースの頂点番号を取得
			u32 mirPoiID;
			if ( this->mirroringPointIDMap [origPoiID] != -1 ){ // 反転登録する頂点がある場合
				mirPoiID = this->mirroringPointIDMap [origPoiID]; // 反転先の頂点番号を取得
			} else{ // 反転登録する頂点がない場合はソースの番号を使用
				mirPoiID = origPoiID;
			}
			GeometryLocationT<T> loc ( mirPoiID, this->targets [i].delta [j].getWeight (), 1 );// 頂点情報を作成
			Vec3T<T> dir = ( this->targets [i].delta [j].getDirection () * mirVec ).normal (); // 反転した差分情報を作成
			T dis = this->targets [i].delta [j].getDistance ();
			
			// 登録
			if ( this->hasNormals ){
				// ターゲットに法線情報が含まれる場合
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
//!	    ブレンドシェイプを実行する
//!	    @brief ブレンドシェイプを実行する
//!		@param[io] mesh 処理の対象
//!		@param[in] blendWeights ブレンドターゲットのブレンド値
//!		@param[in] mirrorWeights ミラーブレンドターゲットのブレンド値
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
	// ブレンドシェイプターゲットを順に確認していく
	for ( u32 i = 0, n = this->targetCount (); i<n; ++i ){
		// ターゲットのブレンドウェイトが入力されている場合はブレンド処理を行う
		if ( T ( 0.0 ) < blendWeights [i] ){
			// ターゲットに登録されている差分情報群を順にブレンド処理していく
			for ( u32 j = 0, m = this->targets [i].delta.size (); j<m; ++j ){
				GeometryDelta delta = this->targets [i].delta [j];
				u32 poiID = delta.getIndex ();// ブレンド処理を行う頂点番号を取得
				Vec3T<T> d = delta.getDelta () * delta.getWeight().x;// 頂点位置の差分情報 = 頂点の差分 * 適応率
				refPoiPos [poiID] += ( d * blendWeights [i] );// (頂点位置の差分情報 * ブレンド率)を頂点位置に加算する
				// 法線のブレンドも実行するのか確認
				if ( this->hasNormals ){ 
					L16Array<u32> verIDs = mesh.getSharedVertexIDs ( poiID );// 頂点を構成するバーテックスのIDリストを取得
					if ( delta.normalCount () != verIDs.size () ){ continue; }// ベースメッシュとターゲットメッシュのトポロジーが同じであることを確認（念押し）
					// 頂点を構成するバーテックスの法線を順に処理する
					for ( u32 h = 0, k = verIDs.size (); h<k; ++h ){
						d = delta.getNormalDelta(h) * delta.getWeight ().x;//　バーテックス法線の差分情報 = 法線の差分 * 適応率
						refNomPos [verIDs [h]] += ( d * blendWeights [i] ); //  (バーテックス法線の差分情報 * ブレンド率)をバーテックス法線に加算する
						refNomPos [verIDs [h]].setNormal ();//　単位ベクトル化
					}
				}
			}
		}
		if ( m == false ){ continue; }

		// 反転ターゲットに対して上記と同じ処理を行う
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
//!	    ベースメッシュを取得する
//!	    @brief ベースメッシュを取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
PolygonMeshT<T> BlendShapeT<T>::getBaseMesh ()const{
	return this->baseMesh;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    シェイプターゲットを取得する
//!	    @brief シェイプターゲットを取得する
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
//!	    シェイプターゲットのミラーを取得する
//!	    @brief シェイプターゲットのミラーを取得する
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
//!	    シェイプターゲットの情報を取得する
//!	    @brief シェイプターゲットの情報を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BlendShapeTargetT<T> BlendShapeT<T>::getTarget ( u32 targetID )const{
	assert ( targetID < this->targetCount () );
	return this->targets[targetID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    シェイプターゲットのミラーの情報を取得する
//!	    @brief シェイプターゲットのミラーの情報を取得する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
BlendShapeTargetT<T> BlendShapeT<T>::getMirrorTarget ( u32 targetID )const{
	assert ( targetID < this->targetCount () );
	return this->mirroringTargets[targetID];
}