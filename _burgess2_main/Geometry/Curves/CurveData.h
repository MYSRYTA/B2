#pragma once
#include <burgess2_main/Geometry/Attributes/Attributes.h>

//******************************************************************************
//! @file	CurveData.h
//! @brief	 Curveの基底クラス
//******************************************************************************
/** @defgroup Geometry
*	   @{
*/
/**		@defgroup Curves
*			@{
*/
/**				@defgroup CurveCore
*					@{
*/
/**				@brief カーブの基本データ <br>
*					#include <burgess2_main/Curves.h><br>
*/

template<typename T>
class CurveDataT{
	u8 type;
	u8 form;
	u8 sample = 20;
	Vec2T<u32> paramLengthTableVirsion;
public:
	enum CurveFrom{
		Open = u8 ( 0 ),
		Close,
		Periodic
	};

	enum CurveType{
		Linear = u8 ( 0 ),
		BSpline,
		NURBS,
		CatmullRom,
	};

	u8  v[5]; //!< ストック
	AttributesT<T> attr; //!< 各種アトリビュートのコンテナ

	CurveDataT ();
	virtual ~CurveDataT () {}

	void setForm ( u8 _form );
	void setSample ( u8 _sample );

	u8 getForm () const;
	u8 getSample () const;

	// Vec2Attribute "paramLengthTable"
	Bool paramLengthTableIsDirty ()const;
	void paramLengthTableCached ();
	void incrementParamLengthTableVirsion ();
};

#include "CurveData.inl"
using CurveData = CurveDataT<f32>;
using CurveDatad = CurveDataT<f64>;

/*
//				@brief Bezierの基本データ <br>
//				#include <burgess2_main/Geometry.h><br>
//
template<typename T>
struct BezierControlsT{
public:
	std::vector<Vec3T<T>> points;
	u32 segmentCount;

	BezierControlsT (){}
	BezierControlsT ( u32 n ){
		this->segmentCount = n;
		this->points.resize ( n * 2 + 1 );
	}
	virtual ~BezierControlsT (){}

	void set ( u32 i, u32 j, Vec3T<T> val ){ this->points[i * 2 + j] = val; }
	Vec3T<T> get(u32 i, u32 j)const{ return this->points[i * 2 + j]; }
};

//				@brief Bezierの基本データ <br>
//				#include <burgess2_main/Geometry/Curves.h><br>
//
template<typename T>
struct ExtendedPlayerControlsT{
public:
	Vec3T<T> top;
	std::vector<Vec3T<T>> ps;
	Vec3T<T> bottom;

	ExtendedPlayerControlsT (){}
	ExtendedPlayerControlsT ( const std::vector<Vec3T<T>> &ps, const BezierControlsT<T> &cs ){
		this->top = cs.get(cs.segmentCount - 1, 1);
		this->ps = ps;
		this->bottom = cs.get(0, 1);
	}
	virtual ~ExtendedPlayerControlsT (){}

	u32 length () const{ return ps.size () + 2; }
	u32 baseLength () const{ return ps.size (); }

	void set ( u32 i, const Vec3T<T> &val ){
		if ( i == 0 ) this->top = val;
		else if ( i == (ps.size () + 1) ) this->bottom = val;
		else this->ps[i - 1] = val;
	}

	Vec3T<T> get ( u32 i ) const{
		return i == 0 ? top : i == (this->ps.size () + 1) ? this->bottom : ps[i - 1];
	}
};

//			@brief Bezierの基本データ <br>
//					#include <burgess2_main/Geometry/Curves.h><br>
//
template<typename T>
struct ExtendedBezierControlsT{
public:
	Vec3T<T> top;
	std::vector<Vec3T<T>> cs;
	Vec3T<T> bottom;
	u32 baseLength;

	ExtendedBezierControlsT(){}
	ExtendedBezierControlsT( const BezierControlsT<T> &v ){
		this->baseLength = v.segmentCount;
		this->top = v.get(this->baseLength - 1, 1);
		this->cs = v.points;
		this->bottom = v.get(0, 1);
	}
	virtual ~ExtendedBezierControlsT(){}
	
	u32 length ()const{ return this->baseLength + 2; }

	void set ( u32 i, const Vec3T<T> &val ){
		if ( i == 0 ) this->top = val;
		else if ( i == (this->baseLength + 1) ) this->bottom = val;
		else this->cs[i * 2 - 1] = val;
	}

	Vec3T<T> get ( u32 i ) const{
		return i == 0 ? this->top : i == (this->baseLength + 1) ? this->bottom : this->cs[i * 2 - 1];
	}
};
*/
/** @} */
/** @} */
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************