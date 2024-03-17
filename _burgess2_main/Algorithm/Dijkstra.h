#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Math.h>
#include <burgess2_main/Containers/LArray.h>
#include <burgess2_main/Geometry.h>

//******************************************************************************
//! @file	Dijkstra.h
//! @brief	 �_�C�N�X�g��
//******************************************************************************
/** @defgroup Algorithm
*�@@{
*/
/**	@brief	�o�H�T���F�_�C�N�X�g�� �A���S���Y��<br>
*		�Q�Ɛ�@
*		@ref http://qiita.com/edo_m18/items/0588d290a19f2afc0a84 <br>
*/
/**		@defgroup Dijkstra
*�@		@{
*/
/**			@brief	�_�C�N�X�g���O���t���\������m�[�h <br>
*/

template<typename T>
struct DijkstraNodeT {
	LArrayT<16, u32> surroundingNodeID; // �אڂ���m�[�hID
	LArrayT<16, T> surroundingNodeCost; // �אڃm�[�h�ւ̃R�X�g�i�����j
	Bool done = false; // ���̃m�[�h���m��̏ꍇ��ture
	T cost = T ( -1.0 ); // ���̃m�[�h�ւ̍ŏ��R�X�g�l���ꎞ�ۊǏꏊ����ϐ�

	Bool assign = false;

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( surroundingNodeID ),
			CEREAL_NVP ( surroundingNodeCost ),
			CEREAL_NVP ( done ),
			CEREAL_NVP ( cost ),
			CEREAL_NVP ( assign )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

/**		@brief	�_�C�N�X�g�� �A���S���Y��<br>
*			#include <burgess2_main/Algorithm.h> <br><br>
*			#alias <br> \b : Dijkstra = DijkstraT<float> <br> \b : Dijkstrad = DijkstraT<double> <br>
*			�Q�Ɛ�@
*			@ref http://qiita.com/edo_m18/items/0588d290a19f2afc0a84 <br>
*			@code
*				// ���b�V���̒��_[4]����e���_�ւ̌o�H�T�����s��
*				PolygonMesh cube;
*				cube.createCube( Xfo(), 1.0f );
*
*				Dijkstra dij;
*				dij.setFromPolygonMesh( cube, false ); // ���b�V�����_�C�N�X�g���ɓo�^����
*				dij.recomputeRouteCost( 4, 1.1f ); // �S�Ԓ��_����e���_�ւ̈ړ��R�X�g(�ړ�����)���v�Z����
*				std::vector<f32> cost = dij.getRouteCost(); // �v�Z���ʂ��擾����
*			@endcode
*/
template<typename T>
class DijkstraT {
	std::vector<DijkstraNodeT<T>> node;
	T maxCost;
	Bool initialized = false;
	Bool computed = false;
public:
	DijkstraT () {};
	virtual ~DijkstraT () {}

	void setFromPolygonMesh ( const PolygonMeshT<T> &mesh, Bool includeAllPolygonPoints );

	void recomputeRouteCost ( std::vector<u32> startNodeIDs, T maxDistance = T(BURGESS2::F32_INFINITE) );
	std::vector<T> getRouteCost ( u8 returnValueType = 0 );

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( node ),
			CEREAL_NVP ( maxCost ),
			CEREAL_NVP ( initialized ),
			CEREAL_NVP ( computed )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

#include "Dijkstra.inl"
	/** @} */
/** @} */
using Dijkstra = DijkstraT<f32>;
using Dijkstrad = DijkstraT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************