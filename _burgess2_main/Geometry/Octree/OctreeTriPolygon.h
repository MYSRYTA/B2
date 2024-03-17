#pragma once
#include <burgess2_main/Math.h>
#include <burgess2_main/Containers.h>

template<typename T> class OctreeNodeT;

//******************************************************************************
//! @file	OctreeTriPolygon.h
//! @brief	 Octree用の三角形ポリゴンクラス
//******************************************************************************
/** @defgroup Geometry
*	  @{
*/
/**		@defgroup Octree
*			@{
*/
/**				@brief Octree用の三角形ポリゴンクラス <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : OctreeTriPolygon <br> \b : OctreeTriPolygond <br>
*/

template<typename T>
class OctreeTriPolygonT{
public:
	Vec3T<T> p0, p1, p2;
	BBoxT<T> bBox;
	u32 triPolygonID;
	u32 localTriID;
	u32 origPolygonID;

	OctreeTriPolygonT () {}
	virtual ~OctreeTriPolygonT () {}
};
/** @} */
/** @} */

using OctreeTriPolygon = OctreeTriPolygonT<f32>;
using OctreeTriPolygond = OctreeTriPolygonT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************