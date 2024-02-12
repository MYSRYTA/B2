#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class Vec3T;
template<typename T> class VecT;
template<typename T> class Mat4T;
template<typename T> class Mat3T;
template<typename T> class Deg3T;
template<typename T> class Rad3T;

//******************************************************************************
//! @file	Quat.h
//! @brief	 Quaternion [クォータニオン]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	Quaternion [クォータニオン] <br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Quat = QuatT<float> <br> \b : Quatd = QuatT<double><br>
*			@code
*				Vec3 v ( 1.0f, 0.3f, -0.2f );
*				v.setNormal ();
*
*				Quat q;
*				q.setFromAxisAndAngle ( v, 0.1f );
*				Xfoing log = q.toString();	/// log = "QuatT:[0.047, 0.014, -0.009, 0.999]"
*			@endcode
*			@date	2019.2 : オリジナル<br>
*					2021.8 : Eigen::Quaternionをラッピング<br>
*/
// Quaternion [クォータニオン]
template<typename T>
class QuatT : public Eigen::Quaternion<T>{
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	QuatT();
	QuatT( T _x, T _y, T _z, T _w );
	QuatT( const std::vector<T>& arrayVal );
	QuatT( const VecT<T>& vec );
	QuatT( const Vec3T<T>& _v, T _w );
	QuatT( const Vec4T<T>& _v );
	QuatT( const Deg3T<T>& deg );
	QuatT( const Rad3T<T>& rad );
	QuatT( const Mat3T<T>& mat );
	QuatT( const Mat4T<T>& mat );
	QuatT( const QuatT& other );
	QuatT( const Eigen::Matrix<T, 3, 3>& mat );
	QuatT( const Eigen::Quaternion<T>& other );
	~QuatT(){}
	/** @} */
	/*! @name XYZW*/
	/* @{ */
	inline T& x();
	inline const T& x() const;
	inline T& y();
	inline const T& y() const;
	inline T& z();
	inline const T& z() const;
	inline T& w();
	inline const T& w() const;
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline VecT<T> toVec() const;
	inline std::vector<T> toTypeArray()const;
	inline Mat3T<T> toMat3() const;
	inline Rad3T<T> toRad3( BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ ) const;
	inline Deg3T<T> toDeg3( BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ ) const;
	inline String toString( const String title = "" )const;

	inline Bool equal( const QuatT& other ) const;
	inline Bool almostEqual( const QuatT& other, T tolerance = BURGESS2::TOLERANCE ) const;
	inline Bool isZero()const;
	inline Bool almostZero( T tolerance = BURGESS2::TOLERANCE )const;

	inline void setIdentity();
	inline void set( const Vec3T<T>& _v, T _w );
	inline void setFromMat3( const Mat3T<T>& mat );
	inline void setFromRad3( const Rad3T<T>& rad );
	inline void setFromDeg3( const Deg3T<T>& deg );
	inline void setFromRadian( const Vec3T<T>& radVal, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	inline void setFromDegree( const Vec3T<T>& degVal, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	inline void setFromAxisAndAngle( const Vec3T<T>& axis, T angle );
	inline void setFromDirectionAndUpvector( const Vec3T<T>& direction, const Vec3T<T>& upvector, u8 type = 0, Bool dirNeg = false, Bool upVectorNeg = false );
	inline void setFrom2Vectors( const Vec3T<T>& sourceVec3, const Vec3T<T>& targetVec3 );
	inline void setFromExpmap( const Vec3T<T>& _expmap );

	inline QuatT<T> addAngle( const Deg3T<T>& deg ) const;
	inline QuatT<T> addAngle( const Rad3T<T>& rad ) const;

	inline Vec3T<T> getXaxis() const;
	inline Vec3T<T> getYaxis() const;
	inline Vec3T<T> getZaxis() const;
	inline Vec3T<T> getAxis() const;
	inline T getAngle() const;

	inline QuatT lerp( const QuatT& other, T t )const;
	inline QuatT linearInterpolate( const QuatT& other, T t )const;
	inline QuatT slerp( const QuatT& other, T t )const;
	inline QuatT sphericalLinearInterpolate( const QuatT& other, T t )const;

	inline T length() const;
	inline T lengthSquared() const;
	inline T dist( const QuatT<T>& other )const;

	inline QuatT normal() const;
	inline void setNormal();
	inline QuatT unit() const;
	inline void setUnit();

	inline QuatT inverse() const;
	inline void setInverse();
	inline QuatT mirror( const BURGESS2::Axis& mirrorAxis )const;
	inline void setMirror( const BURGESS2::Axis& mirrorAxis );
	inline QuatT mirror2( const BURGESS2::Axis& mirrorAxis )const;
	inline void setMirror2( const BURGESS2::Axis& mirrorAxis );

	inline QuatT conjugate() const;
	inline Vec3T<T> expmap()const;
	inline T dot( const QuatT& other ) const;
	inline Vec3T<T> rotateVector( const Vec3T<T>& vec3 ) const;
	/// @cond
	inline Vec3T<T> rotatVector( const Vec3T<T>& vec3 ) const;
	/// @endcond

	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline QuatT& operator = ( const Eigen::Matrix<T, 3, 3>& other );
	inline QuatT& operator = ( const Eigen::Quaternion<T>& other );

	inline T& operator[]( u8 id );
	inline const T& operator[]( u8 id ) const;

	inline Bool operator == ( const QuatT& other ) const;
	inline Bool operator != ( const QuatT& other ) const;

	inline QuatT operator * ( const QuatT& other ) const;
	inline QuatT operator * ( T val ) const;
	inline void operator *= ( const QuatT& other );
	inline void operator *= ( T val );
	inline QuatT operator / ( const QuatT& other ) const;
	inline QuatT operator / ( T  val ) const;
	inline void operator /= ( const QuatT& other );
	inline void operator /= ( T val );
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/*template<class cerealArchive>
	void serialize( cerealArchive& archive ){
		archive(
			CEREAL_NVP( x() ),
			CEREAL_NVP( y() ),
			CEREAL_NVP( z() ),
			CEREAL_NVP( w() )
		);
	}*/

	template<class cerealArchive>
	void save( cerealArchive& archive )const{
		// 旧Mathに合わせるために、保存対象のデータを一旦変数に格納する
		Vec3T<T> v{ this->x(), this->y(), this->z() };
		T w = this->w();
		archive(
			CEREAL_NVP( v ),
			CEREAL_NVP( w )
		);
	}

	template<class cerealArchive>
	void load( cerealArchive& archive ){
		Vec3T<T> v;
		T w;
		archive(
			CEREAL_NVP( v ),
			CEREAL_NVP( w )
		);
		*this = QuatT<T>( v, w );
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};
#include "Quat.inl"

/** @} */
using Quat = QuatT<f32>;
using Quatd = QuatT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************