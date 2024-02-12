#pragma once
#include <burgess2_main/Algorithm.h>
#include <burgess2_main/Geometry.h>

//******************************************************************************
//! @file	RBFRetargeter.h
//! @brief	 RBF Retargeter
//******************************************************************************
/** @defgroup Tools
*�@@{
*/
/**		@brief	RBF Retargeter<br>
*			RBF��p�����|�C���g��ԃx�[�X�̃��^�[�Q�b�g���� <br>
*			alias : \b RBFRetargeter = RBFRetargeterT<float> // \b RBFRetargeterd = RBFRetargeterT<double> <br>
*			#include <Burgess_Tools/RBFRetargeter/RBFRetargeter.h> <br>
*			@code
*				// RBF2 rbfVal;
*				// Bool resetup;
*				// PolygonMesh baseMesh; ���_�l��RBF��Ԃ��\�z���邽�߂̃��b�V��
*				// PolygonMesh targetMesh; RBF��Ԃ̊e���_�ʒu�ɕR�Â���^�[�Q�b�g�l�i���_�ʒu�j
*				if ( resetup ){ 
*					// rbf�̏���쐬�������͍č쐬
*					rbfVal = PSRetargeter::Setup ( baseMesh, targetMesh, 2 );
*					resetup = false;
*				}
*				PSRetargeter::UpdataRetargetMesh ( rbfVal, newTargetMesh ); // �^�[�Q�b�g�l���X�V
*				PolygonMesh result = baseMesh; // ����g�|���W�[�̃��U���g���b�V��������
*				PSRetargeter::Solve ( result, rbfVal, baseMesh );
*			@endcode
*/

template<typename T>
class RBFRetargeterT {
	RBFT<T> rbf;
	u8 size;
public:
	RBFRetargeterT (){}
	virtual ~RBFRetargeterT (){}

	void setup ( const std::vector<Vec3T<T>> &referencePoint, const std::vector<Vec3T<T>> &refarenceValue, u8 rbfType = 1, T rbfSmooth = 0.0, T rbfEpsilon = 0.1 );
	void setup ( const std::vector<Vec3T<T>> &referencePoint, const std::vector<Mat4T<T>> &refarenceValue, u8 rbfType = 1, T rbfSmooth = 0.0, T rbfEpsilon = 0.1 );
	void setup ( const PolygonMeshT<T> &referenceMesh, const PolygonMeshT<T> &retargetMesh, u8 rbfType = 1, T rbfSmooth = 0.0, T rbfEpsilon = 0.1 );

	void solve ( OUT std::vector<Vec3T<T>> &result, const std::vector<Vec3T<T>> &sourcePoint )const;
	void solve ( OUT std::vector<Mat4T<T>> &result, const std::vector<Vec3T<T>> &sourcePoint )const;
	void solve ( IO PolygonMeshT<T> &result, const PolygonMeshT<T> &sourceMesh )const;
};


#include "RBFRetargeter.inl"
/** @} */
using RBFRetargeter = RBFRetargeterT<f32>;
using RBFRetargeterd = RBFRetargeterT<f64>;
//******************************************************************************
//! End of File
//******************************************************************************