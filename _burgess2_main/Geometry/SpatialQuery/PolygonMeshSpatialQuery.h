#pragma once
#include <burgess2_main/Math.h>
#include <burgess2_main/Geometry/SpatialQuery/EmbreeHelper.h>

template<typename T> class GeometryLocationT;
template<typename T> class GeometryDeltaT;

//******************************************************************************
//! @file	PolygonMeshSpatialQuery.h
//! @brief	 PolygonMesh用最近傍検索クラス
//******************************************************************************
/** @defgroup Geometry
*	  @{
*/
/**		@defgroup SpatialQuery
*			@{
*/
/**				@brief PolygonMesh用空間分析クラス <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias \b : PolygonMeshSpatialQuery = PolygonMeshSpatialQueryT<f32> <br>
*					Embreeをラッピングしており高速動作するが、double型は未サポート<br>
*					@code
*						PolygonMesh cube;
*						cube.createCube( Xfo(), 1.0f );
*
*						PolygonMeshSpatialQuery sq = cube.toSpatialQuery( );
*		
*						Vec3 startPos {0.0,2.0,0.0};
*						f32 maxDist = 1.0f;
*						GeometryLocation loc = sq.getClosestLocation( startPos, maxDist, PolygonMeshSpatialQueryT::Side::backFace  );
*						Vec3 locationPos = cube.getPositionAtLocation( loc );
*					@endcode
*/

template<typename T>
class PolygonMeshSpatialQueryT {
	RTCDevice device; // embree
	RTCScene scene; // embree
	std::vector<BurgessEmbree::TrianglePolygonInfo<T>> triangleInfo;
	Bool initialized = false;

	BurgessEmbree::PointQuery<T> getClosest_task(const Vec3T<T>& startPosition, T maxDistance = T(BURGESS2::F32_INFINITE), u8 faceSide = u8(Side::bothFaces))const;
public:
	enum struct Side {
		bothFaces = 0,
		frontFace = 1,
		backFace = 2,
	};

	std::vector<Vec3T<T>> pointPositions;
	std::vector<u32> packedPointIDs;

	PolygonMeshSpatialQueryT ();
	~PolygonMeshSpatialQueryT () { rtcReleaseScene ( this->scene );	rtcReleaseDevice ( this->device ); }

	Bool setup(const PolygonMeshT<T>& mesh);
	Bool setup(const PolygonMeshT<T>& mesh, const String& maskAttrName, Bool invertMask = false);
	
	Bool isValid()const;

	GeometryLocationT<T> raycast ( const Ray3T<T> &ray, T maxDistance = T(BURGESS2::F32_INFINITE), u8 faceSide = u8(Side::bothFaces)	)const;
	std::vector<GeometryLocationT<T>> getAllRayIntersect ( const Ray3T<T>& ray, T maxDistance = T(BURGESS2::F32_INFINITE), u8 faceSide = u8(Side::bothFaces), T tolerance = BURGESS2::TOLERANCE )const;
	std::vector<GeometryLocationT<T>> getAllLineIntersect ( const Vec3T<T> &start, const Vec3T<T> &end,	u8 faceSide = u8(Side::bothFaces), T tolerance = BURGESS2::TOLERANCE )const;
	Vec3T<T> getClosestPosition(const Vec3T<T>& startPosition, T maxDistance = T(BURGESS2::F32_INFINITE), u8 faceSide = u8(Side::bothFaces) )const;
	GeometryLocationT<T> getClosestLocation(const Vec3T<T>& startPosition, T maxDistance = T(BURGESS2::F32_INFINITE), u8 faceSide = u8(Side::bothFaces))const;

};

#include "PolygonMeshSpatialQuery.inl"

/** @} */
/** @} */
using PolygonMeshSpatialQuery = PolygonMeshSpatialQueryT<f32>;
//using PolygonMeshSpatialQueryd = PolygonMeshSpatialQueryT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************