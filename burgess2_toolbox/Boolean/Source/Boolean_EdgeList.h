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
*				���b�V���G�b�W�i�ǂ��炩����̃n�[�t�G�b�W�j�̃��X�g <br>
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
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( edgeRay )
			CEREAL_NVP ( edgeLen )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

#include "Boolean_EdgeList.inl"

	/** @} */
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************