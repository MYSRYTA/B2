#pragma once
#include "Bone.h"

//******************************************************************************
//! @file	Skeleton.h
//! @brief	 スケルトン
//******************************************************************************
/** @defgroup Characters
*　@{
*/
/**		@brief	スケルトン <br>
*			#include <burgess2_main/Characters.h> <br><br>
*			#alias <br> \b : Skeleton <br> \b : Skeletond <br>
*/

template<typename T>
class SkeletonT{
	std::vector<BoneT<T>> bones;
	std::vector<u32> retargetMap;

	void resetHierarchy ();
public:
	String name;

	SkeletonT () {};
	SkeletonT ( const String &name, const std::vector<BoneT<T>> &bones );
	SkeletonT ( const String &name, const std::vector<Mat4T<T>> &boneGMat4, std::vector<s32> parentBoneID );
	virtual ~SkeletonT (){}

	u32 boneCount ()const;

	BoneT<T> getBone ( u32 id )const;
	std::vector<BoneT<T>> getAllBones ()const;
	XfoT<T> getNeutralPose ( u32 boneID )const;
	XfoT<T> getGlobalNeutralPose ( u32 boneID )const;
	u32 getBoneIdByName ( const String &boneName )const;

	void resetSymmetryID ( u8 normal = 0 );

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( bones ),
			CEREAL_NVP ( retargetMap ),
			CEREAL_NVP ( name )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Skeleton.inl"
/** @} */
using Skeleton = SkeletonT<f32>;
using Skeletond = SkeletonT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************