#pragma once
#include <burgess2_main/Math.h>
#include <burgess2_main/Containers.h>

template<typename T> class PolygonMeshT;
template<typename T> class GeometryLocationT;
template<typename T> class GeometryDeltaT;

//******************************************************************************
//! @file	OctreeNode.h
//! @brief	 オクトリーノード
//******************************************************************************
/** @defgroup Geometry
*	  @{
*/
/**		@defgroup Octree
*			@{
*/
/**				@defgroup Macro
*					@{
*/
namespace BurgessOctree {
	const u8 MAX_OCTREE_LEVEL = u8 ( 10 );
	const u16 MAX_TRIPOLYGON_COUNT = u16 ( 500 );
}
/** @} */
/**				@brief オクトリーを構成するノード <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : OctreeNode <br> \b : OctreeNoded <br>
*/

template<typename T>
class OctreeNodeT : public BBoxT<T>{
public:
	u8 level;								/* このノードが属するノード階層のレベル */
	Vec3T<T> position;						/* ノードの中央位置 */
	T sideLength;							/* ノードの一辺の長さ */
	T radius;								/* ノードの中央から角までの距離 (BSphの半径として使用) */
	std::vector<OctreeNodeT<T>> branches;	/* 子供階層 */
	std::vector<u32> triPolyIDs;			/* このノードに属する三角形ポリゴンのID群 */

	OctreeNodeT () {}
	OctreeNodeT ( u8 _level, Vec3T<T> _centerPos, T _sideLength );
	virtual ~OctreeNodeT () {}

	void addTriPolyID ( u32 triPolyID );
	void addNode ( u8 _level, Vec3T<T> _centerPos, T _sideLength );

	u16 polyCount ()const;

	Bool isLeafNode ()const;
};
/** @} */
/** @} */

#include "OctreeNode.inl"

using OctreeNode = OctreeNodeT<f32>;
using OctreeNoded = OctreeNodeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************