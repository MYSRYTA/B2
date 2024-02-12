#pragma once
#include "CurvesBase.h"

//******************************************************************************
//! @file	BezierCurves.h
//! @brief	 BezierCurves
//******************************************************************************
/** @defgroup Geometry
*		@{
*/
/**		@defgroup Curves
*			@{
*/
/**				@brief 複数本のNURBSを扱うクラス <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : BezierCurves = BezierCurvesT<f32> <br> \b : BezierCurvesd = BezierCurvesT<f64> <br>
*					rationalWeight値が全て1.0の場合はBSplineCurvesの方が軽量<br>
*					BSplineCurvesとの違いはrationalWeightのみ（現在はrationalWeightが未対応のためBSplineCurvesと同等のクラス）<br>
*					<br>
*					@par 基本のアトリビュート - basic attributes
*						- <b> コントロールポイント位置 - CP(control point) position </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "pointPositions"
*							- 配列数 - array size = ポイント数 - cp count
*						- <b> ノットベクトル（編集禁止）- knot vector. Do not edit.</b>
*							- type = @link F32AttributeT<T> @endlink
*							- attribute name = "knotVector"
*							- 配列数 - array size = 下記のカーブフォームによって異なる<br>
*								open : CP count + degree + 1<br>
*								close :  CP count + degree + 2<br>
*								periodic : CP count + (degree * 2) + 1 / [0]=0 <br>
*						- <b> カーブ上の値を取得する関数を高速化するためのアトリビュート（編集禁止） </b> 
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "paramLengthTable"
*							- 説明 - note : Vec3.x=param, .y=length, .z=invLengthRatio
*						- <b> ウェイト - rational weight
*							- type = @link F32AttributeT<T> @endlink
*							- attribute name = "rationalWeight"
*							- 配列数 - array size = ポイント数 - cp count
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

*					@note カーブの次数はメンバー変数".v[0]"に保持 <br>
*						curve degree save on .v[0]. ".v" is menber arg.
*
*					<br>
*					@note MayaのCurveデータ <br>
*						<b> CP size </b><br>
*						&nbsp;&nbsp; open form = CP count <br>
*						&nbsp;&nbsp; periodic = CP count + degree<br>
*						<b> knot vector size </b><br>
*						&nbsp;&nbsp; open form = CP count + degree - 1<br>
*						&nbsp;&nbsp; periodic form = CP count + (degree * 2) - 1 / [0] = -degree + 1 <br>
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
*						u8 degree = 3; // 3次
*
*						BezierCurves curves;
*						std::vector<f32> weights2 = { 0.01, 0.3, 1.0, 0.9, 1.0 };
*						curves.add ( cp, degree, CurveData::CurveFrom::Periodic, weights2 );
*					@endcode
*/

template<typename T>
class BezierCurvesT : public BSplineCurvesT<T>{
	Vec3T<T> core(const Vec3T<T>& p0, const Vec3T<T>& p1, const Vec3T<T>& p2, T t);
	Vec3T<T> core(const Vec3T<T>& p0, const Vec3T<T>& p1, const Vec3T<T>& p2, T t, const std::vector<T>& weight);

public:
	enum handleDirection
	{
		X = u8(0),
		neg_X,
		Y,
		neg_Y,
		Z,
		neg_Z,
		Auto,
		Auto_BothEnds_X,
		Auto_BothEnds_neg_X,
		Auto_BothEnds_Y,
		Auto_BothEnds_neg_Y,
		Auto_BothEnds_Z,
		Auto_BothEnds_neg_Z
	};

	BezierCurvesT() {}
	~BezierCurvesT() {}

	u32 add(const std::vector<Vec3T<T>> &points, u8 form, u32 sample = 10 );
	u32 add(const std::vector<Mat4T<T>>& segmentPose, u8 form, BezierCurvesT::handleDirection handleDirType, T handleLengthRatio, u32 sample = 10);
	u32 add(const std::vector<Mat4T<T>>& segmentPose, u8 form, BezierCurvesT::handleDirection handleDirType, std::vector<T>& handleLengthRatio, u32 sample = 10);
};

#include "BezierCurves.inl"

/** @} */
/** @} */
using BezierCurves = BezierCurvesT<f32>;
using BezierCurvesd = BezierCurvesT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************