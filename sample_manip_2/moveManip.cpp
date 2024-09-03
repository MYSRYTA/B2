//-
// ==========================================================================
// Copyright 1995,2006,2008 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

/* 

	This example demonstrates the FreePointTriad and Distance manipulators in 
	the API.  This example uses three classes to accomplish this task: First, 
	a context command (moveManipContext) is provided to create instances of 
	the context.  Next, a custom selection context (MoveManipContext) is 
	created to manage the rotation manipulator.   Finally, the rotation 
	manipulator is provided as a custom node class.

	Loading and unloading:
	----------------------

	The move manipulator context and tool button can be created with the 
	following mel commands:

		moveManipContext;
		setParent Shelf1;
		toolButton	-cl toolCluster
					-t moveManipContext1
					-i1 "moveToolManip.xpm"
					moveManip;

	If the preceding commands were used to create the manipulator context, 
	the following commands can destroy it:

		deleteUI moveManipContext1;
		deleteUI moveManip;

	If the plugin is loaded and unloaded frequently (eg. during testing),
	it is useful to make these command sequences into shelf buttons.

	How to use:
	-----------

	Once the tool button has been created using the script above, select the
	tool button then click on an object.  The move manipulator should appear
	at the center of the selected object and a distance manipulator should 
	appear at the origin.  Use the move manipulator to move the object, and
	the distance manipulator to control the scaling in Y direction.
	
*/

#include <maya/MIOStream.h>
#include <stdio.h>
#include <stdlib.h>

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
#include <maya/MMatrix.h>

// Manipulators
#include <maya/MFnFreePointTriadManip.h>
#include <maya/MFnDistanceManip.h>

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// �J�X�^���}�j�s�����[�^

// MPxManipContainer���p�����ăJ�X�^���}�j�s�����[�^�R���e�i�N���X���쐬����
class moveManip : public MPxManipContainer {
public:
	moveManip(){};
	~moveManip() override{};
	
	// �v���O�C���̏������ŕK�{�̊֐�
	static void* creator(){ return new moveManip(); }
	static MStatus initialize(){ return MPxManipContainer::initialize(); }

	// �����̃x�[�X�}�j�s�����[�^�����R���e�i�ɓo�^����K�{�̊֐�
	// https://help.autodesk.com/view/MAYADEV/2025/JPN/?guid=Maya_DEVHELP_Manipulators_Base_Manipulators_html
	MStatus createChildren() override;

	// �A�g���r���[�g�ƃ}�j�s�����[�^�������N���邽�߂̕K�{�̊֐�
	MStatus connectToDependNode(const MObject &node) override;

	// Viewport 2.0 �Ń}�j�s�����[�^��`�悷�邽�߂̕K�{�̊֐�
	void preDrawUI(const M3dView& view) override;
	void drawUI(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) const override;

private:
	// �}�j�s�����[�^�̕`��ʒu���Z�o���邽�߂̊֐�
    void updateManipLocations(const MObject &node);

	// �e�L�X�g��`�悷��ʒu
	MPoint fTextPosition;

public:
	// �}�j�s�����[�^�̃p�X��ێ����邽�߂̕K�{�̕ϐ�
	MDagPath fDistanceManip; 
	MDagPath fFreePointManip;

	static MTypeId id;
};

MTypeId moveManip::id( 0x8001d );

// �����̃x�[�X�}�j�s�����[�^�����R���e�i�ɓo�^����K�{�̊֐�
// https://help.autodesk.com/view/MAYADEV/2025/JPN/?guid=Maya_DEVHELP_Manipulators_Base_Manipulators_html
MStatus moveManip::createChildren(){
	MStatus stat = MStatus::kSuccess;

	////////////////////////////////////////////////////// ��P
	// �����}�j�s�����[�^��o�^
	this->fDistanceManip = addDistanceManip("distanceManip", "distance");
	// �}�j�s�����[�^�̃p�����[�^��ݒ�
	MFnDistanceManip distanceManipFn(this->fDistanceManip);
	MPoint startPoint(0.0, 0.0, 0.0);
	MVector direction(0.0, 1.0, 0.0);
	distanceManipFn.setStartPoint(startPoint);
	distanceManipFn.setDirection(direction);

	////////////////////////////////////////////////////// ��2
	// �ʒu�}�j�s�����[�^��o�^
	this->fFreePointManip = addFreePointTriadManip("pointManip", "freePoint");

	return stat;
}

// ��F�I�u�W�F�N�g�̐e�̍��W�ňʒu�}�j�s�����[�^���N������
void moveManip::updateManipLocations(const MObject &node)
{
	MFnDagNode dagNodeFn(node);
	MDagPath nodePath;
	dagNodeFn.getPath(nodePath);
	MTransformationMatrix m(nodePath.exclusiveMatrix());

	// �ʒu�}�j�s�����[�^��Fn����������
    MFnFreePointTriadManip manipFn(this->fFreePointManip);
	
	// �ʒu�}�j�s�����[�^�̉�]�ƈʒu���X�V����
	double rot[3];
	MTransformationMatrix::RotationOrder rOrder;
	m.getRotation(rot, rOrder);
	manipFn.setRotation(rot, rOrder);
	MVector trans = m.getTranslation(MSpace::kWorld);
	manipFn.setTranslation(trans, MSpace::kWorld);
}

// �A�g���r���[�g�ƃ}�j�s�����[�^�������N���邽�߂̕K�{�̊֐�
// ��F�����}�j�s�����[�^��sc.y�A�ʒu�}�j�s�����[�^��tr�Ƀ����N����
MStatus moveManip::connectToDependNode(const MObject &node) {
	MStatus stat;

	// �I�u�W�F�N�g�̃A�g���r���[�g�i�v���O�j���擾����
	MFnDependencyNode nodeFn(node);
	MPlug syPlug = nodeFn.findPlug("scaleY",  true,  &stat);
	MPlug tPlug = nodeFn.findPlug("translate",  true,  &stat);

	// �����}�j�s�����[�^��Fn����ăv���O�������N����
	MFnDistanceManip distanceManipFn(fDistanceManip);
	distanceManipFn.connectToDistancePlug(syPlug);

	// �ʒu�}�j�s�����[�^��Fn����ăv���O�������N����
	MFnFreePointTriadManip freePointManipFn(fFreePointManip);
	freePointManipFn.connectToPointPlug(tPlug);

	// �ʒu�}�j�s�����[�^�̃��P�[�V�������X�V����
	this->updateManipLocations(node);

	// �I���錾
	finishAddingManips();
	MPxManipContainer::connectToDependNode(node);		
	return stat;
}

// �}�j�s�����[�^�̕`����X�V����
void	moveManip::preDrawUI(const M3dView& view){
	// Update text drawing position
	//fTextPosition = nodeTranslation();
}

// Viewport 2.0 �Ń}�j�s�����[�^��`�悷��
void moveManip::drawUI(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) const
{
	drawManager.beginDrawable();

	drawManager.setColor(MColor(0.0f, 1.0f, 0.1f));
	drawManager.text(MPoint(0, 0), "Stretch Me!", MHWRender::MUIDrawManager::kLeft);
	
	drawManager.endDrawable();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// �J�X�^���}�j�s�����[�^�p�̃R���e�L�X�g�ƋN���R�}���h

class MoveManipContext : public MPxSelectionContext
{
public:
	MoveManipContext();
	void	toolOnSetup(MEvent &event) override;
	void	toolOffCleanup() override;

	// Callback issued when selection list changes
	static void updateManipulators(void * data);

private:
	MCallbackId id1;
};

MoveManipContext::MoveManipContext()
{
	MString str("Plugin move Manipulator");
	setTitleString(str);
}

// 
void MoveManipContext::toolOnSetup(MEvent &){
	MString str("Move the object using the manipulator");
	setHelpString(str);

	updateManipulators(this);
	MStatus status;
	this->id1 = MModelMessage::addCallback(MModelMessage::kActiveListModified, updateManipulators, this, &status);
	if (!status) { MGlobal::displayError("Model addCallback failed"); }
}


void MoveManipContext::toolOffCleanup(){
	MStatus status;
	status = MModelMessage::removeCallback(this->id1);
	if (!status) { MGlobal::displayError("Model remove callback failed"); }
	MPxContext::toolOffCleanup();
}


void MoveManipContext::updateManipulators(void * data)
{
	MStatus stat = MStatus::kSuccess;
	
	// �o�^�ς݂̃}�j�s�����[�^����U�폜
	MoveManipContext * ctxPtr = (MoveManipContext *) data;
	ctxPtr->deleteManipulators(); 

	// ���݂̑I���I�u�W�F�N�g�Ń}�j�s�����[�^����蒼��
	MSelectionList list;
	stat = MGlobal::getActiveSelectionList(list);
	MItSelectionList iter(list, MFn::kInvalid, &stat);

	if (MS::kSuccess == stat) {
		for (; !iter.isDone(); iter.next()) {

			// �I�����Ă���I�u�W�F�N�g���f�B�y���f���V�\�m�[�h�ł͂Ȃ��ꍇ�͌x�����o���ăX�L�b�v
			MObject dependNode;
			iter.getDependNode(dependNode);
			if (dependNode.isNull() || !dependNode.hasFn(MFn::kDependencyNode))
			{
				MGlobal::displayWarning("Object in selection list is not a depend node.");
				continue;
			}

			// �m�[�h��tr��������scY�̃v���O�������ꍇ�͌x�����o���ăX�L�b�v
			MFnDependencyNode dependNodeFn(dependNode);
			MPlug rPlug = dependNodeFn.findPlug("translate",  true,  &stat);
			MPlug sPlug = dependNodeFn.findPlug("scaleY",  true,  &stat);
			if (rPlug.isNull() || sPlug.isNull()) {
				MGlobal::displayWarning("Object cannot be manipulated: " + dependNodeFn.name());
				continue;
			}

			// �}�j�s�����[�^��V�K�쐬
			MString manipName ("moveManip");
			MObject manipObject;
			moveManip* manipulator = (moveManip *) moveManip::newManipulator(manipName, manipObject);
			
			// �V�K�쐬�����������ꍇ�A�R���e�L�X�g�ɓo�^����
			if (NULL != manipulator) {
				ctxPtr->addManipulator(manipObject);

				// ������I�u�W�F�N�g�̃A�g���r���[�g�ƃ}�j�s�����[�^�̐ڑ��Ɏ��s�����ꍇ�͌x�����o��
				if (!manipulator->connectToDependNode(dependNode))
				{
					MGlobal::displayWarning("Error connecting manipulator to object: " + dependNodeFn.name());
				}
			} 
		}
	}
}

class moveManipContext : public MPxContextCommand {
public:
	moveManipContext() {};
	MPxContext* makeObj() override{ return new MoveManipContext(); }

public:
	static void* creator(){ return new moveManipContext; }
};

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// �R���e�L�X�g�ƃ}�j�s�����[�^�m�[�h���Z�b�g�Ńv���O�C���o�^����

MStatus initializePlugin(MObject obj){
	MStatus status;
	MFnPlugin plugin(obj, PLUGIN_COMPANY, "3.0", "Any");

	status = plugin.registerContextCommand("moveManipContext", &moveManipContext::creator);
	if (!status) { MGlobal::displayError("Error registering moveManipContext command");	return status; }

	status = plugin.registerNode("moveManip", moveManip::id, &moveManip::creator, &moveManip::initialize, MPxNode::kManipContainer);
	if (!status) { MGlobal::displayError("Error registering moveManip node"); return status; }

	return status;
}


MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterContextCommand("moveManipContext");
	if (!status) { MGlobal::displayError("Error deregistering moveManipContext command"); return status; }

	status = plugin.deregisterNode(moveManip::id);
	if (!status) { MGlobal::displayError("Error deregistering moveManip node"); return status; }

	return status;
}
