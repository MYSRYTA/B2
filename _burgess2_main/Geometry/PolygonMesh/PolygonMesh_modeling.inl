//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �g�|���W�[���č\�z����i�A�g���r���[�g�̍\�z���s�����Ă��鎞�p�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::rebuildTopology(){
	this->beginStructureChanges();
	this->resetTopology( this->getU32Attribute( "polygonSizes" ).getValues(), this->getU32Attribute( "packedPointIDs" ).getValues(), true );
	this->endStructureChanges();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �g�|���W�[�̕ҏW���[�h�ɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::beginStructureChanges () {
	this->modelingMode = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �g�|���W�[�̕ҏW���[�h���I������
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::endStructureChanges () {

	// delete polygon�����s����
	this->deletePolygonTask ();

	// ���g�p�̒��_���폜����
	this->deleteUnusedPointsTask();

	// sharedVertexIDs�A�g���r���[�g���X�V
	this->resetSharedPointAttr ();

	// �ҏW���[�h���I��
	this->modelingMode = false;

	// ���_�ɗאڂ��钸�_ID���X�g���ݒ肳��Ă���ꍇ�͍X�V
	if (this->hasAttribute("surroundingPointIDs")) {
		this->updateSurroundingPointIDsAttribute();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    sharedVertexIDs�A�g���r���[�g���X�V����
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::resetSharedPointAttr () {
	//!!!!!!!!!!!!!!!!!!!!!! Boolean���s����deleteUnusedPointsTask��L���ɂ���ƃN���b�V���͖����Ȃ邪�A�H�Ƀ|���S���������Ⴎ����ɂȂ�o�O���m�F //!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!! Boolean�ɂ��܂��H�Ƀo�O���c���Ă���ׁA�����؂蕪���̂��߂ɐV�����͂܂����� //!!!!!!!!!!!!!!!!!!!!!!
	if ( this->modelingMode == false ) { return; }

	s32 poiCnt = s32( this->pointCount() );
	L16ArrayU32AttributeT<T> &sharedIDAttr = this->getL16ArrayU32Attribute ( "sharedVertexIDs" );

	if ( sharedIDAttr.isDirty () == true ) {
		// ���_���\�����Ă���o�[�e�b�N�X�ԍ����i�[����A�g���r���[�g�Ƀo�[�e�b�N�X�ԍ���o�^����
		U32AttributeT<T>& poiIDAttr = this->getU32Attribute ( "packedPointIDs" );
		u32 cnt = poiIDAttr.size();
		sharedIDAttr.clear();
		sharedIDAttr.resize( poiCnt );
		for ( u32 i = 0; i< cnt; ++i ) {
			sharedIDAttr[poiIDAttr [i]].push ( i ); // ���_�ԍ��Ƀo�[�e�b�N�X�ԍ���ǉ����Ă���
		}
		
		// �o�[�e�b�N�X����������|���S����ID�̃��X�g���X�V����
		U32AttributeT<T>& polySizeAttr = this->getU32Attribute( "polygonSizes" );
		U32AttributeT<T>& vertexPolygonAttr = this->getU32Attribute( "vertexPolygonID" );
		vertexPolygonAttr.resize( poiIDAttr.size() );
		#pragma omp parallel for
		for ( s32 i = 0; i < polySizeAttr.size() - 1; ++i ){
			s32 verID = polySizeAttr[i];
			s32 jCnt = polySizeAttr[i + 1] - polySizeAttr[i];
			for ( s32 j = 0; j < jCnt; ++j ){
				vertexPolygonAttr[verID + j] = i;
			}
		}

		// �o�[�e�b�N�X������菇�ɐ�������
		#pragma omp parallel for
		for( s32 i=0; i< poiCnt; ++i ){
			L16Array<u32>& idList = sharedIDAttr[i];
			if ( idList.size() == 0 ){ continue; }

			std::vector<Vec2u> neighberPoiIDs( idList.size() ); // x=�o�[�e�b�N�X�ō\������|���S������1�O�̃|�C���g�ԍ��Ay=���̃|�C���g�ԍ�
			for(s32 j = 0; j < idList.size(); ++j){
				u32 vPolyID = vertexPolygonAttr[idList[j]];
				if(polySizeAttr[vPolyID + 1] - idList[j] == 1){ // �|���S���̃��[�J��ID�̍Ōゾ�����ꍇ
					neighberPoiIDs[j].x() = poiIDAttr[idList[j] - 1];
					neighberPoiIDs[j].y() = poiIDAttr[idList[j] + 1 - this->getPolygonSize( vPolyID )];
				} else if(polySizeAttr[vPolyID] - idList[j] == 0){ // �|���S���̃��[�J��ID�̍ŏ��������ꍇ
					neighberPoiIDs[j].x() = poiIDAttr[idList[j] + this->getPolygonSize( vPolyID ) - 1];
					neighberPoiIDs[j].y() = poiIDAttr[idList[j] + 1];
				} else{
					neighberPoiIDs[j].x() = poiIDAttr[idList[j] - 1];
					neighberPoiIDs[j].y() = poiIDAttr[idList[j] + 1];
				}
			}

			std::vector<u32> restIDs;
			restIDs.reserve( idList.size() );
			restIDs.push_back( idList[0] );
			u32 prevID = neighberPoiIDs[0].x();
			u32 nextID = neighberPoiIDs[0].y();
			neighberPoiIDs[0] = Vec2u( BURGESS2::U32_INFINITE );
			u32 cnt = 1, startID = 0;
			Bool skipL = false;
			while(true){
				// �S�Ẵo�[�e�b�N�X���V�������X�g�ɓo�^��������ΏI��
				if( idList.size() <= cnt ){ break; }

				Bool find = false;

				if(skipL == false){
					// �����Ńo�[�e�b�N�XID���������Ă���
					for(u32 j = 1; j < neighberPoiIDs.size(); ++j){
						if(prevID == neighberPoiIDs[j].y()){
							restIDs.push_back( idList[j] );
							prevID = neighberPoiIDs[j].x();
							neighberPoiIDs[j] = Vec2u( BURGESS2::U32_INFINITE );
							find = true;
							cnt++;
							break;
						}
					}
					if(find){ continue; }
					skipL = true;
				}

				// �����Ńo�[�e�b�N�X��������Ȃ������ꍇ�ArestIDs���E���Ō�������
				for(u32 j = 0; j < idList.size(); ++j){
					if(nextID == neighberPoiIDs[j].x()){
						auto it = restIDs.begin();
						for(u32 k = 0; k < startID; ++k){ ++it; }
						restIDs.insert( it, idList[j] );
						nextID = neighberPoiIDs[j].y();
						neighberPoiIDs[j] = Vec2u( BURGESS2::U32_INFINITE );
						find = true;
						cnt++;
						break;
					}
				}
				if(find){ continue; }

				// �܂���������ID������i=�{�[�_�[�G�b�W�Ŋu�Ă��|���S��������j�ꍇ
				for(u32 j = 0; j < idList.size(); ++j){
					if(neighberPoiIDs[j].x() < this->pointCount()){
						restIDs.push_back( idList[j] );
						prevID = neighberPoiIDs[j].x();
						nextID = neighberPoiIDs[j].y();
						skipL = false;
						cnt++;
						break;
					}
				}
				cnt++;
			}
			sharedIDAttr[i] = L16Array<u32>( restIDs );
		}
		sharedIDAttr.setDirty ( false );
	}
	
	// ��
	/*if ( this->modelingMode == false ) { return; }
	L16ArrayU32AttributeT<T> &sharedIDAttr = this->getL16ArrayU32Attribute ( "sharedVertexIDs" );
	if ( sharedIDAttr.isDirty () == true ) {
		s32 poiCnt = s32(this->pointCount());

		std::vector<L16Array<u32>> newsharedVerIDs (poiCnt);
		U32AttributeT<T>& poiIDAttr = this->getU32Attribute ( "packedPointIDs" );
		u32 cnt = poiIDAttr.size();
		for ( u32 i = 0; i< cnt; ++i ) {
			newsharedVerIDs[poiIDAttr [i]].push ( i );
		}
		sharedIDAttr.resetValues ( newsharedVerIDs );
	
		// �o�[�e�b�N�X������菇�ɐ�������
		std::vector<L16Array<u32>> ordered (poiCnt);
		#pragma omp parallel for
		for( s32 i=0; i< poiCnt; ++i ){
			L16Array<u32>& idList = newsharedVerIDs[i];
			if (idList.size() == 0) {
				continue;
			}

			L16Array<u32> newList;
			if(this->isBorderPoint(i)){
				L16Array<u32> sVer;
				for(u32 j=0; j<idList.size(); ++j ){
					if ( this->isPolygonBorder ( idList[j] ) ){ sVer.push ( idList[j] ); }
				}
				for ( u32 j = 0; j<sVer.size (); ++j ){
					newList.push ( sVer[j] );
					for ( u32 k = 0; k < idList.size (); ++k ){
						u32 pVer = this->getPreviousPolygonVertexID ( newList[newList.size()-1] );
						if ( this->isPolygonBorder ( pVer ) ){ break; }
						newList.push ( this->getEdgeAdjacentEdgeID ( pVer ) );
					}
				}
			}else{
				newList.push ( idList[0] );
				for(u32 j=1; j<idList.size(); ++j ){
					u32 pVer = this->getPreviousPolygonVertexID ( newList[j - 1] ); // ��
					u32 iadjEdgeID = this->getEdgeAdjacentEdgeID ( pVer ); // X
					assert ( iadjEdgeID != BURGESS2::U32_INFINITE );
					newList.push( iadjEdgeID );
				}
			}
			ordered[i] = newList;
		}
		sharedIDAttr.resetValues ( ordered );
		sharedIDAttr.setDirty ( false );
	}*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    sharedVertexIDs�A�g���r���[�g�̍X�V�𑣂���Ԃɂ���
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setSharedPointAttrDirty () {
	L16ArrayU32AttributeT<T> &ids = this->getL16ArrayU32Attribute ( "sharedVertexIDs" );
	ids.setDirty ( true );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_���(���A�ʒu)���Đݒ肷�� (createPoints()�Ɠ���)
//!	    @brief ���_���(���A�ʒu)���Đݒ肷�� (createPoints()�Ɠ���)
//!		@param[in] points�@���_�z��
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::resetPoints ( const std::vector<Vec3T<T>> &points ) {
	if ( this->modelingMode == false ) { return; }
	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	poiPosAttr.resetValues ( points );
	poiPosAttr.setValid ( true );
	this->setSharedPointAttrDirty ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S���g�|���W�[���Đݒ肷�� (createPolygons()�Ɠ���)
//!	    @brief �|���S���g�|���W�[���Đݒ肷�� (createPolygons()�Ɠ���)
//!		@param[in] polygonSizes �e�|���S���̒��_���̃��X�g
//!		@param[in] packedPointIDs �e�|���S�����\�����钸�_ID�̃��X�g
//!		@param[in] sourceIsAdditionStyle polygonSizes�����Z�����ō��ꂽ���X�g�̏ꍇ��true�ɂ���@��F[0,3,6,10,13...]�ł����true,[3,3,4,3...]�ł����false
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::resetTopology ( const std::vector<u32> &polygonSizes, const std::vector<u32> &packedPointIDs, Bool sourceIsAdditionStyle ) {
	if ( this->modelingMode == false ) { return; }

	U32AttributeT<T>& attr = this->getU32Attribute ( "polygonSizes" );
	if ( sourceIsAdditionStyle ) {
		attr.resetValues ( polygonSizes );
	} else {
		attr.resize ( u32(polygonSizes.size ()) + 1 );
		attr [0] = 0;
		for ( s32 i = 0; i<polygonSizes.size (); ++i ) {
			attr [i+1] = attr [i] + polygonSizes [i];
		}
	}
	attr.setValid ( true );

	U32AttributeT<T>& attr2 = this->getU32Attribute ( "packedPointIDs" );
	attr2.resetValues ( packedPointIDs );
	attr2.setValid ( true );

	// �o�[�e�b�N�X����������|���S����ID�̃��X�g�����
	U32AttributeT<T>& attr3 = this->attr.getOrCreateU32Attribute( "vertexPolygonID", 0 );
	attr3.resize( attr2.size() );
	#pragma omp parallel for
	for ( s32 i = 0; i < attr.size()-1; ++i ){
		s32 verID = attr[i];
		s32 jCnt = attr[i + 1] - attr[i];
		for ( s32 j = 0; j < jCnt; ++j ){
			attr3[verID+j] = i;
		}
	}
	attr3.setValid( true );

	this->setSharedPointAttrDirty ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �@�����(���A�@������)���Đݒ肷��
//!	    @brief �@�����(���A�@������)���Đݒ肷��
//!		@param[in] normals�@�@���z��
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::resetNormals ( const std::vector<Vec3T<T>> &normals ) {
	if ( this->modelingMode == false ) { return; }
	Vec3AttributeT<T>& normalAttr = this->getNormalAttribute ();
	normalAttr.resetValues ( normals );
	normalAttr.setValid ( true );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�Q���쐬����i�㏑���j
//!	    @brief ���_�Q���쐬����i�㏑���j
//!		@param[in] newPoints�@���_�z��
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::createPoints ( const std::vector<Vec3T<T>> &newPoints ){
	this->resetPoints ( newPoints );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �����̃|���S�����쐬����i�㏑���j
//!	    @brief �����̃|���S�����쐬����i�㏑���j
//!		@param[in] polygonSizes �e�|���S���̒��_���̃��X�g
//!		@param[in] packedPointIDs �e�|���S�����\�����钸�_ID�̃��X�g
//!		@param[in] sourceIsAdditionStyle polygonSizes�����Z�����ō��ꂽ���X�g�̏ꍇ��true�ɂ���<br>�@
//!			true = [0]=0�Ŏn�܂���Z���̃��X�g�@[0,3,6,10,13...]�@(Bifrost�X�^�C��)<br>
//!			false = �e�|���S���̊p�����ʂŃ��X�g���@[3,3,4,3...] (Maya�X�^�C��)
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::createPolygons ( const std::vector<u32> &polygonSizes, const std::vector<u32> &packedPointIDs, Bool sourceIsAdditionStyle ){
	this->resetTopology ( polygonSizes, packedPointIDs, sourceIsAdditionStyle );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�Q��ǉ�����
//!	    @brief ���_�Q��ǉ�����
//!		@param[in] newPoints�@���_�z��
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::addPoints ( const std::vector<Vec3T<T>> &newPoints ){
	if ( this->modelingMode == false ){ return; }
	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	u32 origSize = poiPosAttr.size ();
	u32 cnt = u32( newPoints.size() );
	poiPosAttr.resize ( origSize + cnt );
	for(u32 i=0; i< cnt; ++i ){
		poiPosAttr[origSize + i] = newPoints[i];
	}
	poiPosAttr.setValid ( true );
	this->setSharedPointAttrDirty ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �����̃|���S����ǉ�����
//!	    @brief �����̃|���S����ǉ�����
//!		@param[in] polygonSizes �ǉ�����|���S���̒��_���̃��X�g
//!		@param[in] packedPointIDs �ǉ�����|���S�����\�����钸�_ID�̃��X�g
//!		@param[in] sourceIsAdditionStyle polygonSizes�����Z�����ō��ꂽ���X�g�̏ꍇ��true�ɂ���<br>�@
//!			true = [0]=0�Ŏn�܂���Z���̃��X�g�@[0,3,6,10,13...]�@(Bifrost�X�^�C��)<br>
//!			false = �e�|���S���̊p�����ʂŃ��X�g���@[3,3,4,3...] (Maya�X�^�C��)
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::addPolygons ( const std::vector<u32> &polygonSizes, const std::vector<u32> &packedPointIDs, Bool sourceIsAdditionStyle ){
	if ( this->modelingMode == false ){ return; }
	
	U32AttributeT<T>& attr = this->getU32Attribute ( "polygonSizes" );
	u32 origSize = attr.size ();
	attr.resize ( origSize + polygonSizes.size() );
	if ( sourceIsAdditionStyle ){
		u32 origVal = attr[attr.size () - 1];
		for ( u32 i = 0; i < polygonSizes.size(); ++i ){
			attr[origSize + i] = polygonSizes[i] + origVal;
		}
	} else{
		for ( u32 i = 0; i < polygonSizes.size (); ++i ){
			attr[origSize + i] = attr[origSize + i - 1] + polygonSizes[i];
		}
	}
	attr.setValid ( true );

	U32AttributeT<T>& attr2 = this->getU32Attribute ( "packedPointIDs" );
	u32 origSize2 = attr2.size ();
	attr2.resize ( origSize2 + packedPointIDs.size() );
	for ( u32 i = 0; i < packedPointIDs.size(); ++i ){
		attr2[origSize2 + i] = packedPointIDs[i];
	}
	attr2.setValid ( true );

	this->setSharedPointAttrDirty ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S����ǉ�����i���p�`�p�j
//!	    @brief �|���S����ǉ�����i���p�`�p�j
//!		@param[in] pointIDs �e�|���S���̒��_���̃��X�g
//!		@return �ǉ������|���S����ID �i�ǉ��Ɏ��s�����ꍇ�͖����l��Ԃ��j
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::addPolygon ( const L8Array<u32> &pointIDs ){
	if ( this->modelingMode == false ){ return BURGESS2::U32_INFINITE;}
	if ( pointIDs.size() < 3 ){ return BURGESS2::U32_INFINITE; }
	
	u32 addSize = pointIDs.size ();
	for(u32 i=0; i<addSize; ++i ){
		if ( this->pointCount () <= pointIDs[i]){ return BURGESS2::U32_INFINITE; }
	}

	U32AttributeT<T>& attr = this->getU32Attribute ( "polygonSizes" );
	u32 origSize = attr.size ();
	attr.resize ( origSize + 1 );
	attr[origSize] = attr[origSize - 1] + addSize;
	attr.setValid ( true );
	
	U32AttributeT<T>& attr2 = this->getU32Attribute ( "packedPointIDs" );
	u32 origSize2 = attr2.size ();
	attr2.resize ( origSize2 + addSize );
	for ( u32 i = 0; i < addSize; ++i ){
		attr2[origSize2 + i] = pointIDs[i];
	}
	attr2.setValid ( true );
	this->setSharedPointAttrDirty ();

	return origSize-1; // �ǉ������|���S����ID "[0]=0"�̕�������*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S����ǉ�����i3�p�`�p�j
//!	    @brief �|���S����ǉ�����i3�p�`�p�j
//!		@param[in] pointID1 �|���S�����\�����钸�_ID
//!		@param[in] pointID2 �|���S�����\�����钸�_ID
//!		@param[in] pointID3 �|���S�����\�����钸�_ID
//!		@return �ǉ������|���S����ID �i�ǉ��Ɏ��s�����ꍇ�͖����l��Ԃ��j
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::addPolygon ( u32 pointID1, u32 pointID2, u32 pointID3 ){
	if ( this->modelingMode == false ){ return BURGESS2::U32_INFINITE; }
	if ( pointID1 == pointID2 || pointID1 == pointID3 || pointID2 == pointID3 ){ return BURGESS2::U32_INFINITE;}
	L8Array<u32> pointIDs ( 3 );
	pointIDs[0] = pointID1;
	pointIDs[1] = pointID2;
	pointIDs[2] = pointID3;
	return this->addPolygon ( pointIDs );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S����ǉ�����i4�p�`�p�j
//!	    @brief �|���S����ǉ�����i4�p�`�p�j
//!		@param[in] pointID1 �|���S�����\�����钸�_ID
//!		@param[in] pointID2 �|���S�����\�����钸�_ID
//!		@param[in] pointID3 �|���S�����\�����钸�_ID
//!		@param[in] pointID4 �|���S�������钸�_ID
//!		@return �ǉ������|���S����ID �i�ǉ��Ɏ��s�����ꍇ�͖����l��Ԃ��j
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::addPolygon ( u32 pointID1, u32 pointID2, u32 pointID3, u32 pointID4 ){
	if ( this->modelingMode == false ){ return BURGESS2::U32_INFINITE;}
	if ( pointID1 == pointID2 || pointID1 == pointID3 || pointID1 == pointID4 ||
		 pointID2 == pointID3 || pointID2 == pointID4 || pointID3 == pointID4 ){ return BURGESS2::U32_INFINITE; }
	L8Array<u32> pointIDs ( 4 );
	pointIDs[0] = pointID1;
	pointIDs[1] = pointID2;
	pointIDs[2] = pointID3;
	pointIDs[3] = pointID4;
	return this->addPolygon ( pointIDs );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|�C���g�ƃ|���S����ǉ����� 
//!	    @brief �|�C���g�ƃ|���S����ǉ�����
//!		@param[in] newPoints �|���S�����\������V�K�|�C���g�Q
//!		@param[in] merge �ǉ��|�W�V�����Ɋ��Ƀ|�C���g���������ꍇ�͂��̃|�C���g�𗬗p����
//!		@param[in] tolerance merge�̔��苗��
//!		@return �ǉ������|���S����ID �i�ǉ��Ɏ��s�����ꍇ�͖����l��Ԃ��j
//!		@note  begin/endStructureChanges�̃X�R�[�v���ł̂ݗL��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 PolygonMeshT<T>::addPolygon ( const L8Array<Vec3T<T>> &newPoints, Bool merge, T tolerance ){
	if ( this->modelingMode == false ){ return BURGESS2::U32_INFINITE; }
	u32 newSize = newPoints.size ();
	if( newSize < 3){ return BURGESS2::U32_INFINITE; }

	Vec3AttributeT<T>& poiPosAttr = this->getPointPositionAttribute ();
	u32 origSize = poiPosAttr.size();
	std::vector<Vec3T<T>> newPoi;
	L8Array<u32> pointIDs;
	if(merge){
		// �}�[�W������ꍇ
		u32 poiID = origSize;
		// �ǉ�����|�C���g���ƂɃ��[�v
		for(u32 i=0; i<newSize; ++i){
			Bool find = false;
			// ���ɑ��݂���S�|�C���g�ɑ΂��č��W�������łȂ����`�F�b�N����
			for(u32 j=0; j< poiPosAttr.size(); ++j ){
				if( newPoints[i].almostEqual( poiPosAttr[j], tolerance ) ){
					// �����|�C���g������΁A�|�C���gID��ۑ����ă��[�v�I��
					pointIDs.push_back ( j );
					find = true;
					break;
				}
			}
			// �����|�C���g��������Ȃ������ꍇ�͐V�K�̃|�C���g�ʒu�ƃ|�C���gID�����X�g�ɒǉ����ďI��
			if(find == false){
				newPoi.push_back ( newPoints[i] );
				pointIDs.push_back ( poiID );
				poiID++;
			}
		}
	}else{
		// �}�[�W���Ȃ��ꍇ�͒��_�ʒu�ƒ��_�ԍ������X�g�ɂ���
		newPoi.resize ( newSize );
		pointIDs.resize ( newSize );
		for(u32 i=0; i<newSize; ++i ){
			newPoi[i] = newPoints[i];
			pointIDs[i] = origSize + i;
		}
	}
	// �|�C���g�̒ǉ��A�|���S���̒ǉ������ďI��
	this->addPoints ( newPoi );
	return this->addPolygon( pointIDs );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���b�V�����}�[�W����i���_�̃}�[�W�͍s��Ȃ��j
//!	    @brief ���b�V�����}�[�W����i���_�̃}�[�W�͍s��Ȃ��j
//!		@param[in] source �}�[�W���郁�b�V���f�[�^
//!		@param[in] creatMaterialIDAttr�@�}�[�W�������b�V���̃|���S��ID�𔻕ʂ���A�g���r���[�g�����i���ݖ��Ή��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::mergeMesh ( const PolygonMeshT<T> &source, Bool creatMaterialIDAttr ) {
	std::vector<PolygonMeshT<T>> rest ( 1 );
	rest [0] = source;
	this->mergeMeshes ( rest, creatMaterialIDAttr );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �����̃��b�V�����}�[�W����i���_�̃}�[�W�͍s��Ȃ��j
//!	    @brief �����̃��b�V�����}�[�W����i���_�̃}�[�W�͍s��Ȃ��j
//!		@param[in] source �}�[�W���郁�b�V���f�[�^�Q
//!		@param[in] creatMaterialIDAttr�@�}�[�W�������b�V���̃|���S��ID�𔻕ʂ���A�g���r���[�g�����i���ݖ��Ή��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::mergeMeshes ( const std::vector<PolygonMeshT<T>> &source, Bool creatMaterialIDAttr ) {	
	if ( source.size () == 0 ) { return; }

	/* �}�[�W���ƃ\�[�X�̃g�|���W�[�̏����i�[����*/
	/* [�}�[�W���A+=source�O�̏��A+=source�P�̏��A+=source�Q�̏��...]*/
	std::vector<u32> poiCntList ( source.size ()+1 );	poiCntList [0] = this->pointCount ();
	std::vector<u32> polyCntList ( source.size ()+1 );	 polyCntList [0] = this->polygonCount ();
	std::vector<u32> vertexCntList ( source.size ()+1 );	  vertexCntList [0] = this->vertexCount ();
	for ( u32 i = 0; i < source.size (); i++ ) {
		poiCntList[i+1] = poiCntList [i] + source [i].pointCount ();
		polyCntList [i+1] = polyCntList [i] + source [i].polygonCount ();
		vertexCntList [i+1] = vertexCntList [i] + source [i].vertexCount ();
	}

	u32 allPoiCount = poiCntList [poiCntList.size () - 1];
	u32 allPolyCount = polyCntList [polyCntList.size () - 1];
	u32 allVertexCount = vertexCntList [vertexCntList.size () - 1];

	if ( this->polygonCount () == allPolyCount ) { return; }

	/* ���_���ŊǗ����Ă���g�|���W�[�v�f���}�[�W���� */
	Vec3AttributeT<T> &poiPosAttr = this->getPointPositionAttribute ();
	poiPosAttr.resize ( allPoiCount );
	L16ArrayU32AttributeT<T> &sharedVerIDAttr = this->getL16ArrayU32Attribute ( "sharedVertexIDs" );
	sharedVerIDAttr.resize ( allPoiCount );
	#pragma omp parallel for
	for ( s32 i = poiCntList [0]; i < allPoiCount; i++ ) {
		u32 sourceID, sourcePoiID, offset;
		for ( u32 k = 0; k < poiCntList.size (); k++ ) {
			if ( poiCntList [k] <= i && i < poiCntList [k+1] ) {
				sourceID = k;
				sourcePoiID = i - poiCntList [k];
				offset = vertexCntList [k];
				break;
			}
		}
		/* ���_�ʒu�̃}�[�W */
		poiPosAttr [i] = source [sourceID].getPointPosition ( sourcePoiID );

		/* ���_�ŋ��L���Ă���o�[�e�b�N�X���}�[�W��̔ԍ��ɍX�V */
		L16Array<u32> sharedVerIDs = source [sourceID].getSharedVertexIDs ( sourcePoiID );
		sharedVerIDAttr [i].resize ( sharedVerIDs.size () );
		for ( u32 k = 0; k<sharedVerIDs.size (); k++ ) {
			sharedVerIDAttr [i].setValue ( k, sharedVerIDs[k] + offset );
		}
	}

	/* �|���S�����ŊǗ����Ă���g�|���W�[�v�f���}�[�W���� */
	U32AttributeT<T> &polySizeAttr = this->getU32Attribute ( "polygonSizes" );
	polySizeAttr.resize ( allPolyCount +1 ); /* [0] = 0 */
	#pragma omp parallel for
	for ( s32 i = polyCntList [0]; i < allPolyCount; i++ ) {
		u32 sourceID, sourcePolyID, offset;
		for ( u32 k = 0; k < polyCntList.size (); k++ ) {
			if ( polyCntList [k] <= i && i < polyCntList [k + 1] ) {
				sourceID = k;
				sourcePolyID = i - polyCntList [k];
				offset = vertexCntList [k];
				break;
			}
		}
		/* �|���S�����\�����钸�_���̏����}�[�W */
		U32AttributeT<T> &sPolySizeAttr = source [sourceID].getU32Attribute ( "polygonSizes" );
		polySizeAttr [i+1] = offset + sPolySizeAttr [sourcePolyID+1]; /* [0]���Ȃ����߂�+1 */
	}

	/* �o�[�e�b�N�X���ŊǗ����Ă���v�f���}�[�W���� */
	Vec3AttributeT<T> &normalAttr = this->getNormalAttribute ();
	normalAttr.resize ( allVertexCount );
	U32AttributeT<T> &packedPoiIDAttr = this->getU32Attribute ( "packedPointIDs" );
	packedPoiIDAttr.resize ( allVertexCount );
	#pragma omp parallel for
	for ( s32 i = vertexCntList [0]; i < allVertexCount; i++ ) {
		u32 sourceID, sourceVerID, offset;
		for ( u32 j = 0; j < vertexCntList.size (); j++ ) {
			if ( vertexCntList [j] <= i && i < vertexCntList [j + 1] ) {
				sourceID = j;
				sourceVerID = i - vertexCntList [j];
				offset = poiCntList [j];
				break;
			}
		}
		/* �@�� */
		normalAttr [i] = source [sourceID].getNormal ( sourceVerID );

		/* �e�|���S�����\�����钸�_�ԍ����}�[�W��̔ԍ��ɍX�V */
		U32AttributeT<T> &sPackedPoiIDAttr = source [sourceID].getU32Attribute ( "packedPointIDs" );
		packedPoiIDAttr [i] = sPackedPoiIDAttr [sourceVerID] + offset;
	}

	/* UV */
	/* �\�[�X���b�V���̑S�Ă�UV�}�b�v��this�ɏ������� */
	std::unordered_map<String, u32> uvMap;
	for ( u32 i = 0, n = source.size (); i<n; ++i ) {
		std::vector<String> sUVNames = source [i].getAllUVNames ();
		for ( u32 k = 0, m = sUVNames.size (); k<m; ++k ) {
			if ( this->hasUVMap ( sUVNames [k] ) == false ) { 
				this->addUVMap ( sUVNames [k] ); 
			}
		}
	}
	/* UV�����Ƃɏ����}�[�W����this�ɃZ�b�g���� */
	std::vector<String> uvNames = this->getAllUVNames ();
	for ( u32 i = 0, n = uvNames.size (); i < n; ++i ) {
		std::vector<Vec2T<T>> uv;
		uv.reserve ( allVertexCount );
		std::vector<u32> packedUVPointIDs;
		packedUVPointIDs.reserve ( allVertexCount );
		for ( u32 k = 0, m = source.size (); k<m; ++k ) {
			if ( this->hasUVMap ( uvNames [i] ) ) {
				Vec2AttributeT<T> &sourceUV = source [k].getUVAttribute ( uvNames [i] );
				for ( u32 h = 0, l = sourceUV.size (); h<l; h++ ) {
					uv.push_back ( sourceUV [h] );
				}
				U32AttributeT<T> &suvIDs = source [k].getPackedUVPointIDsAttribute ( uvNames [i] );
				for ( u32 h = 0, l = suvIDs.size (); h<l; h++ ) {
					packedUVPointIDs.push_back ( suvIDs [h] );
				}
			} else {
				for ( u32 h = 0, l = source [k].vertexCount (); h<l; h++ ) {
					packedUVPointIDs.push_back ( u32(0) );
				}
			}
		}
		this->resetAllUVValues ( uvNames [i], uv, packedUVPointIDs );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �g�����X�t�H�[���̍��W�Ƀ\�[�X���b�V���𕡐����ă}�[�W���� �i���_�̃}�[�W�͍s��Ȃ��j
//!	    @brief ���b�V���𕡐����ă}�[�W����i���_�̃}�[�W�͍s��Ȃ��j
//!		@param[in] source ��������\�[�X���b�V���f�[�^
//!		@param[in] Xfo ������̍��W
//!		@param[in] creatMaterialIDAttr�@�}�[�W�������b�V���̃|���S��ID�𔻕ʂ���A�g���r���[�g�����i���ݖ��Ή��j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::mergeMeshCloneOnXfo ( const PolygonMeshT<T> &source, const std::vector<XfoT<T>> &xfo, Bool creatMaterialIDAttr ) {
	if ( source.polygonCount () < 0 || xfo.size () == 0 ){ return; };
	u32 cloneCnt = xfo.size ();
	std::vector<PolygonMeshT<T>> rest ( cloneCnt );
	for ( u32 i = 0; i<cloneCnt; i++ ) {
		rest [i] = source.clone ();
		rest [i].applyTransform ( xfo [i] );
	}
	this->mergeMeshes ( rest, creatMaterialIDAttr );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S�����폜����
//!	    @brief �|���S�����폜���� 
//!		@param[in] polygonID �폜����|���S��ID
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deletePolygon(u32 polygonID, Bool keepUnusedPoints) {
	if (this->modelingMode == false) { return; }
	if (this->polygonCount() <= polygonID) { return; }

	if (this->hasAttribute("deletePolygonList") == false) {
		S32Attribute& delPolyAttr = this->getOrCreateS32Attribute("deletePolygonList");
		std::vector<s32>& attrVal = delPolyAttr.getValuesRef();
		attrVal.resize(0);
		attrVal.push_back(-1);
	}
	S32Attribute& delPolyAttr = this->getS32Attribute("deletePolygonList");
	std::vector<s32>& attrVal = delPolyAttr.getValuesRef();

	for (u32 i = 0; i < attrVal.size(); i++) {
		if (attrVal[i] == polygonID) {
			return;
		}
	}
	attrVal.push_back(s32(polygonID));
	delPolyAttr.setValid(true);

	this->deleteUnusedPoints = keepUnusedPoints == true ? false : true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S�����폜���� ( endStructureChanges()���Ŏ��s����֐� )
//!	    @brief �|���S�����폜����  ( endStructureChanges()���Ŏ��s����֐� )
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deletePolygonTask() {
	if (this->hasAttribute("deletePolygonList") == false) { return; }

	S32Attribute& delPolyAttr = this->getS32Attribute("deletePolygonList"); // �폜����|���S����ID���X�g�i[0]=-1�j
	std::vector<s32>& delPolyAttrVal = delPolyAttr.getValuesRef();
	if (delPolyAttr.isValid() == false || delPolyAttrVal.size() == 1 ) { return; }
	
	Burgess2MathFn::Sort(delPolyAttrVal);
	std::vector<s32> deletePolySizeOffset(delPolyAttrVal.size()); // �폜����o�[�e�b�N�X�̃��X�g�@[0]=0�ŃX�^�[�g���ĉ��Z��
	deletePolySizeOffset[0] = 0;
	for (u32 i = 1; i < delPolyAttrVal.size(); i++) {
		deletePolySizeOffset[i] = deletePolySizeOffset[i-1] + this->getPolygonSize(delPolyAttrVal[i]);
	}

	s32 vertexCnt = s32(this->vertexCount());
	u32 newVertexCnt = this->vertexCount() - deletePolySizeOffset.back();
	//////////////////////////////////////////////////////////////////////////////
	// �g�|���W�[�ƃA�g���r���[�g��ύX����
	
	U32Attribute& polySizeAttr = this->getU32Attribute("polygonSizes");
	U32Attribute& packedPoiIDAttr = this->getU32Attribute("packedPointIDs");

	// �폜���Ȃ��|���S���Ńg�|���W�[���X�g����蒼��
	std::vector<u32> newPolySize(this->polygonCount() - (delPolyAttr.size() - 1) + 1); newPolySize[0] = 0;
	std::vector<u32> newPackedPoiID(newVertexCnt);
	std::vector<s32> vertexOffset(vertexCnt, -1); // �I�v�V�����A�g���r���[�g�����ŏ������邽�߂̃��X�g�@�V�����o�[�e�b�N�XID�܂ł̍������i�[����
	delPolyAttrVal.push_back(this->polygonCount()); // BinarySearchTree()�Ŏg�p���邽�߂Ƀ��X�g�̍Ō���|���S�����ɂ��Ă���
	s32 polyCnt = s32(this->polygonCount());
	#pragma omp parallel for if(500 < this->polygonCount())
	for (s32 i = 1; i < polyCnt +1; i++) {
		s32 polyID = i-1;
		s32 r;
		s32 offsetID = Burgess2MathFn::BinarySearchTree(delPolyAttrVal, polyID, r);
		if (r == 0) {
			continue;
		}
		newPolySize[i - offsetID] = polySizeAttr.getValue(i) - deletePolySizeOffset[offsetID];

		L8Array<u32> vertexList = this->getPolygonVertexIDs(polyID);
		for (u32 j = 0; j < vertexList.size(); j++) {
			u32 verID = vertexList[j];
			u32 newVerID = verID - deletePolySizeOffset[offsetID];
			// packed point IDs
			newPackedPoiID[newVerID] = packedPoiIDAttr.getValue(verID);

			vertexOffset[verID] = deletePolySizeOffset[offsetID];
		}
	}

	// �g�|���W�[���X�V
	polySizeAttr.resetValues(newPolySize);
	packedPoiIDAttr.resetValues(newPackedPoiID);

	// vertex normal
	if (this->hasAttribute("normals")) {
		std::vector<Vec3T<T>> newVerNormal(newVertexCnt);
		Vec3AttributeT<T>& verNormal = this->getNormalAttribute();
		#pragma omp parallel for if(500 < vertexCnt)
		for (s32 i = 0; i < vertexCnt; i++) {
			if (vertexOffset[i] == -1) { continue; }
			u32 newVerID = i - vertexOffset[i];
			newVerNormal[newVerID] = verNormal.getValue(i);
		}
		verNormal.resetValues(newVerNormal);
	}

	// uv
	if (0 < this->uvCount()) {
		std::vector<Vec2T<T>> newvVal(newVertexCnt);
		std::vector<String> attrNames = this->getAllUVNames();
		for (u32 attrID = 0; attrID < attrNames.size(); attrID++) {
			Vec2AttributeT<T>& uvValAttr = this->uvs.getVec2Attribute(attrNames[0]);
			U32Attribute& uvIDAttr = this->uvs.getU32Attribute(attrNames[0] + "packedUVVertexIDs");

			std::vector<u32> newPackedUVID(newVertexCnt);
			#pragma omp parallel for if(500 < vertexCnt)
			for (s32 i = 0; i < vertexCnt; i++) {
				if (vertexOffset[i] == -1) { continue; }
				u32 newVerID = i - vertexOffset[i];
				newPackedUVID[newVerID] = uvIDAttr.getValue(i);
			}
			this->resetAllUVValues(attrNames[attrID], uvValAttr.getValues(), newPackedUVID);
		}
	}

	// vertex color
	if (0 < this->vertexColorMapCount()) {
		std::vector<Vec4T<T>> newvVal(newVertexCnt);
		std::vector<String> attrNames = this->getAllVertexColorNames();

		for (u32 attrID = 0; attrID < attrNames.size(); attrID++) {
			Vec4AttributeT<T>& attr = this->getVertexColorAttribute(attrNames[attrID]);
			std::vector<Vec4T<T>>& delPolyAttrVal = attr.getValuesRef();
			#pragma omp parallel for if(500 < vertexCnt)
			for (s32 i = 0; i < vertexCnt; i++) {
				if (vertexOffset[i] == -1) { continue; }
				u32 newVerID = i - vertexOffset[i];
				newvVal[newVerID] = delPolyAttrVal[i];
			}
			attr.resetValues(newvVal);
		}
	}

	// weight map
	if (0 < this->weightMapCount()) {
		std::vector<f32> newvVal(newVertexCnt);
		std::vector<String> attrNames = this->getAllWeightMapNames();

		for (u32 attrID = 0; attrID < attrNames.size(); attrID++) {
			F32Attribute& attr = this->getWeightMapAttribute(attrNames[attrID]);
			std::vector<f32>& delPolyAttrVal = attr.getValuesRef();
			#pragma omp parallel for if(500 < vertexCnt)
			for (s32 i = 0; i < vertexCnt; i++) {
				if (vertexOffset[i] == -1) { continue; }
				u32 newVerID = i - vertexOffset[i];
				newvVal[newVerID] = delPolyAttrVal[i];
			}
			attr.resetValues(newvVal);
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	// deletePolygon �̃��X�g���폜����
	this->deleteAttribute("deletePolygonList");
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���g�p�̒��_�i�|���S���Ɍq�����Ă��Ȃ����_�j���폜���� 
//!	    @brief ���g�p�̒��_�i�|���S���Ɍq�����Ă��Ȃ����_�j���폜����  
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::deleteUnusedPointsTask() {
	if (this->deleteUnusedPoints == false) {
		return;
	}

	u32 poiCnt = this->pointCount();
	s32 vertexCnt = s32(this->vertexCount());
	std::vector<Bool> unused(poiCnt, true);

	// packedPointIDs���疢�g�p�̒��_��􂢏o��
	for (s32 i = 0; i < vertexCnt; i++) {
		u32 verPoiID = this->getPointID(i);
		unused[verPoiID] = false;
	}

	// true�̐������Z�������X�g����� = �e���_�ԍ��̃I�t�Z�b�g�l���X�g
	u32 sum = 0;
	std::vector<u32> pointOffset(poiCnt);
	for (u32 i = 0; i < poiCnt; i++) {
		if (unused[i]) { sum++; }
		pointOffset[i] = sum;
	}
	if (sum == 0) { return; }

	s32 newPointCnt = s32( poiCnt - sum );
	// point position
	std::vector<Vec3T<T>> newPoiPos(newPointCnt);
	#pragma omp parallel for if(500 < newPointCnt)
	for (s32 i = 0; i < newPointCnt; i++) {
		u32 origID = i + pointOffset[i];
		newPoiPos[i] = this->getPointPosition(origID);
	}
	this->resetPoints(newPoiPos);
	
	// packedPointIDs
	U32Attribute& packedPoiIDAttr = this->getU32Attribute("packedPointIDs");
	std::vector<u32>& packedPoiIDAttrVal = packedPoiIDAttr.getValuesRef();
	#pragma omp parallel for if(500 < vertexCnt)
	for (s32 i = 0; i < vertexCnt; i++) {
		u32 verPoiID = packedPoiIDAttrVal[i];
		u32 newPoiID = verPoiID - pointOffset[verPoiID];
		packedPoiIDAttrVal[i] = newPoiID;
	}

	this->deleteUnusedPoints = false;
}