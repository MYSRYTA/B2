#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class Vec3T;
template<typename T> class Ray3T;

//******************************************************************************
//! @file	BSph.h
//! @brief	 Bounding Sphere [�o�E���f�B���O �X�t�B�A]
//******************************************************************************
/** @defgroup Math
*�@@{
*/
/**		@brief	Bounding Sphere [�o�E���f�B���O �X�t�B�A] <br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : BSph = BSphT<float> <br> \b : BSphd  = BSphT<double><br>
*			@code
*				BSph bsph ( Vec3 ( 1, 2, 3 ), 1 );
*				Ray3 ray( Vec3 ( 0, 5, 3 ), Vec3 ( -3, -5, -3 ), false );
*				Bool e = bsph.testRayIntersect ( ray );
*			@endcode
*			@date	2019.2 : �I���W�i��<br>
*/
// Bounding Sphere [�o�E���f�B���O �X�t�B�A]
template<typename T>
class BSphT {
public:
	Vec3T<T> position;
	T radius;

	/*! @name Constructor/Destructor */
	/* @{ */
	BSphT() {};
	BSphT ( const Vec3T<T>& position, T _radius );
	~BSphT () {}
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline String toString ( const String title = "" )const;

	inline Bool testRayIntersect ( const Ray3T<T>& ray ) const;
	inline Bool testClosest ( const Vec3T<T>& _position, T maxDistance ) const;
	inline Vec3T<T> getClosestPosition ( const Vec3T<T>& _position, T maxDistance = BURGESS2::F64_INFINITE ) const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline Bool operator == ( const BSphT& other ) const;
	inline Bool operator != ( const BSphT& other ) const;
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize( cerealArchive& archive ){
		archive(
			CEREAL_NVP( position ),
			CEREAL_NVP( radius )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

#include "BSph.inl"
/** @} */
using BSph = BSphT<f32>;
using BSphd = BSphT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************