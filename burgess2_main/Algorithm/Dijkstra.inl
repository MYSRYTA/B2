//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ���b�V������_�C�N�X�g���O���t���\������
//!	    @brief	���b�V������_�C�N�X�g���O���t���\������
//!		@param[in] mesh
//!		@param[in] includeAllPolygonPoints �|���S�����̃G�b�W�ŗאڂ��Ȃ��Ίp�|�C���g���܂߂�ꍇ��true
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void DijkstraT<T>::setFromPolygonMesh ( const PolygonMeshT<T> &mesh, Bool includeAllPolygonPoints ) {
	if ( mesh.polygonCount () < 2 ) { return; }

	this->node.resize ( mesh.pointCount () );
	#pragma omp parallel for
	for ( u32 i = 0, n = mesh.pointCount (); i<n; i++ ) {
		//	�|�C���g����G�b�W�łȂ������ׂ̃|�C���gID���X�g���擾
		//	includeAllPolygonPoints�̓|���S�����̑Ίp�|�C���g���܂߂邩�ۂ�
		this->node [i].surroundingNodeID = mesh.getPointSurroundingPoints ( i, includeAllPolygonPoints );
		//	�擾�����e�|�C���g�ւ̋������i�[���Ă���
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
//!	    �X�^�[�g����e�m�[�h�ւ̈ړ��R�X�g���Z�o���ĕϐ��ɕۑ�����
//!	    @brief	�X�^�[�g����e�m�[�h�ւ̈ړ��R�X�g���Z�o���ĕϐ��ɕۑ�����
//!		@param[in] startNodeIDs �X�^�[�g�n�_�Ƃ���m�[�hID
//!		@param[in] maxDistance �R�X�g���Z�o����ő勗��
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void DijkstraT<T>::recomputeRouteCost ( std::vector<u32> startNodeIDs, T maxDistance ) {
	if ( maxDistance <= 0.0 || this->initialized == false ) { this->computed = false;  return; }

	//  ���ꂼ��̃X�^�[�g�n�_����R�X�g�v�Z���s���Ă����A�R�X�g���������Ȃ�ꍇ�̂݃R�X�g�̍X�V���s��
	for ( u32 h = 0, hn=startNodeIDs.size (); h < hn;  ++h ) {
		//  �X�^�[�g�n�_����������ꍇ�p
		//  �ʂ̃X�^�[�g�n�_����̌v�Z���s���ۂɁA�K��ς݂̔�������Z�b�g����
		for ( u32 g = 0, gn=this->node.size (); g<gn; ++g ) {
			this->node [g].done = false;
		}
		//	�X�^�[�g�m�[�h�̈ړ��R�X�g���O�ɂ���
		this->node [startNodeIDs [h]].cost = 0;
		// �o�H��������܂Ń��[�v����
		while ( true ) {
			// �������s���m�[�h���i�[�i���ꂪ������Ȃ���ΏI���j
			DijkstraNodeT<T> processVertex;
			for ( u32 i = 0, n=this->node.size (); i<n; ++i ) {
				// �K��ς� or �܂��R�X�g�����ݒ�̏ꍇ�̓X�L�b�v
				// �ŏ��̓X�^�[�g�m�[�h�������R�X�g `0` �A����ȊO�͑S�Ă��h���K��+�R�X�g-1�h�ɂȂ��Ă��邽�߁Ai=�X�^�[�g�m�[�h�̏ꍇ�������֐i��
				if ( this->node [i].done || this->node [i].cost<0 ) {
					continue;
				}
				// �m�[�h�����I���ł���Ό��݂̃m�[�h��I�����ď����ւƐi��
				if ( processVertex.assign == false ) {
					processVertex = this->node [i];
					continue;
				}
				// �K��ς݁i�m��ς݁j�łȂ��m�[�h�̂����A��ԏ������R�X�g�̃m�[�h��T��
				if ( this->node [i].cost < processVertex.cost ) {
					processVertex = this->node [i];
				}
			}
			// �������̃m�[�h���Ȃ��Ȃ�����(�S�ă`�F�b�N���I�������)���[�v�I��
			if ( processVertex.assign == false ) {
				break;
			}
			// �������m�[�h�Ɂu�K��ς݁v�̃t���O��ݒ肷��
			// �i���m��m�[�h�̂����A��ԃR�X�g���������ꍇ�͂����ɂ�����܂ł̌o�H���v�Z���ꂽ���ʂȂ̂Łu�m��v�ł���
			processVertex.done = true;

			// �R�X�g�̃A�b�v�f�[�g
			// �I�����ꂽ�m�[�h�iprocessNode�j�̌��݂̃R�X�g�Ɛڑ�����Ă���m�[�h�̃R�X�g�𑫂��A�����ڑ���̃m�[�h�ɐݒ肳��Ă���R�X�g�Ɣ�r����
			// ������������΂��̒l�ɃA�b�v�f�[�g����
			for ( u32 i = 0, n = processVertex.surroundingNodeID.size(); i<n; ++i ) {
				DijkstraNodeT<T> restNode;
				T restCost;
				restNode = this->node [processVertex.surroundingNodeID [i]];
				restCost = processVertex.cost + processVertex.surroundingNodeCost [i];
				// �R�X�g�����ݒ� or �R�X�g�̏��Ȃ��o�H������ꍇ�̓A�b�v�f�[�g
				if ( restNode.cost < 0 || restCost < restNode.cost ) {
					//	�R�X�g�̍��v�l��maxDistance���̏ꍇ�̂ݒl���X�V
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
//!	     �X�^�[�g�m�[�h����e�m�[�h�ւ̈ړ��R�X�g���擾����
//!	    @brief	�X�^�[�g�m�[�h����e�m�[�h�ւ̈ړ��R�X�g���擾����
//!		@param[in] returnValueType�@0=�������擾(�ő勗���ȍ~�̃m�[�h��-1.0), 1=�X�^�[�g�`�ő勗����1.0~0.0�Ƃ������V�I���擾
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