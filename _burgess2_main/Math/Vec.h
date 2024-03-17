#pragma once
#include <burgess2_main/Core/base.h>

//******************************************************************************
//! @file	Vec.h
//! @brief	 ND Vector [ND�x�N�g��]
//******************************************************************************
/** @defgroup Math
*�@@{
*/
/**		@brief	ND Vector [ND�x�N�g��]<br>
*			#include <burgess2_main/Math.h> <br><br>
*			#alias <br> \b : Vec = VecT<float> <br> \b : Vecd = VecT<double> <br>
*			@code
*				Vec a ( 2 );
*				a.set ( 0, 1.1f );
*				a.set ( 1, 2.1f );
*
*				Vec b;
*				b.resize ( 2 );
*				b.setAll ( 1.1f );
*				b.insert ( 1, 2.1f );
*
*				a.combine ( bb );
*			@endcode
*			@date	2019.2 : �I���W�i��<br>
*					2021.8 : Eigen::Vector�����b�s���O<br>
*/
// ND Vector [ND�x�N�g��]
template<typename T>
class VecT : public Eigen::Vector<T, Eigen::Dynamic>{
public:
	/*! @name Constructor/Destructor */
	/* @{ */
	VecT();
	VecT( s32 size );
	VecT( s32 size, T val );
	VecT( const VecT<T>& val );
	VecT( std::vector<T>& val );
	VecT( Eigen::Vector<T, Eigen::Dynamic>& eigenVal );
	~VecT(){}
	/** @} */
	/*! @name Function*/
	/* @{ */
	inline s32 size()const;

	inline void resize( s32 newSize );
	inline void setZero( );
	inline void set( s32 id, T val );
	inline void setAll( T val );
	inline void insert( s32 id, T val );
	inline void combine( const VecT& other );
	inline void push_top( T val );
	inline void push_back( T val );

	inline T get( s32 id ) const;
	inline std::vector<T> getValues() const;
	inline std::vector<T> toTypeArray() const;
	inline String toString( const String title = "" )const;

	inline T remove( s32 id );
	inline T pop_top();
	inline T pop_back();

	inline Bool equal( const VecT& other ) const;
	inline Bool almostEqual( const VecT& other, T tolerance = BURGESS2::TOLERANCE ) const;
	inline Bool isZero()const;
	inline Bool almostZero( T tolerance = BURGESS2::TOLERANCE )const;

	inline T minimum( OUT s32& id ) const;
	inline T minimum() const;
	inline T maximum( OUT s32& id ) const;
	inline T maximum() const;

	inline VecT lerp( const VecT& other, T t ) const;
	inline VecT linearInterpolate( const VecT& other, T t ) const;
	
	inline T lengthSquared() const;
	inline T length() const;
	inline T magnitudeSquared()const;
	inline T magnitude()const;

	inline VecT normal() const;
	inline void setNormal();
	inline VecT unit()const;
	inline void setUnit();

	inline T dot( const VecT& other ) const;
	/** @} */
	/*! @name Operator*/
	/* @{ */
	inline VecT& operator = ( const Eigen::Vector<T, Eigen::Dynamic>& other );

	inline T& operator[]( s32 id );
	inline const T& operator[]( s32 id ) const;

	inline Bool operator == ( const VecT& other ) const;
	inline Bool operator != ( const VecT& other ) const;

	inline VecT operator + ( const VecT& other ) const;
	inline void operator += ( const VecT& other );
	inline VecT operator - ( const VecT& other ) const;
	inline void operator -= ( const VecT& other );

	inline VecT operator * ( const T& val ) const;
	inline VecT operator * ( const VecT& other ) const;
	inline void operator *= ( const T& val );
	inline void operator *= ( const VecT& other );
	inline VecT operator / ( const T& val ) const;
	inline VecT operator / ( const VecT& other ) const;
	inline void operator /= ( const T& val );
	inline void operator /= ( const VecT& other );
	/** @} */

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void save( cerealArchive& archive )const{
		std::vector<T> vals = this->toTypeArray();
		archive(
			CEREAL_NVP( vals )
		);
	}

	template<class cerealArchive>
	void load( cerealArchive& archive ){
		std::vector<T> vals;
		archive(
			CEREAL_NVP( vals )
		);
		*this = VecT<T>( vals );
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};
#include "Vec.inl"

/** @} */
using Vec = VecT<f32>;
using Vecd = VecT<f64>;
using Vecs = VecT<s32>;
using Vecu = VecT<u32>;

//******************************************************************************
//! End of File
//******************************************************************************