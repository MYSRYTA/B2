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
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Xfo = XfoT<float> <br> \b : Xfod = XfoT<double> <br>
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
*			@date	2019.2 : オリジナル<br>
*/
// Transform [トランスフォーム]
template<typename T>
class XfoT {
public:
	Vec3T<T> sc{ T( 1.0 ) };
	QuatT<T> ori;
	Vec3T<T> tr;

	/*! @name Constructor/Destructor */
	/* @{ */
	XfoT(){};
	XfoT( const Vec3T<T>& _tr );
	XfoT( const QuatT<T>& _ori );
	XfoT( const Deg3T<T>& _ori );
	XfoT( const Rad3T<T>& _ori );
	XfoT( const Mat4T<T>& mat4 );
	XfoT( const Mat3T<T>& mat3 );
	XfoT( const SRTT<T>& srt );
	XfoT( const Vec3T<T>& _sc, const QuatT<T>& _ori, const Vec3T<T>& _tr );
	XfoT( const Vec3T<T>& _sc, const Deg3T<T>& _ori, const Vec3T<T>& _tr );
	XfoT( const Vec3T<T>& _sc, const Rad3T<T>& _ori, const Vec3T<T>& _tr );
	virtual ~XfoT() {};
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline void setIdentity();

	inline Mat4T<T> toMat4() const;
	inline SRTT<T> toSRT() const;
	inline String toString( const String title = "" )const;

	inline Bool equal( const XfoT<T>& other ) const;
	inline Bool almostEqual( const XfoT<T> & other, T tolerance = BURGESS2::TOLERANCE )const;

	inline XfoT<T> lerp( const XfoT<T>& other, T t ) const;
	inline XfoT<T> linearInterpolate( const XfoT<T>& other, T t ) const;
	inline XfoT<T> slerp( const XfoT<T> & other, T t ) const;
	inline XfoT<T> sphericalLinearInterpolate( const XfoT<T> & other, T t ) const;

	inline XfoT<T> inverse() const;
	inline void setInverse();

	inline Vec3T<T> transformVector( const Vec3T<T>& val )const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline Bool operator == ( const XfoT<T> & other ) const;
	inline Bool operator != ( const XfoT<T> & other ) const;

	inline XfoT<T> operator * ( const XfoT<T> & other ) const;
	inline void operator *= ( const XfoT<T> & other );
	inline Vec3T<T> operator * ( const Vec3T<T>& vec ) const;
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize( cerealArchive& archive ){
		archive(
			CEREAL_NVP( sc ),
			CEREAL_NVP( ori ),
			CEREAL_NVP( tr )
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