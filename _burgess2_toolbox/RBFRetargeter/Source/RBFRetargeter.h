#pragma once
#include <burgess2_main/Algorithm.h>
#include <burgess2_main/Geometry.h>

//******************************************************************************
//! @file	RBFRetargeter.h
//! @brief	 RBF Retargeter
//******************************************************************************
/** @defgroup Tools
*　@{
*/
/**		@brief	RBF Retargeter<br>
*			RBFを用いたポイント空間ベースのリターゲット処理 <br>
*			alias : \b RBFRetargeter = RBFRetargeterT<float> // \b RBFRetargeterd = RBFRetargeterT<double> <br>
*			#include <Burgess_Tools/RBFRetargeter/RBFRetargeter.h> <br>
*			@code
*				// RBF2 rbfVal;
*				// Bool resetup;
*				// PolygonMesh baseMesh; 頂点値でRBF空間を構築するためのメッシュ
*				// PolygonMesh targetMesh; RBF空間の各頂点位置に紐づけるターゲット値（頂点位置）
*				if ( resetup ){ 
*					// rbfの初回作成もしくは再作成
*					rbfVal = PSRetargeter::Setup ( baseMesh, targetMesh, 2 );
*					resetup = false;
*				}
*				PSRetargeter::UpdataRetargetMesh ( rbfVal, newTargetMesh ); // ターゲット値を更新
*				PolygonMesh result = baseMesh; // 同一トポロジーのリザルトメッシュを準備
*				PSRetargeter::Solve ( result, rbfVal, baseMesh );
*			@endcode
*/

template<typename T>
class RBFRetargeterT {
	RBFT<T> rbf;
	u8 size;
public:
	RBFRetargeterT (){}
	virtual ~RBFRetargeterT (){}

	void setup ( const std::vector<Vec3T<T>> &referencePoint, const std::vector<Vec3T<T>> &refarenceValue, u8 rbfType = 1, T rbfSmooth = 0.0, T rbfEpsilon = 0.1 );
	void setup ( const std::vector<Vec3T<T>> &referencePoint, const std::vector<Mat4T<T>> &refarenceValue, u8 rbfType = 1, T rbfSmooth = 0.0, T rbfEpsilon = 0.1 );
	void setup ( const PolygonMeshT<T> &referenceMesh, const PolygonMeshT<T> &retargetMesh, u8 rbfType = 1, T rbfSmooth = 0.0, T rbfEpsilon = 0.1 );

	void solve ( OUT std::vector<Vec3T<T>> &result, const std::vector<Vec3T<T>> &sourcePoint )const;
	void solve ( OUT std::vector<Mat4T<T>> &result, const std::vector<Vec3T<T>> &sourcePoint )const;
	void solve ( IO PolygonMeshT<T> &result, const PolygonMeshT<T> &sourceMesh )const;
};


#include "RBFRetargeter.inl"
/** @} */
using RBFRetargeter = RBFRetargeterT<f32>;
using RBFRetargeterd = RBFRetargeterT<f64>;
//******************************************************************************
//! End of File
//******************************************************************************