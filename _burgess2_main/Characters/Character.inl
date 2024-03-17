//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template <typename T>
std::vector<Mat4T<T>> CharacterT<T>::getSkinningMatrices ()const{
	
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	   シンメトリーマップを作る
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool CharacterT<T>::setSkeletonAndSkinnedMeshes ( const SkeletonT<T> &_skeleton, const PolygonMeshT<T> &_meshes ){
	u16 boneCnt = _skeleton.boneCount ();
	for( u32 i=0; i<_meshes.size(); ++i ){
		if(_meshes[i].hasSkinningAttribute () != false ){ return false; }
		SkinningAttributeT<T> &skinAttr = _meshes[i].getSkinningAttribute ();
		u16 skinPartsMaxID = skinAttr.partsMaxID ();
		if ( boneCnt <= skinPartsMaxID ){ return false; }
	}
	this->meshes = _meshes;
	this->skeleton = _skeleton;
	return true;
}*/