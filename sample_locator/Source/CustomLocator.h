#pragma once
#include <maya/MPxLocatorNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MDistance.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MEvaluationNode.h>

// Viewport 2.0 includes
#include <maya/MDrawRegistry.h>
#include <maya/MPxGeometryOverride.h>
#include <maya/MUserData.h>
#include <maya/MDrawContext.h>
#include <maya/MShaderManager.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MPointArray.h>

#include <unordered_map>

// Burgess
#include "../../MayaPluginIDList.h"
#include <Burgess_Maya/Integration.h>

/*
 *　カスタムシェイプの追加方法
 *  1 CustomLocator_shape.inlにシェイプを登録
 *  2 CustomLocator.cpp 38: _iconTypeに新シェイプを登録
 *  3 CustomLocator_draw.cpp 130:,178:,216:,246: this->iconTypeに新シェイプを登録
 *  4 AETKCMCustomLocatorTemplate.mel 4: プルダウンメニューに新シェイプ名を登録
 */


//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ロケーター
//-----------------------------------------------------------------------------------------------------------------------------------------------------
class TKCMCustomLocator : public MPxLocatorNode
{
public:
	static	MTypeId		id;
	static	MString		drawDbClassification;
	static	MString		drawRegistrantId;
	bool valueDirty;
	s32 iconType = 1;
	static  MObject icon;
	static  MObject size;
	static  MObject color0;
	static  MObject color1;
	static  MObject offsetTr;
	static  MObject offsetRot;
	static  MObject wire;
	static  MObject face;

	TKCMCustomLocator () {};
	virtual ~TKCMCustomLocator () override {};

	void draw ( M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status ) override {};
	MStatus compute ( const MPlug& plug, MDataBlock& data ) override;
	bool isBounded () const override { return true; };
	MSelectionMask getShapeSelectionMask () const override{ return MSelectionMask ( "TKCMCustomLocatorSelection" ); };
	SchedulingType schedulingType () const { return MPxNode::kParallel; }
	static void * creator (){ return new TKCMCustomLocator (); };

	virtual MStatus setDependentsDirty ( const MPlug& plug, MPlugArray& plugArray );
	MStatus preEvaluation ( const MDGContext& context, const MEvaluationNode& evaluationNode ) override;
	MBoundingBox boundingBox () const override;
	static MStatus initialize ();
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// Viewport 2.0用　描画クラス
//-----------------------------------------------------------------------------------------------------------------------------------------------------
class TKCMCustomLocatorGeoOverride : public MHWRender::MPxGeometryOverride
{
public:
	MObject mLocatorNode;
	Bool changed;
	
	u8 iconType = 1;
	Vec3 iconScale;
	Bool wireDraw;
	Bool faceDraw;
	Vec3 color0, color1;
	Vec3 offsetTr;
	Rad3 offsetRot;
	
	TKCMCustomLocatorGeoOverride ( const MObject& obj )
		: MHWRender::MPxGeometryOverride ( obj )
		, mLocatorNode ( obj )
		, iconScale ( 1.0f )
		, changed ( true )
	{}
	~TKCMCustomLocatorGeoOverride () override {};

	static MHWRender::MPxGeometryOverride* Creator ( const MObject& obj ) { return new TKCMCustomLocatorGeoOverride ( obj ); }
	MHWRender::DrawAPI supportedDrawAPIs () const override { return (MHWRender::kOpenGL | MHWRender::kDirectX11); };
	bool isIndexingDirty ( const MHWRender::MRenderItem &item ) override { return this->changed; }
	bool isStreamDirty ( const MHWRender::MVertexBufferDescriptor &desc ) override { return this->changed; }
	bool hasUIDrawables () const override { return false; }
	bool traceCallSequence () const override { return false; }
	
	void updateDG () override;
	void updateRenderItems ( const MDagPath &path, MHWRender::MRenderItemList& list ) override;
	void populateGeometry ( const MHWRender::MGeometryRequirements &requirements, const MHWRender::MRenderItemList &renderItems, MHWRender::MGeometry &data ) override;
	void cleanUp () override {};
};

#include "CustomLocator.inl"
#include "CustomLocator_shape.inl"