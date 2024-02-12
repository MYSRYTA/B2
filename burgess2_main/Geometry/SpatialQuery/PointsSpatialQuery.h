#pragma once
#include <burgess2_main/Math.h>
#include <NanoFLANN/include/nanoflann.hpp>
using namespace nanoflann;

template <typename T>
struct NanoPointCloudT {
public:
	std::vector<VecT<T>> points;

	NanoPointCloudT () {}
	NanoPointCloudT ( const std::vector<Vec3T<T>>& position ) {
		this->points.resize ( position.size () );
		for (u32 i = 0; i < position.size (); i++) {
			this->points[i] = position[i].toVec ();
		}
	}
	virtual ~NanoPointCloudT () {}

	inline size_t kdtree_get_point_count () const { 
		return this->points.size ();
	}

	inline T kdtree_get_pt (const size_t idx, const size_t dim) const { 
		return  this->points.at ( idx )[dim];
	}

	template <class BBOX>
	bool kdtree_get_bbox ( BBOX& /* bb */ ) const { 
		return false; 
	}
};

template <typename T>
using NanoFlannTree2T = nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor<T, NanoPointCloudT<T> >, NanoPointCloudT<T>, 2>;
template <typename T>
using NanoFlannTree3T = nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor<T, NanoPointCloudT<T> >, NanoPointCloudT<T>, 3>;
template <typename T>
using NanoFlannTree4T = nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor<T, NanoPointCloudT<T> >, NanoPointCloudT<T>, 4>;

//******************************************************************************
//! @file	PointsSpatialQuery.h
//! @brief	 Points用最近傍検索クラス
//******************************************************************************
/** @defgroup Geometry
*	  @{
*/
/**		@defgroup SpatialQuery
*			@{
*/
/**				@brief Points用最近傍検索クラス <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : PointsSpatialQuery <br> \b : PointsSpatialQueryd <br>
*					Pointsの最近傍検索クラス<br>
*
*					@code
*						std::vector<Vec3> vec3Points ( 50000 );
*						vec3Points [0] = Vec3 ( 0.f, 0.f, 4.f );
*						vec3Points [1] = Vec3 ( 10.f, 0.f, 2.f );
*						vec3Points [2] = Vec3 ( 0.f, 0.f, 1.f );
*						vec3Points [3] = Vec3 ( -10.f, 0.f, -2.f );
*						vec3Points [4] = Vec3 ( 0.f, 0.f, -5.f );
*						......
*
*						VecSpatialQuery sq ( vec3Points );
*						
*						// 指定した値に最も近いポイントの配列番号を取得
*						u32 arrayId = sq.getClosestIndex( Vec3(1.0f, 0.0f 0.0f) );
*
*						// 指定した値に近い順に最大10個までのポイントを探し出し、それらの配列番号を取得
*						std::vector<u32> ids = sq.getNClosestIndexes( Vec3(1.0f, 0.0f 0.0f), 10 );
*					@endcode
*/

template<typename T>
class PointsSpatialQueryT{
	std::unique_ptr<NanoPointCloudT<T>> pointCloud;
	std::unique_ptr<NanoFlannTree2T<T>> nTree2;
	std::unique_ptr<NanoFlannTree3T<T>> nTree3;
	Bool initialized = false;
public:
	PointsSpatialQueryT () {}
	PointsSpatialQueryT ( const std::vector<Vec3T<T>>& vec3Array );
	~PointsSpatialQueryT () {}

	s32 getClosestIndex ( const Vec2T<T>& position, T maxDistance = T ( BURGESS2::F32_INFINITE ) )const;
	s32 getClosestIndex ( const Vec3T<T>& position, T maxDistance = T ( BURGESS2::F32_INFINITE ) )const;

	std::vector<u32> getNClosestIndexes ( const Vec2T<T>& position, u32 maxCount, T maxDistance = T ( BURGESS2::F32_INFINITE ) )const;
	std::vector<u32> getNClosestIndexes ( const Vec3T<T>& position, u32 maxCount, T maxDistance = T ( BURGESS2::F32_INFINITE ) )const;

	std::vector<u32> getRadiusSearchIndexes ( const Vec2T<T>& position, T searchRadius, Bool sort = false)const;
	std::vector<u32> getRadiusSearchIndexes ( const Vec3T<T>& position, T searchRadius, Bool sort = false)const;
	
	Bool isValid ()const { return this->initialized; };
};

#include "PointsSpatialQuery.inl"

/** @} */
/** @} */
using PointsSpatialQuery = PointsSpatialQueryT<f32>;
using PointsSpatialQueryd = PointsSpatialQueryT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************