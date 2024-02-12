#pragma once
#include <burgess2_main/Geometry.h>

//******************************************************************************
//! @file	Boolean_EdgeList.h
//! @brief	 Boolean_EdgeList
//******************************************************************************
/** @defgroup Tools
*	   @{
*/
/**		@defgroup Boolean
*			@{
*/
/**				@brief Boolean_EdgeList <br>
*				メッシュエッジ（どちらか一方のハーフエッジ）のリスト <br>
*				#include <ToolBox/Boolean/Boolean.h><br>
*/

template<typename T>
struct Boolean_EdgeListT {
	std::vector<Ray3T<T>> edgeRay;
	std::vector<T> edgeLen;

	void set( const PolygonMeshT<T>& mesh );
	void clear();

	u32 edgeCount() const;

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( edgeRay )
			CEREAL_NVP ( edgeLen )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Boolean_EdgeList.inl"

	/** @} */
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************