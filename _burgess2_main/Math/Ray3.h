#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class XfoT;
template<typename T> class Mat4T;
template<typename T> class Vec3T;

//******************************************************************************
//! @file	Ray3.h
//! @brief	3D-Ray [3Dレイ]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	3D-Ray [3Dレイ]<br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Ray3 = Ray3T<float> <br> \b : Ray3d = Ray3T<double><br>
*			@code
*				Vec3 dir = Vec3 ( 4, 5, 6 ) - Vec3 ( 1, 2, 3 ).unit ();
*				Ray3 ray1 ( Vec3 ( 1, 2, 3 ), dir );
*				Ray3 ray;
*				ray.setStartPosition( Vec3 ( 1, 2, 3 ) );
*				ray.setTargetPosition( Vec3 ( 4, 5, 6 ) );
*
*				Bool e = ray.almostEqual( ray1 );
*			@endcode
*			@date	2019.2 : オリジナル<br>
*/
// 3D-Ray [3Dレイ]
template<typename T>
class Ray3T {
	Vec3T<T> start;
	Vec3T<T> direction;
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	Ray3T(){};
	Ray3T( const Vec3T<T>& start, const Vec3T<T>& directionUnitVector );
	~Ray3T(){};
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline Bool equal( const Ray3T& other ) const;
	inline Bool almostEqual( const Ray3T& other, T tolerance = BURGESS2::TOLERANCE ) const;

	inline String toString( const String title = "" )const;

	inline void setFromStartPositionAndDirection( const Vec3T<T>& start, const Vec3T<T>& direction );
	inline void setFromStartAndTargetPosition( const Vec3T<T>& start, const Vec3T<T>& target );

	inline Vec3T<T> getStartPosition() const;
	inline Vec3T<T> getDirection() const;
	inline Vec3T<T> getPositionOnRay( T distance ) const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	Bool operator == ( const Ray3T& other ) const;
	Bool operator != ( const Ray3T& other ) const;
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize( cerealArchive& archive ){
		archive(
			CEREAL_NVP( start ),
			CEREAL_NVP( direction )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond

};
#include "Ray3.inl"

/** @} */
using Ray3 = Ray3T<f32>;
using Ray3d = Ray3T<f64>;

//******************************************************************************
//! End of File
//******************************************************************************