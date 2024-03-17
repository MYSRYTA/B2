#pragma once
#include <burgess2_main/Math.h>

//******************************************************************************
//! @file	Bone.h
//! @brief	 ボーン
//******************************************************************************
/** @defgroup Characters
*　@{
*/
/**		@brief	ボーン <br>
*			#include <burgess2_main/Characters.h> <br><br>
*			#alias <br> \b : Bone <br> \b : Boned <br>
*/

template<typename T>
class BoneT : XfoT<T>{
	s32 parentID;
	std::vector<u32> childIDs;
	s32 mirrorID;
	XfoT<T> neutralPose; /* Local Xfo */
	XfoT<T> gNeutralPose;/* Global Xfo */
	Mat4T<T> invNeutralPose;
public:
	String name;

	BoneT ();
	virtual ~BoneT(){}

	void set ( const String &name, const XfoT<T> &globalNeutralPose, s32 parentBoneID = s32 ( -1 ) );
	void setChildBoneIDs ( const std::vector<u32> childBoneIDs );
	void pushChildBoneID ( u32 childBoneID );
	void setMirrorBoneID ( u32 mirrorBoneID );
	void setLocalNeutralPose ( const XfoT<T> &localNeutralPose );

	std::vector<u32> getChildBoneIDs ( )const;
	u32 getMirrorBoneID ( )const;
	s32 getParentBoneID ()const;
	XfoT<T> getLocalNeutralPose ( )const;
	Mat4T<T> getInvLocalNeutralPose ()const;
	XfoT<T> getGlobalNeutralPose ()const;
	Mat4T<T> getInvGlobalNeutralPose ()const;

	Bool hasMirrorBone ()const;

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( parentID ),
			CEREAL_NVP ( childIDs ),
			CEREAL_NVP ( mirrorID ),
			CEREAL_NVP ( neutralPose ),
			CEREAL_NVP ( gNeutralPose ),
			CEREAL_NVP ( invNeutralPose ),
			CEREAL_NVP ( name )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Bone.inl"
/** @} */
using Bone = BoneT<f32>;
using Boned = BoneT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************