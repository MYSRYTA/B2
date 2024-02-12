#include "IO.h"

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnNurbs port ⇒  BSplineCurves
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Input ( IO BSplineCurves &curves, MDataBlock& dataBlock, const MObject &mFnCurve ) {
	MFnNurbsCurve mCurve = dataBlock.inputValue ( mFnCurve ).asNurbsCurve ();
	if ( mCurve.numCVs () == 0 ) {
		return false;
	}
	return Convert ( curves, mCurve );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnNurbs port ⇒  NURBSCurves
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Input ( IO NURBSCurves &curves, MDataBlock& dataBlock, const MObject &mFnCurve ) {
	MFnNurbsCurve mCurve = dataBlock.inputValue ( mFnCurve ).asNurbsCurve ();
	if ( mCurve.numCVs () == 0 ) {
		return false;
	}
	return Convert ( curves, mCurve );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnNurbs port ⇒  CatmullRomCurves
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Input ( IO CatmullRomCurves &curves, MDataBlock& dataBlock, const MObject &mFnCurve ) {
	MFnNurbsCurve mCurve = dataBlock.inputValue ( mFnCurve ).asNurbsCurve ();
	if ( mCurve.numCVs () == 0 ) {
		return false;
	}
	return Convert ( curves, mCurve );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnNurbs port ⇒  LinearCurves
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Input ( IO LinearCurves &curves, MDataBlock& dataBlock, const MObject &mFnCurve ) {
	MFnNurbsCurve mCurve = dataBlock.inputValue ( mFnCurve ).asNurbsCurve ();
	if ( mCurve.numCVs () == 0 ) {
		return false;
	}
	return Convert ( curves, mCurve );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnMesh port ⇒  PolygonMesh
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Input ( OUT PolygonMesh &mesh, MDataBlock& dataBlock, const MObject &mFnMesh, Bool allAttribute ){
	MFnMesh mMesh = dataBlock.inputValue ( mFnMesh ).asMesh ();
	if ( mMesh.numPolygons () == 0 ){
		return false;
	}
	mesh = Convert ( mMesh, allAttribute );
	return 0 < mesh.polygonCount ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnMesh port ⇒  PolygonMesh
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Input ( OUT PolygonMesh& mesh, MDataBlock& dataBlock, const MObject& mFnMesh, const std::vector<u32>& faceMask, Bool allAttribute ) {
	MFnMesh mMesh = dataBlock.inputValue ( mFnMesh ).asMesh ();
	if ( mMesh.numPolygons () == 0 ) {
		return false;
	}
	mesh = Convert ( mMesh, faceMask, allAttribute );
	return 0 < mesh.polygonCount ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnMesh port ⇒  Vec3[] 頂点座標のみ取得
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool InputPoints ( OUT std::vector<Vec3> &vec3, MDataBlock& dataBlock, const MObject &mFnMesh ) {
	MFnMesh mMesh = dataBlock.inputValue ( mFnMesh ).asMesh ();
	if ( mMesh.numPolygons() == 0 ) {
		return false;
	}
	MPointArray mPoints;
	mMesh.getPoints ( mPoints );
	vec3 = Convert ( mPoints );
	mPoints.clear ();
	return 0 < vec3.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnMesh port ⇒  トポロジー(頂点とポリゴン)のみのPolygonMesh
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool InputTopology ( OUT PolygonMesh &topology, MDataBlock& dataBlock, const MObject &mFnMesh ){
	MFnMesh mMesh = dataBlock.inputValue ( mFnMesh ).asMesh ();
	if ( mMesh.numPolygons () == 0 ){
		return false;
	}
	topology = Convert ( mMesh, false );
	return 0 < topology.polygonCount ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnMesh[] port ⇒  PolygonMesh[]
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Input ( OUT std::vector<PolygonMesh> &mesh, MDataBlock& dataBlock, const MObject &mFnMesh ){
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( mFnMesh );
	mesh.resize ( arrayHandle.elementCount () );
	for ( u32 i = 0; i < arrayHandle.elementCount (); ++i ){
		arrayHandle.jumpToElement ( i );
		MFnMesh mMesh = arrayHandle.inputValue ().asMesh ();
		if ( mMesh.numPolygons () == 0 ){
			return false;
		}
		mesh[i] = Convert ( mMesh );
	}
	for ( u32 i = 0; i < arrayHandle.elementCount (); ++i ){
		if( mesh[i].polygonCount () == 0 ){
			mesh.resize ( 0 );
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnMesh[] port ⇒  Vec3[][] 頂点座標のみ取得
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool InputPoints ( OUT std::vector<std::vector<Vec3>> &vec3, MDataBlock& dataBlock, const MObject &mFnMesh ){
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( mFnMesh );
	vec3.resize ( arrayHandle.elementCount () );
	for ( u32 i = 0; i < arrayHandle.elementCount (); ++i ){
		arrayHandle.jumpToElement ( i );
		MFnMesh mMesh = arrayHandle.inputValue ().asMesh ();
		if ( mMesh.numPolygons () == 0 ){
			return false;
		}
		MPointArray mPoints;
		mMesh.getPoints ( mPoints );
		vec3[i] = Convert ( mPoints );
		mPoints.clear ();
	}
	for ( u32 i = 0; i < arrayHandle.elementCount (); ++i ){
		if ( vec3[i].size() == 0 ){
			vec3.resize ( 0 );
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnMesh[] port ⇒  トポロジー(頂点とポリゴン)のみのPolygonMesh[]
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool InputTopology ( OUT std::vector<PolygonMesh> &topology, MDataBlock& dataBlock, const MObject &mFnMesh ){
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( mFnMesh );
	topology.resize ( arrayHandle.elementCount () );
	for ( u32 i = 0; i < arrayHandle.elementCount (); ++i ){
		arrayHandle.jumpToElement ( i );
		MFnMesh mMesh = arrayHandle.inputValue ().asMesh ();
		if ( mMesh.numPolygons () == 0 ){
			return false;
		}
		topology[i] = Convert ( mMesh, false );
	}
	for ( u32 i = 0; i < arrayHandle.elementCount (); ++i ){
		if ( topology[i].polygonCount () == 0 ){
			topology.resize ( 0 );
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		BSplineCurves ⇒ MFnNurbs port 
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Output ( OUT MObject &mFnCurve, OUT MDataBlock& dataBlock, BSplineCurves &curves, u32 curveID ) {
	if ( curves.curveCount () == 0 || curves.curveCount () < curveID ) {
		return false;
	}
	MFnNurbsCurve mOutCurve;
	MObject outCurveObj = Convert ( mOutCurve, curves, curveID );
	if ( 1 < mOutCurve.numCVs() ) {
		MDataHandle outCurveHandle = dataBlock.outputValue ( mFnCurve );
		outCurveHandle.setMObject ( outCurveObj );
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		NURBSCurves  ⇒  MFnNurbs port
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Output ( OUT MObject &mFnCurve, OUT MDataBlock& dataBlock, NURBSCurves &curves, u32 curveID ) {
	if ( curves.curveCount () == 0 || curves.curveCount () < curveID ) {
		return false;
	}
	MFnNurbsCurve mOutCurve;
	MObject outCurveObj = Convert ( mOutCurve, curves, curveID );
	if ( 1 < mOutCurve.numCVs () ) {
		MDataHandle outCurveHandle = dataBlock.outputValue ( mFnCurve );
		outCurveHandle.setMObject ( outCurveObj );
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		CatmullRomCurves  ⇒  MFnNurbs port
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Output ( OUT MObject &mFnCurve, OUT MDataBlock& dataBlock, CatmullRomCurves &curves, u32 curveID ) {
	if ( curves.curveCount () == 0 || curves.curveCount () < curveID ) {
		return false;
	}
	MFnNurbsCurve mOutCurve;
	MObject outCurveObj = Convert ( mOutCurve, curves, curveID );
	if ( 1 < mOutCurve.numCVs () ) {
		MDataHandle outCurveHandle = dataBlock.outputValue ( mFnCurve );
		outCurveHandle.setMObject ( outCurveObj ); 
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		LinearCurves  ⇒  MFnNurbs port
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Output ( OUT MObject &mFnCurve, OUT MDataBlock& dataBlock, LinearCurves &curves, u32 curveID ) {
	if ( curves.curveCount () == 0 || curves.curveCount () < curveID ) {
		return false;
	}
	MFnNurbsCurve mOutCurve;
	MObject outCurveObj = Convert ( mOutCurve, curves, curveID );
	if ( 1 < mOutCurve.numCVs () ) {
		MDataHandle outCurveHandle = dataBlock.outputValue ( mFnCurve );
		outCurveHandle.setMObject ( outCurveObj );
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		PolygonMesh ⇒ MFnMesh port 
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Output ( OUT MObject &mFnMesh, OUT MDataBlock& dataBlock, const PolygonMesh &mesh ) {
	if ( mesh.polygonCount() == 0 ) {
		return false;
	}
	MFnMesh mOutMesh;
	MObject outMeshObject = Convert ( mOutMesh, mesh );
	if ( 0 < mOutMesh.numPolygons() ) {
		MDataHandle outMeshHandle = dataBlock.outputValue ( mFnMesh );
		outMeshHandle.setMObject ( outMeshObject );
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		PolygonMesh[] ⇒ MFnMesh[] port
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Output ( OUT MObject &mFnMeshArray, OUT MDataBlock& dataBlock, const std::vector<PolygonMesh> &mesh ){
	MArrayDataHandle arrayHandle = dataBlock.outputArrayValue ( mFnMeshArray );
	MArrayDataBuilder arrayBuilder = arrayHandle.builder ();
	s32 n = s32( mesh.size() );
	for ( s32 i = 0; i < n; ++i ){
		MFnMesh mOutMesh;
		MObject outMeshObject = Convert ( mOutMesh, mesh[i] );
		arrayBuilder.addElement ( i ).set ( outMeshObject );
		arrayBuilder.addElement ( i ).setClean ();
	}
	arrayHandle.set ( arrayBuilder );
	arrayHandle.setAllClean ();
	return true;
}