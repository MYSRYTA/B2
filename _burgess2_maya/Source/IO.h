#pragma once
#include "Converter.h"
#include "Burgess2MayaData.h"

#include <maya/MPxNode.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MFnPluginData.h>

//******************************************************************************
//! @file	IO.h
//! @brief	 Maya node input port/output port ⇔ Burgess
//******************************************************************************
/** @defgroup Maya_Integration
*　@{
*/
/**		@brief	Mayaノードのin/outポートとBurgess間のデータコンバータ <br>
*			#include <burgess2_maya/Integration.h><br>
*			
*			@code
*				MObject NodeClass::mObjAAA; // maya in vector3
*				MObject NodeClass::mObjBBB; // maya out vector3
*				MObject NodeClass::mObjCCC; // maya in PolygonMesh
*				MObject NodeClass::mObjDDD; // maya in Curve
*				
*				MStatus NodeClass::compute ( const MPlug &plug, MDataBlock& dataBlock ) {
*					Vec3 bVec3;
*					Input ( bVec3, dataBlock, mObjAAA ); // MVector型のポートの値をVec3データとして取得する
*					Output ( mObjBBB, dataBlock, bVec3 );　// Vec3の値をMVector型のポートにセットする
*
*					PolygonMesh mesh;
*					Input ( mesh, dataBlock, mObjCCC ); // MFnMesh型のポートのデータをPolygonMeshデータとして取得する
*
*					BSplineCurves curves;
*					Input ( curves, dataBlock, mObjDDD ); // MFnNurbsCurve型のポートのデータをBSplineCurvesデータとして取得する( curvesに追加 )
*
*					dataBlock.setClean ( plug );
*					return status;
*				}
*			@endcode
*/

/**		@defgroup Maya_IO
*　		@{
*/
inline void Input ( OUT s32 &val, MDataBlock& dataBlock, const MObject &mS32 );
inline void Input ( OUT f32 &val, MDataBlock& dataBlock, const MObject &mF32 );
inline void Input ( OUT f64 &val, MDataBlock& dataBlock, const MObject &mF64 );
inline void Input ( OUT Bool &val, MDataBlock& dataBlock, const MObject &mBool );
inline void Input ( OUT String &val, MDataBlock& dataBlock, const MObject &mStr );
inline void Input ( OUT std::vector<s32>& val, MDataBlock& dataBlock, const MObject& mS32 );
inline void Input ( OUT std::vector<f32>& val, MDataBlock& dataBlock, const MObject& mF32 );
inline void Input ( OUT std::vector<f64>& val, MDataBlock& dataBlock, const MObject& mF64 );
inline void Input ( OUT std::vector<Bool>& val, MDataBlock& dataBlock, const MObject& mBool );
inline void Input ( OUT std::vector<String>& val, MDataBlock& dataBlock, const MObject& mStr );

inline void Output ( MObject &mBool, OUT MDataBlock& dataBlock, const std::vector<Bool> &b );

inline void Input ( OUT std::vector < std::vector<f32>> &val,
			 MDataBlock& dataBlock, 
			 const MObject &compVal, 
			 const MObject &arrayVal );
inline void Input ( OUT std::vector < std::vector<f32>> &val,
			 MDataBlock& dataBlock,
			 const MObject &compVal,
			 const MObject &arrayVal,
			 u32 compSize,
			 u32 valSize);
inline void InputSkinWeightList ( IO PolygonMesh &mesh,
						   MDataBlock& dataBlock, 
						   const MObject &weightList, 
						   const MObject &weights, 
						   const String &attrName = "weightList" );
inline void InputSkinWeightList ( IO PolygonMesh &mesh,
						   MDataBlock& dataBlock,
						   const MObject &weightList,
						   const MObject &weights,
						   u32 jointSize,
						   const String &attrName = "weightList" );

/*! @name Maya Math*/
/* @{ */
inline void Input ( OUT Mat4 &mat4, MDataBlock& dataBlock, const MObject &mMat4 );
inline void Input ( OUT Xfo &Xfo, MDataBlock& dataBlock, const MObject &mMat4 );
inline void Input ( OUT Vec3 &vec3, MDataBlock& dataBlock, const MObject &mVec3 );
inline void Input ( OUT Vec2& vec2, MDataBlock& dataBlock, const MObject& mVec2 );
inline void Input ( OUT Vec4& vec4, MDataBlock& dataBlock, const MObject& mVec4 );
inline void Input ( OUT Rad3 &rad3, MDataBlock& dataBlock, const MObject &mEuler );
inline void Input ( OUT std::vector<Mat4> &mat4, MDataBlock& dataBlock, const MObject &mMat4 );
inline void Input ( OUT std::vector<Xfo> &Xfo, MDataBlock& dataBlock, const MObject &mMat4 );
inline void Input ( OUT std::vector<Vec3> &vec3, MDataBlock& dataBlock, const MObject &mVec3 );
inline void Input ( OUT std::vector<Rad3> &rad3, MDataBlock& dataBlock, const MObject &mEuler );

inline void Output ( MObject &mMat4, OUT MDataBlock& dataBlock, const Mat4 &mat4 );
inline void Output ( MObject &mMat4, OUT MDataBlock& dataBlock, const Xfo &Xfo );
inline void Output ( MObject &mVec3, OUT MDataBlock& dataBlock, const Vec3 &vec3 );
inline void Output ( MObject &mEuler, OUT MDataBlock& dataBlock, const Rad3 &rad3 );
inline void Output ( MObject &mEuler, OUT MDataBlock& dataBlock, const Rad3d &rad3 );
inline void Output ( MObject &mMat4, OUT MDataBlock& dataBlock, const std::vector<Mat4> &mat4 );
inline void Output ( MObject &mMat4, OUT MDataBlock& dataBlock, const std::vector<Xfo> &Xfo );
inline void Output ( MObject &mVec3, OUT MDataBlock& dataBlock, const std::vector<Vec3> &vec3 );
inline void Output ( MObject &mEuler, OUT MDataBlock& dataBlock, const std::vector<Rad3> &rad3 );
inline void Output ( MObject &mEuler, OUT MDataBlock& dataBlock, const std::vector<Rad3d> &rad3 );
/** @} */

/*! @name Maya Geometry*/
/* @{ */
inline Bool Input ( IO BSplineCurves &curves, MDataBlock& dataBlock, const MObject &mFnCurve);
inline Bool Input ( IO NURBSCurves &curves, MDataBlock& dataBlock, const MObject &mFnCurve );
inline Bool Input ( IO CatmullRomCurves &curves, MDataBlock& dataBlock, const MObject &mFnCurve );
inline Bool Input ( IO LinearCurves &curves, MDataBlock& dataBlock, const MObject &mFnCurve );
inline Bool Input ( OUT PolygonMesh &mesh, MDataBlock& dataBlock, const MObject &mFnMesh, Bool allAttribute = true );
inline Bool Input ( OUT PolygonMesh& mesh, MDataBlock& dataBlock, const MObject& mFnMesh, const std::vector<u32>& faceMask, Bool allAttribute = true );
inline Bool InputPoints ( OUT std::vector<Vec3> &vec3, MDataBlock& dataBlock, const MObject &mFnMesh );
inline Bool InputTopology ( OUT PolygonMesh &topology, MDataBlock& dataBlock, const MObject &mFnMesh );
inline Bool Input ( OUT std::vector<PolygonMesh> &mesh, MDataBlock& dataBlock, const MObject &mFnMesh );
inline Bool InputPoints ( OUT std::vector<std::vector<Vec3>> &vec3, MDataBlock& dataBlock, const MObject &mFnMesh );
inline Bool InputTopology ( OUT std::vector<PolygonMesh> &topology, MDataBlock& dataBlock, const MObject &mFnMesh );

inline Bool Output ( OUT MObject &mFnCurve, OUT MDataBlock& dataBlock, BSplineCurves &curves, u32 curveID );
inline Bool Output ( OUT MObject &mFnCurve, OUT MDataBlock& dataBlock, NURBSCurves &curves, u32 curveID );
inline Bool Output ( OUT MObject &mFnCurve, OUT MDataBlock& dataBlock, CatmullRomCurves &curves, u32 curveID );
inline Bool Output ( OUT MObject &mFnCurve, OUT MDataBlock& dataBlock, LinearCurves &curves, u32 curveID );
inline Bool Output ( OUT MObject &mFnMesh, OUT MDataBlock& dataBlock, const PolygonMesh &mesh );
inline Bool Output ( OUT MObject &mFnMeshArray, OUT MDataBlock& dataBlock, const std::vector<PolygonMesh> &mesh );
/** @} */
/*! @name custom class data */
/* @{ */
template<class C>
Bool InputC ( OUT C &val, MDataBlock& dataBlock, const MObject &burgessMayaData );

template<class C>
Bool OutputC ( OUT MObject &burgessMayaData, OUT MDataBlock& dataBlock, const C &val );

#include "IO.cpp"
#include "IO_Burgess2MayaData.inl"
#include "IO_Geometry.cpp"
#include "IO_Math.cpp"
/** @} */

/** @} */
/** @} */
//******************************************************************************
//! End of File
//******************************************************************************