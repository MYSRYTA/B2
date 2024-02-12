#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;
template<typename T> class Vec3T;
template<typename T> class QuatT;
template<typename T> class Deg3T;
template<typename T> class Rad3T;

//******************************************************************************
//! @file	Vec3.h
//! @brief	3D Vector [3Dベクトル]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	3D Vector [3Dベクトル]<br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Vec3  = Vec3T<float> <br> \b : Vec3d = Vec3T<double> <br> \b : Vec3s = Vec3T<int> <br> \b : Vec3u = Vec3T<unsigned int> <br>
*			@code
*				Vec3 a;
*				a.x() = 2.0f;
*				a[1] = 5.9f; // .y
*				Vec3 b ( 1.0f, 3.9f, -2.2f );
*			
*				Vec3 c = a.lerp ( b, 0.5f );
*			@endcode
*			@date	2019.2 : オリジナル<br>
*					2021.8 : Eigen::Vectorをラッピング<br>
*/
// 3D Vector [3Dベクトル]
template<typename T>
class Vec3T : public Eigen::Vector<T, 3>{
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	Vec3T();
	Vec3T( T val );
	Vec3T( T _x, T _y, T _z );
	Vec3T( const VecT<T>& val );
	Vec3T( const std::vector<T>& val );
	Vec3T( const Eigen::Vector<T, 3>& other );
	~Vec3T(){}
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
	inline String toString( const String title = "" )const;

	inline Bool equal( const Vec3T& other ) const;
	inline Bool almostEqual( const Vec3T& other, T tolerance = BURGESS2::TOLERANCE ) const;
	inline Bool isZero()const;
	inline Bool almostZero( T tolerance = BURGESS2::TOLERANCE )const;
	inline Bool parallel( const Vec3T& other ) const;
	inline Bool almostParallel( const Vec3T& other, T tolerance = T( 0.001 ) ) const;
	inline Bool sameDirection( const Vec3T& other )const;
	inline Bool almostSameDirection( const Vec3T& other, T tolerance = T( 0.001 ) ) const;
	inline Bool isUnit()const;
	inline Bool almostUnit( T tolerance = BURGESS2::TOLERANCE ) const;

	inline Vec3T getMin( const Vec3T& other )const;
	inline Vec3T getMax( const Vec3T& other )const;
	inline Vec3T clamp( const Vec3T& limitVal0, const Vec3T& limitVal1 ) const;

	inline Vec3T lerp( const Vec3T& other, T t ) const;
	inline Vec3T slerp( const Vec3T& other, T t ) const;
	inline Vec3T linearInterpolate( const Vec3T& other, T t ) const;
	inline Vec3T sphericalLinearInterpolate( const Vec3T& other, T t ) const;

	inline T lengthSquared() const;
	inline T length() const;
	inline T magnitudeSquared()const;
	inline T magnitude()const;

	inline Vec3T normal() const;
	inline void setNormal();
	inline Vec3T unit()const;
	inline void setUnit();

	inline Vec3T negate() const;
	inline void setNegate();
	inline Vec3T inverse() const;
	inline void setInverse();
	inline Vec3T reflect( const Vec3T& _normal ) const;
	inline void setReflect( const Vec3T& _normal );

	inline Vec3T<T> angleLimited( const Vec3T<T>& baseDir, T limitAngle )const;

	inline T dot( const Vec3T& other ) const;
	inline Vec3T cross( const Vec3T& other ) const;

	inline T distanceTo( const Vec3T& other ) const;
	inline Bool distanceLessThan( const Vec3T& other, T dist )const;

	inline T angleTo( const Vec3T& other ) const;
	inline Vec3T<T> rotate( const QuatT<T>& rot ) const;
	//inline Vec3T<T> rotate( const Rad3T<T>& rot ) const;
	//inline Vec3T<T> rotate( const Deg3T<T>& rot ) const;
	inline QuatT<T> rotateTo( const Vec3T<T>& other ) const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline Vec3T& operator = ( const Eigen::Vector<T, 3>& other );
	
	inline T& operator[]( u8 id );
	inline const T& operator[]( u8 id ) const;

	inline Bool operator == ( const Vec3T& other ) const;
	inline Bool operator != ( const Vec3T& other ) const;

	inline Vec3T operator + ( const Vec3T& other ) const;
	inline void operator += ( const Vec3T& other );
	inline Vec3T operator - ( const Vec3T& other ) const;
	inline void operator -= ( const Vec3T& other );

	inline Vec3T operator * ( const T val ) const;
	inline Vec3T operator * ( const Vec3T& other ) const;
	inline void operator *= ( const T val );
	inline void operator *= ( const Vec3T& other );
	inline Vec3T operator / ( const T val ) const;
	inline Vec3T operator / ( const Vec3T& other ) const;
	inline void operator /= ( const T val );
	inline void operator /= ( const Vec3T& other );	
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/*template<class cerealArchive>
	void serialize( cerealArchive& archive ){
		archive(
			CEREAL_NVP( x() ),
			CEREAL_NVP( y() ),
			CEREAL_NVP( z() )
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
			CEREAL_NVP( z )
		);
	}

	template<class cerealArchive>
	void load( cerealArchive& archive ){
		T x, y, z;
		archive(
			CEREAL_NVP( x ),
			CEREAL_NVP( y ),
			CEREAL_NVP( z )
		);
		*this = Vec3T<T>( x, y, z );
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};
#include "Vec3.inl"
/** @} */

using Vec3  = Vec3T<f32>;
using Vec3d = Vec3T<f64>;
using Vec3s = Vec3T<s32>;
using Vec3u = Vec3T<u32>;

//******************************************************************************
//! End of File
//******************************************************************************