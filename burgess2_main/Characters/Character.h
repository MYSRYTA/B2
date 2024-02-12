#pragma once
#include "Skeleton.h"
#include <burgess2_main/Geometry.h>

//******************************************************************************
//! @file	Character.h
//! @brief	 キャラクター 
//******************************************************************************
/** @defgroup Characters
*　@{
*/
/**		@brief	キャラクター <br>
*			Skeleton + PolygonMesh(SkinningAttribute)のセットでスキンデフォームを行うクラス
*			alias : \b Character = CharacterT<f32> // \b Characterd = CharacterT<f64> <br>
*			#include <burgess2_main/Characters.h> <br>
*/

/*template<typename T>
class CharacterT{
	std::vector<PolygonMeshT<T>> meshes;
	SkeletonT<T> skeleton;
	
	SRTT<T> globalSRT;
	PoseT<T> pose;
	Bool poseDirty;

	std::vector<Mat4T<T>> getSkinningMatrices ()const;

public:
	String name;

	CharacterT () {};
	CharacterT ( const String &_name ) : name(_name){};
	virtual ~CharacterT (){}

	Bool setSkeletonAndSkinnedMeshes ( const SkeletonT<T> &_skeleton, const PolygonMeshT<T> &_meshes );

	void setPose ( const SRTT<T> &newPose );
	void setNeutralPose ();

	std::vector<PolygonMeshT<T>> getDeformedMeshes ();
};

#include "Character.inl"*/
/** @} */
//using Character = CharacterT<f32>;
//using Characterd = CharacterT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************