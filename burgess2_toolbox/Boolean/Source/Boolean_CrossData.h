#pragma once
#include <burgess2_main/Math.h>

//******************************************************************************
//! @file	Boolean_CrossData.h
//! @brief	 Boolean_CrossData
//******************************************************************************
/** @defgroup Tools
*	   @{
*/
/**			@defgroup Boolean
*			@{
*/
/**				@brief Boolean_CrossData <br>
*				メッシュエッジとメッシュフェースの交差をリスト管理するために使用 <br>
*				#include <ToolBox/Boolean/Boolean.h><br>
*/

template<typename T>
struct Boolean_CrossDataT {
	std::vector<std::vector<Vec3T<T>>> crossPosA; // meshA側の接触ポジション
	std::vector<std::vector<Vec3T<T>>> crossPosB; // meshB側の接触ポジション
	std::vector<std::vector<Vec2u>> crossPolyA; // meshA側の接触ポジションと交差するmeshBのエッジ番号（Boolean_EdgeListのエッジ番号）
	std::vector<std::vector<Vec2u>> crossPolyB; // meshB側の接触ポジションと交差するmeshAのエッジ番号（Boolean_EdgeListのエッジ番号）

	void set( const PolygonMeshT<T>& mesh );
	void clear();

	u32 edgeCount() const;

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP( crossPosA )
			CEREAL_NVP( crossPosB )
			CEREAL_NVP( crossPolyA )
			CEREAL_NVP( crossPolyB )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Boolean_CrossData.inl"

	/** @} */
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************