#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class Vec3T;
template<typename T> class Ray3T;

//******************************************************************************
//! @file	BSph.h
//! @brief	 Bounding Sphere [バウンディング スフィア]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	Bounding Sphere [バウンディング スフィア] <br>
*			alias : \b BSph = BSphT<float>// \b BSphd  = BSphT<double><br>
*			#include <burgess2_main/Math.h> <br>
*			@code
*				BSph bsph ( Vec3 ( 1, 2, 3 ), 1 );
*				Ray3 ray( Vec3 ( 0, 5, 3 ), Vec3 ( -3, -5, -3 ), false );
*				Bool e = bsph.testRayIntersect ( ray );
*			@endcode
*/
// Bounding Sphere [バウンディング スフィア]
template<typename T>
class BSphT {
public:
	Vec3T<T> position;
	T radius;

	BSphT ();
	BSphT ( const Vec3T<T> &position, T _radius );
	virtual ~BSphT () {}

	String toString ( const String title = "" )const;
	
	Bool testRayIntersect ( const Ray3T<T> &ray ) const;
	Bool testClosest ( const Vec3T<T> &_position, T maxDistance ) const;
	Vec3T<T> getClosestPosition ( const Vec3T<T> &_position, T maxDistance = Burgess::F64_INFINITE ) const;

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( position ),
			CEREAL_NVP ( radius )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "BSph.inl"
/** @} */
using BSph = BSphT<f32>;
using BSphd = BSphT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************