#pragma once
#include <vector>

// MayaAPI
#include <maya/MPxNode.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MPoint.h>
#include <maya/MMatrix.h>
#include <maya/MGlobal.h>

class AAA : public MPxNode{
	public:
		AAA () {};
		virtual ~AAA () {};

		virtual MStatus compute(const MPlug &plug, MDataBlock& dataBlock) override;
		static  void*   creator() { return new AAA ();  };
		static  MStatus initialize();

		static MObject mainCurve;
		static MObject mainVal;
		static MObject mainValType; // Param or LengthRatio
		static MObject subCurve;
		static MObject subParam;
		static MObject subValType;
		
		static MObject offsetMatrix;
		static MObject offsetRatio;
		static MObject offsetDirLerp;

		static MObject directionAxis;
		static MObject upvectorAxis;
		static MObject result;
};