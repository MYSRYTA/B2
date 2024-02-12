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
*				ブレンドシェイプターゲット <br>
*				#include <burgess2_main/Geometry/Geometry.h><br>
*/

template<typename T>
struct BlendShapeTargetT{
	std::vector<GeometryDeltaT<T>> delta;

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( delta )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

/** @} */
/**		@defgroup BlendShape
*			@{
*/
/**			@brief BlendShape <br>
*			ブレンドシェイプ <br>
*			#include <burgess2_main/Geometry/Geometry.h><br>
*			@code
*				// ベースメッシュを準備
*				PolugonMesh baseMesh;
*				baseMesh = ......
*				
*				// ブレンドシェイプターゲットのリストを準備
*				std::vector<PolygonMesh> _targetMeshes( 3 );
*				_targetMeshes[0] = ......
*
*				// 各シェイプターゲットのブレンド値のリストを準備
*				std::vector<f32> _blendWeights( 3, 0.0f );
*				_blendWeights[0] = ......
*
*				// 各反転シェイプターゲットのブレンド値のリストを準備
*				std::vector<f32> _blendWeightsMirror( 3, 0.0f );
*				_blendWeightsMirror[0] = ......
*
*				BlendShape bs;
*				bs.setBaseMeshAndMirroringMap ( baseMesh, 0 ); // ベースメッシュの登録とX軸での反転マップの作成
*				bs.setAllTargetMesh ( _targetMeshes ); // シェイプターゲットを登録
*				bs.setSymmetry( 2 ); // 2番のターゲットを左右対称にする
*				bs.createMirroringTargets( 0 ); // 登録している全てのターゲットをX軸で反転して反転ターゲットに登録する
*
*				PolygonMesh result = baseMesh.clone(); // 処理対象とするメッシュを準備
*				bs.evaluate ( result, _blendWeights, _blendWeightsMirror ); // ブレンドシェイプを実行
*
*			@endcode
*/
template<typename T>
class BlendShapeT {
protected:
	PolygonMeshT<T> baseMesh;
	std::vector<s32> mirroringPointIDMap; // 対称位置にある頂点番号のリスト(反転ターゲットを作る際に使用する対象マップ）　
	Bool initialized = false;
	std::vector<BlendShapeTargetT<T>> targets; // ブレンドシェイプターゲットの登録場所
	std::vector<BlendShapeTargetT<T>> mirroringTargets; // ↑targets[]の反転ターゲットの登録場所
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

	// void targetsTransfar (); 未実装・ベースメッシュの変形に合わせてブレンドターゲットメッシュ群を更新する

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
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
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
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