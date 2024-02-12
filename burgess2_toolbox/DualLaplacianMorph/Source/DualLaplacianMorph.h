#pragma once
#include <burgess2_main/Geometry.h>

//******************************************************************************
//! @file	DualLaplacianMorph.h
//! @brief	 Dual-Laplacianを用いてメッシュのディティールを転送する
//******************************************************************************
/** @defgroup Tools
*　@{
*/
/**			@defgroup Dual-Laplacian-Morph
*			@{
*/
/**			@brief	Dual-Laplacianを用いてメッシュのディティールを転送する<br>
*			参考論文：Dual-Laplacian 
*			@ref http://www.geometrie.tugraz.at/events/poly/liu_strobl07.pdf <br>
*			alias : \b DualLaplacianMorph = DualLaplacianMorphT<f32> // \b DualLaplacianMorphd = DualLaplacianMorphT<f64> <br>
*			#include <burgess2_toolbox/DualLaplacianMorph/DualLaplacianMorph.h><br>
*			@code
*				PolygonMesh baseMesh;
*				PolygonMesh targetMeshes;
*				std::vector<u32> mask = { 2,4,5,6,7,10,11,12,14,15,17,18 };
*
*				DualLaplacianMorph dlm;
*				dlm.setDualLaplacianInfo( baseMesh, targetMesh, mask );
*				
*				PolygonMesh result = baseMesh.clone();
*				u32 ite = 10;
*				f32 weight = 0.7f;
*				dlm.solve( result, ite, weight );
*			@endcode
*/

template<typename T>
class DualLaplacianMorphT{
	std::vector<s32> baseMeshPoiIDs;
	PolygonMeshT<T> baseDualMesh;
	std::vector<Vec3T<T>> baseDualMeshPoiPos;
	std::vector<Vec3T<T>> targetDualInfo;

	std::vector<Vec3T<T>> computeDualLaplacianInfo ( const PolygonMeshT<T> &dualMesh, 
												  const std::vector<L16Array<u32>> &surroundPoiIDs )const;
public:
	bool initialized = false;

	DualLaplacianMorphT ();
	virtual ~DualLaplacianMorphT (){};

	void setDualLaplacianInfo ( const PolygonMeshT<T> &baseMesh,
								const PolygonMeshT<T> &targetMesh,
								const std::vector<u32> &mask = std::vector<int>(0) );
	
	void solve ( IO PolygonMeshT<T> &mesh, T iterateRatio, T weight );
};

#include "DualLaplacianMorph.inl"
using DualLaplacianMorph = DualLaplacianMorphT<f32>;
using DualLaplacianMorphd = DualLaplacianMorphT<f64>;
/** @} */
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************