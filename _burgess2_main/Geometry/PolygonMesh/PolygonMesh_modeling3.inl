//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// slitEdge() slitPolygon() slitEdgeAndPolygon()

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �G�b�W�𕪊����� <br>�@
//!	    @brief �G�b�W�𕪊�����
//!		@param[in] vertexID �G�b�W�̎n�܂�̃o�[�e�b�N�X�ԍ�
//!		@param[in] ratio �����ʒu���w�肷��G�b�W��̃��V�I(0.0~1.0)
//!		@return �V�K�ǉ��������_�ԍ�
//!		@warning ����ł�UV��J���[���̕t���A�g���r���[�g�͖��Ή�
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
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
        //�@���_��ǉ�
        this->addPoints( addPoiPos );
        // �g�|���W�[���X�V
        this->resetTopology( polygonSizes, packedPointIDs, true );
    }

    return returnVal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �G�b�W�𕪊����� <br>�@
//!	    @brief �G�b�W�𕪊�����
//!		@param[in] polygonID �|���S���ԍ�
//!		@param[in] pointID �G�b�W�̎n�܂�̒��_�ԍ�
//!		@param[in] ratio �����ʒu���w�肷��G�b�W��̃��V�I(0.0~1.0)
//!		@return �V�K�ǉ��������_�ԍ�
//!		@warning ����ł�UV��J���[���̕t���A�g���r���[�g�͖��Ή�
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitEdge( u32 polygonID, u32 pointID, f32 ratio ) {
    if ( this->modelingMode == false ) { return BURGESS2::U32_INFINITE; }
    if ( this->polygonCount() <= polygonID || this->pointCount() <= pointID ) { return BURGESS2::U32_INFINITE; }

    u32 edgeID = this->getPolygonPointEdgeID( polygonID, pointID );
    if ( this->edgeCount() < edgeID ) { return BURGESS2::U32_INFINITE; } // �G�b�W��������Ȃ������ꍇ�͏I��

    std::vector<u32> edgeIDs( 1, edgeID );
    std::vector<f32> ratios2( 1, ratio );

    std::vector<u32> packedPointIDs;
    std::vector<u32> polygonSizes;
    std::vector<Vec3T<T>> addPoiPos;
    u32 returnVal = this->splitEdgeCore( packedPointIDs, polygonSizes, addPoiPos, edgeIDs, ratios2 );

    if ( returnVal != BURGESS2::U32_INFINITE ) {
        //�@���_��ǉ�
        this->addPoints( addPoiPos );
        // �g�|���W�[���X�V
        this->resetTopology( polygonSizes, packedPointIDs, true );
    }

    return returnVal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �����{�̃G�b�W����C�ɕ�������isplitEdge()���J��Ԃ��������I�j�j<br>�@
//!	    @brief �����{�̃G�b�W����C�ɕ�������isplitEdge()���J��Ԃ��������I�j
//!		@param[in] vertexIDs �G�b�W�̃X�^�[�g�o�[�e�b�N�X�ԍ�
//!		@param[in] ratio �����ʒu���w�肷��G�b�W��̃��V�I(0.0~1.0)
//!		@return �V�K�ǉ��������_�Q�̍ŏ��̒��_�ԍ�
//!		@warning ����ł�UV��J���[���̕t���A�g���r���[�g�͖��Ή�
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
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
        //�@���_��ǉ�
        this->addPoints( addPoiPos );
        // �g�|���W�[���X�V
        this->resetTopology( polygonSizes, packedPointIDs, true );
    }

    return returnVal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �����{�̃G�b�W����C�ɕ�������isplitEdge()���J��Ԃ��������I�j<br>�@
//!	    @brief �����{�̃G�b�W����C�ɕ�������isplitEdge()���J��Ԃ��������I�j
//!		@param[in] polygonIDs 
//!		@param[in] pointIDs �G�b�W�̃X�^�[�g�|�C���g
//!		@param[in] ratios �����ʒu���w�肷��G�b�W��̃��V�I(0.0~1.0)
//!		@return �V�K�ǉ��������_�Q�̍ŏ��̒��_�ԍ�
//!		@warning ����ł�UV��J���[���̕t���A�g���r���[�g�͖��Ή�
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitEdges( const std::vector<u32>& polygonIDs, const std::vector<u32>& pointIDs, const std::vector<f32>& ratios, const T tolerance ) {
    if ( this->modelingMode == false ) { return BURGESS2::U32_INFINITE; }
    if ( polygonIDs.size() != pointIDs.size() || polygonIDs.size() != ratios.size() ) { return BURGESS2::U32_INFINITE; }
    if ( polygonIDs.size() == 0 ) { return BURGESS2::U32_INFINITE; }

    // �d���△���Ȓl���Ȃ����z����쐬����
    std::vector<u32> edgeIDs( 0 );
    std::vector<f32> ratios2( 0 );
    for ( u32 i = 0; i < polygonIDs.size(); ++i ) {
        u32 edgeID = this->getPolygonPointEdgeID( polygonIDs[i], pointIDs[i] );
        if ( this->edgeCount() < edgeID ) { continue; } // �G�b�W��������Ȃ������ꍇ�͏I��

        edgeIDs.push_back( edgeID );
        ratios2.push_back( ratios[i] );
    }
    if ( edgeIDs.size() == 0 ) { return BURGESS2::U32_INFINITE; } // �L���ȃG�b�W�̓o�^�����������ꍇ�͏I��

    std::vector<u32> packedPointIDs;
    std::vector<u32> polygonSizes;
    std::vector<Vec3T<T>> addPoiPos;
    u32 returnVal = this->splitEdgeCore( packedPointIDs, polygonSizes, addPoiPos, edgeIDs, ratios2, tolerance );

    if ( returnVal != BURGESS2::U32_INFINITE ) {
        //�@���_��ǉ�
        this->addPoints( addPoiPos );
        // �g�|���W�[���X�V
        this->resetTopology( polygonSizes, packedPointIDs, true );
    }

    return returnVal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    private: splitEdge()�n�̃R�A�֐�
//!	    @brief private: splitEdge()�n�̃R�A�֐�
//!		@return �V�K�ǉ��������_�Q�̍ŏ��̒��_�ԍ�
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

    // �אڃG�b�W�̕����w���ǉ��������X�g���쐬������ �����ɕ����w�肪�S���������m��Aratio��0.0��������1.0�̃��m�̓��X�g����Ȃ�
    std::vector<u32> newEdgeIDs( 0 );
    std::vector<f32> newRatios( 0 );
    for ( u32 i = 0; i < edgeIDs.size(); i++ ) {
        T ratio = Burgess2MathFn::Clamp( _ratios[i], T( 0.0 ), T( 1.0 ) );
        // ratio��0.0��������1.0�ꍇ�̓X�L�b�v
        if ( Burgess2MathFn::AlmostEqual( ratio, T( 0.0 ), ratio_tolerance ) || Burgess2MathFn::AlmostEqual( ratio, T( 1.0 ), ratio_tolerance ) ){
            continue;
        }
        // �G�b�WID�����݂��Ȃ��ꍇ�̓X�L�b�v
        if ( this->edgeCount() <= edgeIDs[i] ) { continue; }

        // new���X�g�ɓ����ݒ�l�����ɑ��݂���ꍇ�̓X�L�b�v
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

        // new���X�g�ɒǉ�
        newEdgeIDs.push_back( edgeIDs[i] );
        newRatios.push_back( ratio );

        // ���E�G�b�W�łȂ��ꍇ�͗אڃG�b�W��new���X�g�ɒǉ�
        if ( this->isPolygonBorder( edgeIDs[i] ) == false ) {
            u32 adjEdgeID = this->getEdgeAdjacentEdgeID( edgeIDs[i] );
            newEdgeIDs.push_back( this->getEdgeStartVertexID( adjEdgeID ) ); // verID = edgeID
            newRatios.push_back( T( 1.0 ) - ratio );
        }
    }
    u32 addPoiCnt = u32(newEdgeIDs.size());
    if ( addPoiCnt == 0 ) { return BURGESS2::U32_INFINITE; }

    // �ǉ����钸�_�̃��X�g�ƁA���_ID���X�g���쐬����
    u32 cnt = 0;
    Bool firstLoop = true;
    std::vector<Bool> done( addPoiCnt, false );
    std::vector<std::vector<u32>> newPointIDs( this->polygonCount() );
    while ( true ) {
        for ( u32 i = 0; i < addPoiCnt; i++ ) {
            if ( firstLoop ) { // ����̏���
                // �n�[�t�G�b�W�ō\�����Ă���|���S��ID���擾���A���_ID���X�g�𔲂��o���ĐV�K���X�g�̉��n�����
                // ���Ƀ��X�g�������o����Ă���ꍇ�̓X�L�b�v�i�����G�b�W�ɑ΂��ĕ����񕪊����s����j
                u32 polyID = this->getEdgeLeftPolygonID( newEdgeIDs[i] );
                if ( newPointIDs[polyID].size() != 0 ) {
                    continue;
                }
                L8Array<u32> origPolyIDs = this->getPolygonPointIDs( polyID );
                for ( u32 j = 0; j < origPolyIDs.size(); j++ ) {
                    newPointIDs[polyID].push_back( origPolyIDs[j] );
                }

                // �����ʒu
                Vec2u poiID = this->getEdgePointIDs( newEdgeIDs[i] );
                Vec3T<T> fPos = this->getPointPosition( poiID.x() );
                Vec3T<T> ePos = this->getPointPosition( poiID.y() );
                Vec3T<T> newPos = fPos.lerp( ePos, newRatios[i] );

                // ���ɓo�^���Ă���V�K�|�C���g�ʒu�ƈ�v������̂��������m�F
                s32 find = -1;
                for ( s32 j = 0; j < addPoiPos.size(); j++ ) {
                    if ( addPoiPos[j].almostEqual( newPos, pos_tolerance ) ) {
                        find = j;
                        break;
                    }
                }
                u32 newPoiID;
                if ( find == -1 ) { // ���X�g���Ɉ�v����|�W�V���������������ꍇ�̓��X�g�ɒǉ�
                    newPoiID = origPoiCnt + u32(addPoiPos.size());
                    addPoiPos.push_back( newPos );
                } else { // ���X�g���ɑ��݂����ꍇ��ID���擾
                    newPoiID = origPoiCnt + find;
                }

                // �V�K�|�C���gID�𒸓_ID���X�g�ɑ}������
                // �}���ʒu�̓G�b�W�̃X�^�[�g�|�C���gID�Ŕ��f
                for ( u32 j = 0; j < newPointIDs[polyID].size(); j++ ) {
                    if ( newPointIDs[polyID][j] == poiID.x() ) {
                        newPointIDs[polyID].insert( newPointIDs[polyID].begin() + j + 1, newPoiID );
                    }
                }

                done[i] = true;
                cnt++;
            } else { // �����G�b�W�𕡐��񕪊����Ă���ꍇ�̏���
                if ( done[i] == true ) { continue; }

                u32 polyID = this->getEdgeLeftPolygonID( newEdgeIDs[i] );
                // �����ʒu
                Vec2u poiID = this->getEdgePointIDs( newEdgeIDs[i] );
                Vec3T<T> fPos = this->getPointPosition( poiID.x() );
                Vec3T<T> ePos = this->getPointPosition( poiID.y() );
                Vec3T<T> newPos = fPos.lerp( ePos, newRatios[i] );

                // ���ɓo�^���Ă���V�K�|�C���g�ʒu�ƈ�v������̂��������m�F
                s32 find = -1;
                for ( s32 j = 0; j < addPoiPos.size(); j++ ) {
                    if ( addPoiPos[j].almostEqual( newPos, pos_tolerance ) ) {
                        find = j;
                        break;
                    }
                }
                u32 newPoiID;
                if ( find == -1 ) { // ���X�g���Ɉ�v����|�W�V���������������ꍇ�̓��X�g�ɒǉ�
                    newPoiID = origPoiCnt + u32(addPoiPos.size());
                    addPoiPos.push_back( newPos );
                } else { // ���X�g���ɑ��݂����ꍇ��ID���擾
                    newPoiID = origPoiCnt + u32(find);
                }

                // �V�K�|�C���gID�𒸓_ID���X�g�ɑ}������
                // �}���ʒu�͑Ώۃ|���S���̃G�b�W�̃X�^�[�g�G���h�̃|�C���g�ւ̃x�N�g���Ŕ��f����
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
                    Vec3T<T> dir1 = ( ePos - sPos ).unit();		// �X�^�[�g����G���h�܂ł̃x�N�g��
                    Vec3T<T> dir2 = ( ePos - newPos ).unit();	// �ǉ��|�C���g����G���h�܂ł̃x�N�g��
                    if ( dir1.almostSameDirection( dir2 ) ) { // �����������m�F
                        dir1 = dir1 * T( -0.1 );					// �G���h����X�^�[�g�܂ł̃x�N�g��
                        dir2 = ( sPos - newPos ).unit();			// �ǉ��|�C���g����X�^�[�g�܂ł̃x�N�g��
                        if ( dir1.almostSameDirection( dir2 ) && newPointIDs[polyID][j+1] != newPoiID && newPointIDs[polyID][j] != newPoiID ) { // �����������m�F
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

    // �g�|���W�[���X�g���쐬
    packedPointIDs.reserve( origPoiCnt + addPoiPos.size() );
    polygonSizes.resize( this->polygonCount() + 1 );
    polygonSizes[0] = 0;
    for ( u32 i = 0; i < this->polygonCount(); i++ ) {
        u32 polySize;
        if ( newPointIDs[i].size() == 0 ) { // �I���W�i���̒l���p��
            polySize = this->getPolygonSize( i );
            L8Array<u32> poiIDs = this->getPolygonPointIDs( i );
            for ( u32 j = 0; j < poiIDs.size(); j++ ) {
                packedPointIDs.push_back( poiIDs[j] );
            }
        } else { // �V�������X�g�̒l��K�p
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
//!	    �|���S���𕪊�����<br>�@
//!	    @brief�@�|���S���𕪊�����
//!		@param[in] polygonID �ΏۂƂ���|���S���ԍ�
//!		@param[in] point1 �����G�b�W���\������1�߂̒��_�ԍ�
//!		@param[in] point2 �����G�b�W���\������2�߂̒��_�ԍ�
//!		@return �V�K�ǉ������|���S���ԍ�
//!		@warning ����ł�UV��J���[���̕t���A�g���r���[�g�͖��Ή�
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
//!	    �����̃|���S���𕪊�����isplitPolygon()���J��Ԃ��������I�j�j<br>�@
//!	    @brief�@�����̃|���S���𕪊�����isplitPolygon()���J��Ԃ��������I�j�j
//!		@param[in] polygonIDs �ΏۂƂ���|���S���ԍ�
//!		@param[in] points �����G�b�W���\������2�̒��_�ԍ�
//!		@return �V�K�ǉ������|���S���Q�̍ŏ��̃|���S���ԍ�
//!		@warning ����ł�UV��J���[���̕t���A�g���r���[�g�͖��Ή�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::splitPolygons( const std::vector<u32>& polygonIDs, const std::vector<Vec2u>& points ) {
    if ( this->modelingMode == false ) { return BURGESS2::U32_INFINITE; }
    if ( polygonIDs.size() != points.size() ) { return BURGESS2::U32_INFINITE; }

    u32 origSize = this->polygonCount();

    // �����ΏۂƂ���|���S�����č\�����邽�߂̃|���S���T�C�Y�ƒ��_ID���X�g���쐬
    std::vector<u32> polyIDs;
    std::vector<std::vector<u32>> newPointIDs; // ������̃|���S�����\�����钸�_ID���X�g
    std::vector<std::vector<u32>> nwePolyPointIDs; // �V�K�쐬����|���S�����\�����钸�_ID���X�g
    u32 cnt{ 0 };
    for ( u32 i = 0; i < polygonIDs.size(); ++i ) {
        if ( this->polygonCount() <= polygonIDs[i] ) { continue; }

        std::vector<u32> restIDs1, restIDs2;
        Bool newPoly = false;

        L8Array<u32> polyPoiIDs = this->getPolygonPointIDs( polygonIDs[i] ); // ��������|���S�����\�����钸�_ID�Q���擾
        for ( u32 j = 0; j < polyPoiIDs.size(); ++j ) {
            if ( polyPoiIDs[j] == points[i].x() || polyPoiIDs[j] == points[i].y() ) {
                // �����̒��_�͂Q�̃|���S���ǂ���ł��g�p
                restIDs1.push_back( polyPoiIDs[j] );
                restIDs2.push_back( polyPoiIDs[j] );
                newPoly = newPoly == false;
            } else {
                // �����Ŏg�p���Ȃ����_�����ꂼ��̃��X�g�֊���U��
                if ( newPoly ) {
                    restIDs2.push_back( polyPoiIDs[j] );
                } else {
                    restIDs1.push_back( polyPoiIDs[j] );
                }
            }
        }
        if ( restIDs1.size() < 3 || restIDs2.size() < 3 ) { continue; } // �|���S�����\���ł��邾���̒��_���������ꍇ�͏I��

        polyIDs.push_back( polygonIDs[i] ); // ����Ƀ��X�g�𐶐��ł����|���S��ID��V�K���X�g�ɕێ�����
        newPointIDs.push_back( restIDs1 );
        nwePolyPointIDs.push_back( restIDs2 );
    }
    if ( polyIDs.size() == 0 ) {
        // �V�K�̃|���S�����X�g����������Ȃ������ꍇ�͏I��
        return BURGESS2::U32_INFINITE;
    }

    // ���X�g���g���ĐV����polygonSizes�A�g���r���[�g�f�[�^���쐬
    std::vector<u32> newPolygonSizes( this->polygonCount() + nwePolyPointIDs.size() + 1 ); // +1 �� [0]=0
    newPolygonSizes[0] = 0;
    for ( u32 i = 0; i < this->polygonCount(); ++i ) { // �����̃g�|���W�[�͈͂��X�V
        u32 polySize = this->getPolygonSize( i );
        for ( u32 j = 0; j < polyIDs.size(); ++j ) {
            if ( i == polyIDs[j] ) {
                polySize = newPointIDs[j].size();
                break;
            }
        }
        newPolygonSizes[i + 1] = newPolygonSizes[i] + polySize; // +1 �� [0]=0
    }
    for ( u32 i = 0; i < nwePolyPointIDs.size(); ++i ) { // �V�K�|���S���̏���ǉ�
        newPolygonSizes[origSize + i + 1] = newPolygonSizes[origSize + i] + nwePolyPointIDs[i].size(); // +1 �� [0]=0
    }

    // ���X�g���g���ĐV����packedPointIDs�A�g���r���[�g�f�[�^���쐬
    std::vector<u32> newPackedPoiIDs;
    for ( u32 i = 0; i < this->polygonCount(); ++i ) { // �����̃g�|���W�[�͈͂��X�V
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
    for ( u32 i = 0; i < nwePolyPointIDs.size(); ++i ) { // �V�K�|���S���̏���ǉ�
        for ( u32 j = 0; j < nwePolyPointIDs[i].size(); ++j ) {
            newPackedPoiIDs.push_back( nwePolyPointIDs[i][j] );
        }
    }

    // �g�|���W�[�A�g���r���[�g���X�V
    U32AttributeT<T>& polySizeAttr = this->getU32Attribute( "polygonSizes" );
    polySizeAttr.resetValues( newPolygonSizes );
    U32AttributeT<T>& packedPoiIDAttr = this->getU32Attribute( "packedPointIDs" );
    packedPoiIDAttr.resetValues( newPackedPoiIDs );

    return origSize;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �G�b�W�����ƃ|���S�������𓯎��ɍs�� <br>�@
//!	    @brief�@�G�b�W�����ƃ|���S�������𓯎��ɍs��
//!		@param[in] polygonIDs �ΏۂƂ���|���S���ԍ�
//!		@param[in] poiLocalID1 �����Ώ̂̃G�b�W�̃X�^�[�g�|�C���g(�|���S�����̃��[�J��ID) 
//!		@param[in] ratio1 �����ʒu���w�肷��G�b�W��̃��V�I(0.0~1.0)�@�|���S�������̃X�^�[�g�ʒu
//!		@param[in] poiLocalID2 �����Ώ̂̃G�b�W�̃X�^�[�g�|�C���g�i�|���S�����̃��[�J��ID�j
//!		@param[in] ratio2 �����ʒu���w�肷��G�b�W��̃��V�I(0.0~1.0)�@�|���S�������̃G���h�ʒu
//!		@param[in] addPointPositions �|���S�������̃X�^�[�g�`�G���h�̊Ԃɒ��_�����ޏꍇ�̒��_�ʒu
//!		@return �V�K�ǉ������|���S���̔ԍ�
//!		@warning ����ł�UV��J���[���̕t���A�g���r���[�g�͖��Ή�
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
//!	    �G�b�W�����ƃ|���S�������𓯎��ɍs�� <br>�@
//!	    @brief�@�G�b�W�����ƃ|���S�������𓯎��ɍs��
//!		@param[in] polygonIDs �ΏۂƂ���|���S���ԍ�
//!		@param[in] poiLocalID1 �����Ώ̂̃G�b�W�̃X�^�[�g�|�C���g(���_�ԍ�) 
//!		@param[in] ratio1 �����ʒu���w�肷��G�b�W��̃��V�I(0.0~1.0)�@�|���S�������̃X�^�[�g�ʒu
//!		@param[in] poiLocalID2 �����Ώ̂̃G�b�W�̃X�^�[�g�|�C���g�i���_�ԍ��j
//!		@param[in] ratio2 �����ʒu���w�肷��G�b�W��̃��V�I(0.0~1.0)�@�|���S�������̃G���h�ʒu
//!		@param[in] addPointPositions �|���S�������̃X�^�[�g�`�G���h�̊Ԃɒ��_�����ޏꍇ�̒��_�ʒu
//!		@return �V�K�ǉ������|���S���̔ԍ�
//!		@warning ����ł�UV��J���[���̕t���A�g���r���[�g�͖��Ή�
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
//!	    �����̃G�b�W�����ƃ|���S�������𓯎��ɍs���isplitEdge()��splitPolygon()���J��Ԃ��������I�j�j�@PolyHall()���T�|�[�g<br>�@
//!	    @brief�@�����̃G�b�W�����ƃ|���S�������𓯎��ɍs���isplitEdge()��splitPolygon()���J��Ԃ��������I�j�j�@PolyHall()���T�|�[�g
//!		@param[in] polygonIDs �ΏۂƂ���|���S���ԍ�
//!		@param[in] spritPositionsList �����G�b�W�̒��_���X�g�@�e�v�f��[0]�������G�b�W�̃X�^�[�g/[1]���G���h�@�Е����I���G�b�W�E�I���|�C���g�łȂ��ꍇ�̓X�L�b�v�@�������I���G�b�W�E�I���|�C���g�łȂ��ꍇ��PolyHall()�����s
//!		@return �V�K�ǉ������|���S���Q�̍ŏ��̃|���S���ԍ�
//!		@warning ����ł�UV��J���[���̕t���A�g���r���[�g�͖��Ή�
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
//!	    �����̃G�b�W�����ƃ|���S�������𓯎��ɍs���isplitEdge()��splitPolygon()���J��Ԃ��������I�j�j<br>�@
//!	    @brief�@�����̃G�b�W�����ƃ|���S�������𓯎��ɍs���isplitEdge()��splitPolygon()���J��Ԃ��������I�j�j
//!		@param[in] polygonIDs �ΏۂƂ���|���S���ԍ�
//!		@param[in] pointIDs1 �����Ώ̂̃G�b�W�̃X�^�[�g�|�C���g�i���_�ԍ��j 
//!		@param[in] ratio1 �����ʒu���w�肷��G�b�W��̃��V�I(0.0~1.0)�@�|���S�������̃X�^�[�g�ʒu
//!		@param[in] pointIDs2 �����Ώ̂̃G�b�W�̃X�^�[�g�|�C���g�i���_�ԍ��j
//!		@param[in] ratio2 �����ʒu���w�肷��G�b�W��̃��V�I(0.0~1.0)�@�|���S�������̃G���h�ʒu
//!		@param[in] addPointPositionsList �|���S�������̃X�^�[�g�`�G���h�̊Ԃɒ��_�����ޏꍇ�̒��_�ʒu
//!		@return �V�K�ǉ������|���S���Q�̍ŏ��̃|���S���ԍ�
//!		@warning ����ł�UV��J���[���̕t���A�g���r���[�g�͖��Ή�
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

    // �G�b�W�����p�̏��i�[�ꏊ
    std::vector<u32> edgeIDs;
    std::vector<f32> ratios;
    edgeIDs.reserve( cnt * 2 );
    ratios.reserve( cnt * 2 );

    // �|���S�������p�̏��i�[�ꏊ
    std::vector<Bool> rev;
    std::vector<u32> validIndex;

    // �G�b�W�����p�̏��𐮗�����
    u32 c = u32( cnt );
    for ( u32 i = 0; i < c; i++ ) {
        u32 polyID = polygonIDs[i];

        // �����̃G�b�W���������m�F����
        // �����ʒu�̃��V�I��1.0�̏ꍇ�A�|���S�����̎��̒��_�ԍ��ɒu���ς��ă��V�I�l��0.0�ɂ���
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
        // �����̒ǉ����_�̓��͂������ꍇ
        if ( addPointPositionsList[i].size() == 0 ) {
            // �X�ɃG�b�W���������G�b�W��̏ꍇ�́A�|���S���̕������o���Ȃ��׃X�L�b�v
            if ( poiID1 == poiID2 ) {
                continue;
            }

            // ���V�I��0.0�̏ꍇ�A��O�̃G�b�WID���Е��̃G�b�W���������̃G�b�WID�ƈꏏ���m�F����
            // ��v�����ꍇ�͓���G�b�W�𕪊����邱�ƂɂȂ�A�|���S���̕������o���Ȃ��׃X�L�b�v
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

        // p1/p2�𑖍����Ăǂ��炪�X�^�[�g/�G���h�ɂȂ邩���f����
        // p2�̕����X�^�[�g�ɂȂ�ꍇ�́A�|���S�������̍ۂ�addPoiPos�̔z��v�f���t���Ŏg�p����Ђ悤�����邽��rev[]��true�ɂ���
        if ( poiID1 == poiID2 ) {
            u32 edgeID = this->getPolygonPointEdgeID( polyID, poiID1 );
            edgeIDs.push_back( edgeID );
            edgeIDs.push_back( edgeID );
            // �G�b�W���������G�b�W��̏ꍇ�A���V�I�̒l���r����
            // p2�̕����X�^�[�g�ɂȂ�ꍇ�́AaddPoiPos�̔z�񏇂��t�]����
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
            // �G�b�W�������ʃG�b�W�̏ꍇ�A�|���S�����̃��[�J���C���f�b�N�X���r����
            // p2�̕����X�^�[�g�ɂȂ�ꍇ�́AaddPoiPos�̔z�񏇂��t�]����
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
    // �G�b�W�������s���A�V���b�V���g�|���W�[�f�[�^���쐬����
    std::vector<u32> packedPointIDs;
    std::vector<u32> polygonSizes;
    std::vector<Vec3T<T>> addPoiPos;
    u32 addCnt = this->splitEdgeCore( packedPointIDs, polygonSizes, addPoiPos, edgeIDs, ratios, ratio_tolerance, pos_tolerance );
    if ( addCnt == BURGESS2::U32_INFINITE ) { return BURGESS2::U32_INFINITE; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // �|���S�������p�Ƀf�[�^����

    // �G�b�W�����̒��_�ǉ��������A��ɓK�p���Ă���
    this->addPoints( addPoiPos );
    // packedPointIDs��3�c�z��ɕϊ����� [�|���S����][�|���S��������][�|���S���̉搔]�@
    // �����ō��f�[�^�ł́A[�|���S���̕�����]�̔z�񐔂͂��ׂ�1��
    std::vector<std::vector<std::vector<u32>>> newPolyPointIDList( polygonSizes.size()-1 );
    for ( u32 i = 0; i < polygonSizes.size()-1; i++ ) {
        newPolyPointIDList[i].resize( 1 );
        std::vector<u32> rest(0);
        for ( u32 j = polygonSizes[i]; j < polygonSizes[i + 1]; j++ ) {
            newPolyPointIDList[i][0].push_back( packedPointIDs[j] );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // �|���S���������s���A�V���b�V���g�|���W�[�f�[�^���X�V����

    // �����̃X�^�[�g/�G���h�̃|�C���gID��V�g�|���W�[�f�[�^����擾����
    std::vector<Vec2u> sePoiIDs( validIndex.size() );
    Bool end = false;
    #pragma omp parallel for 
    for ( s32 i = 0; i < edgeIDs.size(); i++ ) {
        u32 id = u32( i / 2 );
        u32 xyID = u32( i - ( id * 2 ) );

        // �G�b�W�����ʒu���擾���ē����ʒu�̃|�C���gID����������
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

    // newPolyPointIDList��addPoiPos���X�V����
    u32 newOrigPoiCount = origPoiCnt + u32(addPoiPos.size());
    addPoiPos.resize( 0 );
    for ( u32 i = 0; i < validIndex.size(); i++ ) {
        u32 polyID = polygonIDs[validIndex[i]];

        // �X�^�[�g/�G���h�̂Q�̃|�C���gID���܂܂�Ă��郊�X�g��T��
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
        // ���X�g��������Ȃ��ꍇ�̓X�L�b�v
        if ( targetListID == -1 ) { continue; }

        // �ǉ����_��addPoiPos�ɃZ�b�g����
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

        // �V����PolyPointIDList���Q���
        //  ��F[0,8,1,7,2,3]�̃|���S���|�C���gID���X�g�@�E �����X�^�[�g/�G���h�̃|�C���gID��[8,2]�@�E �ǉ����_��2��(�|�C���gID��20,21)�@�̏����̏ꍇ
        //      resetList = [0,8,20,21,2,3]
        //      newList   = [8,1,7,2,21,20]
        std::vector<u32> resetList; // targetListID�̃��X�g��u�������邽�߂̃��X�g
        std::vector<u32> newList; // �V��������|���S���p�̃��X�g
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
                // ��L��� "20,21" �̕��������
                for ( u32 k = 0; k < addPoiCnt; k++ ) {
                    resetList.push_back( newOrigPoiCount + u32(addPoiPos.size()) - addPoiCnt + k );
                }
                resetList.push_back( polyPoiID );

                // ��L��� "21,20" �̕��������
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

    // �g�|���W�[�f�[�^���쐬
    packedPointIDs.resize(0);
    polygonSizes.resize(0);
    polygonSizes.push_back( 0 );
    u32 sum = 0;
    for ( u32 i = 0; i < newPolyPointIDList.size(); i++ ) {
        // �܂��͊e�|���S����[0]�̃��X�g�������g�|���W�[�f�[�^�ɕϊ�����
        for ( u32 j = 0; j < newPolyPointIDList[i][0].size(); j++ ) {
            packedPointIDs.push_back( newPolyPointIDList[i][0][j] );
        }
        polygonSizes.push_back( sum + u32(newPolyPointIDList[i][0].size()) );
        sum += u32(newPolyPointIDList[i][0].size());
    }
    for ( u32 i = 0; i < newPolyPointIDList.size(); i++ ) {
        // ���Ɋe�|���S����[1~]�̃��X�g���g�|���W�[�f�[�^�ɕϊ�����
        for ( u32 j = 1; j < newPolyPointIDList[i].size(); j++ ) {
            for ( u32 k = 0; k < newPolyPointIDList[i][j].size(); k++ ) {
                packedPointIDs.push_back( newPolyPointIDList[i][j][k] );
            }
            polygonSizes.push_back( sum + u32(newPolyPointIDList[i][j].size()) );
            sum += u32(newPolyPointIDList[i][j].size());
        }
    }

    //�@���_��ǉ�
    this->addPoints( addPoiPos );
    // �g�|���W�[���X�V
    this->resetTopology( polygonSizes, packedPointIDs, true );

    this->deleteUnusedPoints = true;

    return origPolyCnt;
}
