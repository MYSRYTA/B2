#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class Vec3T;
template<typename T> class Rad3T;
template<typename T> class Deg3T;
template<typename T> class QuatT;
template<typename T> class Mat4T;
template<typename T> class Mat3T;
template<typename T> class XfoT;

//******************************************************************************
//! @file	SRT.h
//! @brief	Transform
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	Transform  <br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : SRT = SRTT<float> <br> \b : SRTd = SRTT<double> <br>
*			@code
*				Vec3 s ( 1.0f, 1.0f, 1.0f );
*				Vec3 t ( 0.01f, 0.01f, 0.01f );
*				Deg3 deg( 130.f, -20.f, 30.f, 0 );
*				Rad3 rad ( 2.268928f, -0.349065f, 0.523598f, 0 );	//  = Deg3 ( 130.f, -20.f, 30.f );
*				Quat quat ( 0.88112f, 0.160119f, 0.259736f, 0.361283f );	//  = Deg3 ( 130.f, -20.f, 30.f );
*		
*				SRT srt ( s, quat, t );
*				SRT srt2 ( s, rad, t );
*				SRT srt3 ( s, deg, t );
*				Bool e = srt.almostEqual( srt2 ) && SRT.almostEqual( srt3 );
*			@endcode
*			@date	2019.2 : オリジナル<br>
*/
// Transform [トランスフォーム]
template<typename T>
class SRTT{
public:
	Vec3T<T> sc;
	Deg3T<T> rot;
	Vec3T<T> tr;

	/*! @name Constructor/Destructor */
	/* @{ */
	SRTT(){};
	SRTT( const Vec3T<T> & _tr );
	SRTT( const QuatT<T> & _rot, const BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	SRTT( const Deg3T<T> & _rot );
	SRTT( const Rad3T<T> & _rot );
	SRTT( const Vec3T<T> & _sc, const QuatT<T> & _rot, const Vec3T<T> & _tr, const BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ	);
	SRTT( const Vec3T<T> & _sc, const Deg3T<T> & _rot, const Vec3T<T> & _tr	);
	SRTT( const Vec3T<T> & _sc, const Rad3T<T> & _rot, const Vec3T<T> & _tr	);
	SRTT( const Mat4T<T> & mat4, const BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	SRTT( const Mat3T<T> & mat3, const BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	SRTT( const XfoT<T> & xfo, const BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	~SRTT(){};
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline void setIdentity();

	inline Mat4T<T> toMat4() const;
	inline XfoT<T> toXfo() const;
	inline String toString( const String title = "" )const;

	inline Bool equal( const SRTT<T> & other ) const;
	inline Bool almostEqual( const SRTT<T> & other, T tolerance = BURGESS2::TOLERANCE )const;

	inline SRTT<T> slerp( const SRTT<T> & other, T t ) const;
	inline SRTT<T> sphericalLinearInterpolate( const SRTT<T> & other, T t ) const;

	inline SRTT<T> inverse() const;
	inline void setInverse();

	inline Vec3T<T> transformVector( const Vec3T<T> & val )const;
	/* @{ */
	/*! @name Operator*/
	/* @{ */
	inline Bool operator == ( const SRTT<T> & other ) const;
	inline Bool operator != ( const SRTT<T> & other ) const;

	inline SRTT<T> operator * ( const SRTT<T> & other ) const;
	inline void operator *= ( const SRTT<T> & other );
	inline Vec3T<T> operator * ( const Vec3T<T> & vec ) const;
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize( cerealArchive& archive ){
		archive(
			CEREAL_NVP( sc ),
			CEREAL_NVP( rot ),
			CEREAL_NVP( tr )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond

};
#include "SRT.inl"

/** @} */
using SRT = SRTT<f32>;
using SRTd = SRTT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************