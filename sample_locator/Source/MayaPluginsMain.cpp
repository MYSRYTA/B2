#include "CustomLocator.h"

#include <maya/MFnPlugin.h>

// ロード時の処理（初期化やノードの書式の登録など）
MStatus initializePlugin ( MObject obj ) {
	MStatus   status;
	MFnPlugin plugin ( obj, "TKCM", "1.0", "Any" );

	status = plugin.registerNode ("TKCMCustomLocator", TKCMCustomLocator::id, &TKCMCustomLocator::creator, &TKCMCustomLocator::initialize, MPxNode::kLocatorNode, &TKCMCustomLocator::drawDbClassification );
	CHECK_MSTATUS_AND_RETURN_IT ( status );
	status = MHWRender::MDrawRegistry::registerGeometryOverrideCreator ( TKCMCustomLocator::drawDbClassification, TKCMCustomLocator::drawRegistrantId, TKCMCustomLocatorGeoOverride::Creator );
	CHECK_MSTATUS_AND_RETURN_IT ( status );
	MSelectionMask::registerSelectionType ( "TKCMCustomLocatorSelection", 2 );
	status = MGlobal::executeCommand ( "selectType -byName \"TKCMCustomLocatorSelection\" 1" );
	CHECK_MSTATUS_AND_RETURN_IT ( status );

	return status;
}

// アンロード時の処理
MStatus uninitializePlugin ( MObject obj ) {
	MStatus   status;
	MFnPlugin plugin ( obj );

	status = MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator ( TKCMCustomLocator::drawDbClassification, TKCMCustomLocator::drawRegistrantId );
	CHECK_MSTATUS_AND_RETURN_IT ( status );
	status = releaseShaders ();
	CHECK_MSTATUS_AND_RETURN_IT ( status );
	status = plugin.deregisterNode ( TKCMCustomLocator::id );
	CHECK_MSTATUS_AND_RETURN_IT ( status );
	MSelectionMask::deregisterSelectionType ( "TKCMCustomLocatorSelection" );

	return status;
}