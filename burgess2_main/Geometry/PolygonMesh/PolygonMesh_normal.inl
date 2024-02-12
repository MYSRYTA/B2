//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �@����S�Đݒ肷��@(�z��=�o�[�e�b�N�X��)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setAllNormals ( const std::vector<Vec3T<T>> &_normals ) {
	Vec3AttributeT<T>& normalAttr = this->getNormalAttribute ();
	assert ( normalAttr.size () == _normals.size () );
	normalAttr.setValues( _normals );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�@����S�Đݒ肷��@(�z��=���_��)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setAllPointNormals ( const std::vector<Vec3T<T>> &_normals ) {
	assert ( _normals.size () == this->pointCount () );
	#pragma omp parallel for
	for ( s32 i = 0; i < this->pointCount (); i++ ) {
		this->setPointNormal ( i, _normals [i] );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �@����ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setNormal ( u32 vertexID, const Vec3T<T> &_normal ){
	Vec3AttributeT<T>& normalAttr = this->getNormalAttribute ();
	assert ( vertexID < normalAttr.size () );
	normalAttr[vertexID] = _normal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�@����ݒ肷��
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::setPointNormal ( u32 pointID, const Vec3T<T> _normal ) {
	assert ( pointID < this->pointCount () );
	Vec3AttributeT<T>& normalAttr = this->getNormalAttribute ();
	L16Array<u32> poiVerIDs = this->getSharedVertexIDs ( pointID );
	for ( u32 i = 0; i<poiVerIDs.size (); i++ ) {
		normalAttr [poiVerIDs [i]] = _normal;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �@����S�Ď擾����@(�z��=�o�[�e�b�N�X��)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec3T<T>> PolygonMeshT<T>::getAllNormals ()const {
	Vec3AttributeT<T>& normalAttr = this->getNormalAttribute ();
	return normalAttr.getValues ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �@�����擾����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> PolygonMeshT<T>::getNormal ( u32 vertexID )const {
	Vec3AttributeT<T>& normalAttr = this->getNormalAttribute ();
	assert ( vertexID < normalAttr.size () );
	return normalAttr[vertexID];
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�@�����擾���� (�擾�����@���̃o�[�e�b�N�XID��getSharedVertexIDs()�Ƒ΂ɂȂ�)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
L16Array<Vec3T<T>> PolygonMeshT<T>::getPointNormal ( u32 pointID )const {
	assert ( pointID < this->pointCount () );
	assert ( this->hasNormal () );
	L16Array<u32> poiVerIDs = this->getSharedVertexIDs ( pointID );
	L16Array<Vec3T<T>> result ( poiVerIDs.size () );
	for ( u32 i = 0; i<poiVerIDs.size (); i++ ) {
		result [i] = this->getNormal ( poiVerIDs [i] );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�Ă̒��_�@�����擾���� (�o�[�e�b�N�X�@���̕��ϒl��Ԃ�)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::vector<Vec3T<T>> PolygonMeshT<T>::getAllPointAverageNormals ()const {
	u32 cnt = this->pointCount ();
	std::vector<Vec3T<T>> result ( cnt );
	#pragma omp parallel for
	for ( s32 i = 0; i < cnt; i++ ) {
		result[i] = this->getPointAverageNormal ( i );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ���_�@�����擾���� (�o�[�e�b�N�X�@���̕��ϒl��Ԃ�)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> PolygonMeshT<T>::getPointAverageNormal ( u32 pointID )const{
	assert ( this->hasNormal () );
	L16Array<Vec3T<T>> normals = this->getPointNormal ( pointID );
	u32 s = normals.size ();
	Vec3T<T> result;
	for ( u32 i = 0; i < s; ++i ){ 
		result += normals[i]; 
	}
	assert ( Burgess2MathFn::IsZero ( result.lengthSquared () ) == false );
	return result.unit();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �|���S���@�����Z�o����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> PolygonMeshT<T>::getPolygonNormal ( u32 polygonID )const {
	assert ( polygonID < this->polygonCount () );
	assert ( this->hasNormal () );
	L8Array<u32> polyPoiIDs = this->getPolygonPointIDs ( polygonID );
	if ( polyPoiIDs.size () == 4 ) {
		Vec3T<T> seg0 = this->getPointPosition ( polyPoiIDs [2] ) - this->getPointPosition ( polyPoiIDs [0] );
		Vec3T<T> seg1 = this->getPointPosition ( polyPoiIDs [3] ) - this->getPointPosition ( polyPoiIDs [1] );
		return seg0.cross ( seg1 ).unit ();
	} else {
		Vec3T<T> pos0 = this->getPointPosition ( polyPoiIDs [0] );
		Vec3T<T> prevSeg = this->getPointPosition ( polyPoiIDs [1] ) - pos0;
		Vec3T<T> sum ( T(0.0) );
		for ( u32 i = 2; i < polyPoiIDs.size(); i++ ) {
			Vec3T<T> seg = this->getPointPosition ( polyPoiIDs [i] ) - pos0;
			sum += prevSeg.cross ( seg );
			prevSeg = seg;
		}
		return  sum.unit ();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�Ă̒��_�@�����Čv�Z�i�X���[�X���j����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::recomputePointNormals () {
	Vec3AttributeT<T>& normalAttr = this->getNormalAttribute ();
	normalAttr.resize ( this->vertexCount () );

	/* �e�|���S���̖@�����v�Z���A�|���S�����\������S�Ẵo�[�e�b�N�X�ɐݒ肷�� */
	s32 polyCnt = s32(this->polygonCount());
	#pragma omp parallel for
	for ( s32 i = 0; i < polyCnt; i++ ) {
		Vec3T<T> pN = this->getPolygonNormal ( i );
		u32 start = this->getPolygonFirstVertexID ( i );
		for ( u32 k = 0; k < this->getPolygonSize ( i ); k++ ) {
			normalAttr [start + k] = pN;
		}
	}

	/* ���_���Ƃɋ��L�o�[�e�b�N�X�̖@���𕽋ϒl���� */
	s32 poiCnt = s32(this->pointCount());
	#pragma omp parallel for
	for ( s32 i = 0; i < poiCnt; i++ ) {
		L16Array<u32> pVer = this->getSharedVertexIDs ( i );
		Vec3T<T> newNormal;
		for ( u32 k = 0; k < pVer.size (); k++ ) {
			newNormal += normalAttr [pVer [k]];
		}
		newNormal.setNormal ();
		for ( u32 k = 0; k < pVer.size (); k++ ) {
			normalAttr [pVer [k]] = newNormal;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�Ă̒��_�@����V�K�쐬����
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::createPointNormals (){
	this->recomputePointNormals ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �@���A�g���r���[�g������ɐݒ肳��Ă���ꍇ��true��Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Bool PolygonMeshT<T>::hasNormal ()const{
	Vec3AttributeT<T>& normalAttr = this->getNormalAttribute ();
	return normalAttr.size () == this->vertexCount ();
}