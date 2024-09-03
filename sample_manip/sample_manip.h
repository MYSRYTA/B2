// �T���v���F�I���������b�V�����_���X�P�[�����O����}�j�s�����[�^

////////////////////////////////////////////////////////////////////////
// �}�j�s�����[�^�̌Ăяo�����@
// 
// import maya.cmds as cmds
// temp_ctx = cmds.MeshComponentScaleContext()
// cmds.setToolTo(temp_ctx)

#include <stdio.h>
#include <stdlib.h>

#include <maya/MIOStream.h>
#include <maya/MFn.h>
#include <maya/MPxNode.h>
#include <maya/MPxManipContainer.h>
#include <maya/MPxSelectionContext.h>
#include <maya/MPxContextCommand.h>
#include <maya/MModelMessage.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MItSelectionList.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MDagPath.h>
#include <maya/MManipData.h>
#include <maya/MSelectionList.h>
#include <maya/MItMeshVertex.h>
#include <maya/MFnComponent.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MPxToolCommand.h>
// Manipulators
#include <maya/MFnScaleManip.h>
#include <maya/MFnRotateManip.h>
#include <maya/MFnFreePointTriadManip.h> 

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// �J�X�^���}�j�s�����[�^
// �V�F�[�v�m�[�h����CP�̔z��|�[�g�փA�N�Z�X�ł���̂ŁA�}�j�s�����[�^�ł͂��̒l������

// MPxManipContainer���p�����ăJ�X�^���}�j�s�����[�^�R���e�i�N���X���쐬����
class MeshComponentScaleManip : public MPxManipContainer{
public:
	MeshComponentScaleManip():
		numComponents(0),
		initialPositions(NULL),
		initialControlPoint(NULL){}

	~MeshComponentScaleManip() override	{
		delete[] initialPositions;
		delete[] initialControlPoint;
	}
	
	// �v���O�C���̏������ŕK�{�̊֐�
	static void* creator(){ return new MeshComponentScaleManip(); }
	static MStatus initialize(){ return MPxManipContainer::initialize(); }

	// Maya�g�ݍ��݃}�j�s�����[�^���R���e�i�ɓo�^����K�{�̊֐�
	// https://help.autodesk.com/view/MAYADEV/2025/JPN/?guid=Maya_DEVHELP_Manipulators_Base_Manipulators_html
	MStatus createChildren() override;

	// �A�g���r���[�g�ƃ}�j�s�����[�^�������N���邽�߂̕K�{�̊֐�
	MStatus connectToDependNode(const MObject &node) override;

	// Viewport 2.0 �Ń}�j�s�����[�^��`�悷�邽�߂̕K�{�̊֐�
	void draw(M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status) override{ MPxManipContainer::draw(view, path, style, status); }

	// �}�j�s�����[�^�ƃv���O�ԂŒl���󂯓n���ۂɌĂяo�����֐�
	// �󂯓n���̍ۂɒǉ��������s�������ꍇ�ɏ�������
	// ���ڕR�t������ꍇ�͕s�v
	MManipData manipToPlugConversion(unsigned index) override;
	MManipData plugToManipConversion(unsigned index) override;

	// �}�j�s�����[�^�̃p�X��ێ����邽�߂̕K�{�̕ϐ�
	MDagPath fScaleManip;
	MDagPath fRotateManip; // �Q�l�Ƃ��ă}�j�s�����[�^��`�悷�邽�߂Ɏg�p�i����͏o���邪�����͂��Ȃ��j
	MDagPath fTranseManip; // �Q�l�Ƃ��ă}�j�s�����[�^��`�悷�邽�߂Ɏg�p�i����͏o���邪�����͂��Ȃ��j

	// �����o�ϐ��̃Z�b�^�[
	void setSurfaceDagPath(const MDagPath& dagPath){ surfaceDagPath = dagPath; }
	void setComponentObject(const MObject& obj){ component = obj; }

public:
	static MTypeId id;

private:
	MDagPath surfaceDagPath;
	MObject component;

	MPoint centroid;

	int numComponents;
	MPoint* initialPositions;
	MPoint* initialControlPoint;

	unsigned dummyPlugIndex;
};

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// �J�X�^���}�j�s�����[�^�p�̃R���e�L�X�g�ƋN���R�}���h

class ComponentScaleContext : public MPxSelectionContext
{
public:
	ComponentScaleContext();
	void toolOnSetup(MEvent &event) override;
	void toolOffCleanup() override;

	static void updateManipulators(void * data);

private:
	MCallbackId id1;
};

class MeshComponentScaleContext : public MPxContextCommand
{
public:
	MeshComponentScaleContext() {};
	MPxContext* makeObj() override{ return new ComponentScaleContext(); }

public:
	static void* creator(){ return new MeshComponentScaleContext; };
};

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// float3�|�[�g�̒l��MVector�^�Ŏ擾����֐�
MVector vectorPlugValue(const MPlug& plug);