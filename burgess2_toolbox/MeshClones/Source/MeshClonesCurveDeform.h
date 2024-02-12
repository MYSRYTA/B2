#pragma once
#include "MeshClones.h"

//******************************************************************************
//! @file	MeshClonesCurveDeform.h
//! @brief	 MeshClonesCurveDeform
//******************************************************************************
/** @defgroup Tools
*	   @{
*/
/**		@defgroup MeshClones
*			@{
*/
/**				@brief MeshClonesCurveDeform <br>
*					#include <burgess2_main/Geometry/Geometry.h><br>
*/

template<typename T>
class MeshClonesCurveDeformT : public MeshClonesT<T> {
	std::vector<GeometryDeltaT<T>> sourceDelta;
	std::vector<u32> sourceBorderPointIDs; /* sourceMeshのボーダーの頂点ID群 */
	T sourceCurveLengthInv; /* curveDeformのスケール計算で使用　( 1.0 / sourceDeltaの算出で使用したカーブの長さ ) */
public:
	MeshClonesCurveDeformT () {};
	virtual ~MeshClonesCurveDeformT (){}

	void set ( const PolygonMeshT<T> &sourceMesh, u32 size, const std::vector<GeometryDeltaT<T>> &sourceDelta, T sourceCurveLength, Bool checkBorderEdge = false );

	void applayCurveDeform ( const BSplineCurvesT<T> &curves );
	void applayCurveDeform ( const BSplineCurvesT<T> &curves, const std::vector<CurveSectionsT<T>> &sectionsList );
};

#include "MeshClonesCurveDeform.inl"

	/** @} */
/** @} */

using MeshClonesCurveDeform = MeshClonesCurveDeformT<f32>;
using MeshClonesCurveDeformd = MeshClonesCurveDeformT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************