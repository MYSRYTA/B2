#pragma once
#include "BSplineCurves.h"

//******************************************************************************
//! @file	CatmullRomCurves.h
//! @brief	 CatmullRomCurves
//******************************************************************************
/** @defgroup Geometry
*		@{
*/
/**		@defgroup Curves
*			@{
*/
/**				@brief 複数本のCatmull-Romカーブを扱うクラス <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : CatmullRomCurves = CatmullRomCurvesT<f32> <br> \b : CatmullRomCurvesd = CatmullRomCurvesT<f64><br>
*					<br>
*					@par 基本のアトリビュート - basic attributes
*						- <b> コントロールポイント位置 - CP(control point) position </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "pointPositions"
*							- 配列数 - array size = ポイント数 - point count
*						- <b> カーブ上の値を取得する関数を高速化するためのアトリビュート（編集禁止） </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "paramLengthTable"
*							- 説明 - note : Vec3.x=param, .y=length, .z=invLengthRatio
*					<br>

*					@par オプション アトリビュート - sub attributes
*						- <b> カーブ法線 - normals </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "normals"
*							- 配列数 - array size = compute***Normals(u32 sample)のsample * point cpunt
*					<br>
*					@note 法線アトリビュートの値の設定は下記の関数で行う <br>
*						To set the value of the normals attribute, the following functions are used. <br>
*						&nbsp;&nbsp; computeNormals (u32 curveID, u32 sample=1) <br>
*						&nbsp;&nbsp; computePTFNormals (u32 curveID, const Vec3T< T > &upvector, u32 sample=1) <br>
*						&nbsp;&nbsp; computePTFNormals2 (u32 curveID, const Vec3T< T > &upvector, u32 sample=1) <br>
*						&nbsp;&nbsp; computeFrenetFrameNormals (u32 curveID, const Vec3T< T > &upvector, u32 sample=1) <br>
*						&nbsp;&nbsp; computeDoubleReflectionNormals (u32 curveID, const Vec3T< T > &upvector, u32 sample=1) <br>
*
*					<br>
*					@code
*						// prepar cp
*						std::vector<Vec3> cp ( 5 );
*						cp [0] = Vec3 ( 0.f, 0.f, 4.f );
*						cp [1] = Vec3 ( 10.f, 0.f, 2.f );
*						cp [2] = Vec3 ( 0.f, 0.f, 1.f );
*						cp [3] = Vec3 ( -10.f, 0.f, -2.f );
*						cp [4] = Vec3 ( 0.f, 0.f, -5.f );
*
*						CatmullRomCurves curves;
*						curves.add ( cp, CurveData::CurveFrom::Open );
*
*						curves.computePTFNormals ( 0, Vec3(0,1,0) );
*
*						Xfo pose = curves.getXfoAtLengthRatio ( 0, 0.35f );
*					@endcode
*/
template<typename T>
class CatmullRomCurvesT : public BSplineCurvesT<T>{
	std::vector<std::vector<Vec3T<T>>> initializeArray ( const std::vector<Vec3T<T>> &ctrlPoints, u8 form );

	Vec3T<T> core ( T fillT, const std::vector<Vec3T<T>> &p, T t[4] );

	T calculate_t ( const Vec3T<T> &p0, const Vec3T<T> &p1, T alpha, T t0 );

	std::vector<Vec3T<T>> segmentsToSpline ( const std::vector<std::vector<Vec3T<T>>> &segments );
public:
	CatmullRomCurvesT () {}
	~CatmullRomCurvesT () {}

	u32 add ( const std::vector<Vec3T<T>> &points, u8 form, T parameterization = 0.5, u8 sample = 10 );
};

/** @} */
/** @} */

#include "CatmullRomCurves.inl"
using CatmullRomCurves = CatmullRomCurvesT<f32>;
using CatmullRomCurvesd = CatmullRomCurvesT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************