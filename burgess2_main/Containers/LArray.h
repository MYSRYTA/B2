#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Math.h>

//******************************************************************************
//! @file	LocalArrays.h
//! @brief	 固定長配列 + 可変長配列
//******************************************************************************
/** @defgroup Containers
*　@{
*/
/**		@brief	固定長配列 + 可変長配列 <br>
*			#include <burgess2_main/Containers.h> <br><br>
*			#alias <br> \b : L4Array<Type> <br> \b : L8Array<Type> <br> \b : L16Array<Type> <br> \b : L32Array<Type> <br> \b : L64Array<Type><br>
*/

template<u8 N, typename T>
class LArrayT {
	T val [N];
	std::vector<T> others;
	u32 count = 0;
public:
	LArrayT ();
	LArrayT ( u32 size );
	LArrayT ( const LArrayT<N, T> &other );
	LArrayT ( const std::vector<T> &val );
	~LArrayT () {}
	
	void resize ( u32 size );
	void clearOtherArray ();

	void setValue ( u32 index, T value );
	void setValues ( const std::vector<T>& val );
	void push ( T value );
	void push_back ( T value );

	T getValue ( u32 index )const;
	std::vector<T> getValues ()const;
	T back ()const;

	u32 size ()const;
	u8 classLength ()const;

	void sort ( OUT std::vector<u32> &arrayOrigID, Bool descendingOrder = false );
	void sort ( Bool descendingOrder = false );

	String toString ( const String title = "" )const;

	/*! @name Operator*/
	/* @{ */
	T& operator[]( u32 id );
	const T& operator[]( u32 id ) const;
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( val ),
			CEREAL_NVP ( others ),
			CEREAL_NVP ( count )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "LArray.inl"

/** @} */

template<typename T>
using L4Array = LArrayT<4, T>;
template<typename T>
using L8Array = LArrayT<8, T>;
template<typename T>
using L16Array = LArrayT<16, T>;
template<typename T>
using L32Array = LArrayT<32, T>;
template<typename T>
using L64Array = LArrayT<64, T>;

//******************************************************************************
//! End of File
//******************************************************************************