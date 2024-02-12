#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;
template<typename T> class QuatT;
template<typename T> class Vec3T;
template<typename T> class Deg3T;

//******************************************************************************
//! @file	Rad3.h
//! @brief	 Euler (radian) [オイラー(ラジアン角)]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	Euler (radian) [オイラー(ラジアン角)]<br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Rad3 = Rad3T<float> <br> \b : Rad3d = Rad3T<double><br>
*			@code
*				Mat3 m3 { 0.813797f, 0.094492f, 0.573414f, 0.469846f, -0.687671f, -0.55349f, 0.34202f, 0.719846f, -0.604022f };
*				Rad3 r1;
*				r1.setFromMat3 ( m3, 0 );
*				Rad r2 = Rad3 { 2.268928f,  -0.349065f, 0.523598f, 0 };
*				Bool e = r1.almostEqual( r2 );
*			@endcode
*			@date	2019.2 : オリジナル<br>
*					2021.8 : Eigen::Vectorをラッピング<br>
*/
// Euler (radian) [オイラー(ラジアン角)]
template<typename T>
class Rad3T : public Eigen::Vector<T, 3> {
	BURGESS2::RotationOrder rotationOrder = BURGESS2::RotationOrder::XYZ;
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	Rad3T ();
	Rad3T ( T val, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Rad3T ( T _x, T _y, T _z, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Rad3T ( const std::vector<T>& arrayVal, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Rad3T ( const VecT<T>& vec, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Rad3T ( const Vec3T<T>& vec3, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Rad3T ( const QuatT<T>& quat, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Rad3T ( const Mat3T<T>& mat3, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Rad3T ( const Mat4T<T>& mat4, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Rad3T ( const Deg3T<T>& deg );
	Rad3T( const Eigen::Vector<T, 3>& other );
	~Rad3T () {}
	/** @} */
	/*! @name XYZ*/
	/* @{ */
	inline T& x ();
	inline const T& x () const;
	inline T& y ();
	inline const T& y () const;
	inline T& z ();
	inline const T& z () const;
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline VecT<T> toVec () const;
	inline std::vector<T> toTypeArray () const;
	inline Deg3T<T> toDeg3 ()const;
	inline QuatT<T> toQuat ()const;
	inline Mat3T<T> toMat3 ()const;
	inline String toString ( const String title = "" )const;

	inline Bool equal ( const Rad3T& other ) const;
	inline Bool almostEqual ( const Rad3T& other, T tolerance = BURGESS2::TOLERANCE ) const;

	inline void setFromMat3 ( const Mat3T<T>& mat3, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	inline void setFromQuat ( const QuatT<T>& quat, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	inline void setFromDeg3 ( const Deg3T<T>& deg3 );

	inline BURGESS2::RotationOrder getRotationOrder()const;
	inline u8 getRotationOrderU8()const;

	inline Vec3T<T> rotatVector ( const Vec3T<T>& val ) const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline Rad3T& operator = ( const Eigen::Vector<T, 3>& other );

	inline T& operator[]( u8 id );
	inline const T& operator[]( u8 id ) const;

	inline Bool operator == ( const Rad3T& other )const;
	inline Bool operator != ( const Rad3T& other )const;

	inline Rad3T operator + ( const Rad3T& other )const;
	inline void operator += ( const Rad3T& other );
	inline Rad3T operator - ( const Rad3T& other ) const;
	inline void operator -= ( const Rad3T& other );
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/*template<class cerealArchive>
	void serialize( cerealArchive& archive ){
		archive(
			CEREAL_NVP( x() ),
			CEREAL_NVP( y() ),
			CEREAL_NVP( z() ),
			CEREAL_NVP( rotationOrder )
		);
	}*/

	template<class cerealArchive>
	void save( cerealArchive& archive )const{
		// 旧Mathに合わせるために、保存対象のデータを一旦変数に格納する
		T x = this->x();
		T y = this->y();
		T z = this->z();
		archive(
			CEREAL_NVP( x ),
			CEREAL_NVP( y ),
			CEREAL_NVP( z ),
			CEREAL_NVP( rotationOrder )
		);
	}

	template<class cerealArchive>
	void load( cerealArchive& archive ){
		T x, y, z;
		BURGESS2::RotationOrder rotationOrder;
		archive(
			CEREAL_NVP( x ),
			CEREAL_NVP( y ),
			CEREAL_NVP( z ),
			CEREAL_NVP( rotationOrder )
		);
		*this = Rad3T<T>( x, y, z, rotationOrder );
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};
#include "Rad3.inl"

/** @} */
using Rad3 = Rad3T<f32>;
using Rad3d = Rad3T<f64>;

//******************************************************************************
//! End of File
//******************************************************************************