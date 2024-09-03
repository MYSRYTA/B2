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
// カスタムマニピュレータ

// MPxManipContainerを継承してカスタムマニピュレータコンテナクラスを作成する
class moveManip : public MPxManipContainer {
public:
	moveManip(){};
	~moveManip() override{};
	
	// プラグインの初期化で必須の関数
	static void* creator(){ return new moveManip(); }
	static MStatus initialize(){ return MPxManipContainer::initialize(); }

	// 既存のベースマニピュレータ↓をコンテナに登録する必須の関数
	// https://help.autodesk.com/view/MAYADEV/2025/JPN/?guid=Maya_DEVHELP_Manipulators_Base_Manipulators_html
	MStatus createChildren() override;

	// アトリビュートとマニピュレータをリンクするための必須の関数
	MStatus connectToDependNode(const MObject &node) override;

	// Viewport 2.0 でマニピュレータを描画するための必須の関数
	void preDrawUI(const M3dView& view) override;
	void drawUI(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) const override;

private:
	// マニピュレータの描画位置を算出するための関数
    void updateManipLocations(const MObject &node);

	// テキストを描画する位置
	MPoint fTextPosition;

public:
	// マニピュレータのパスを保持するための必須の変数
	MDagPath fDistanceManip; 
	MDagPath fFreePointManip;

	static MTypeId id;
};

MTypeId moveManip::id( 0x8001d );

// 既存のベースマニピュレータ↓をコンテナに登録する必須の関数
// https://help.autodesk.com/view/MAYADEV/2025/JPN/?guid=Maya_DEVHELP_Manipulators_Base_Manipulators_html
MStatus moveManip::createChildren(){
	MStatus stat = MStatus::kSuccess;

	////////////////////////////////////////////////////// 例１
	// 距離マニピュレータを登録
	this->fDistanceManip = addDistanceManip("distanceManip", "distance");
	// マニピュレータのパラメータを設定
	MFnDistanceManip distanceManipFn(this->fDistanceManip);
	MPoint startPoint(0.0, 0.0, 0.0);
	MVector direction(0.0, 1.0, 0.0);
	distanceManipFn.setStartPoint(startPoint);
	distanceManipFn.setDirection(direction);

	////////////////////////////////////////////////////// 例2
	// 位置マニピュレータを登録
	this->fFreePointManip = addFreePointTriadManip("pointManip", "freePoint");

	return stat;
}

// 例：オブジェクトの親の座標で位置マニピュレータを起動する
void moveManip::updateManipLocations(const MObject &node)
{
	MFnDagNode dagNodeFn(node);
	MDagPath nodePath;
	dagNodeFn.getPath(nodePath);
	MTransformationMatrix m(nodePath.exclusiveMatrix());

	// 位置マニピュレータのFnを準備する
    MFnFreePointTriadManip manipFn(this->fFreePointManip);
	
	// 位置マニピュレータの回転と位置を更新する
	double rot[3];
	MTransformationMatrix::RotationOrder rOrder;
	m.getRotation(rot, rOrder);
	manipFn.setRotation(rot, rOrder);
	MVector trans = m.getTranslation(MSpace::kWorld);
	manipFn.setTranslation(trans, MSpace::kWorld);
}

// アトリビュートとマニピュレータをリンクするための必須の関数
// 例：距離マニピュレータをsc.y、位置マニピュレータをtrにリンクする
MStatus moveManip::connectToDependNode(const MObject &node) {
	MStatus stat;

	// オブジェクトのアトリビュート（プラグ）を取得する
	MFnDependencyNode nodeFn(node);
	MPlug syPlug = nodeFn.findPlug("scaleY",  true,  &stat);
	MPlug tPlug = nodeFn.findPlug("translate",  true,  &stat);

	// 距離マニピュレータのFnを介してプラグをリンクする
	MFnDistanceManip distanceManipFn(fDistanceManip);
	distanceManipFn.connectToDistancePlug(syPlug);

	// 位置マニピュレータのFnを介してプラグをリンクする
	MFnFreePointTriadManip freePointManipFn(fFreePointManip);
	freePointManipFn.connectToPointPlug(tPlug);

	// 位置マニピュレータのロケーションを更新する
	this->updateManipLocations(node);

	// 終了宣言
	finishAddingManips();
	MPxManipContainer::connectToDependNode(node);		
	return stat;
}

// マニピュレータの描画を更新する
void	moveManip::preDrawUI(const M3dView& view){
	// Update text drawing position
	//fTextPosition = nodeTranslation();
}

// Viewport 2.0 でマニピュレータを描画する
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
// カスタムマニピュレータ用のコンテキストと起動コマンド

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
	
	// 登録済みのマニピュレータを一旦削除
	MoveManipContext * ctxPtr = (MoveManipContext *) data;
	ctxPtr->deleteManipulators(); 

	// 現在の選択オブジェクトでマニピュレータを作り直す
	MSelectionList list;
	stat = MGlobal::getActiveSelectionList(list);
	MItSelectionList iter(list, MFn::kInvalid, &stat);

	if (MS::kSuccess == stat) {
		for (; !iter.isDone(); iter.next()) {

			// 選択しているオブジェクトがディペンデンシ―ノードではない場合は警告を出してスキップ
			MObject dependNode;
			iter.getDependNode(dependNode);
			if (dependNode.isNull() || !dependNode.hasFn(MFn::kDependencyNode))
			{
				MGlobal::displayWarning("Object in selection list is not a depend node.");
				continue;
			}

			// ノードにtrもしくはscYのプラグが無い場合は警告を出してスキップ
			MFnDependencyNode dependNodeFn(dependNode);
			MPlug rPlug = dependNodeFn.findPlug("translate",  true,  &stat);
			MPlug sPlug = dependNodeFn.findPlug("scaleY",  true,  &stat);
			if (rPlug.isNull() || sPlug.isNull()) {
				MGlobal::displayWarning("Object cannot be manipulated: " + dependNodeFn.name());
				continue;
			}

			// マニピュレータを新規作成
			MString manipName ("moveManip");
			MObject manipObject;
			moveManip* manipulator = (moveManip *) moveManip::newManipulator(manipName, manipObject);
			
			// 新規作成が成功した場合、コンテキストに登録する
			if (NULL != manipulator) {
				ctxPtr->addManipulator(manipObject);

				// 万が一オブジェクトのアトリビュートとマニピュレータの接続に失敗した場合は警告を出す
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
// コンテキストとマニピュレータノードをセットでプラグイン登録する

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
