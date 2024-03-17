#pragma once
#include <burgess2_main/Math.h>
template<typename T> class GeometryLocationT;
template<typename T> class GeometryDeltaT;

//******************************************************************************
//! @file	LinesSpatialQuery.h
//! @brief	 Curves用最近傍検索クラス
//******************************************************************************
/** @defgroup Geometry
*	  @{
*/
/**		@defgroup SpatialQuery
*			@{
*/
/**				@brief Curves用空間分析クラス <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : LinesSpatialQuery <br> \b : LinesSpatialQueryd <br>
*					Curvesの最近傍検索クラス<br>
*					@code
*						std::vector<Vec3> cp ( 5 );
*						cp [0] = Vec3 ( 0.f, 0.f, 4.f );
*						cp [1] = Vec3 ( 10.f, 0.f, 2.f );
*						cp [2] = Vec3 ( 0.f, 0.f, 1.f );
*						cp [3] = Vec3 ( -10.f, 0.f, -2.f );
*						cp [4] = Vec3 ( 0.f, 0.f, -5.f );
*						u8 degree = 3;
*						BSplineCurves curves;
*						curves.add ( cp, degree, CurveData::CurveFrom::Open );
*
*						LinesSpatialQuery sq = curves.toSpatialQuery( 0 );
*						
*						GeometryLocation loc = sq.getClosestLocation( Vec3(1.0f) );
*						Vec3 locPosition = curves.getPositionAtLocation( loc );
*					@endcode
*/

template<typename T>
class LinesSpatialQueryT{
	std::vector<Vec3T<T>> pointPositions;/*Linesを構成する頂点群*/
	std::vector<T> lineLength; /*配列番号==ライン番号*/
	T invTotalLengthRatio; /*lengthRatioを算出するための逆数を準備しておく*/
public:
	u32 v [5]; /*v[0] = LinesSpatialQueryを作成したCurves内のカーブID*/

	LinesSpatialQueryT () {}
	LinesSpatialQueryT ( const std::vector<Vec3T<T>> &pointPositions );
	~LinesSpatialQueryT () {}

	GeometryLocationT<T> getClosestLocation ( const Vec3T<T> &position, T maxDistance = T(BURGESS2::F32_INFINITE) )const;
	Vec3T<T> getLineP0Position ( u32 lineID )const;
	Vec3T<T> getLineP1Position ( u32 lineID )const;
	T getLineLength ( u32 lineID )const;

	u32 lineCount ()const;
	Bool isValid ()const;
};

#include "LinesSpatialQuery.inl"

/** @} */
/** @} */
using LinesSpatialQuery = LinesSpatialQueryT<f32>;
using LinesSpatialQueryd = LinesSpatialQueryT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************