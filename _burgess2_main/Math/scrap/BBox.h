#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class Vec3T;
template<typename T> class Mat4T;
template<typename T> class Ray3T;
template<typename T> class XfoT;

//******************************************************************************
//! @file	BBox.h
//! @brief	 Bounding Box [バウンディング ボックス]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	Bounding Box [バウンディング ボックス] <br>
*			alias : \b BBox = BBoxT<float> // \b BBoxd = BBoxT<double><br>
*			#include <burgess2_main/Math.h> <br>
*			@code
*				BBox aa ( Vec3 ( -1, -20, -3 ), Vec3 ( 40, 5, 60 ) );
*				BBox bb ( Vec3 ( -10, -2, -30 ), Vec3 ( 4, 50, 6 ) );
*				aa.merge ( bb );
*				Ray3 ray ( Vec3 ( 2,3,4 ), Vec3 ( -1,-2,-3 ), false );
*				Vec3 pos = aa.getRayIntersectPosition ( ray );
*			@endcode
*/
// Bounding Box [バウンディング ボックス]
template<typename T>
class BBoxT{
public:
	Vec3T<T> min;
	Vec3T<T> max;

	BBoxT ();
	BBoxT ( const Vec3T<T> &val0, const Vec3T<T> &val1 );
	BBoxT ( const std::vector<Vec3T<T>> &points );
	BBoxT ( const Vec3T<T> &centerPosition, T sideLength );
	BBoxT ( const BBoxT<T> &other );
	virtual ~BBoxT (){}

	void setEmpty ();

	String toString ( const String title = "" )const;

	void merge ( const Vec3T<T> &position );
	void merge ( const BBoxT &bbox );

	Vec3T<T> getCenter () const;
	Vec3T<T> getDiagonal ()const;
	T getDiameter () const;
	T getVolume () const;
	T getLongestSideLength ()const;

	Bool isEmpty () const;
	Bool testInside ( const Vec3T<T> &p )const;
	Bool testIntersect ( const BBoxT<T> &other )const;

	Bool testRayIntersect ( const Ray3T<T> &ray ) const;
	Vec3T<T> getRayIntersectPosition ( const Ray3T<T> &ray ) const;
	Bool testClosest ( const Vec3T<T> &position, T maxDistance ) const;
	Vec3T<T> getClosestPosition ( const Vec3T<T> &position, T maxDistance ) const;

	/*! @name Operator*/
	/* @{ */
	Bool operator == ( const BBoxT &other ) const;
	Bool operator != ( const BBoxT &other ) const;

	BBoxT operator * ( const Mat4T<T> &mat4 ) const;
	BBoxT operator * ( const XfoT<T> &Xfo ) const;
	BBoxT operator * ( const Vec3T<T> &scale ) const;
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( min ),
			CEREAL_NVP ( max )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "BBox.inl"
/** @} */
using BBox = BBoxT<f32>;
using BBoxd = BBoxT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************