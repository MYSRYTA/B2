#pragma once
#include <burgess2_main/Math.h>
#include <burgess2_main/Containers.h>

//******************************************************************************
//! @file	IKFK.h
//! @brief	 Inverse Kinematics / Forward Kinematics
//******************************************************************************
/** @defgroup Rig
*　@{
*/
/**		@brief	IK/FK (2Bone,3Bone,4Bone,NBoneをサポート) <br> <br>
*			alias : \b IKFK = IKFKT<float> // \b IKFKd = IKFKT<double> <br>
*			#include <burgess2_toolbox/Rig/Rig.h> <br><br>
*			fkHandleの+X方向が子fkHandle<br>
*			@code
*				// 2Bone IKFK
*				//全てGlobalMatrix
*				Mat4 root;
*				Mat4 ikHandle;
*				Mat4 upvector;
*				Mat4 fkHandle(2);
*				fkHandle[0] = ......
*				fkHandle[1] = ......
*
*				IKFK fkik;
*				fkik.initialize ( Vec2 ( 3.5f, 5.0f ) ); // 2Bone type
*				fkik.setSoftness ( 0.2f );
*				std::vector<Mat4> resultIKFK = fkik.solve ( root, fkHandle, upvector, ikHandle, 0.9f );
*			@endcode
*			@code
*				// 3Bone IKFK
*				//全てGlobalMatrix
*				Mat4 root;
*				Mat4 ikHandle;
*				Mat4 upvector;
*				Mat4 fkHandle(3);
*				fkHandle[0] = ......
*				fkHandle[1] = ......
*				fkHandle[2] = ......
*				Mat4 jointNeutralPose(4);
*				jointNeutralPose[0] = ......
*				jointNeutralPose[1] = ......
*				jointNeutralPose[2] = ......
*				jointNeutralPose[3] = ...... // end joint
*
*				IKFK fkik;
*				fkik.initialize ( jointNeutralPose ); // 3Bone type
*				std::vector<Mat4> resultIKFK = fkik.solve ( root, fkHandle, upvector, ikHandle, 0.9f, 20.4f );
*			@endcode
*			@code
*				// 3BoneでNBoneタイプのIKFKを実行する場合はsolve()ではなくsolveNBone()
*				//全てGlobalMatrix
*				Mat4 root;
*				Mat4 ikHandle;
*				Mat4 upvector;
*				Mat4 fkHandle(3);
*				fkHandle[0] = ......
*				fkHandle[1] = ......
*				fkHandle[2] = ......
*
*				IKFK fkik;
*				fkik.initialize ( Vec3 ( 3.5f, 5.0f, 1.6f ) );
*				std::vector<Mat4> resultIKFK = fkik.solveNBone ( root, fkHandle, upvector, ikHandle, 0.9f );
*			@endcode
*/

template<typename T>
class IKFKT {
	u8 type{ 0 }; // 0,1=uninitialized, 2=2Bone, 3=3Bone, 4=4Bone, 5=NBone
	L8Array<T> boneLength;
	T totalLength{ 0.0 };
	T scale{ 1.0 };
	u8 upvectorType { 0 };
	Bool enableUpVector{ true };
	T softness{ 0.0 }; // 2Boneでのみ使用
	u8 directionType = 2; //0=ZY, 1=ZX, 2=XZ, 3=XY, 4=YX, 5=YZ
	bool chainDirNeg = false;
	bool upvectorDirNeg = false;
public:
	IKFKT () {}
	IKFKT( const IKFKT<T>& other );
	virtual ~IKFKT () {}

	Bool initialize ( const std::vector<T> boneLength );
	Bool initialize ( const std::vector<Mat4T<T>> boneGlobalPose );
	Bool initialize ( const Vec2T<T> &boneLength );
	Bool initialize ( const Mat4T<T> &bone0GMat4, const Mat4T<T> &bone1GMat4, const Mat4T<T> &endJointGMat4 );
	Bool initialize ( const Vec3T<T> &boneLength );
	Bool initialize ( const Mat4T<T> &bone0GMat4, const Mat4T<T> &bone1GMat4, const Mat4T<T> &bone2GMat4, const Mat4T<T> &endJointGMat4 );
	Bool initialize ( const Vec4T<T> &boneLength );
	Bool initialize ( const Mat4T<T> &bone0GMat4, const Mat4T<T> &bone1GMat4, const Mat4T<T> &bone2GMat4, const Mat4T<T> &bone3GMat4, const Mat4T<T> &endJointGMat4 );

	void setScale ( T scale );
	void setUpvectorType ( u8 type );
	void setSoftness ( T softRatio );
	void setDirectionType(u8 dirType, Bool chainNeg, Bool upvNeg);
	void useUpVector ( Bool use );

	void axisChange(IO std::vector<Mat4T<T>> &pose)const;

	u32 boneCount ()const;
	u8 getType ()const;

	IKFKT<T> clone ()const;
	void copy(const IKFKT<T>& other);

	std::vector<Mat4T<T>> solve (
		const Mat4T<T> &root,
		const std::vector<Mat4T<T>> &fkHandle,
		const Mat4T<T> &ikUpVector,
		const Mat4T<T> &ikHandle,
		T blend,
		T ikAnkleAngle = T( 0.0 )
	) const;

	std::vector<Mat4T<T>> fkSolve ( const Mat4T<T> &root, const std::vector<Mat4T<T>> &fkHandle )const;

	std::vector<Mat4T<T>> solve2Bone (
		const Mat4T<T> &root,
		const std::vector<Mat4T<T>> &fkHandle,
		const Mat4T<T> &ikUpVector,
		const Mat4T<T> &ikHandle,
		T blend 
	)const;

	std::vector<Mat4T<T>> solve3Bone (
		const Mat4T<T> &root,
		const std::vector<Mat4T<T>> &fkHandle,
		const Mat4T<T> &ikUpVector,
		const Mat4T<T> &ikHandle,
		T blend,
		T ikAnkleAngle
	)const;

	std::vector<Mat4T<T>> solve4Bone (
		const Mat4T<T> &root,
		const std::vector<Mat4T<T>> &fkHandle,
		const Mat4T<T> &ikUpVector,
		const Mat4T<T> &ikHandle,
		T blend,
		T ikAnkleAngle
	)const;

	std::vector<Mat4T<T>> solveNBone (
		const Mat4T<T> &root,
		const std::vector<Mat4T<T>> &fkHandle,
		const Mat4T<T> &ikHandle,
		T blend
	) const;

	std::vector<Mat4T<T>> solveNBone (
		const Mat4T<T> &root,
		const std::vector<Mat4T<T>> &fkHandle,
		const Mat4T<T> &ikUpVector,
		const Mat4T<T> &ikHandle,
		T blend
	) const;
};

#include "IKFK.inl"
/** @} */
using IKFK = IKFKT<f32>;
using IKFKd = IKFKT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************