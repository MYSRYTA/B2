#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Math.h>

//******************************************************************************
//! @file	CurveSections.h
//! @brief	 カーブ上の一定区間を示す
//******************************************************************************
/**@defgroup Containers
*	@{
*/
/**	@brief	カーブ上の一定区間を示す <br>
*		#include <burgess2_main/Containers.h> <br><br>
*		#alias <br> \b : CurveSections <br> \b : CurveSectionsd <br>
*		区間のstart/endの値（LengthRatio）を @ref Vec2T<T> "Vec2T<T>" ( start, end )に格納する <br>
*/

template<typename T>
class CurveSectionsT {
	std::vector<Vec2T<T>> values;
	u32 index;
	Bool dirty; /*不正な値が設定されていないかチェックするためのフラグ*/
public:
	CurveSectionsT () {}
	CurveSectionsT ( const std::vector<Vec2T<T>> &val );
	CurveSectionsT ( u32 size );
	~CurveSectionsT () {}

	void resize ( u32 size );

	u32 add ( const Vec2T<T> &v );
	u32 add ( T start, T end );

	void setValue ( u32 sectionID, Vec2T<T> v );
	void setValue ( u32 sectionID, T start, T end );
	void setAllValues (const  std::vector<Vec2T<T>> &val );
	void resetAllValues ( const std::vector<Vec2T<T>> &val );
	void setIndex ( u32 _index );

	Vec2T<T> getValue ( u32 sectionID ) const;
	std::vector<Vec2T<T>> getAllValues ()const;
	T getStart ( u32 sectionID ) const;
	T getCenter ( u32 sectionID ) const;
	T getEnd ( u32 sectionID )const;
	T getLengthRatio ( u32 sectionID )const;
	u32 getIndex ()const;

	void clear ();

	u32 sectionCount ()const;

	void duplicate ( u32 sectionID );
	void copyFrom ( const CurveSectionsT<T> &source );
	void copySectionFrom ( const CurveSectionsT<T> &source, u32 sectionID );

	void cleanup ();

	void setDirty ();
	Bool isValid ();

	T changeRange ( u32 sectionID, T origRatio )const;

	/*! @name Modify*/
	/* @{ */
	Bool flip ();

	void scaling ( u32 sectionID, T scale, Bool clamp = true );
	void allScaling ( T scale, Bool clamp = true );

	void addStartEndValue ( u32 sectionID, T startSide, T endSide, Bool clamp = true );
	void addAllStartEndValues ( T startSide, T endSide, Bool clamp = true );
	/* @{ */
};

#include "CurveSections.inl"
#include "CurveSections_modify.inl"
/** @} */

using CurveSections = CurveSectionsT<f32>;
using CurveSectionsd = CurveSectionsT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************