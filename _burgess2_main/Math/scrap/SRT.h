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
*			@ref Vec3T<T> "Vec3T<T>" : tr <br> @ref Deg3T<T> "Deg3T<T>" : rot <br> @ref Vec3T<T> "Vec3T<T>" : sc <br>
*			alias : \b SRT = SRTT<float>// \b SRTd = SRTT<double> <br>
*			#include <burgess2_main/Math.h> <br>
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
*/
// Transform [トランスフォーム]
template<typename T>
class SRTT
{
public:
	Vec3T<T> sc;
	Deg3T<T> rot;
	Vec3T<T> tr;

	SRTT ();
	SRTT ( const Vec3T<T> &_tr );
	SRTT ( const QuatT<T> &_rot, u8 order = 0 );
	SRTT ( const Deg3T<T> &_rot );
	SRTT ( const Rad3T<T> &_rot );
	SRTT (
		const Vec3T<T> &_sc,
		const QuatT<T> &_rot,
		const Vec3T<T> &_tr,
		u8 order = 0
	);
	SRTT (
		const Vec3T<T> &_sc,
		const Deg3T<T> &_rot,
		const Vec3T<T> &_tr
	);
	SRTT (
		const Vec3T<T> &_sc,
		const Rad3T<T> &_rot,
		const Vec3T<T> &_tr
	);
	SRTT ( const Mat4T<T> &mat4, u8 order = 0 );
	SRTT ( const Mat3T<T> &mat3, u8 order = 0 );
	SRTT ( const XfoT<T> &mat3, u8 order = 0 );

	virtual ~SRTT () {};
	
	void setIdentity ();

	Mat4T<T> toMat4 () const;
	String toString ( const String title = "" )const;

	Bool equal ( const SRTT &other ) const;
	Bool almostEqual ( const SRTT &other, T tolerance = Burgess::TOLERANCE )const;

	SRTT lerp ( const SRTT &other, T t ) const;
	SRTT linearInterpolate ( const SRTT &other, T t ) const;
	SRTT slerp ( const SRTT &other, T t ) const;
	SRTT sphericalLinearInterpolate ( const SRTT &other, T t ) const;

	void addSc ( const Vec3T<T> &val );
	void addRot ( const QuatT<T> &val, u8 order = 0 );
	void addRot ( const Deg3T<T>& val );
	void addRot ( const Rad3T<T> &val );
	void addTr ( const Vec3T<T> &val );

	Vec3T<T> transformVector ( const Vec3T<T> &val )const;

	/*! @name Operator*/
	/* @{ */
	Bool operator == ( const SRTT &other ) const;
	Bool operator != ( const SRTT &other ) const;

	SRTT operator * ( const SRTT &other ) const;
	void operator *= ( const SRTT &other );
	Vec3T<T> operator * ( const Vec3T<T> &vec ) const;
	/* @{ */


	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( sc ),
			CEREAL_NVP ( rot ),
			CEREAL_NVP ( tr )
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