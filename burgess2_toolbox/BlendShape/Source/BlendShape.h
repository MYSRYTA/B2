#pragma once
#include <burgess2_main/Geometry.h>
#include <burgess2_main/Containers.h>

//******************************************************************************
//! @file	BlendShape.h
//! @brief	 BlendShape
//******************************************************************************
/** @defgroup Tools
*	   @{
*/
/**		@defgroup BlendShape
*			@{
*/
/**				@brief BlendShapeTarget <br>
*				�u�����h�V�F�C�v�^�[�Q�b�g <br>
*				#include <burgess2_main/Geometry/Geometry.h><br>
*/

template<typename T>
struct BlendShapeTargetT{
	std::vector<GeometryDeltaT<T>> delta;

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( delta )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

/** @} */
/**		@defgroup BlendShape
*			@{
*/
/**			@brief BlendShape <br>
*			�u�����h�V�F�C�v <br>
*			#include <burgess2_main/Geometry/Geometry.h><br>
*			@code
*				// �x�[�X���b�V��������
*				PolugonMesh baseMesh;
*				baseMesh = ......
*				
*				// �u�����h�V�F�C�v�^�[�Q�b�g�̃��X�g������
*				std::vector<PolygonMesh> _targetMeshes( 3 );
*				_targetMeshes[0] = ......
*
*				// �e�V�F�C�v�^�[�Q�b�g�̃u�����h�l�̃��X�g������
*				std::vector<f32> _blendWeights( 3, 0.0f );
*				_blendWeights[0] = ......
*
*				// �e���]�V�F�C�v�^�[�Q�b�g�̃u�����h�l�̃��X�g������
*				std::vector<f32> _blendWeightsMirror( 3, 0.0f );
*				_blendWeightsMirror[0] = ......
*
*				BlendShape bs;
*				bs.setBaseMeshAndMirroringMap ( baseMesh, 0 ); // �x�[�X���b�V���̓o�^��X���ł̔��]�}�b�v�̍쐬
*				bs.setAllTargetMesh ( _targetMeshes ); // �V�F�C�v�^�[�Q�b�g��o�^
*				bs.setSymmetry( 2 ); // 2�Ԃ̃^�[�Q�b�g�����E�Ώ̂ɂ���
*				bs.createMirroringTargets( 0 ); // �o�^���Ă���S�Ẵ^�[�Q�b�g��X���Ŕ��]���Ĕ��]�^�[�Q�b�g�ɓo�^����
*
*				PolygonMesh result = baseMesh.clone(); // �����ΏۂƂ��郁�b�V��������
*				bs.evaluate ( result, _blendWeights, _blendWeightsMirror ); // �u�����h�V�F�C�v�����s
*
*			@endcode
*/
template<typename T>
class BlendShapeT {
protected:
	PolygonMeshT<T> baseMesh;
	std::vector<s32> mirroringPointIDMap; // �Ώ̈ʒu�ɂ��钸�_�ԍ��̃��X�g(���]�^�[�Q�b�g�����ۂɎg�p����Ώۃ}�b�v�j�@
	Bool initialized = false;
	std::vector<BlendShapeTargetT<T>> targets; // �u�����h�V�F�C�v�^�[�Q�b�g�̓o�^�ꏊ
	std::vector<BlendShapeTargetT<T>> mirroringTargets; // ��targets[]�̔��]�^�[�Q�b�g�̓o�^�ꏊ
	Bool hasNormals;
public:
	BlendShapeT () {};
	virtual ~BlendShapeT (){}

	u32 targetCount ()const;

	void setBaseMeshAndMirroringMap ( const PolygonMeshT<T> &baseMesh, u8 mirroringAxis = 0, T tolerance = T(0.001) );
	void resetBaseMeshAndMirroringMap ( const PolygonMeshT<T> &baseMesh, u8 mirroringAxis = 0, T tolerance = T ( 0.001 ) );

	void resetTargetSize ( u32 size );

	void setTargetMesh ( 
		u32 targetID, 
		const PolygonMeshT<T> &targetMesh, 
		Bool normal = true, 
		T threshold = T ( 0.001 ) 
	);
	void setTargetMesh ( 
		u32 targetID, 
		const PolygonMeshT<T> &targetMesh, 
		std::vector<T> &weight, 
		Bool normal = true, 
		T threshold = T ( 0.001 ) 
	);
	void setTargetMesh ( 
		u32 targetID, 
		const PolygonMeshT<T> &refMesh, 
		const PolygonMeshT<T> &targetMesh, 
		Bool normal = true, 
		T threshold = T ( 0.001 ) 
	);
	void setTargetMesh ( 
		u32 targetID, 
		const PolygonMeshT<T> &refMesh, 
		const PolygonMeshT<T> &targetMesh, 
		std::vector<T> &weight, 
		Bool normal = true, 
		T threshold = T ( 0.001 ) 
	);
	void setAllTargetMesh (
		const std::vector<PolygonMeshT<T>> &targetMesh,
		Bool normal = true,
		T threshold = T ( 0.001 )
	);

	void setTargetPoints ( 
		u32 targetID, 
		const std::vector<Vec3T<T>> &targetPoints,
		T threshold = T ( 0.001 ) 
	);
	void setTargetPoints ( 
		u32 targetID, 
		const std::vector<Vec3T<T>> &targetPoints,
		const std::vector<T> &weight, 
		T threshold = T ( 0.001 ) 
	);
	void setTargetPoints (
		u32 targetID,
		const std::vector<Vec3T<T>> &refPoints,
		const std::vector<Vec3T<T>> &targetPoints,
		T threshold = T ( 0.001 )
	);
	void setTargetPoints ( 
		u32 targetID, 
		const std::vector<Vec3T<T>> &refPoints,
		const std::vector<Vec3T<T>> &targetPoints,
		const std::vector<T> &weight,
		T threshold = T ( 0.001 ) 
	);
	
	void setSymmetry ( u32 targetID, u8 symmetryAxis = 0 );
	void mirroring ( u32 targetID, u8 mirroringAxis = 0 );

	void copyTarget ( u32 targetID, u32 from );

	void createMirroringTargets ( u8 mirroringAxis = 0 );

	PolygonMeshT<T> getBaseMesh ()const;
	PolygonMeshT<T> getTargetMesh ( u32 targetID )const;
	PolygonMeshT<T> getMirrorTargetMesh ( u32 targetID )const;
	BlendShapeTargetT<T> getTarget ( u32 targetID )const;
	BlendShapeTargetT<T> getMirrorTarget ( u32 targetID )const;

	void evaluate ( IO PolygonMeshT<T> &mesh, const std::vector<T> &weights, const std::vector<T> &mirrorWeights = std::vector<T>(0) ) const;

	// void targetsTransfar (); �������E�x�[�X���b�V���̕ό`�ɍ��킹�ău�����h�^�[�Q�b�g���b�V���Q���X�V����

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( baseMesh ),
			CEREAL_NVP ( mirroringPointIDMap ),
			CEREAL_NVP ( initialized ),
			CEREAL_NVP ( targets ),
			CEREAL_NVP ( mirroringTargets ),
			CEREAL_NVP ( hasNormals )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

#include "BlendShape.inl"

	/** @} */
/** @} */

using BlendShape = BlendShapeT<f32>;
using BlendShaped = BlendShapeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************