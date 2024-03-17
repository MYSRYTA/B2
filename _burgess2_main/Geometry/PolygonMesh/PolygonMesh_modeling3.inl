//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// slitEdge() slitPolygon() slitEdgeAndPolygon()

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    エッジを分割する <br>　
//!	    @brief エッジを分割する
//!		@param[in] vertexID エッジの始まりのバーテックス番号
//!		@param[in] ratio 分割位置を指定するエッジ上のレシオ(0.0~1.0)
//!		@return 新規追加した頂点番号
//!		@warning 現状ではUVやカラー等の付属アトリビュートは未対応
//!		@note  begin/endStructureChangesのスコープ内でのみ有効
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitEdge( u32 vertexID, f32 ratio ) {
    if ( this->modelingMode == false ) { return BURGESS2::U32_INFINITE; }

    std::vector<u32> edgeIDs( 1, vertexID );// vertexID == edgeID
    std::vector<f32> ratios2( 1, ratio );

    std::vector<u32> packedPointIDs;
    std::vector<u32> polygonSizes;
    std::vector<Vec3T<T>> addPoiPos;
    u32 returnVal = this->splitEdgeCore( packedPointIDs, polygonSizes, addPoiPos, edgeIDs, ratios2 );

    if ( returnVal != BURGESS2::U32_INFINITE ) {
        //　頂点を追加
        this->addPoints( addPoiPos );
        // トポロジーを更新
        this->resetTopology( polygonSizes, packedPointIDs, true );
    }

    return returnVal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    エッジを分割する <br>　
//!	    @brief エッジを分割する
//!		@param[in] polygonID ポリゴン番号
//!		@param[in] pointID エッジの始まりの頂点番号
//!		@param[in] ratio 分割位置を指定するエッジ上のレシオ(0.0~1.0)
//!		@return 新規追加した頂点番号
//!		@warning 現状ではUVやカラー等の付属アトリビュートは未対応
//!		@note  begin/endStructureChangesのスコープ内でのみ有効
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitEdge( u32 polygonID, u32 pointID, f32 ratio ) {
    if ( this->modelingMode == false ) { return BURGESS2::U32_INFINITE; }
    if ( this->polygonCount() <= polygonID || this->pointCount() <= pointID ) { return BURGESS2::U32_INFINITE; }

    u32 edgeID = this->getPolygonPointEdgeID( polygonID, pointID );
    if ( this->edgeCount() < edgeID ) { return BURGESS2::U32_INFINITE; } // エッジが見つからなかった場合は終了

    std::vector<u32> edgeIDs( 1, edgeID );
    std::vector<f32> ratios2( 1, ratio );

    std::vector<u32> packedPointIDs;
    std::vector<u32> polygonSizes;
    std::vector<Vec3T<T>> addPoiPos;
    u32 returnVal = this->splitEdgeCore( packedPointIDs, polygonSizes, addPoiPos, edgeIDs, ratios2 );

    if ( returnVal != BURGESS2::U32_INFINITE ) {
        //　頂点を追加
        this->addPoints( addPoiPos );
        // トポロジーを更新
        this->resetTopology( polygonSizes, packedPointIDs, true );
    }

    return returnVal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    複数本のエッジを一気に分割する（splitEdge()を繰り返すより効率的））<br>　
//!	    @brief 複数本のエッジを一気に分割する（splitEdge()を繰り返すより効率的）
//!		@param[in] vertexIDs エッジのスタートバーテックス番号
//!		@param[in] ratio 分割位置を指定するエッジ上のレシオ(0.0~1.0)
//!		@return 新規追加した頂点群の最初の頂点番号
//!		@warning 現状ではUVやカラー等の付属アトリビュートは未対応
//!		@note  begin/endStructureChangesのスコープ内でのみ有効
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitEdges( const std::vector<u32>& vertexIDs, const std::vector<f32>& ratios, const T tolerance ) {
    if ( this->modelingMode == false ) { return BURGESS2::U32_INFINITE; }
    if ( vertexIDs.size() != ratios.size() ) { return BURGESS2::U32_INFINITE; }
    if ( vertexIDs.size() == 0 ) { return BURGESS2::U32_INFINITE; }

    std::vector<u32> packedPointIDs;
    std::vector<u32> polygonSizes;
    std::vector<Vec3T<T>> addPoiPos;
    u32 returnVal = this->splitEdgeCore( packedPointIDs, polygonSizes, addPoiPos, vertexIDs, ratios, tolerance );// vertexID == edgeID

    if ( returnVal != BURGESS2::U32_INFINITE ) {
        //　頂点を追加
        this->addPoints( addPoiPos );
        // トポロジーを更新
        this->resetTopology( polygonSizes, packedPointIDs, true );
    }

    return returnVal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    複数本のエッジを一気に分割する（splitEdge()を繰り返すより効率的）<br>　
//!	    @brief 複数本のエッジを一気に分割する（splitEdge()を繰り返すより効率的）
//!		@param[in] polygonIDs 
//!		@param[in] pointIDs エッジのスタートポイント
//!		@param[in] ratios 分割位置を指定するエッジ上のレシオ(0.0~1.0)
//!		@return 新規追加した頂点群の最初の頂点番号
//!		@warning 現状ではUVやカラー等の付属アトリビュートは未対応
//!		@note  begin/endStructureChangesのスコープ内でのみ有効
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitEdges( const std::vector<u32>& polygonIDs, const std::vector<u32>& pointIDs, const std::vector<f32>& ratios, const T tolerance ) {
    if ( this->modelingMode == false ) { return BURGESS2::U32_INFINITE; }
    if ( polygonIDs.size() != pointIDs.size() || polygonIDs.size() != ratios.size() ) { return BURGESS2::U32_INFINITE; }
    if ( polygonIDs.size() == 0 ) { return BURGESS2::U32_INFINITE; }

    // 重複や無効な値を省いた配列を作成する
    std::vector<u32> edgeIDs( 0 );
    std::vector<f32> ratios2( 0 );
    for ( u32 i = 0; i < polygonIDs.size(); ++i ) {
        u32 edgeID = this->getPolygonPointEdgeID( polygonIDs[i], pointIDs[i] );
        if ( this->edgeCount() < edgeID ) { continue; } // エッジが見つからなかった場合は終了

        edgeIDs.push_back( edgeID );
        ratios2.push_back( ratios[i] );
    }
    if ( edgeIDs.size() == 0 ) { return BURGESS2::U32_INFINITE; } // 有効なエッジの登録が無かった場合は終了

    std::vector<u32> packedPointIDs;
    std::vector<u32> polygonSizes;
    std::vector<Vec3T<T>> addPoiPos;
    u32 returnVal = this->splitEdgeCore( packedPointIDs, polygonSizes, addPoiPos, edgeIDs, ratios2, tolerance );

    if ( returnVal != BURGESS2::U32_INFINITE ) {
        //　頂点を追加
        this->addPoints( addPoiPos );
        // トポロジーを更新
        this->resetTopology( polygonSizes, packedPointIDs, true );
    }

    return returnVal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    private: splitEdge()系のコア関数
//!	    @brief private: splitEdge()系のコア関数
//!		@return 新規追加した頂点群の最初の頂点番号
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitEdgeCore(
    IO std::vector<u32>& packedPointIDs,
    IO std::vector<u32>& polygonSizes,
    IO std::vector<Vec3T<T>>& addPoiPos,
    const std::vector<u32>& edgeIDs,
    const std::vector<f32>& _ratios,
    const T ratio_tolerance,
    const T pos_tolerance ) const
{
    if ( this->modelingMode == false ) { return BURGESS2::U32_INFINITE; }
    if ( edgeIDs.size() != _ratios.size() ) { return BURGESS2::U32_INFINITE; }
    if ( edgeIDs.size() == 0 ) { return BURGESS2::U32_INFINITE; }

    u32 origPoiCnt = this->pointCount();

    // 隣接エッジの分割指定を追加したリストを作成し直す 同時に分割指定が全く同じモノや、ratioが0.0もしくは1.0のモノはリストから省く
    std::vector<u32> newEdgeIDs( 0 );
    std::vector<f32> newRatios( 0 );
    for ( u32 i = 0; i < edgeIDs.size(); i++ ) {
        T ratio = Burgess2MathFn::Clamp( _ratios[i], T( 0.0 ), T( 1.0 ) );
        // ratioが0.0もしくは1.0場合はスキップ
        if ( Burgess2MathFn::AlmostEqual( ratio, T( 0.0 ), ratio_tolerance ) || Burgess2MathFn::AlmostEqual( ratio, T( 1.0 ), ratio_tolerance ) ){
            continue;
        }
        // エッジIDが存在しない場合はスキップ
        if ( this->edgeCount() <= edgeIDs[i] ) { continue; }

        // newリストに同じ設定値が既に存在する場合はスキップ
        Bool add = true;
        for ( u32 j = 0; j < newEdgeIDs.size(); j++ ) {
            if ( edgeIDs[i] == newEdgeIDs[j] && Burgess2MathFn::AlmostEqual( ratio, newRatios[j], ratio_tolerance ) ) {
                add = false;
                break;
            }
        }
        if ( add == false ) {
            continue;
        }

        // newリストに追加
        newEdgeIDs.push_back( edgeIDs[i] );
        newRatios.push_back( ratio );

        // 境界エッジでない場合は隣接エッジもnewリストに追加
        if ( this->isPolygonBorder( edgeIDs[i] ) == false ) {
            u32 adjEdgeID = this->getEdgeAdjacentEdgeID( edgeIDs[i] );
            newEdgeIDs.push_back( this->getEdgeStartVertexID( adjEdgeID ) ); // verID = edgeID
            newRatios.push_back( T( 1.0 ) - ratio );
        }
    }
    u32 addPoiCnt = u32(newEdgeIDs.size());
    if ( addPoiCnt == 0 ) { return BURGESS2::U32_INFINITE; }

    // 追加する頂点のリストと、頂点IDリストを作成する
    u32 cnt = 0;
    Bool firstLoop = true;
    std::vector<Bool> done( addPoiCnt, false );
    std::vector<std::vector<u32>> newPointIDs( this->polygonCount() );
    while ( true ) {
        for ( u32 i = 0; i < addPoiCnt; i++ ) {
            if ( firstLoop ) { // 初回の処理
                // ハーフエッジで構成しているポリゴンIDを取得し、頂点IDリストを抜き出して新規リストの下地を作る
                // 既にリストが抜き出されている場合はスキップ（同じエッジに対して複数回分割が行われる）
                u32 polyID = this->getEdgeLeftPolygonID( newEdgeIDs[i] );
                if ( newPointIDs[polyID].size() != 0 ) {
                    continue;
                }
                L8Array<u32> origPolyIDs = this->getPolygonPointIDs( polyID );
                for ( u32 j = 0; j < origPolyIDs.size(); j++ ) {
                    newPointIDs[polyID].push_back( origPolyIDs[j] );
                }

                // 分割位置
                Vec2u poiID = this->getEdgePointIDs( newEdgeIDs[i] );
                Vec3T<T> fPos = this->getPointPosition( poiID.x() );
                Vec3T<T> ePos = this->getPointPosition( poiID.y() );
                Vec3T<T> newPos = fPos.lerp( ePos, newRatios[i] );

                // 既に登録している新規ポイント位置と一致するものが無いか確認
                s32 find = -1;
                for ( s32 j = 0; j < addPoiPos.size(); j++ ) {
                    if ( addPoiPos[j].almostEqual( newPos, pos_tolerance ) ) {
                        find = j;
                        break;
                    }
                }
                u32 newPoiID;
                if ( find == -1 ) { // リスト内に一致するポジションが無かった場合はリストに追加
                    newPoiID = origPoiCnt + u32(addPoiPos.size());
                    addPoiPos.push_back( newPos );
                } else { // リスト内に存在した場合はIDを取得
                    newPoiID = origPoiCnt + find;
                }

                // 新規ポイントIDを頂点IDリストに挿入する
                // 挿入位置はエッジのスタートポイントIDで判断
                for ( u32 j = 0; j < newPointIDs[polyID].size(); j++ ) {
                    if ( newPointIDs[polyID][j] == poiID.x() ) {
                        newPointIDs[polyID].insert( newPointIDs[polyID].begin() + j + 1, newPoiID );
                    }
                }

                done[i] = true;
                cnt++;
            } else { // 同じエッジを複数回分割している場合の処理
                if ( done[i] == true ) { continue; }

                u32 polyID = this->getEdgeLeftPolygonID( newEdgeIDs[i] );
                // 分割位置
                Vec2u poiID = this->getEdgePointIDs( newEdgeIDs[i] );
                Vec3T<T> fPos = this->getPointPosition( poiID.x() );
                Vec3T<T> ePos = this->getPointPosition( poiID.y() );
                Vec3T<T> newPos = fPos.lerp( ePos, newRatios[i] );

                // 既に登録している新規ポイント位置と一致するものが無いか確認
                s32 find = -1;
                for ( s32 j = 0; j < addPoiPos.size(); j++ ) {
                    if ( addPoiPos[j].almostEqual( newPos, pos_tolerance ) ) {
                        find = j;
                        break;
                    }
                }
                u32 newPoiID;
                if ( find == -1 ) { // リスト内に一致するポジションが無かった場合はリストに追加
                    newPoiID = origPoiCnt + u32(addPoiPos.size());
                    addPoiPos.push_back( newPos );
                } else { // リスト内に存在した場合はIDを取得
                    newPoiID = origPoiCnt + u32(find);
                }

                // 新規ポイントIDを頂点IDリストに挿入する
                // 挿入位置は対象ポリゴンのエッジのスタートエンドのポイントへのベクトルで判断する
                for ( u32 j = 0; j < newPointIDs[polyID].size(); j++ ) {
                    u32 sID = newPointIDs[polyID][j];
                    u32 eID = j == newPointIDs[polyID].size() - 1 ? newPointIDs[polyID][0] : newPointIDs[polyID][j + 1];
                    Vec3T<T> sPos, ePos;
                    if ( sID < origPoiCnt ) {
                        sPos = this->getPointPosition( sID );
                    } else {
                        sPos = addPoiPos[sID - origPoiCnt];
                    }
                    if ( eID < origPoiCnt ) {
                        ePos = this->getPointPosition( eID );
                    } else {
                        ePos = addPoiPos[eID - origPoiCnt];
                    }
                    Vec3T<T> dir1 = ( ePos - sPos ).unit();		// スタートからエンドまでのベクトル
                    Vec3T<T> dir2 = ( ePos - newPos ).unit();	// 追加ポイントからエンドまでのベクトル
                    if ( dir1.almostSameDirection( dir2 ) ) { // 同じ向きか確認
                        dir1 = dir1 * T( -0.1 );					// エンドからスタートまでのベクトル
                        dir2 = ( sPos - newPos ).unit();			// 追加ポイントからスタートまでのベクトル
                        if ( dir1.almostSameDirection( dir2 ) && newPointIDs[polyID][j+1] != newPoiID && newPointIDs[polyID][j] != newPoiID ) { // 同じ向きか確認
                            newPointIDs[polyID].insert( newPointIDs[polyID].begin() + j + 1, newPoiID );
                            break;
                        }
                    }
                }
                done[i] = true;
                cnt++;
            }
        }

        if ( cnt < addPoiCnt ) {
            firstLoop = false;
        } else {
            break;
        }
    }

    // トポロジーリストを作成
    packedPointIDs.reserve( origPoiCnt + addPoiPos.size() );
    polygonSizes.resize( this->polygonCount() + 1 );
    polygonSizes[0] = 0;
    for ( u32 i = 0; i < this->polygonCount(); i++ ) {
        u32 polySize;
        if ( newPointIDs[i].size() == 0 ) { // オリジナルの値を継続
            polySize = this->getPolygonSize( i );
            L8Array<u32> poiIDs = this->getPolygonPointIDs( i );
            for ( u32 j = 0; j < poiIDs.size(); j++ ) {
                packedPointIDs.push_back( poiIDs[j] );
            }
        } else { // 新しいリストの値を適用
            polySize = u32(newPointIDs[i].size());
            for ( u32 j = 0; j < polySize; j++ ) {
                packedPointIDs.push_back( newPointIDs[i][j] );
            }
        }
        polygonSizes[i + 1] = polygonSizes[i] + polySize;
    }
    return origPoiCnt;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ポリゴンを分割する<br>　
//!	    @brief　ポリゴンを分割する
//!		@param[in] polygonID 対象とするポリゴン番号
//!		@param[in] point1 分割エッジを構成する1つめの頂点番号
//!		@param[in] point2 分割エッジを構成する2つめの頂点番号
//!		@return 新規追加したポリゴン番号
//!		@warning 現状ではUVやカラー等の付属アトリビュートは未対応
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitPolygon( u32 polygonID, u32 point1, u32 point2 ) {
    if ( this->polygonCount() <= polygonID ) { return BURGESS2::U32_INFINITE; }
    if ( this->pointCount() <= point1 || this->pointCount() <= point2 ) { return BURGESS2::U32_INFINITE; }

    std::vector<u32> polygonIDs;
    polygonIDs.push_back( polygonID );
    std::vector<Vec2u> points;
    points.push_back( Vec2u( point1, point2 ) );
    return this->splitPolygons( polygonIDs, points );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    複数のポリゴンを分割する（splitPolygon()を繰り返すより効率的））<br>　
//!	    @brief　複数のポリゴンを分割する（splitPolygon()を繰り返すより効率的））
//!		@param[in] polygonIDs 対象とするポリゴン番号
//!		@param[in] points 分割エッジを構成する2つの頂点番号
//!		@return 新規追加したポリゴン群の最初のポリゴン番号
//!		@warning 現状ではUVやカラー等の付属アトリビュートは未対応
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitPolygons( const std::vector<u32>& polygonIDs, const std::vector<Vec2u>& points ) {
    if ( this->modelingMode == false ) { return BURGESS2::U32_INFINITE; }
    if ( polygonIDs.size() != points.size() ) { return BURGESS2::U32_INFINITE; }

    u32 origSize = this->polygonCount();

    // 分割対象とするポリゴンを再構成するためのポリゴンサイズと頂点IDリストを作成
    std::vector<u32> polyIDs;
    std::vector<std::vector<u32>> newPointIDs; // 分割後のポリゴンを構成する頂点IDリスト
    std::vector<std::vector<u32>> nwePolyPointIDs; // 新規作成するポリゴンを構成する頂点IDリスト
    u32 cnt{ 0 };
    for ( u32 i = 0; i < polygonIDs.size(); ++i ) {
        if ( this->polygonCount() <= polygonIDs[i] ) { continue; }

        std::vector<u32> restIDs1, restIDs2;
        Bool newPoly = false;

        L8Array<u32> polyPoiIDs = this->getPolygonPointIDs( polygonIDs[i] ); // 分割するポリゴンを構成する頂点ID群を取得
        for ( u32 j = 0; j < polyPoiIDs.size(); ++j ) {
            if ( polyPoiIDs[j] == points[i].x() || polyPoiIDs[j] == points[i].y() ) {
                // 分割の頂点は２つのポリゴンどちらでも使用
                restIDs1.push_back( polyPoiIDs[j] );
                restIDs2.push_back( polyPoiIDs[j] );
                newPoly = newPoly == false;
            } else {
                // 分割で使用しない頂点をそれぞれのリストへ割り振る
                if ( newPoly ) {
                    restIDs2.push_back( polyPoiIDs[j] );
                } else {
                    restIDs1.push_back( polyPoiIDs[j] );
                }
            }
        }
        if ( restIDs1.size() < 3 || restIDs2.size() < 3 ) { continue; } // ポリゴンを構成できるだけの頂点数が無い場合は終了

        polyIDs.push_back( polygonIDs[i] ); // 正常にリストを生成できたポリゴンIDを新規リストに保持する
        newPointIDs.push_back( restIDs1 );
        nwePolyPointIDs.push_back( restIDs2 );
    }
    if ( polyIDs.size() == 0 ) {
        // 新規のポリゴンリストが生成されなかった場合は終了
        return BURGESS2::U32_INFINITE;
    }

    // リストを使って新しいpolygonSizesアトリビュートデータを作成
    std::vector<u32> newPolygonSizes( this->polygonCount() + nwePolyPointIDs.size() + 1 ); // +1 は [0]=0
    newPolygonSizes[0] = 0;
    for ( u32 i = 0; i < this->polygonCount(); ++i ) { // 既存のトポロジー範囲を更新
        u32 polySize = this->getPolygonSize( i );
        for ( u32 j = 0; j < polyIDs.size(); ++j ) {
            if ( i == polyIDs[j] ) {
                polySize = newPointIDs[j].size();
                break;
            }
        }
        newPolygonSizes[i + 1] = newPolygonSizes[i] + polySize; // +1 は [0]=0
    }
    for ( u32 i = 0; i < nwePolyPointIDs.size(); ++i ) { // 新規ポリゴンの情報を追加
        newPolygonSizes[origSize + i + 1] = newPolygonSizes[origSize + i] + nwePolyPointIDs[i].size(); // +1 は [0]=0
    }

    // リストを使って新しいpackedPointIDsアトリビュートデータを作成
    std::vector<u32> newPackedPoiIDs;
    for ( u32 i = 0; i < this->polygonCount(); ++i ) { // 既存のトポロジー範囲を更新
        L8Array<u32> polyPoiIDs = this->getPolygonPointIDs( i );
        for ( u32 j = 0; j < polyIDs.size(); ++j ) {
            if ( i == polyIDs[j] ) {
                polyPoiIDs = L8Array<u32>{ newPointIDs[j] };
                break;
            }
        }
        for ( u32 j = 0; j < polyPoiIDs.size(); ++j ) {
            newPackedPoiIDs.push_back( polyPoiIDs[j] );
        }
    }
    for ( u32 i = 0; i < nwePolyPointIDs.size(); ++i ) { // 新規ポリゴンの情報を追加
        for ( u32 j = 0; j < nwePolyPointIDs[i].size(); ++j ) {
            newPackedPoiIDs.push_back( nwePolyPointIDs[i][j] );
        }
    }

    // トポロジーアトリビュートを更新
    U32AttributeT<T>& polySizeAttr = this->getU32Attribute( "polygonSizes" );
    polySizeAttr.resetValues( newPolygonSizes );
    U32AttributeT<T>& packedPoiIDAttr = this->getU32Attribute( "packedPointIDs" );
    packedPoiIDAttr.resetValues( newPackedPoiIDs );

    return origSize;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    エッジ分割とポリゴン分割を同時に行う <br>　
//!	    @brief　エッジ分割とポリゴン分割を同時に行う
//!		@param[in] polygonIDs 対象とするポリゴン番号
//!		@param[in] poiLocalID1 分割対称のエッジのスタートポイント(ポリゴン内のローカルID) 
//!		@param[in] ratio1 分割位置を指定するエッジ上のレシオ(0.0~1.0)　ポリゴン分割のスタート位置
//!		@param[in] poiLocalID2 分割対称のエッジのスタートポイント（ポリゴン内のローカルID）
//!		@param[in] ratio2 分割位置を指定するエッジ上のレシオ(0.0~1.0)　ポリゴン分割のエンド位置
//!		@param[in] addPointPositions ポリゴン分割のスタート～エンドの間に頂点を挟む場合の頂点位置
//!		@return 新規追加したポリゴンの番号
//!		@warning 現状ではUVやカラー等の付属アトリビュートは未対応
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitEdgesAndPolygon( u32 polygonID, LocalIndex poiLocalID1, T ratio1, LocalIndex poiLocalID2, T ratio2, const std::vector<Vec3T<T>>& addPointPositions, const T tolerance ) {
    std::vector<u32> polygonIDs, pointIDs1, pointIDs2;
    std::vector<f32> ratios1, ratios2;
    std::vector<std::vector<Vec3>> addPointPositionsList;

    polygonIDs.push_back( polygonID );
    ratios1.push_back( ratio1 );
    ratios2.push_back( ratio2 );
    addPointPositionsList.push_back( addPointPositions );

    L8Array<u32> poiList = this->getPolygonPointIDs( polygonID );
    pointIDs1.push_back( poiList[poiLocalID1] );
    pointIDs2.push_back( poiList[poiLocalID2] );

    return this->splitEdgesAndPolygons( polygonIDs, pointIDs1, ratio1, pointIDs2, ratio2, addPointPositionsList, tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    エッジ分割とポリゴン分割を同時に行う <br>　
//!	    @brief　エッジ分割とポリゴン分割を同時に行う
//!		@param[in] polygonIDs 対象とするポリゴン番号
//!		@param[in] poiLocalID1 分割対称のエッジのスタートポイント(頂点番号) 
//!		@param[in] ratio1 分割位置を指定するエッジ上のレシオ(0.0~1.0)　ポリゴン分割のスタート位置
//!		@param[in] poiLocalID2 分割対称のエッジのスタートポイント（頂点番号）
//!		@param[in] ratio2 分割位置を指定するエッジ上のレシオ(0.0~1.0)　ポリゴン分割のエンド位置
//!		@param[in] addPointPositions ポリゴン分割のスタート～エンドの間に頂点を挟む場合の頂点位置
//!		@return 新規追加したポリゴンの番号
//!		@warning 現状ではUVやカラー等の付属アトリビュートは未対応
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitEdgesAndPolygon( u32 polygonID, u32 pointID1, T ratio1, u32 pointID2, T ratio2, const std::vector<Vec3T<T>>& addPointPositions, const T tolerance ) {
    std::vector<u32> polygonIDs, pointIDs1, pointIDs2;
    std::vector<f32> ratios1, ratios2;
    std::vector<std::vector<Vec3>> addPointPositionsList;

    polygonIDs.push_back( polygonID );
    pointIDs1.push_back( pointID1 );
    ratios1.push_back( ratio1 );
    pointIDs2.push_back( pointID2 );
    ratios2.push_back( ratio2 );
    addPointPositionsList.push_back( addPointPositions );

    return this->splitEdgesAndPolygons( polygonIDs, pointIDs1, ratios1, pointIDs2, ratios2, addPointPositionsList, tolerance );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    複数のエッジ分割とポリゴン分割を同時に行う（splitEdge()とsplitPolygon()を繰り返すより効率的））　PolyHall()もサポート<br>　
//!	    @brief　複数のエッジ分割とポリゴン分割を同時に行う（splitEdge()とsplitPolygon()を繰り返すより効率的））　PolyHall()もサポート
//!		@param[in] polygonIDs 対象とするポリゴン番号
//!		@param[in] spritPositionsList 分割エッジの頂点リスト　各要素の[0]が分割エッジのスタート/[1]がエンド　片方がオンエッジ・オンポイントでない場合はスキップ　両方がオンエッジ・オンポイントでない場合はPolyHall()を実行
//!		@return 新規追加したポリゴン群の最初のポリゴン番号
//!		@warning 現状ではUVやカラー等の付属アトリビュートは未対応
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
u32 PolygonMeshT<T>::splitEdgesAndPolygons( const std::vector<u32>& polygonIDs, const std::vector<std::vector<Vec3T<T>>>& spritPositionsList ) {
    
    std::vector<u32> polygonIDs;
    std::vector<u32> pointIDs1;
    std::vector<T> ratio1;
    std::vector<u32> pointIDs2;
    std::vector<T> ratio2;
    std::vector<std::vector<Vec3T<T>>> addPointPositionsList;


}*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    複数のエッジ分割とポリゴン分割を同時に行う（splitEdge()とsplitPolygon()を繰り返すより効率的））<br>　
//!	    @brief　複数のエッジ分割とポリゴン分割を同時に行う（splitEdge()とsplitPolygon()を繰り返すより効率的））
//!		@param[in] polygonIDs 対象とするポリゴン番号
//!		@param[in] pointIDs1 分割対称のエッジのスタートポイント（頂点番号） 
//!		@param[in] ratio1 分割位置を指定するエッジ上のレシオ(0.0~1.0)　ポリゴン分割のスタート位置
//!		@param[in] pointIDs2 分割対称のエッジのスタートポイント（頂点番号）
//!		@param[in] ratio2 分割位置を指定するエッジ上のレシオ(0.0~1.0)　ポリゴン分割のエンド位置
//!		@param[in] addPointPositionsList ポリゴン分割のスタート～エンドの間に頂点を挟む場合の頂点位置
//!		@return 新規追加したポリゴン群の最初のポリゴン番号
//!		@warning 現状ではUVやカラー等の付属アトリビュートは未対応
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitEdgesAndPolygons(
    const std::vector<u32>& polygonIDs,
    const std::vector<u32>& pointIDs1,
    const std::vector<T>& ratio1,
    const std::vector<u32>& pointIDs2,
    const std::vector<T>& ratio2,
    const std::vector<std::vector<Vec3T<T>>>& addPointPositionsList,
    const T ratio_tolerance,
    const T pos_tolerance )
{
    if ( this->modelingMode == false ) { return BURGESS2::U32_INFINITE; }
    std::size_t cnt = polygonIDs.size();
    if ( cnt != pointIDs1.size() || cnt != ratio1.size() || cnt != pointIDs2.size() || cnt != ratio2.size() || cnt != addPointPositionsList.size() ) {
        return BURGESS2::U32_INFINITE;
    }
    
    u32 origPolyCnt = this->polygonCount();
    u32 origPoiCnt = this->pointCount();

    // エッジ分割用の情報格納場所
    std::vector<u32> edgeIDs;
    std::vector<f32> ratios;
    edgeIDs.reserve( cnt * 2 );
    ratios.reserve( cnt * 2 );

    // ポリゴン分割用の情報格納場所
    std::vector<Bool> rev;
    std::vector<u32> validIndex;

    // エッジ分割用の情報を整理する
    u32 c = u32( cnt );
    for ( u32 i = 0; i < c; i++ ) {
        u32 polyID = polygonIDs[i];

        // 引数のエッジ分割情報を確認する
        // 分割位置のレシオが1.0の場合、ポリゴン内の次の頂点番号に置き変えてレシオ値を0.0にする
        u32 poiID1 = pointIDs1[i];
        T r1 = ratio1[i];
        if (Burgess2MathFn::AlmostEqual( ratio1[i], T( 1.0 ), ratio_tolerance ) || T( 1.0 ) < ratio1[i] ) {
            poiID1 = this->getNextPolygonPointID( polyID, poiID1 );
            r1 = T( 0.0 );
        }
        u32 poiID2 = pointIDs2[i];
        T r2 = ratio2[i];
        if (Burgess2MathFn::AlmostEqual( ratio2[i], T( 1.0 ), ratio_tolerance ) || T( 1.0 ) < ratio2[i] ) {
            poiID2 = this->getNextPolygonPointID( polyID, poiID2 );
            r2 = T( 0.0 );
        }
        // 引数の追加頂点の入力が無い場合
        if ( addPointPositionsList[i].size() == 0 ) {
            // 更にエッジ分割が同エッジ上の場合は、ポリゴンの分割が出来ない為スキップ
            if ( poiID1 == poiID2 ) {
                continue;
            }

            // レシオが0.0の場合、一つ前のエッジIDが片方のエッジ分割処理のエッジIDと一緒か確認する
            // 一致した場合は同一エッジを分割することになり、ポリゴンの分割が出来ない為スキップ
            if ( Burgess2MathFn::AlmostEqual( r1, T( 0.0 ), ratio_tolerance ) || r1 < T( 0.0 ) ) {
                u32 prevPoiID = this->getPreviousPolygonPointID( polyID, poiID1 );
                if( poiID2 == prevPoiID ){ 
                    continue; 
                }
            }
            if ( Burgess2MathFn::AlmostEqual( r2, T( 0.0 ), ratio_tolerance ) || r1 < T( 0.0 ) ) {
                u32 prevPoiID = this->getPreviousPolygonPointID( polyID, poiID2 );
                if ( poiID1 == prevPoiID ) {
                    continue;
                }
            }
        }

        // p1/p2を走査してどちらがスタート/エンドになるか判断する
        // p2の方がスタートになる場合は、ポリゴン分割の際にaddPoiPosの配列要素を逆順で使用するひつようがあるためrev[]をtrueにする
        if ( poiID1 == poiID2 ) {
            u32 edgeID = this->getPolygonPointEdgeID( polyID, poiID1 );
            edgeIDs.push_back( edgeID );
            edgeIDs.push_back( edgeID );
            // エッジ分割が同エッジ上の場合、レシオの値を比較する
            // p2の方がスタートになる場合は、addPoiPosの配列順を逆転する
            if ( r1 <= r2 ) {
                ratios.push_back( r1 );
                ratios.push_back( r2 );
                rev.push_back( false );
            } else {
                ratios.push_back( r2 );
                ratios.push_back( r1 );
                rev.push_back( true );
            }
        } else {
            u32 edgeID1 = this->getPolygonPointEdgeID( polyID, poiID1 );
            u32 edgeID2 = this->getPolygonPointEdgeID( polyID, poiID2 );
            // エッジ分割が別エッジの場合、ポリゴン内のローカルインデックスを比較する
            // p2の方がスタートになる場合は、addPoiPosの配列順を逆転する
            u32 locID1 = this->getLocalVertexID( polyID, poiID1 );
            u32 locID2 = this->getLocalVertexID( polyID, poiID2 );
            if ( locID1 < locID2 ) {
                edgeIDs.push_back( edgeID1 );
                edgeIDs.push_back( edgeID2 );
                ratios.push_back( r1 );
                ratios.push_back( r2 );
                rev.push_back( false );
            } else {
                edgeIDs.push_back( edgeID2 );
                edgeIDs.push_back( edgeID1 );
                ratios.push_back( r2 );
                ratios.push_back( r1 );
                rev.push_back( true );
            }
        }
        validIndex.push_back( i );
    }

    if (edgeIDs.size () == 0 || validIndex.size () * 2 != edgeIDs.size ()) { return BURGESS2::U32_INFINITE; }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // エッジ分割を行い、新メッシュトポロジーデータを作成する
    std::vector<u32> packedPointIDs;
    std::vector<u32> polygonSizes;
    std::vector<Vec3T<T>> addPoiPos;
    u32 addCnt = this->splitEdgeCore( packedPointIDs, polygonSizes, addPoiPos, edgeIDs, ratios, ratio_tolerance, pos_tolerance );
    if ( addCnt == BURGESS2::U32_INFINITE ) { return BURGESS2::U32_INFINITE; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ポリゴン分割用にデータ整理

    // エッジ分割の頂点追加分だけ、先に適用しておく
    this->addPoints( addPoiPos );
    // packedPointIDsを3Ｄ配列に変換する [ポリゴン数][ポリゴン分割数][ポリゴンの画数]　
    // ここで作るデータでは、[ポリゴンの分割数]の配列数はすべて1つ
    std::vector<std::vector<std::vector<u32>>> newPolyPointIDList( polygonSizes.size()-1 );
    for ( u32 i = 0; i < polygonSizes.size()-1; i++ ) {
        newPolyPointIDList[i].resize( 1 );
        std::vector<u32> rest(0);
        for ( u32 j = polygonSizes[i]; j < polygonSizes[i + 1]; j++ ) {
            newPolyPointIDList[i][0].push_back( packedPointIDs[j] );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ポリゴン分割を行い、新メッシュトポロジーデータを更新する

    // 分割のスタート/エンドのポイントIDを新トポロジーデータから取得する
    std::vector<Vec2u> sePoiIDs( validIndex.size() );
    Bool end = false;
    #pragma omp parallel for 
    for ( s32 i = 0; i < edgeIDs.size(); i++ ) {
        u32 id = u32( i / 2 );
        u32 xyID = u32( i - ( id * 2 ) );

        // エッジ分割位置を取得して同じ位置のポイントIDを検索する
        Bool find = false;
        Vec2u poiIDs = this->getEdgePointIDs( edgeIDs[i] );
        if (Burgess2MathFn::AlmostEqual ( ratios[i], T ( 0.0 ), ratio_tolerance )) {
            sePoiIDs[id][xyID] = poiIDs[0];
            find = true;
        } else if (Burgess2MathFn::AlmostEqual ( ratios[i], T ( 1.0 ), ratio_tolerance )) {
            sePoiIDs[id][xyID] = poiIDs[1];
            find = true;
        } else {
            Vec3T<T> edgeSplitPos = this->getPointPosition ( poiIDs[0] ).lerp ( this->getPointPosition ( poiIDs[1] ), ratios[i] );
            
            u32 polyID = polygonIDs[validIndex[id]];
            for (u32 j = 0; j < newPolyPointIDList[polyID][0].size (); j++) {
                u32 poiID = newPolyPointIDList[polyID][0][j];
                Vec3T<T> poiPos = this->getPointPosition ( poiID );
                if (edgeSplitPos.almostEqual ( poiPos, pos_tolerance ) == true) {
                    sePoiIDs[id][xyID] = poiID;
                    find = true;
                    break;
                }
            }
        }        
        if ( find == false ) {
            end = true;
            break;
        }
    }
    if(end){ return BURGESS2::U32_INFINITE; }

    // newPolyPointIDListとaddPoiPosを更新する
    u32 newOrigPoiCount = origPoiCnt + u32(addPoiPos.size());
    addPoiPos.resize( 0 );
    for ( u32 i = 0; i < validIndex.size(); i++ ) {
        u32 polyID = polygonIDs[validIndex[i]];

        // スタート/エンドの２つのポイントIDが含まれているリストを探す
        s32 targetListID = -1;
        for ( u32 j = 0; j < newPolyPointIDList[polyID].size(); j++ ) {
            Bool f = false, s = false;
            for ( u32 k = 0; k < newPolyPointIDList[polyID][j].size(); k++ ) {
                if ( sePoiIDs[i][0] == newPolyPointIDList[polyID][j][k] ) { f = true; }
                if ( sePoiIDs[i][1] == newPolyPointIDList[polyID][j][k] ) { s = true; }
            }
            if ( f && s ) {
                targetListID = j;
                break;
            }
        }
        // リストが見つからない場合はスキップ
        if ( targetListID == -1 ) { continue; }

        // 追加頂点をaddPoiPosにセットする
        u32 addPoiCnt = u32(addPointPositionsList[validIndex[i]].size());
        if ( addPoiCnt != 0 ) {
            if ( rev[i] == false ) {
                for ( u32 j = 0; j < addPointPositionsList[validIndex[i]].size(); ++j ) {
                    addPoiPos.push_back( addPointPositionsList[validIndex[i]][j] );
                }
            } else {
                for ( s32 j = s32(addPointPositionsList[validIndex[i]].size() - 1); 0 <= j; --j ) {
                    addPoiPos.push_back( addPointPositionsList[validIndex[i]][j] );
                }
            }
        }

        // 新しいPolyPointIDListを２つ作る
        //  例：[0,8,1,7,2,3]のポリゴンポイントIDリスト　・ 分割スタート/エンドのポイントIDは[8,2]　・ 追加頂点は2つ(ポイントIDは20,21)　の条件の場合
        //      resetList = [0,8,20,21,2,3]
        //      newList   = [8,1,7,2,21,20]
        std::vector<u32> resetList; // targetListIDのリストを置き換えるためのリスト
        std::vector<u32> newList; // 新しくつくるポリゴン用のリスト
        Bool forNewList = false;
        for ( u32 j = 0; j < newPolyPointIDList[polyID][targetListID].size(); j++ ) {
            u32 polyPoiID = newPolyPointIDList[polyID][targetListID][j];
            if ( forNewList == false ) {
                resetList.push_back( polyPoiID );
            } else {
                newList.push_back( polyPoiID );
            }

            if ( sePoiIDs[i][0] == polyPoiID ) {
                newList.push_back( polyPoiID );
                forNewList = true;
            }

            if ( sePoiIDs[i][1] == polyPoiID ) {
                // 上記例の "20,21" の部分を作る
                for ( u32 k = 0; k < addPoiCnt; k++ ) {
                    resetList.push_back( newOrigPoiCount + u32(addPoiPos.size()) - addPoiCnt + k );
                }
                resetList.push_back( polyPoiID );

                // 上記例の "21,20" の部分を作る
                for ( u32 k = 0; k < addPoiCnt; k++ ) {
                    newList.push_back( newOrigPoiCount + u32(addPoiPos.size()) - 1 - k );
                }

                forNewList = false;
            }
        }

        if ( resetList.size() < 3 || newList.size() < 3 ){
            continue;
        }
        newPolyPointIDList[polyID][targetListID] = resetList;
        newPolyPointIDList[polyID].push_back( newList );
    }

    // トポロジーデータを作成
    packedPointIDs.resize(0);
    polygonSizes.resize(0);
    polygonSizes.push_back( 0 );
    u32 sum = 0;
    for ( u32 i = 0; i < newPolyPointIDList.size(); i++ ) {
        // まずは各ポリゴンの[0]のリストだけをトポロジーデータに変換する
        for ( u32 j = 0; j < newPolyPointIDList[i][0].size(); j++ ) {
            packedPointIDs.push_back( newPolyPointIDList[i][0][j] );
        }
        polygonSizes.push_back( sum + u32(newPolyPointIDList[i][0].size()) );
        sum += u32(newPolyPointIDList[i][0].size());
    }
    for ( u32 i = 0; i < newPolyPointIDList.size(); i++ ) {
        // 次に各ポリゴンの[1~]のリストをトポロジーデータに変換する
        for ( u32 j = 1; j < newPolyPointIDList[i].size(); j++ ) {
            for ( u32 k = 0; k < newPolyPointIDList[i][j].size(); k++ ) {
                packedPointIDs.push_back( newPolyPointIDList[i][j][k] );
            }
            polygonSizes.push_back( sum + u32(newPolyPointIDList[i][j].size()) );
            sum += u32(newPolyPointIDList[i][j].size());
        }
    }

    //　頂点を追加
    this->addPoints( addPoiPos );
    // トポロジーを更新
    this->resetTopology( polygonSizes, packedPointIDs, true );

    this->deleteUnusedPoints = true;

    return origPolyCnt;
}
