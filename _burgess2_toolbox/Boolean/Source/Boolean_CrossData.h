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
*				���b�V���G�b�W�ƃ��b�V���t�F�[�X�̌��������X�g�Ǘ����邽�߂Ɏg�p <br>
*				#include <ToolBox/Boolean/Boolean.h><br>
*/

template<typename T>
struct Boolean_CrossDataT {
	std::vector<std::vector<Vec3T<T>>> crossPosA; // meshA���̐ڐG�|�W�V����
	std::vector<std::vector<Vec3T<T>>> crossPosB; // meshB���̐ڐG�|�W�V����
	std::vector<std::vector<Vec2u>> crossPolyA; // meshA���̐ڐG�|�W�V�����ƌ�������meshB�̃G�b�W�ԍ��iBoolean_EdgeList�̃G�b�W�ԍ��j
	std::vector<std::vector<Vec2u>> crossPolyB; // meshB���̐ڐG�|�W�V�����ƌ�������meshA�̃G�b�W�ԍ��iBoolean_EdgeList�̃G�b�W�ԍ��j

	void set( const PolygonMeshT<T>& mesh );
	void clear();

	u32 edgeCount() const;

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP( crossPosA )
			CEREAL_NVP( crossPosB )
			CEREAL_NVP( crossPolyA )
			CEREAL_NVP( crossPolyB )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

#include "Boolean_CrossData.inl"

	/** @} */
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************