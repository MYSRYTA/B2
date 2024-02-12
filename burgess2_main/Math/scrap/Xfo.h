#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class Vec3T;
template<typename T> class Rad3T;
template<typename T> class Deg3T;
template<typename T> class QuatT;
template<typename T> class Mat4T;
template<typename T> class Mat3T;

//******************************************************************************
//! @file	Xfo.h
//! @brief	Xform
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	Xform  <br>
*			@ref Vec3T<T> "Vec3T<T>" : tr <br> @ref QuatT<T> "QuatT<T>" : ori <br> @ref Vec3T<T> "Vec3T<T>" : sc <br>
*			alias : \b Xfo = XfoT<float>// \b Xfod = XfoT<double> <br>
*			#include <burgess2_main/Math.h> <br>
*			@code
*				Vec3 s ( 1.0f, 1.0f, 1.0f );
*				Vec3 t ( 0.01f, 0.01f, 0.01f );
*				Deg3 deg( 130.f, -20.f, 30.f, 0 );
*				Rad3 rad ( 2.268928f, -0.349065f, 0.523598f, 0 );	//  = Deg3 ( 130.f, -20.f, 30.f );
*				Quat quat ( 0.88112f, 0.160119f, 0.259736f, 0.361283f );	//  = Deg3 ( 130.f, -20.f, 30.f );
*		
*				Xfo Xfo ( s, quat, t );
*				Xfo Xfo2 ( s, rad, t );
*				Xfo Xfo3 ( s, deg, t );
*				Bool e = Xfo.almostEqual( Xfo2 ) && Xfo.almostEqual( Xfo3 );
*			@endcode
*/
// Transform [トランスフォーム]
template<typename T>
class XfoT
{
public:
	Vec3T<T> sc;
	QuatT<T> ori; // quaternion rotation
	Vec3T<T> tr;

	XfoT ();
	XfoT ( const Vec3T<T> &_tr );
	XfoT ( const QuatT<T> &_ori );
	XfoT ( const Deg3T<T> &_ori );
	XfoT ( const Rad3T<T> &_ori );
	XfoT (
		const Vec3T<T> &_sc,
		const QuatT<T> &_ori,
		const Vec3T<T> &_tr
	);
	XfoT (
		const Vec3T<T> &_sc,
		const Deg3T<T> &_ori,
		const Vec3T<T> &_tr
	);
	XfoT (
		const Vec3T<T> &_sc,
		const Rad3T<T> &_ori,
		const Vec3T<T> &_tr
	);
	XfoT ( const Mat4T<T> &mat4 );
	XfoT ( const Mat3T<T> &mat3 );
	XfoT ( const SRTT<T> &srt );
	virtual ~XfoT () {};
	
	void setIdentity ();

	Mat4T<T> toMat4 () const;
	String toString ( const String title = "" )const;

	Bool equal ( const XfoT &other ) const;
	Bool almostEqual ( const XfoT &other, T tolerance = Burgess::TOLERANCE )const;

	XfoT inverse () const;
	void setInverse ();

	XfoT lerp ( const XfoT &other, T t ) const;
	XfoT linearInterpolate ( const XfoT &other, T t ) const;
	XfoT slerp ( const XfoT &other, T t ) const;
	XfoT sphericalLinearInterpolate ( const XfoT &other, T t ) const;

	void addSc ( const Vec3T<T> &val );
	void addRot ( const QuatT<T> &val );
	void addRot ( const Deg3T<T>& val );
	void addRot ( const Rad3T<T> &val );
	void addTr ( const Vec3T<T> &val );

	Vec3T<T> transformVector ( const Vec3T<T> &val )const;

	/*! @name Operator*/
	/* @{ */
	Bool operator == ( const XfoT &other ) const;
	Bool operator != ( const XfoT &other ) const;

	XfoT operator * ( const XfoT &other ) const;
	void operator *= ( const XfoT &other );
	Vec3T<T> operator * ( const Vec3T<T> &vec ) const;
	/* @{ */


	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( sc ),
			CEREAL_NVP ( ori ),
			CEREAL_NVP ( tr )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Xfo.inl"

/** @} */

using Xfo = XfoT<f32>;
using Xfod = XfoT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************