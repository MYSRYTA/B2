#pragma once
#include "CurvesBase.h"
#include <burgess2_main/Geometry/SpatialQuery/LinesSpatialQuery.h>

template<typename T> class CurveSectionsT;

//******************************************************************************
//! @file	BSplineCurves.h
//! @brief	 BSplineCurves
//******************************************************************************
/** @defgroup Geometry
*	  @{
*/
/**		@defgroup Curves
*			@{
*/
/**				@brief 複数本のBスプラインカーブを扱うクラス <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : BSplineCurves = BSplineCurvesT<f32> <br> \b : BSplineCurvesd = BSplineCurvesT<f64> <br>
*					<br>
*					@par 基本のアトリビュート - basic attributes
*						- <b> コントロールポイント位置 - CP(control point) position </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "pointPositions"
*							- 配列数 - array size = ポイント数 - point count
*						- <b> ノットベクトル（編集禁止）- knot vector. Do not edit.</b>
*							- type = @link F64AttributeT<T> @endlink
*							- attribute name = "knotVector"
*							- 配列数 - array size = 下記のカーブフォームによって異なる<br>
*								open : CP count + degree + 1<br>
*								close :  CP count + degree + 2<br>
*								periodic : CP count + (degree * 2) + 1 / [0]=0 <br>
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
*						u8 degree = 3;
*
*						BSplineCurves curves;
*						curves.add ( cp, degree, CurveData::CurveFrom::Open );
*						curves.add ( cp, degree, CurveData::CurveFrom::Periodic );
*
*						curves.duplicate( 1 ); // duplicate [1] to [2]
*
*						u32 cnt = curves.curveCount();	 // cnt = 3
*
*						Vec3 tPos = curves.getPositionAtLengthRatio( 0, 0.5f );
*						f32 len = curves.getCurveLength( 1 );
*
*						curves.computePTFNormals ( 0, Vec3(0,1,0) );
*						Xfo pose = curves.getXfoAtLengthRatio ( 0, 0.35f );
*					@endcode
*/

template<typename T>
class BSplineCurvesT : public CurvesBaseT<T>{
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	BSplineCurvesT () {}
	virtual ~BSplineCurvesT () {}
	/** @} */
	/*! @name */
	/* @{ */
	u32 add ( const std::vector<Vec3T<T>> &points, u8 degree, u8 form, const std::vector<T> &knots );
	u32 add ( const std::vector<Vec3T<T>> &points, u8 degree, u8 form );

	std::vector<T> createBasicKnotVector ( u8 degree, u32 pointSize, u8 form );

	u8 getDegree ( u32 curveID )const;
	std::vector<T> getKnotVector ( u8 curveID )const;

	u32 duplicate ( u32 curveID );
	u32 copyFrom(const BSplineCurvesT<T> &src ) ;
	u32 copyCurveFrom(const BSplineCurvesT<T> &src, u32 curveID) ;
	/** @} */
	/*! @name param / normalized param*/
	/* @{ */
	T getParamFromNormalizedParam ( u32 curveID, T nParam )const;
	T getNormalizedParamFromParam(u32 curveID, T param)const;
	T getMaxParam ( u32 curveID )const;
	T getMinParam ( u32 curveID )const;

	std::vector<T> getEvenlyDivideParams( u32 curveID, u32 count ) const;
	std::vector<T> getRegularIntervalParams(u32 curveID, f32 _length ) const;
	std::vector<T> getDynamicIntervalParams(u32 curveID, f32 _length) const;
	std::vector<T> getRandomParams(u32 curveID, u32 count, u32 seed ) const;
	std::vector<T> getReductionParams ( u32 curveID, T maxDegree = 3.0, u8 precision = 3 ) const;
	/** @} */
	/*! @name value on curve*/
	/* @{ */
	Vec3T<T> getPositionAtParam ( u32 curveID, T param ) const override;
	Vec3T<T> getTangentAtParam ( u32 curveID, T param ) const override;
	Vec3T<T> getNormalAtParam ( u32 curveID, T param ) const override;
	XfoT<T> getXfoAtParam ( u32 curveID, T param, u8 directionType = 0 ) const override;
	
	Vec3T<T> getPositionAtNormalizedParam ( u32 curveID, T nParam ) const override;
	Vec3T<T> getTangentAtNormalizedParam ( u32 curveID, T nParam ) const override;
	Vec3T<T> getNormalAtNormalizedParam ( u32 curveID, T nParam ) const override;
	XfoT<T> getXfoAtNormalizedParam ( u32 curveID, T nParam, u8 directionType = 0 ) const override;

	Vec3T<T> getPositionAtLengthRatio ( u32 curveID, T ratio ) const override;
	Vec3T<T> getTangentAtLengthRatio ( u32 curveID, T ratio ) const override;
	Vec3T<T> getNormalAtLengthRatio ( u32 curveID, T ratio ) const override;
	XfoT<T> getXfoAtLengthRatio ( u32 curveID, T ratio, u8 directionType = 0 ) const override;
	/** @} */
	/*! @name length*/
	/* @{ */
	void recomputeParamLengthTable ( u32 curveID );
	T getCurveLength ( u32 curveID );
	T getCurveLength ( u32 curveID ) const;

	T getParamFromLength (u32 curveID, T length )const;
	T getParamFromLengthRatio ( u32 curveID, T ratio )const;
	T getNormalizedParamFromLength(u32 curveID, T length)const;
	T getNormalizedParamFromLengthRatio ( u32 curveID, T ratio )const;

	T getLengthAtParam ( u32 curveID, T param )const;
	T getLengthAtNormalizedParam ( u32 curveID, T nParam )const;
	T getLengthAtLengthRatio ( u32 curveID, T ratio )const;

	T getLengthRatioFromLength ( u32 curveID, T length ) const;
	T getLengthRatioAtParam ( u32 curveID, T param )const;
	T getLengthRatioAtNormalizedParam ( u32 curveID, T nParam )const;

	std::vector<T> getEvenlyDivideLengthRatio ( u32 curveID, u32 count ) const;
	std::vector<T> getRegularIntervalLengthRatio ( u32 curveID, f32 _length ) const;
	std::vector<T> getDynamicIntervalLengthRatio ( u32 curveID, f32 _length ) const;
	std::vector<T> getRandomLengthRatio ( u32 curveID, u32 count, u32 seed ) const;
	/** @} */
	/*! @name spatial query*/
	/* @{ */
	LinesSpatialQueryT<T> toSpatialQuery ( u32 curveID )const;
	Vec3T<T> getPositionAtLocation ( const GeometryLocationT<T> &loc ) const;
	Vec3T<T> getPositionAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const;
	Vec3T<T> getTangentAtLocation ( const GeometryLocationT<T> &loc ) const;
	Vec3T<T> getTangentAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const;
	Vec3T<T> getNormalAtLocation ( const GeometryLocationT<T> &loc ) const;
	Vec3T<T> getNormalAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const;
	XfoT<T> getXfoAtLocation ( const GeometryLocationT<T> &loc ) const;
	XfoT<T> getXfoAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const;
	/** @} */
	/*! @name section*/
	/* @{ */
	void createStartSideSections( IO CurveSectionsT<T> &section, u32 curveID, T sectionVal, u8 valType, Bool onSection = false )const;
	void createEndSideSections ( IO CurveSectionsT<T> &section, u32 curveID, T sectionVal, u8 valType, Bool onSection = false )const;
	void createBothEndSections ( IO CurveSectionsT<T> &section, u32 curveID, T sectionVal, u8 valType, Bool onSection = false )const;
	void createEvenlyDivideSections ( IO CurveSectionsT<T> &section, u32 curveID, T divisionSize, Bool onSection = false )const;
	void createRegularIntervalSections ( IO CurveSectionsT<T> &section, u32 curveID, T sectionVal, T intervalVal, u8 valType, Bool onSection = false )const;
	void createDynamicIntervalSections ( IO CurveSectionsT<T> &section, u32 curveID, T sectionVal, T intervalVal, u8 valType, Bool onSection = false )const;
	void createAreaSections ( IO CurveSectionsT<T> &section, u32 curveID, u32 areaSize, T areaVal, u8 valType, Bool onSection = false )const;

	T getSectionLength ( u32 curveID, const CurveSectionsT<T> &section, u32 sectionID ) const;
	/** @} */
};

#include "BSplineCurves.inl"
#include "BSplineCurves_param.inl"
#include "BSplineCurves_value_on_curve.inl"
#include "BSplineCurves_length.inl"
#include "BSplineCurves_spatial_query.inl"
#include "BSplineCurves_section.inl"

/** @} */
/** @} */
using BSplineCurves = BSplineCurvesT<f32>;
using BSplineCurvesd = BSplineCurvesT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************