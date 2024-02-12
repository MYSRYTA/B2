#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;
template<typename T> class Vec3T;
template<typename T> class Rad3T;
template<typename T> class QuatT;
template<typename T> class Mat4T;

//******************************************************************************
//! @file	Deg3.h
//! @brief	 Euler(degree) [オイラー（デグリー角）]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	Euler(degree) [オイラー（デグリー角）] <br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Deg3 = Deg3T<float> <br> \b : Deg3d = Deg3T<double> <br>
*			@code
*				Deg3 aa ( 45, -10.3, 0.32 );
*				Deg3 bb ( 5, -180.3, 40.32 );
*				aa += bb;
*				Quat q( aa );
*			@endcode
*			@date	2019.2 : オリジナル<br>
*					2021.8 : Eigen::Vectorをラッピング<br>
*/
// Euler(degree) [オイラー（デグリー角）]
template<typename T>
class Deg3T : public Eigen::Vector<T, 3> {
	BURGESS2::RotationOrder rotationOrder = BURGESS2::RotationOrder::XYZ;
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	Deg3T();
	Deg3T( T val, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Deg3T( T _x, T _y, T _z, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Deg3T( const std::vector<T>& arrayVal, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Deg3T( const VecT<T>& vec, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Deg3T( const Vec3T<T>& vec, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Deg3T( const QuatT<T>& quat, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Deg3T( const Mat3T<T>& mat3, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Deg3T( const Mat4T<T>& mat4, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	Deg3T( const Rad3T<T>& rad );
	Deg3T( const Eigen::Vector<T, 3>& other );
	~Deg3T() {};
	/** @} */
	/*! @name XYZ*/
	/* @{ */
	inline T& x();
	inline const T& x() const;
	inline T& y();
	inline const T& y() const;
	inline T& z();
	inline const T& z() const;
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline VecT<T> toVec() const;
	inline std::vector<T> toTypeArray() const;
	inline Rad3T<T> toRad3()const;
	inline QuatT<T> toQuat()const;
	inline Mat3T<T> toMat3()const;
	inline String toString( const String title = "" )const;

	inline Bool equal( const Deg3T& other ) const;
	inline Bool almostEqual( const Deg3T& other, T tolerance = BURGESS2::TOLERANCE ) const;

	inline void setFromMat3( const Mat3T<T>& mat3, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	inline void setFromQuat( const QuatT<T>& quat, BURGESS2::RotationOrder order = BURGESS2::RotationOrder::XYZ );
	inline void setFromRad3( const Rad3T<T>& rad3 );

	inline BURGESS2::RotationOrder getRotationOrder()const;
	inline u8 getRotationOrderU8()const;

	inline Vec3T<T> rotatVector( const Vec3T<T>& val ) const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline Deg3T& operator = ( const Eigen::Vector<T, 3>& other );

	inline T& operator[]( u8 id );
	inline const T& operator[]( u8 id ) const;

	inline Bool operator == ( const Deg3T& other )const;
	inline Bool operator != ( const Deg3T& other )const;

	inline Deg3T operator + ( const Deg3T& other )const;
	inline void operator += ( const Deg3T& other );
	inline Deg3T operator - ( const Deg3T& other ) const;
	inline void operator -= ( const Deg3T& other );
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
		*this = Deg3T<T>( x, y, z, rotationOrder );
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};
#include "Deg3.inl"

/** @} */
using Deg3 = Deg3T<f32>;
using Deg3d = Deg3T<f64>;

//******************************************************************************
//! End of File
//******************************************************************************