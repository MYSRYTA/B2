//-
// ==========================================================================
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

////////////////////////////////////////////////////////////////////////
// 
// DESCRIPTION:
// 
// Produces these dependency graph nodes: "footPrintLocator" and "footPrintLocatorManip".  
//
// This plug-in demonstrates how to use the Show Manip Tool with
// a user-defined manipulator. The user-defined manipulator corresponds to the foot print locator.
//
// This is the script for running this plug-in:
// 
//	loadPlugin footPrintManip;
//	createNode footPrintLocator -n f1;
// 
// To use this plug-in:
//	(1) Create a foot print locator (type createNode footPrintLocator).
//	(2) Select the foot print.
//	(3) Click the Show Manip Tool.
// 
////////////////////////////////////////////////////////////////////////

#include <maya/MIOStream.h>

#include <maya/MPxNode.h>   

#include <maya/MPxLocatorNode.h> 

#include <maya/MString.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MFnPlugin.h>
#include <maya/MDistance.h>
#include <maya/MFnUnitAttribute.h>

#include <maya/MFn.h>
#include <maya/MPxNode.h>
#include <maya/MPxManipContainer.h> 
#include <maya/MFnDistanceManip.h> 
#include <maya/MPxContext.h>
#include <maya/MPxSelectionContext.h>

#include <maya/MFnNumericData.h>
#include <maya/MManipData.h>

#include <maya/MEventMessage.h>

// Viewport 2.0
#include <maya/MDrawRegistry.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>
#include <maya/MDrawContext.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MPointArray.h>

// Foot Data
//
static float sole[][3] = { {  0.00f, 0.0f, -0.70f},
						   {  0.04f, 0.0f, -0.69f },
						   {  0.09f, 0.0f, -0.65f },
						   {  0.13f, 0.0f, -0.61f }, 
						   {  0.16f, 0.0f, -0.54f },
						   {  0.17f, 0.0f, -0.46f },
						   {  0.17f, 0.0f, -0.35f },
						   {  0.16f, 0.0f, -0.25f },
						   {  0.15f, 0.0f, -0.14f },
						   {  0.13f, 0.0f,  0.00f },
						   {  0.00f, 0.0f,  0.00f },
						   { -0.13f, 0.0f,  0.00f },
						   { -0.15f, 0.0f, -0.14f },
						   { -0.16f, 0.0f, -0.25f },
						   { -0.17f, 0.0f, -0.35f },
						   { -0.17f, 0.0f, -0.46f }, 
						   { -0.16f, 0.0f, -0.54f },
						   { -0.13f, 0.0f, -0.61f },
						   { -0.09f, 0.0f, -0.65f },
						   { -0.04f, 0.0f, -0.69f }, 
						   { -0.00f, 0.0f, -0.70f } }; 
static float heel[][3] = { {  0.00f, 0.0f,  0.06f },
						   {  0.13f, 0.0f,  0.06f },
						   {  0.14f, 0.0f,  0.15f },
						   {  0.14f, 0.0f,  0.21f }, 
						   {  0.13f, 0.0f,  0.25f },
						   {  0.11f, 0.0f,  0.28f },
						   {  0.09f, 0.0f,  0.29f },
						   {  0.04f, 0.0f,  0.30f },
						   {  0.00f, 0.0f,  0.30f },
						   { -0.04f, 0.0f,  0.30f },
						   { -0.09f, 0.0f,  0.29f },
						   { -0.11f, 0.0f,  0.28f }, 
						   { -0.13f, 0.0f,  0.25f },
						   { -0.14f, 0.0f,  0.21f },
						   { -0.14f, 0.0f,  0.15f },
						   { -0.13f, 0.0f,  0.06f },  
						   { -0.00f, 0.0f,  0.06f } }; 
static int heelCount = 17;
static int soleCount = 21;
 
// MPxManipContainerを継承してカスタムマニピュレータコンテナクラスを作成する
class footPrintLocatorManip : public MPxManipContainer
{
public:
	footPrintLocatorManip(){};
	~footPrintLocatorManip() override{};
    
	// プラグインの初期化で必須の関数
	static void* creator(){ return new footPrintLocatorManip(); }
    static MStatus initialize();

	// ベースマニピュレータをセットするための関数
    MStatus createChildren() override;

	// 特定のノードのアトリビュートとリンクするための関数
    MStatus connectToDependNode(const MObject & node) override;

	// Viewport 2.0 でマニピュレータを描画するための関数
	void		preDrawUI( const M3dView &view ) override;
	void		drawUI( MHWRender::MUIDrawManager& drawManager,
						const MHWRender::MFrameContext& frameContext) const override;

	MManipData startPointCallback(unsigned index) const;
	MVector nodeTranslation() const;

    MDagPath fDistanceManip; // マニピュレータのパス
	MDagPath fNodePath;

	// Value prepared for Viewport 2.0 draw
	MPoint fTextPosition;

public:
    static MTypeId id;
};

MTypeId footPrintLocatorManip::id(0x8001b);

MManipData footPrintLocatorManip::startPointCallback(unsigned index) 
const
{
	MFnNumericData numData;
	MObject numDataObj = numData.create(MFnNumericData::k3Double);
	MVector vec = nodeTranslation();
	numData.setData(vec.x, vec.y, vec.z);
	return MManipData(numDataObj);
}


MVector footPrintLocatorManip::nodeTranslation() const
{
	MFnDagNode dagFn(fNodePath);
	MDagPath path;
	dagFn.getPath(path);
	path.pop();  // pop from the shape to the transform
	MFnTransform transformFn(path);
	return transformFn.getTranslation(MSpace::kWorld);
}

MStatus footPrintLocatorManip::initialize()
{ 
    MStatus stat;
    stat = MPxManipContainer::initialize();
    return stat;
}


MStatus footPrintLocatorManip::createChildren()
{
    MString manipName("distanceManip");
    MString distanceName("distance");

	// ベースマニピュレータをセットする
    this->fDistanceManip = addDistanceManip(manipName, distanceName);
	
	// セットしたマニピュレータの初期セットアップを行う
	MFnDistanceManip distanceManipFn(this->fDistanceManip);
	MPoint startPoint(0.0, 0.0, 0.0);
	MVector direction(0.0, 1.0, 0.0);
	distanceManipFn.setStartPoint(startPoint);
	distanceManipFn.setDirection(direction);
	
    return MStatus::kSuccess;
}


MStatus footPrintLocatorManip::connectToDependNode(const MObject &node)
{
    MStatus stat;

	// ノードのDAGパスをメンバー変数に保存する
	MFnDagNode dagNodeFn(node);
	dagNodeFn.getPath(this->fNodePath);

    // マニピュレータとノードのアトリビュートをリンクする
    MFnDistanceManip distanceManipFn(this->fDistanceManip);
    MFnDependencyNode nodeFn(node);
	MPlug sizePlug = nodeFn.findPlug("size",  true,  &stat);
    if (MStatus::kFailure != stat) {
		// マニピュレータで操作する値とアトリビュートをリンクする（この例ではdistanceとsizeのプラグが紐づけされている）
	    distanceManipFn.connectToDistancePlug(sizePlug);

		// マニピュレータ起動時に実行する関数を登録する（この例ではマニピュレータの描画位置を更新する処理が紐づけされている）
		unsigned startPointIndex = distanceManipFn.startPointIndex();
	    addPlugToManipConversionCallback(startPointIndex, 
										 (plugToManipConversionCallback) 
										 &footPrintLocatorManip::startPointCallback);

		// also let the user tweak the size via the In-View Editor
		addPlugToInViewEditor( sizePlug );

		finishAddingManips();
	    MPxManipContainer::connectToDependNode(node);
	}

    return stat;
}

// マニピュレータの描画にUIを追加する場合
// UIの描画前に実行する関数
void	footPrintLocatorManip::preDrawUI( const M3dView &view )
{
	// テキストUIを描画する位置情報を更新する
	fTextPosition = nodeTranslation();
}

// テキストUIを描画する例
void	footPrintLocatorManip::drawUI(
	MHWRender::MUIDrawManager& drawManager,
	const MHWRender::MFrameContext& frameContext ) const
{
	drawManager.beginDrawable();

	drawManager.setColor( MColor( 0.0f, 1.0f, 0.1f ) );
	drawManager.text( fTextPosition, "Stretch Me!!!!!!!!", MHWRender::MUIDrawManager::kLeft );

	drawManager.text2d( MPoint(100,100), "Stretch Me 2D!!!!!!!!!!", MHWRender::MUIDrawManager::kLeft );

	drawManager.endDrawable();
}


class footPrintLocator : public MPxLocatorNode
{
public:
	footPrintLocator();
	~footPrintLocator() override; 

    MStatus   		compute(const MPlug& plug, MDataBlock &data) override;

	bool            isBounded() const override;
	MBoundingBox    boundingBox() const override; 

	static  void *          creator();
	static  MStatus         initialize();

	static  MObject         size;         // The size of the foot

public: 
	static	MTypeId		id;
	static  MString		drawDbClassification;
	static  MString		drawRegistrantId;
};


MTypeId footPrintLocator::id( 0x8001c );
MObject footPrintLocator::size;
MString	footPrintLocator::drawDbClassification("drawdb/geometry/footPrintLocator");
MString	footPrintLocator::drawRegistrantId("FootPrintManipPlugin");

footPrintLocator::footPrintLocator() 
{}


footPrintLocator::~footPrintLocator() 
{}


MStatus footPrintLocator::compute(const MPlug &plug, MDataBlock &data)
{ 
	return MS::kUnknownParameter;
}


bool footPrintLocator::isBounded() const
{ 
	return true;
}


MBoundingBox footPrintLocator::boundingBox() const
{   
	// Get the size
	//
	MObject thisNode = thisMObject();
	MPlug plug(thisNode, size);
	MDistance sizeVal;
	plug.getValue(sizeVal);

	double multiplier = sizeVal.asCentimeters();
 
	MPoint corner1(-0.17, 0.0, -0.7);
	MPoint corner2(0.17, 0.0, 0.3);

	corner1 = corner1 * multiplier;
	corner2 = corner2 * multiplier;

	return MBoundingBox(corner1, corner2);
}


void* footPrintLocator::creator()
{
	return new footPrintLocator();
}


MStatus footPrintLocator::initialize()
{ 
	MFnUnitAttribute unitFn;
	MStatus			 stat;

	size = unitFn.create("size", "sz", MFnUnitAttribute::kDistance);
	unitFn.setDefault(10.0);
	unitFn.setStorable(true);
	unitFn.setWritable(true);

	stat = addAttribute(size);
	if (!stat) {
		stat.perror("addAttribute");
		return stat;
	}
	
	MPxManipContainer::addToManipConnectTable(id);

	return MS::kSuccess;
}


class FootPrintLocatorData : public MUserData
{
public:
	MColor fColor;
	MPointArray fLineList;
	MPointArray fTriangleList;
};

class FootPrintLocatorDrawOverride : public MHWRender::MPxDrawOverride
{
public:
	static MHWRender::MPxDrawOverride* Creator(const MObject& obj)
	{
		return new FootPrintLocatorDrawOverride(obj);
	}

	~FootPrintLocatorDrawOverride() override;

	MHWRender::DrawAPI supportedDrawAPIs() const override;

	bool isBounded(
		const MDagPath& objPath,
		const MDagPath& cameraPath) const override;

	MBoundingBox boundingBox(
		const MDagPath& objPath,
		const MDagPath& cameraPath) const override;

	bool disableInternalBoundingBoxDraw() const override;

	MUserData* prepareForDraw(
		const MDagPath& objPath,
		const MDagPath& cameraPath,
		const MHWRender::MFrameContext& frameContext,
		MUserData* oldData) override;

	bool hasUIDrawables() const override { return true; }

	void addUIDrawables(
		const MDagPath& objPath,
		MHWRender::MUIDrawManager& drawManager,
		const MHWRender::MFrameContext& frameContext,
		const MUserData* data) override;

protected:
	MBoundingBox mCurrentBoundingBox;

	MCallbackId fModelEditorChangedCbId;
	MObject fFootPrintLocator;

private:
	FootPrintLocatorDrawOverride(const MObject& obj);
	float getMultiplier(const MDagPath& objPath) const;

	static void OnModelEditorChanged(void *clientData);
};

// By setting isAlwaysDirty to false in MPxDrawOverride constructor, the
// draw override will be updated (via prepareForDraw()) only when the node
// is marked dirty via DG evaluation or dirty propagation. Additional
// callback is also added to explicitly mark the node as being dirty (via
// MRenderer::setGeometryDrawDirty()) for certain circumstances. Note that
// the draw callback in MPxDrawOverride constructor is set to NULL in order
// to achieve better performance.
FootPrintLocatorDrawOverride::FootPrintLocatorDrawOverride(const MObject& obj)
	: MHWRender::MPxDrawOverride(obj, NULL, false)
	, fFootPrintLocator(obj)
{
	fModelEditorChangedCbId = MEventMessage::addEventCallback(
		"modelEditorChanged", OnModelEditorChanged, this);
}

FootPrintLocatorDrawOverride::~FootPrintLocatorDrawOverride()
{
	if (fModelEditorChangedCbId != 0)
	{
		MMessage::removeCallback(fModelEditorChangedCbId);
		fModelEditorChangedCbId = 0;
	}
}

void FootPrintLocatorDrawOverride::OnModelEditorChanged(void *clientData)
{
	// Mark the node as being dirty so that it can update on display mode switch,
	// e.g. between wireframe and shaded.
	FootPrintLocatorDrawOverride *ovr = static_cast<FootPrintLocatorDrawOverride*>(clientData);
	if (ovr) MHWRender::MRenderer::setGeometryDrawDirty(ovr->fFootPrintLocator);
}

MHWRender::DrawAPI FootPrintLocatorDrawOverride::supportedDrawAPIs() const
{
	return MHWRender::kAllDevices;
}

float FootPrintLocatorDrawOverride::getMultiplier(const MDagPath& objPath) const
{
	// Retrieve value of the size attribute from the node
	MStatus status;
	MObject footprintLocatorNode = objPath.node(&status);
	if (status)
	{
		MPlug plug(footprintLocatorNode, footPrintLocator::size);
		if (!plug.isNull())
		{
			MDistance sizeVal;
			if (plug.getValue(sizeVal))
			{
				return (float)sizeVal.asCentimeters();
			}
		}
	}

	return 1.0f;
}

bool FootPrintLocatorDrawOverride::isBounded(const MDagPath& /*objPath*/,
									  const MDagPath& /*cameraPath*/) const
{
	return true;
}

MBoundingBox FootPrintLocatorDrawOverride::boundingBox(
	const MDagPath& objPath,
	const MDagPath& cameraPath) const
{
	MPoint corner1( -0.17, 0.0, -0.7 );
	MPoint corner2( 0.17, 0.0, 0.3 );

	float multiplier = getMultiplier(objPath);
	corner1 = corner1 * multiplier;
	corner2 = corner2 * multiplier;

	FootPrintLocatorDrawOverride *nonConstThis = (FootPrintLocatorDrawOverride *)this;
	nonConstThis->mCurrentBoundingBox.clear();
	nonConstThis->mCurrentBoundingBox.expand( corner1 );
	nonConstThis->mCurrentBoundingBox.expand( corner2 );

	return mCurrentBoundingBox;
}

bool FootPrintLocatorDrawOverride::disableInternalBoundingBoxDraw() const
{
	return false;
}

MUserData* FootPrintLocatorDrawOverride::prepareForDraw(
	const MDagPath& objPath,
	const MDagPath& cameraPath,
	const MHWRender::MFrameContext& frameContext,
	MUserData* oldData)
{
	// Retrieve data cache (create if does not exist)
	FootPrintLocatorData* data = dynamic_cast<FootPrintLocatorData*>(oldData);
	if (!data)
	{
		data = new FootPrintLocatorData();
	}

	float fMultiplier = getMultiplier(objPath);

	data->fLineList.clear();

	for (int i = 0; i <= soleCount - 2; i++)
	{
		data->fLineList.append(sole[i][0] * fMultiplier, sole[i][1] * fMultiplier, sole[i][2] * fMultiplier);
		data->fLineList.append(sole[i + 1][0] * fMultiplier, sole[i + 1][1] * fMultiplier, sole[i + 1][2] * fMultiplier);
	}

	for (int i = 0; i <= heelCount - 2; i++)
	{
		data->fLineList.append(heel[i][0] * fMultiplier, heel[i][1] * fMultiplier, heel[i][2] * fMultiplier);
		data->fLineList.append(heel[i + 1][0] * fMultiplier, heel[i + 1][1] * fMultiplier, heel[i + 1][2] * fMultiplier);
	}

	data->fTriangleList.clear();

	for (int i = 1; i <= soleCount - 2; i++)
	{
		data->fTriangleList.append(sole[0][0] * fMultiplier, sole[0][1] * fMultiplier, sole[0][2] * fMultiplier);
		data->fTriangleList.append(sole[i][0] * fMultiplier, sole[i][1] * fMultiplier, sole[i][2] * fMultiplier);
		data->fTriangleList.append(sole[i + 1][0] * fMultiplier, sole[i + 1][1] * fMultiplier, sole[i + 1][2] * fMultiplier);
	}

	for (int i = 1; i <= heelCount - 2; i++)
	{
		data->fTriangleList.append(heel[0][0] * fMultiplier, heel[0][1] * fMultiplier, heel[0][2] * fMultiplier);
		data->fTriangleList.append(heel[i][0] * fMultiplier, heel[i][1] * fMultiplier, heel[i][2] * fMultiplier);
		data->fTriangleList.append(heel[i + 1][0] * fMultiplier, heel[i + 1][1] * fMultiplier, heel[i + 1][2] * fMultiplier);
	}

	// compute data and cache it
	data->fColor = MHWRender::MGeometryUtilities::wireframeColor(objPath);

	return data;
}

void FootPrintLocatorDrawOverride::addUIDrawables(
	const MDagPath& objPath,
	MHWRender::MUIDrawManager& drawManager,
	const MHWRender::MFrameContext& frameContext,
	const MUserData* data)
{
	FootPrintLocatorData* pLocatorData = (FootPrintLocatorData*)data;

	drawManager.beginDrawable();

	// Draw the foot print solid/wireframe
	drawManager.setColor( pLocatorData->fColor );
	drawManager.setDepthPriority(5);

	if (frameContext.getDisplayStyle() & MHWRender::MFrameContext::kGouraudShaded) {
		drawManager.mesh(MHWRender::MUIDrawManager::kTriangles, pLocatorData->fTriangleList);
	}

	drawManager.mesh(MHWRender::MUIDrawManager::kLines, pLocatorData->fLineList);

	// Draw a text "Foot"
	MPoint pos( 0.0, 0.0, 0.0 ); // Position of the text
	MColor textColor( 0.1f, 0.8f, 0.8f, 1.0f ); // Text color

	drawManager.setColor( textColor );
	drawManager.setFontSize( MHWRender::MUIDrawManager::kSmallFontSize );
	drawManager.text( pos,  MString("FootprintLocator"), MHWRender::MUIDrawManager::kCenter );

	drawManager.endDrawable();
}

MStatus initializePlugin(MObject obj)
{ 
	MStatus status;
	MFnPlugin plugin(obj, PLUGIN_COMPANY, "3.0", "Any");

	status = plugin.registerNode("footPrintLocator", 
								 footPrintLocator::id, 
								 &footPrintLocator::creator, 
								 &footPrintLocator::initialize,
								 MPxNode::kLocatorNode, 
								 &footPrintLocator::drawDbClassification
								 );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
		footPrintLocator::drawDbClassification,
		footPrintLocator::drawRegistrantId,
		FootPrintLocatorDrawOverride::Creator);
	if (!status) {
		status.perror("registerDrawOverrideCreator");
		return status;
	}
	
	
	status = plugin.registerNode("footPrintLocatorManip", 
								 footPrintLocatorManip::id, 
								 &footPrintLocatorManip::creator, 
								 &footPrintLocatorManip::initialize,
								 MPxNode::kManipContainer);
    if (!status) {
        status.perror("registerNode");
        return status;
    }

	return status;
}


MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterNode(footPrintLocator::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(
		footPrintLocator::drawDbClassification,
		footPrintLocator::drawRegistrantId);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	status = plugin.deregisterNode(footPrintLocatorManip::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
