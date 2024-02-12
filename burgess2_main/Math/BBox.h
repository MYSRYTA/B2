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
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : BBox = BBoxT<float> <br> \b : BBoxd = BBoxT<double><br>
*			@code
*				BBox aa ( Vec3 ( -1, -20, -3 ), Vec3 ( 40, 5, 60 ) );
*				BBox bb ( Vec3 ( -10, -2, -30 ), Vec3 ( 4, 50, 6 ) );
*				aa.merge ( bb );
*				Ray3 ray ( Vec3 ( 2,3,4 ), Vec3 ( -1,-2,-3 ), false );
*				Vec3 pos = aa.getRayIntersectPosition ( ray );
*			@endcode
*			@date	2019.2 : オリジナル<br>
*/
// Bounding Box [バウンディング ボックス]
template<typename T>
class BBoxT {
public:
	Vec3T<T> min;
	Vec3T<T> max;

	/*! @name Constructor/Destructor */
	/* @{ */
	BBoxT () {};
	BBoxT ( const Vec3T<T>& val0, const Vec3T<T>& val1 );
	BBoxT ( const std::vector<Vec3T<T>>& points );
	BBoxT ( const Vec3T<T>& centerPosition, T sideLength );
	BBoxT ( const BBoxT<T>& other );
	~BBoxT () {}
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline String toString ( const String title = "" )const;

	inline void setEmpty ();
	inline void merge ( const Vec3T<T>& position );
	inline void merge ( const BBoxT& bbox );

	inline Vec3T<T> getCenter () const;
	inline Vec3T<T> getDiagonal ()const;
	inline T getDiameter () const;
	inline T getVolume () const;
	inline T getLongestSideLength ()const;

	inline Bool isEmpty () const;
	inline Bool testInside ( const Vec3T<T>& p )const;
	inline Bool testIntersect ( const BBoxT<T>& other )const;

	inline Bool testRayIntersect ( const Ray3T<T>& ray ) const;
	inline Vec3T<T> getRayIntersectPosition ( const Ray3T<T>& ray ) const;
	inline Bool testClosest ( const Vec3T<T>& position, T maxDistance ) const;
	inline Vec3T<T> getClosestPosition ( const Vec3T<T>& position, T maxDistance ) const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline Bool operator == ( const BBoxT& other ) const;
	inline Bool operator != ( const BBoxT& other ) const;
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize( cerealArchive& archive ){
		archive(
			CEREAL_NVP( min ),
			CEREAL_NVP( max )
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