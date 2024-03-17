#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class Vec3T;

//******************************************************************************
//! @file	GeometryLocation.h
//! @brief	 ジオメトリ上のロケーションを表す
//******************************************************************************
/**@defgroup Containers
*	 @{
*/
/**	@brief	ジオメトリ上のロケーションを表す <br>
*		#include <burgess2_main/Containers.h> <br><br>
*		#alias <br> \b : GeometryLocation <br> \b : GeometryLocationd <br>
*/

template<typename T>
class GeometryLocationT {
	u32 index; // pointID / CurveID / PolygonFaceID
	s8 subIndex; // PolygonTiangle ID
	Vec3T<T> weight; // .x = pointWeight / .x = curvelengthRatio / .xyz = 三角の各頂点への相対的ウェイト値
	u8 geoType = 0; // 0=not valid, 1=Points, 2=Curves, 3=PolygonMesh
protected:
	void setValues ( u32 _index, s8 _subIndex, const Vec3T<T> &_weight, u8 _type );
	void resetIndex ( u32 _index );
	void resetSubIndex ( s8 _subIndex );
	void resetWeight ( const Vec3T<T> &_weight );
public:
	GeometryLocationT<T> () {}
	GeometryLocationT<T> ( u32 _index, Vec3T<T> _weight, u8 _type );
	GeometryLocationT<T> ( u32 _index, s8 _subIndex, Vec3T<T> _weight, u8 _type );
	virtual ~GeometryLocationT<T> () {}

	Bool almostEqual( const GeometryLocationT<T>& other, T tolerance = BURGESS2::TOLERANCE )const;

	u8 getType () const;
	u32 getIndex () const;
	u8 getSubIndex () const;
	Vec3T<T> getWeight () const;
	T getCurveLengthRatio () const;

	String toString( const String title = "" )const;
	virtual Bool isValid ()const;

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( index ),
			CEREAL_NVP ( subIndex ),
			CEREAL_NVP ( weight ),
			CEREAL_NVP ( geoType )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "GeometryLocation.inl"
/** @} */

using GeometryLocation = GeometryLocationT<f32>;
using GeometryLocationd = GeometryLocationT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************