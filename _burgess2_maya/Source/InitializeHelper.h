#pragma once
// Maya API
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MString.h>

#include "burgess2_main/Core/base.h"

//******************************************************************************
//! @file	InitializeHelper.h
//! @brief	 Initialize ヘルパー
//******************************************************************************
/** @defgroup Maya_Integration
*　@{
*/
/**		@brief	Initialize ヘルパー <br>
*			#include <burgess2_maya/Integration.h><br>
*/

/**		@defgroup Initialize_Helper
 *		@code
 *			MObject NodeClass::mObjAAA; // maya in Euler
 *		
	*		MStatus NodeClass::initialize () {
	*			MFnCompoundAttribute cAttr;
	*			mObjAAA = CreateEulerPort ( cAttr, "portName" );
	*			cAttr.setKeyable ( true );
	*			cAttr.setArray ( true );
	*		}
	*	@endcode
*　		@{
*/

inline MObject CreateEulerPort ( IO MFnCompoundAttribute &cAttr, const String &portName );

inline void CreateFloat2DArrayPort ( IO MFnNumericAttribute &nAttr, IO MObject &compObj, IO MObject &portOBJ, const String &compName, const String &portName );

#include "InitializeHelper.cpp"
/** @} */
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************