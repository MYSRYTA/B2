#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;
template<typename T> class Vec3T;

//******************************************************************************
//! @file	Vec4.h
//! @brief	4D Vector [4Dベクトル]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	4D Vector [4Dベクトル]<br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias : <br> \b : Vec4 = Vec4T<float> <br> \b : Vec4d = Vec4T<double><br>
*			@code
*				Vec4 a ( 1.0f );
*				Vec4 b ( 1.1f, -3.9f, -2.2f, 2.0f );
*
*				Vec4 c = a.max ( b );
*			@endcode
*			@date	2019.2 : オリジナル<br>
*					2021.8 : Eigen::Vectorをラッピング<br>
*/
// 4D Vector [4Dベクトル]
template<typename T>
class Vec4T : public Eigen::Vector<T, 4>{
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	Vec4T();
	Vec4T( T val );
	Vec4T( T _x, T _y, T _z, T _t );
	Vec4T( const VecT<T>& val );
	Vec4T( const std::vector<T>& val );
	Vec4T( const Eigen::Vector<T, 4>& other );
	~Vec4T(){}
	/** @} */
	/*! @name XYZW*/
	/* @{ */
	inline T& x();
	inline const T& x() const;
	inline T& y();
	inline const T& y() const;
	inline T& z();
	inline const T& z() const;
	inline T& t();
	inline const T& t() const;
	inline T& w();
	inline const T& w() const;
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline VecT<T> toVec() const;
	inline std::vector<T> toTypeArray() const;
	inline String toString( const String title = "" )const;

	inline Bool equal( const Vec4T& other ) const;
	inline Bool almostEqual( const Vec4T& other, T tolerance = BURGESS2::TOLERANCE ) const;
	inline Bool isZero()const;
	inline Bool almostZero( T tolerance = BURGESS2::TOLERANCE )const;
	inline Bool isUnit()const;
	inline Bool almostUnit( T tolerance = BURGESS2::TOLERANCE ) const;

	inline Vec4T getMin( const Vec4T& other )const;
	inline Vec4T getMax( const Vec4T& other )const;
	inline Vec4T clamp( const Vec4T& limitVal0, const Vec4T& limitVal1 ) const;

	inline Vec4T lerp( const Vec4T& other, T t ) const;
	inline Vec4T linearInterpolate( const Vec4T& other, T t ) const;

	inline T lengthSquared() const;
	inline T length() const;
	inline T magnitudeSquared()const;
	inline T magnitude()const;

	inline Vec4T normal() const;
	inline void setNormal();
	inline Vec4T unit()const;
	inline void setUnit();

	inline Vec4T negate() const;
	inline void setNegate();
	inline Vec4T inverse() const;
	inline void setInverse();

	inline T dot( const Vec4T& other ) const;

	inline T distanceTo( const Vec4T& other ) const;
	inline Bool distanceLessThan( const Vec4T& other, T dist )const;

	inline T angleTo( const Vec4T& other ) const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	Vec4T& operator = ( const Eigen::Vector<T, 4>& other );
	
	inline T& operator[]( u8 id );
	inline const T& operator[]( u8 id ) const;

	inline Bool operator == ( const Vec4T& other ) const;
	inline Bool operator != ( const Vec4T& other ) const;

	inline Vec4T operator + ( const Vec4T& other ) const;
	inline void operator += ( const Vec4T& other );
	inline Vec4T operator - ( const Vec4T& other ) const;
	inline void operator -= ( const Vec4T& other );

	inline Vec4T operator * ( const T val ) const;
	inline Vec4T operator * ( const Vec4T& other ) const;
	inline void operator *= ( const T val );
	inline void operator *= ( const Vec4T& other );
	inline Vec4T operator / ( const T val ) const;
	inline Vec4T operator / ( const Vec4T& other ) const;
	inline void operator /= ( const T val );
	inline void operator /= ( const Vec4T& other );
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/*template<class cerealArchive>
	void serialize( cerealArchive& archive ){
		archive(
			CEREAL_NVP( x() ),
			CEREAL_NVP( y() ),
			CEREAL_NVP( z() ),
			CEREAL_NVP( t() )
		);
	}*/

	template<class cerealArchive>
	void save( cerealArchive& archive )const{
		// 旧Mathに合わせるために、保存対象のデータを一旦変数に格納する
		T x = this->x();
		T y = this->y();
		T z = this->z();
		T t = this->t();
		archive(
			CEREAL_NVP( x ),
			CEREAL_NVP( y ),
			CEREAL_NVP( z ),
			CEREAL_NVP( t )
		);
	}

	template<class cerealArchive>
	void load( cerealArchive& archive ){
		T x, y, z, t;
		archive(
			CEREAL_NVP( x ),
			CEREAL_NVP( y ),
			CEREAL_NVP( z ),
			CEREAL_NVP( t )
		);
		*this = Vec4T<T>( x, y, z, t );
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};
#include "Vec4.inl"
/** @} */

using Vec4 = Vec4T<f32>;
using Vec4d = Vec4T<f64>;

//******************************************************************************
//! End of File
//******************************************************************************