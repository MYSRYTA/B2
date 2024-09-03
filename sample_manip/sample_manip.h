// サンプル：選択したメッシュ頂点をスケーリングするマニピュレータ

////////////////////////////////////////////////////////////////////////
// マニピュレータの呼び出し方法
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
// カスタムマニピュレータ
// シェープノードからCPの配列ポートへアクセスできるので、マニピュレータではその値を扱う

// MPxManipContainerを継承してカスタムマニピュレータコンテナクラスを作成する
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
	
	// プラグインの初期化で必須の関数
	static void* creator(){ return new MeshComponentScaleManip(); }
	static MStatus initialize(){ return MPxManipContainer::initialize(); }

	// Maya組み込みマニピュレータをコンテナに登録する必須の関数
	// https://help.autodesk.com/view/MAYADEV/2025/JPN/?guid=Maya_DEVHELP_Manipulators_Base_Manipulators_html
	MStatus createChildren() override;

	// アトリビュートとマニピュレータをリンクするための必須の関数
	MStatus connectToDependNode(const MObject &node) override;

	// Viewport 2.0 でマニピュレータを描画するための必須の関数
	void draw(M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status) override{ MPxManipContainer::draw(view, path, style, status); }

	// マニピュレータとプラグ間で値を受け渡す際に呼び出される関数
	// 受け渡しの際に追加処理を行いたい場合に準備する
	// 直接紐付けする場合は不要
	MManipData manipToPlugConversion(unsigned index) override;
	MManipData plugToManipConversion(unsigned index) override;

	// マニピュレータのパスを保持するための必須の変数
	MDagPath fScaleManip;
	MDagPath fRotateManip; // 参考としてマニピュレータを描画するために使用（操作は出来るが処理はしない）
	MDagPath fTranseManip; // 参考としてマニピュレータを描画するために使用（操作は出来るが処理はしない）

	// メンバ変数のセッター
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
// カスタムマニピュレータ用のコンテキストと起動コマンド

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
// float3ポートの値をMVector型で取得する関数
MVector vectorPlugValue(const MPlug& plug);