#pragma once
#include <burgess2_main/Math.h>

template<typename T> class GeometryLocationT;
template<typename T> class GeometryDeltaT;

//******************************************************************************
//! @file	PolygonMeshSpatialQuery_Octree.h
//! @brief	 PolygonMesh用最近傍検索クラス　(Geometry>Octreeを用いた旧SpatialQueryクラス)
//******************************************************************************
/** @defgroup Geometry
*	  @{
*/
/**		@defgroup SpatialQuery
*			@{
*/
/**				@brief Octreeクラスを使用するPolygonMesh用空間分析クラス <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : PolygonMeshSpatialQuery_Octree = PolygonMeshSpatialQueryT_Octree<f32><br> \b : PolygonMeshSpatialQueryd_Octree = PolygonMeshSpatialQueryT_Octree<f64><br>
*					Embreeを使用するPolygonMeshSpatialQueryT<T>クラスの方が高速<br>
*					@code
*						PolygonMesh cube;
*						cube.createCube( Xfo(), 1.0f );
*
*						PolygonMeshSpatialQuery sq = cube.toSpatialQuery( );
*
*						f32 maxDist = 100.0f;
*						GeometryLocation loc = sq.getClosestLocation( Vec3(1.0f), maxDist, PolygonMeshSpatialQueryT::Side::backFace  );
*						Vec3 locPosition = cube.getPositionAtLocation( loc );
*					@endcode
*/

template<typename T>
class PolygonMeshSpatialQueryT_Octree {
public:
	AttributesT<T> attr;
	Bool initialized = false;
	/*enum struct Side{
		bothFaces = u8(0),
		frontFace = u8(1),
		backFace = u8(2),
	};*/

	OctreeT<T> octree;

	PolygonMeshSpatialQueryT_Octree();
	PolygonMeshSpatialQueryT_Octree( const PolygonMeshT<T> &mesh );
	PolygonMeshSpatialQueryT_Octree( const PolygonMeshT<T> &mesh, const std::vector<f32> &vertexMaskValue, Bool invertMask = false );
	~PolygonMeshSpatialQueryT_Octree() {}

	GeometryLocationT<T> getClosestLocation ( 
		const Vec3T<T> &startPosition, 
		T maxDistance = T(BURGESS2::F32_INFINITE),
		u8 faceSide = 0,
		T tolerance = BURGESS2::TOLERANCE
	)const;

	GeometryLocationT<T> raycast ( 
		const Ray3T<T> &ray, 
		T maxDistance = T(BURGESS2::F32_INFINITE), 
		u8 faceSide = 0,
		T tolerance = BURGESS2::TOLERANCE
	)const;

	std::vector<GeometryLocationT<T>> getAllRayIntersect ( 
		const Ray3T<T> &ray, 
		T maxDistance = T(BURGESS2::F32_INFINITE),
		u8 faceSide = 0,
		Bool omitDuplication = false,
		T tolerance = BURGESS2::TOLERANCE
	)const;

	std::vector<GeometryLocationT<T>> getAllLineIntersect ( 
		const Vec3T<T> &start, 
		const Vec3T<T> &end,
		u8 faceSide = 0,
		Bool omitDuplication = false,
		T tolerance = BURGESS2::TOLERANCE
	)const;

	Bool isValid ()const;
};

#include "PolygonMeshSpatialQuery_Octree.inl"

/** @} */
/** @} */
using PolygonMeshSpatialQuery_Octree = PolygonMeshSpatialQueryT_Octree<f32>;
using PolygonMeshSpatialQueryd_Octree = PolygonMeshSpatialQueryT_Octree<f64>;

//******************************************************************************
//! End of File
//******************************************************************************