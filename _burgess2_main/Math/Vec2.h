#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class VecT;

//******************************************************************************
//! @file	Vec2.h
//! @brief	 2D Vector [2Dベクトル]
//******************************************************************************
/** @defgroup Math
*　@{
*/
/**		@brief	2D Vector [2Dベクトル]<br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Vec2 = Vec2T<float> <br> \b : Vec2d = Vec2T<double> <br> \b : Vec2s = Vec2T<int> <br> \b : Vec2u = Vec2T<unsigned int> <br>
*			@code
*				Vec2 a ( 2.0f );
*				Vec2 b ( 1.1f, -3.9f );
*
*				Vec2 c = a * b;
*				Xfoing log = c.toString();	/// log = "Vec2T:[2.2, -7.8]"
*			@endcode
*			@date	2019.2 : オリジナル<br>
*					2021.8 : Eigen::Vectorをラッピング<br>
*/
// 2D Vector [2Dベクトル]
template<typename T>
class Vec2T : public Eigen::Vector<T, 2>{
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	Vec2T();
	Vec2T( T val );
	Vec2T( T _x, T _y );
	Vec2T( const VecT<T>& val );
	Vec2T( const std::vector<T>& val );
	Vec2T( const Eigen::Vector<T, 2>& other );
	~Vec2T(){}
	/** @} */
	/*! @name XY*/
	/* @{ */
	inline T& x();
	inline const T& x() const;
	inline T& y();
	inline const T& y() const;
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline VecT<T> toVec() const;
	inline std::vector<T> toTypeArray() const;
	inline String toString( const String title = "" )const;

	inline Bool equal( const Vec2T& other ) const;
	inline Bool almostEqual( const Vec2T& other, T tolerance = BURGESS2::TOLERANCE ) const;
	inline Bool isZero()const;
	inline Bool almostZero( T tolerance = BURGESS2::TOLERANCE )const;
	inline Bool parallel( const Vec2T& other ) const;
	inline Bool almostParallel( const Vec2T& other, T tolerance = BURGESS2::TOLERANCE ) const;
	inline Bool sameDirection( const Vec2T& other )const;
	inline Bool almostSameDirection( const Vec2T& other, T tolerance = BURGESS2::TOLERANCE ) const;
	inline Bool isUnit()const;
	inline Bool almostUnit( T tolerance = BURGESS2::TOLERANCE ) const;

	inline Vec2T getMin( const Vec2T& other )const;
	inline Vec2T getMax( const Vec2T& other )const;
	inline Vec2T clamp( const Vec2T& limitVal0, const Vec2T& limitVal1 ) const;

	inline Vec2T lerp( const Vec2T& other, T t ) const;
	inline Vec2T slerp( const Vec2T& other, T t ) const;
	inline Vec2T linearInterpolate( const Vec2T& other, T t ) const;
	inline Vec2T sphericalLinearInterpolate( const Vec2T& other, T t ) const;

	inline T lengthSquared() const;
	inline T length() const;
	inline T magnitudeSquared()const;
	inline T magnitude()const;

	inline Vec2T normal() const;
	inline void setNormal();
	inline Vec2T unit()const;
	inline void setUnit();

	inline Vec2T negate() const;
	inline void setNegate();
	inline Vec2T inverse() const;
	inline void setInverse();
	inline Vec2T reflect( const Vec2T& _normal ) const;
	inline void setReflect( const Vec2T& _normal );

	inline T dot( const Vec2T& other ) const;
	inline Vec2T cross( const Vec2T& other ) const;

	inline T distanceTo( const Vec2T& other ) const;
	inline Bool distanceLessThan( const Vec2T& other, T dist )const;

	inline T angleTo( const Vec2T& other ) const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	Vec2T& operator = ( const Eigen::Vector<T, 2>& other );
	
	T& operator[]( u8 id );
	const T& operator[]( u8 id ) const;

	Bool operator == ( const Vec2T& other ) const;
	Bool operator != ( const Vec2T& other ) const;

	Vec2T operator + ( const Vec2T& other ) const;
	void operator += ( const Vec2T& other );
	Vec2T operator - ( const Vec2T& other ) const;
	void operator -= ( const Vec2T& other );

	Vec2T operator * ( const T val ) const;
	Vec2T operator * ( const Vec2T& other ) const;
	void operator *= ( const T val );
	void operator *= ( const Vec2T& other );
	Vec2T operator / ( const T val ) const;
	Vec2T operator / ( const Vec2T& other ) const;
	void operator /= ( const T val );
	void operator /= ( const Vec2T& other );
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/*template<class cerealArchive>
	void serialize( cerealArchive& archive ){
		archive(
			CEREAL_NVP( x() ),
			CEREAL_NVP( y() )
		);
	}*/

	template<class cerealArchive>
	void save( cerealArchive& archive )const{
		// 旧Mathに合わせるために、保存対象のデータを一旦変数に格納する
		T x = this->x();
		T y = this->y();

		archive(
			CEREAL_NVP( x ),
			CEREAL_NVP( y )
		);
	}

	template<class cerealArchive>
	void load( cerealArchive& archive ){
		T x, y;
		archive(
			CEREAL_NVP( x ),
			CEREAL_NVP( y )
		);
		*this = Vec2T<T>( x, y );
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};
#include "Vec2.inl"
/** @} */
using Vec2 = Vec2T<f32>;
using Vec2d = Vec2T<f64>;
using Vec2s = Vec2T<s32>;
using Vec2u = Vec2T<u32>;

//******************************************************************************
//! End of File
//******************************************************************************