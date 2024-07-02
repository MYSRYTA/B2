#include "AAA.h"

#define kPluginNodeId 0x19970002
#define kPluginNodeName "AAA"

MObject AAA::mainCurve;
MObject AAA::mainVal;
MObject AAA::mainValType;
MObject AAA::subCurve;
MObject AAA::subParam;
MObject AAA::subValType;

MObject AAA::offsetMatrix;
MObject AAA::offsetRatio;
MObject AAA::offsetDirLerp;

MObject AAA::directionAxis;
MObject AAA::upvectorAxis;
MObject AAA::result;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MStatus AAA::compute ( const MPlug &plug, MDataBlock& dataBlock )
{
	MStatus status;

	MFnNurbsCurve _mainCurve = dataBlock.inputValue(this->mainCurve).asNurbsCurve();
	double _mainVal = dataBlock.inputValue(this->mainVal).asDouble();

	MFnNurbsCurve _subCurve = dataBlock.inputValue(this->subCurve).asNurbsCurve();
	double _subParam = dataBlock.inputValue(this->subParam).asDouble();

// length param
//	float len = _mainCurve.length();
//	len *= _subParam;
//	float _param = _mainCurve.findParamFromLength(len);
	
	MPoint poi;
	_mainCurve.getPointAtParam(_mainVal, poi);

	// output
	MTransformationMatrix xfo;
	xfo.setTranslation(poi, MSpace::kWorld);

	MDataHandle mDHandle = dataBlock.outputValue(this->result);
	mDHandle.set(xfo.asMatrix());

	dataBlock.setClean(plug);
	return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MStatus AAA::initialize()
{
	MFnTypedAttribute gAttr;
	MFnNumericAttribute nAttr;
	MFnCompoundAttribute cAttr;
	MFnMatrixAttribute mAttr;

	mainCurve = gAttr.create("mainCurve", "mainCurve", MFnData::kNurbsCurve);
	gAttr.setKeyable(true);
	mainVal = nAttr.create("mainVal", "mainVal", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	mainValType = nAttr.create("mainValType", "mainValType", MFnNumericData::kInt);
	nAttr.setKeyable(true);

	subCurve = gAttr.create("subCurve", "subCurve", MFnData::kNurbsCurve);
	gAttr.setKeyable(true);
	subParam = nAttr.create("subParam", "subParam", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	subValType = nAttr.create("subValType", "subValType", MFnNumericData::kInt);
	nAttr.setKeyable(true);

	offsetMatrix = mAttr.create("offsetMatrix", "offsetMatrix");
	mAttr.setKeyable(true);
	offsetRatio = nAttr.create("offsetRatio", "offsetRatio", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	offsetDirLerp = nAttr.create("offsetDirLerp", "offsetDirLerp", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setMax(1.0);
	nAttr.setMin(0.0);

	directionAxis = nAttr.create("directionAxis", "directionAxis", MFnNumericData::k3Double);
	nAttr.setKeyable(true);
	upvectorAxis = nAttr.create("upvectorAxis", "upvectorAxis", MFnNumericData::k3Double);
	nAttr.setKeyable(true);
	result = mAttr.create("result", "result");

	addAttribute (mainCurve);
	addAttribute (mainVal);
	addAttribute (mainValType);
	addAttribute (subCurve);
	addAttribute (subParam);
	addAttribute (subValType);
	addAttribute (offsetMatrix);
	addAttribute (offsetRatio);
	addAttribute (offsetDirLerp);
	addAttribute (directionAxis);
	addAttribute (upvectorAxis);
	addAttribute (result);

	attributeAffects(mainCurve, result);
	attributeAffects(mainVal, result);
	attributeAffects(mainValType, result);
	attributeAffects(subCurve, result);
	attributeAffects(subParam, result);
	attributeAffects(subValType, result);
	attributeAffects(offsetMatrix, result);
	attributeAffects(offsetRatio, result);
	attributeAffects(offsetDirLerp, result);
	attributeAffects(directionAxis, result);
	attributeAffects(upvectorAxis, result);
	attributeAffects(result, result);

	return MS::kSuccess;
}

MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, "TKCM", "20220216", "Any");

	status = plugin.registerNode(kPluginNodeName, kPluginNodeId, AAA::creator, AAA::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterNode(kPluginNodeId);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;
}