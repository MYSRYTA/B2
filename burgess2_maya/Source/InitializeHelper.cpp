#include "initializeHelper.h"

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Euler型のポートを作成する
//------------------------------------------------------------------------------------------------------------------------------------------------
MObject CreateEulerPort ( IO MFnCompoundAttribute &cAttr, const String &portName ) {
	MFnUnitAttribute uAttr;
	MString n = MString () + portName.c_str ();
	MString x = MString () + portName.c_str () + "RotX";
	MString y = MString () + portName.c_str () + "RotY";
	MString z = MString () + portName.c_str () + "RotZ";
	auto result = cAttr.create ( n, n );
	cAttr.addChild ( uAttr.create ( x, x, MFnUnitAttribute::kAngle ) );
	cAttr.addChild ( uAttr.create ( y, y, MFnUnitAttribute::kAngle ) );
	cAttr.addChild ( uAttr.create ( z, z, MFnUnitAttribute::kAngle ) );
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Float型の2DArrayポートを作成する （nAttrには関数内でsetArray(true)を実行済み）<br>
//!		**Maya2018では動的ポート生成にバグあり。compObjではなく全てのportOBJを接続して回避できる
//------------------------------------------------------------------------------------------------------------------------------------------------
void CreateFloat2DArrayPort ( IO MFnNumericAttribute &nAttr, 
							  IO MObject &compObj, 
							  IO MObject &portOBJ, 
							  const String &compName, 
							  const String &portName )
{
	MString n = MString () + portName.c_str ();
	portOBJ = nAttr.create ( n, n, MFnNumericData::kFloat );
	nAttr.setArray ( true );

	MFnCompoundAttribute cmpAttr;
	n = MString () + compName.c_str ();
	compObj = cmpAttr.create ( n, n );
	cmpAttr.setArray ( true );
	cmpAttr.addChild ( portOBJ );
}
