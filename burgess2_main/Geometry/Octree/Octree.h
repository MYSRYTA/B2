#pragma once
#include "OctreeNode.h"
#include "OctreeTriPolygon.h"

//******************************************************************************
//! @file	Octree.h
//! @brief	 �I�N�g���[
//******************************************************************************
/** @defgroup Geometry
*	  @{
*/
/**		@defgroup Octree
*			@{
*/
/**				@brief ���b�V���p��ԕ��̓N���X <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					# alias <br> \b : Octree <br> \b : Octreed <br>
*/

template<typename T>
class OctreeT {
public:
	OctreeNodeT<T> nodeTree; /* OctreeNode�̃c���[�K�w */
	L8Array<Vec3T<T>> branchOffset; /* �q���K�w�����ۂ̌W���i�l�̕ҏW�͌���!!�j */
	std::vector<OctreeTriPolygonT<T>> triPolygons; /* �O�p�`�|���S���Q */

	OctreeT ();
	virtual ~OctreeT () {}

	void set ( const PolygonMeshT<T> &mesh );
	void set ( const PolygonMeshT<T> &mesh, const std::vector<f32> &maskVertexValue, Bool invertMask = false );

	void getAllNodeTreeXfos ( const OctreeNodeT<T> &node, IO std::vector<XfoT<T>> &result, Bool includeEmptyNode = false )const;
	void getAllNodeTreePositions ( const OctreeNodeT<T> &node, IO std::vector<Vec3T<T>> &result, Bool includeEmptyNode = false )const;

	std::vector<u32> getAllClosestTriPolygonIDs ( const Vec3T<T> &startPosition, T maxDist )const;

	std::vector<u32> getAllRayCastTriPolygonIDs ( const Ray3T<T> &ray )const;

	u32 triangleCount ()const{ return this->triPolygons.size (); }
private:
	void buildNodeTreeTask ( IO OctreeNodeT<T> &node, const OctreeTriPolygonT<T> &triPoly );
	void addBranchAndReallocate ( IO OctreeNodeT<T> &node );
	void getTriPolyIDsShortestDistance ( const OctreeNodeT<T> &node, const Vec3T<T> &startPosition, T maxDist, OUT std::vector<u32> &result )const;
	void getTriPolyIDsRayIntersect ( const OctreeNodeT<T> &node, const Ray3T<T> &ray, OUT std::vector<u32> &result )const;
public:
	void test_getTriPolyIDsRayIntersect ( const OctreeNodeT<T>& node, const Ray3T<T>& ray, OUT std::vector<u32>& result )const;
};

#include "Octree.inl"

/** @} */
/** @} */
using Octree = OctreeT<f32>;
using Octreed = OctreeT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************