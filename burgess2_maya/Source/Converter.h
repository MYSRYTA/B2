#pragma once
#include "LogMessage.h"
#include <burgess2_main/Math.h>
#include <burgess2_main/Geometry.h>

// Maya API
#include <maya/MMatrix.h>
#include <maya/MGlobal.h>
#include <maya/MFloatArray.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnMesh.h>
#include <maya/MDoubleArray.h>
#include <maya/MEulerRotation.h>
//#include <maya/MColor.h>

//******************************************************************************
//! @file	Converter.h
//! @brief	 Maya ⇔ Burgess コンバータ
//******************************************************************************
/** @defgroup Maya_Integration
*　@{
*/
/**		@brief	Maya ⇔ Burgess コンバータ <br>
*			#include <burgess2_maya/Integration.h><br>
*			@code
*				// MMatrixをMat4へ変換
*				MMatrix mMat;
*				Mat4 bMat4 = Convert( mMat );
*				
*				// Vec3[]をMVectorArrayへ変換
*				std::vector<Vec3> bVec3Array;
*				MVectorArray mVecArray = Convert( bVec3Array );
*			@endcode
*/
/**		@defgroup Maya Converter
*　		@{
*/

inline std::vector<u32> Convert ( const MIntArray &maya );
inline MIntArray Convert ( const std::vector<u32> &u32Array );

inline std::vector<f32> Convert ( const MFloatArray &maya );
inline MFloatArray Convert ( const std::vector<f32> &u32Array );

/*! @name Maya Math*/
/* @{ */
inline String Convert ( const MString &maya );
inline MString Convert ( const String &str );

inline Mat4 Convert ( const MMatrix &maya );
inline MMatrix Convert ( const Mat4 &bgs );

inline Vec3 Convert ( const MVector &maya );
inline Vec3 Convert ( const MPoint &maya );
inline Vec3 Convert ( const MFloatVector &maya );
inline MVector Convert ( const Vec3 &bgs );
inline MVector ConvertMVector ( const Vec3 &bgs );
inline MPoint ConvertMPoint ( const Vec3 &bgs );
inline MFloatVector ConvertMFloatVector ( const Vec3 &bgs );

inline Rad3 Convert ( const MEulerRotation &maya );
inline MEulerRotation Convert ( const Rad3 &bgs );
inline MEulerRotation Convert ( const Rad3d &bgs );

inline MColor Convert ( const Vec4 &bgs );
inline Vec4 Convert ( const MColor &maya );

inline std::vector<Vec3> Convert ( const MVectorArray &maya );
inline std::vector<Vec3> Convert ( const MPointArray &maya );
inline std::vector<Vec3> Convert ( const MFloatVectorArray &maya );
inline std::vector<Vec4> Convert ( const MColorArray &maya );
inline MVectorArray  Convert ( const std::vector<Vec3>  &bgs );
inline MVectorArray  ConvertMVectorArray ( const std::vector<Vec3>  &bgs );
inline MPointArray  ConvertMPointArray ( const std::vector<Vec3>  &bgs );
inline MFloatVectorArray ConvertMFloatVectorArray ( const std::vector<Vec3>  &bgs );
inline MColorArray Convert ( const std::vector<Vec4>  &bgs );

/** @} */
/*! @name Maya Geometry*/
/* @{ */
inline Bool Convert ( IO BSplineCurves &curves, const MFnNurbsCurve &mFnCurve );
inline Bool Convert ( IO NURBSCurves &curves, const MFnNurbsCurve &mFnCurve );
inline Bool Convert ( IO CatmullRomCurves &curves, const MFnNurbsCurve &mFnCurve );
inline Bool Convert ( IO LinearCurves &curves, const MFnNurbsCurve &mFnCurve );
inline PolygonMesh Convert ( MFnMesh &mFnMesh, Bool allAttribute = true );
inline PolygonMesh Convert ( MFnMesh& mFnMesh, const std::vector<u32>& faceMask, Bool allAttribute = true );

inline MObject Convert ( OUT MFnNurbsCurve &mFnCurve, BSplineCurves &bgs, u32 curveIndex);
inline MObject Convert ( OUT MFnNurbsCurve &mFnCurve, NURBSCurves &bgs, u32 curveIndex );
inline MObject Convert ( OUT MFnNurbsCurve &mFnCurve, CatmullRomCurves &bgs, u32 curveIndex );
inline MObject Convert ( OUT MFnNurbsCurve &mFnCurve, LinearCurves &bgs, u32 curveIndex );
inline MObject Convert ( OUT MFnMesh &mFnMesh, const PolygonMesh &bgs );

/** @} */
/** @} */
/** @} */

inline Bool CreateBurgessCurveDataFromMayaCurve ( u8 &degree, u8 &curveForm, std::vector<Vec3> &curvePoints, std::vector<f32> &knots, const MFnNurbsCurve &mFnCurve );
inline Bool CreateMayaCurveDataFromBurgessCurve ( u8 &mDegree, MFnNurbsCurve::Form &mForm, MPointArray &mayaPoints, MDoubleArray &mayaKnots, BSplineCurves &curves, u32 curveID );

#include "Converter_Geometry.cpp"
#include "Converter_Math.cpp"
#include "Converter.cpp"
//******************************************************************************
//! End of File
//******************************************************************************