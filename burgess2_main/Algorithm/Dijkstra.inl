//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     メッシュからダイクストラグラフを構成する
//!	    @brief	メッシュからダイクストラグラフを構成する
//!		@param[in] mesh
//!		@param[in] includeAllPolygonPoints ポリゴン内のエッジで隣接しない対角ポイントも含める場合はtrue
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void DijkstraT<T>::setFromPolygonMesh ( const PolygonMeshT<T> &mesh, Bool includeAllPolygonPoints ) {
	if ( mesh.polygonCount () < 2 ) { return; }

	this->node.resize ( mesh.pointCount () );
	#pragma omp parallel for
	for ( u32 i = 0, n = mesh.pointCount (); i<n; i++ ) {
		//	ポイントからエッジでつながった隣のポイントIDリストを取得
		//	includeAllPolygonPointsはポリゴン内の対角ポイントも含めるか否か
		this->node [i].surroundingNodeID = mesh.getPointSurroundingPoints ( i, includeAllPolygonPoints );
		//	取得した各ポイントへの距離を格納していく
		this->node [i].surroundingNodeCost.resize ( this->node [i].surroundingNodeID.size() );
		Vec3T<T> pos = mesh.getPointPosition ( i );
		for ( u32 k = 0, kn = this->node [i].surroundingNodeID.size (); k<kn; ++ k ) {
			Vec3T<T> tPos = mesh.getPointPosition ( this->node [i].surroundingNodeID [k] );
			this->node [i].surroundingNodeCost [k] = pos.distanceTo ( tPos );
		}
	}
	this->initialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    スタートから各ノードへの移動コストを算出して変数に保存する
//!	    @brief	スタートから各ノードへの移動コストを算出して変数に保存する
//!		@param[in] startNodeIDs スタート地点とするノードID
//!		@param[in] maxDistance コストを算出する最大距離
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void DijkstraT<T>::recomputeRouteCost ( std::vector<u32> startNodeIDs, T maxDistance ) {
	if ( maxDistance <= 0.0 || this->initialized == false ) { this->computed = false;  return; }

	//  それぞれのスタート地点からコスト計算を行っていき、コストが小さくなる場合のみコストの更新を行う
	for ( u32 h = 0, hn=startNodeIDs.size (); h < hn;  ++h ) {
		//  スタート地点が複数ある場合用
		//  別のスタート地点からの計算を行う際に、訪問済みの判定をリセットする
		for ( u32 g = 0, gn=this->node.size (); g<gn; ++g ) {
			this->node [g].done = false;
		}
		//	スタートノードの移動コストを０にする
		this->node [startNodeIDs [h]].cost = 0;
		// 経路が見つかるまでループする
		while ( true ) {
			// 処理を行うノードを格納（これが見つからなければ終了）
			DijkstraNodeT<T> processVertex;
			for ( u32 i = 0, n=this->node.size (); i<n; ++i ) {
				// 訪問済み or まだコストが未設定の場合はスキップ
				// 最初はスタートノードだけがコスト `0` 、それ以外は全てが”未訪問+コスト-1”になっているため、i=スタートノードの場合だけ次へ進む
				if ( this->node [i].done || this->node [i].cost<0 ) {
					continue;
				}
				// ノードが未選択であれば現在のノードを選択して処理へと進む
				if ( processVertex.assign == false ) {
					processVertex = this->node [i];
					continue;
				}
				// 訪問済み（確定済み）でないノードのうち、一番小さいコストのノードを探す
				if ( this->node [i].cost < processVertex.cost ) {
					processVertex = this->node [i];
				}
			}
			// 処理中のノードがなくなったら(全てチェックが終わったら)ループ終了
			if ( processVertex.assign == false ) {
				break;
			}
			// 処理中ノードに「訪問済み」のフラグを設定する
			// （未確定ノードのうち、一番コストが小さい場合はそこにいたるまでの経路が計算された結果なので「確定」できる
			processVertex.done = true;

			// コストのアップデート
			// 選択されたノード（processNode）の現在のコストと接続されているノードのコストを足し、それを接続先のノードに設定されているコストと比較する
			// もし小さければその値にアップデートする
			for ( u32 i = 0, n = processVertex.surroundingNodeID.size(); i<n; ++i ) {
				DijkstraNodeT<T> restNode;
				T restCost;
				restNode = this->node [processVertex.surroundingNodeID [i]];
				restCost = processVertex.cost + processVertex.surroundingNodeCost [i];
				// コストが未設定 or コストの少ない経路がある場合はアップデート
				if ( restNode.cost < 0 || restCost < restNode.cost ) {
					//	コストの合計値がmaxDistance内の場合のみ値を更新
					if ( restCost < maxDistance ) {
						restNode.cost = restCost;
					}
				}
			}
		}
	}
	this->computed = true;
	this->maxCost = maxDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     スタートノードから各ノードへの移動コストを取得する
//!	    @brief	スタートノードから各ノードへの移動コストを取得する
//!		@param[in] returnValueType　0=距離を取得(最大距離以降のノードは-1.0), 1=スタート～最大距離を1.0~0.0としたレシオを取得
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<T> DijkstraT<T>::getRouteCost ( u8 returnValueType ) {
	std::vector<T> result ( this->node.size () );
	if ( this->computed == false ) { return result;	}

	switch ( returnValueType ) {
		case 0:
			#pragma omp parallel for
			for ( u32 i = 0, n = this->node.size (); i<n; ++i ) {
				result [i] = this->node [i].cost;
			}
			break;
		case 1:
			#pragma omp parallel for
			for ( u32 i = 0, n = this->node.size (); i<n; ++i ) {
				result [i] = this->node [i].cost < 0 ? T ( 0.0 ) : T ( 1.0 ) - T ( this->node [i].cost / this->maxCost );
			}
			break;
	}
	return result;
}