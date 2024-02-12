#pragma once
#include <burgess2_main/Geometry.h>
#include <burgess2_main/Characters.h>

//******************************************************************************
//! @file	MeshClones.h
//! @brief	 MeshClones
//******************************************************************************
/** @defgroup Tools
*	   @{
*/
/**		@defgroup MeshClones
*			@{
*/
/**			@brief MeshClones <br>
*			メッシュのクローン(リファレンスではない)を効率よく扱うためのクラス<br>
*			#include <Burgess_Tools/MeshClones/MeshClones.h><br>
*			@code
*
*			@endcode
*/

template<typename T>
class MeshClonesT {
protected:
	PolygonMeshT<T> mesh;
	PolygonMeshT<T> sourceMesh;
	u32 cloneSize = 0;
public:
	MeshClonesT () {};
	virtual ~MeshClonesT (){}

	virtual void set ( const PolygonMeshT<T> &sourceMesh, u32 size ) final;

	void resize ( u32 size );
	u32 cloneCount ()const;
	void clear ();
	
	void applayTransform ( const std::vector<XfoT<T>> &Xfo );
	
	PolygonMeshT<T> getMesh ( Bool borderMerge = false );
	const PolygonMeshT<T>& getMeshRef ()const;
};

#include "MeshClones.inl"

	/** @} */
/** @} */

using MeshClones = MeshClonesT<f32>;
using MeshClonesd = MeshClonesT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************