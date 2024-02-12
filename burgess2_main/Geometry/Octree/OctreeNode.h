#pragma once
#include <burgess2_main/Math.h>
#include <burgess2_main/Containers.h>

template<typename T> class PolygonMeshT;
template<typename T> class GeometryLocationT;
template<typename T> class GeometryDeltaT;

//******************************************************************************
//! @file	OctreeNode.h
//! @brief	 �I�N�g���[�m�[�h
//******************************************************************************
/** @defgroup Geometry
*	  @{
*/
/**		@defgroup Octree
*			@{
*/
/**				@defgroup Macro
*					@{
*/
namespace BurgessOctree {
	const u8 MAX_OCTREE_LEVEL = u8 ( 10 );
	const u16 MAX_TRIPOLYGON_COUNT = u16 ( 500 );
}
/** @} */
/**				@brief �I�N�g���[���\������m�[�h <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : OctreeNode <br> \b : OctreeNoded <br>
*/

template<typename T>
class OctreeNodeT : public BBoxT<T>{
public:
	u8 level;								/* ���̃m�[�h��������m�[�h�K�w�̃��x�� */
	Vec3T<T> position;						/* �m�[�h�̒����ʒu */
	T sideLength;							/* �m�[�h�̈�ӂ̒��� */
	T radius;								/* �m�[�h�̒�������p�܂ł̋��� (BSph�̔��a�Ƃ��Ďg�p) */
	std::vector<OctreeNodeT<T>> branches;	/* �q���K�w */
	std::vector<u32> triPolyIDs;			/* ���̃m�[�h�ɑ�����O�p�`�|���S����ID�Q */

	OctreeNodeT () {}
	OctreeNodeT ( u8 _level, Vec3T<T> _centerPos, T _sideLength );
	virtual ~OctreeNodeT () {}

	void addTriPolyID ( u32 triPolyID );
	void addNode ( u8 _level, Vec3T<T> _centerPos, T _sideLength );

	u16 polyCount ()const;

	Bool isLeafNode ()const;
};
/** @} */
/** @} */

#include "OctreeNode.inl"

using OctreeNode = OctreeNodeT<f32>;
using OctreeNoded = OctreeNodeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************