#include "IO.h"

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MMatrix port ÅÀ Mat4
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT Mat4 &mat4, MDataBlock& dataBlock, const MObject &mMat4 ) {
	mat4 = Convert ( dataBlock.inputValue ( mMat4 ).asMatrix () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MMatrix port ÅÀ Xfo
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT Xfo &xfo, MDataBlock& dataBlock, const MObject &mMat4 ){
	xfo = Xfo ( Convert ( dataBlock.inputValue ( mMat4 ).asMatrix () ) );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MVector port ÅÀ Vec3
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT Vec3 &vec3, MDataBlock& dataBlock, const  MObject  &mVec3 ) {
	vec3 = Convert ( dataBlock.inputValue ( mVec3 ).asVector () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MVector port ÅÀ Vec2
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT Vec2& vec2, MDataBlock& dataBlock, const  MObject& mVec2 ) {
	double2& d2 = dataBlock.inputValue ( mVec2 ).asDouble2 ();
	std::vector<f32> val ( 2 );
	val[0] = d2[0];
	val[1] = d2[1];
	vec2 = Vec2 ( val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MVector port ÅÀ Vec4
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT Vec4& vec4, MDataBlock& dataBlock, const  MObject& mVec4 ) {
	vec4 = Convert ( dataBlock.inputValue ( mVec4 ).asVector () );
	double4& d4 = dataBlock.inputValue ( mVec4 ).asDouble4 ();
	std::vector<f32> val ( 4 );
	val[0] = d4[0];
	val[1] = d4[1];
	val[2] = d4[2];
	val[3] = d4[3];
	vec4 = Vec4 ( val );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MEulerRotation port ÅÀ Rad3
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT Rad3 &rad3, MDataBlock& dataBlock, const MObject &mEuler ) {
	auto a = dataBlock.inputValue ( mEuler ).asDouble3 ();
	rad3.x() = f32(a[0]);
	rad3.y() = f32(a[1]);
	rad3.z() = f32(a[2]);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MMatrix[] port ÅÀ Mat4[]
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT std::vector<Mat4> &mat4, MDataBlock& dataBlock, const MObject &mMat4 ) {
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( mMat4 );
	mat4.resize ( arrayHandle.elementCount () );
	for (u32 i = 0; i < arrayHandle.elementCount (); ++i) {
		arrayHandle.jumpToElement ( i );
		mat4[i] = Convert ( arrayHandle.inputValue ().asMatrix () );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MMatrix[] port ÅÀ Xfo[]
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT std::vector<Xfo> &xfo, MDataBlock& dataBlock, const MObject &mMat4 ){
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( mMat4 );
	xfo.resize ( arrayHandle.elementCount () );
	for (u32 i = 0; i < arrayHandle.elementCount (); ++i){
		arrayHandle.jumpToElement ( i );
		xfo[i] = Xfo ( Convert ( arrayHandle.inputValue ().asMatrix () ) );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MMVector[] port ÅÀ Vec3[]
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT std::vector<Vec3> &vec3, MDataBlock& dataBlock, const MObject &mVec3 ) {
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( mVec3 );
	vec3.resize ( arrayHandle.elementCount () );
	for (u32 i = 0; i < arrayHandle.elementCount (); ++i) {
		arrayHandle.jumpToElement ( i );
		vec3[i] = Convert ( arrayHandle.inputValue ().asVector () );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MEulerRotation [] port ÅÀ Rad3[]
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT std::vector<Rad3> &rad3, MDataBlock& dataBlock, const MObject &mEuler ) {
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( mEuler );
	rad3.resize ( arrayHandle.elementCount () );
	for (u32 i = 0; i < arrayHandle.elementCount (); ++i) {
		arrayHandle.jumpToElement ( i );
		auto a = arrayHandle.inputValue ().asDouble3 ();
		rad3[i].x() = f32(a[0]);
		rad3[i].y() = f32(a[1]);
		rad3[i].z() = f32(a[2]);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Mat4 ÅÀ MMatrix port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mMat4, OUT MDataBlock& dataBlock, const Mat4 &mat4 ) {
	MMatrix mMat = Convert ( mat4 );
	MDataHandle mDHandle = dataBlock.outputValue ( mMat4 );
	mDHandle.set ( mMat );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Xfo ÅÀ MMatrix port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mMat4, OUT MDataBlock& dataBlock, const Xfo &Xfo ){
	MMatrix mMat = Convert ( Mat4(Xfo) );
	MDataHandle mDHandle = dataBlock.outputValue ( mMat4 );
	mDHandle.set ( mMat );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Vec3 ÅÀ MVector port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mVec3, OUT MDataBlock& dataBlock, const Vec3 &vec3 ) {
	MDataHandle mDHandle = dataBlock.outputValue ( mVec3 );
	mDHandle.set3Double ( vec3.x(), vec3.y(), vec3.z() );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Rad3<f32> ÅÀ MEulerRotation port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mEuler, OUT MDataBlock& dataBlock, const Rad3 &rad3 ){
	MDataHandle mDHandle = dataBlock.outputValue ( mEuler );
	mDHandle.set3Double ( f64(rad3.x() ), f64(rad3.y() ), f64(rad3.z() ) );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Rad3 ÅÀ MEulerRotation port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mEuler, OUT MDataBlock& dataBlock, const Rad3d &rad3 ) {
	MDataHandle mDHandle = dataBlock.outputValue ( mEuler );
	mDHandle.set3Double ( rad3.x(), rad3.y(), rad3.z() );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Mat4[] ÅÀ MMatrix[] port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mMat4, OUT MDataBlock& dataBlock, const std::vector<Mat4> &mat4 ) {
	MArrayDataHandle arrayHandle = dataBlock.outputArrayValue ( mMat4 );
	MArrayDataBuilder arrayBuilder = arrayHandle.builder ();
	s32 n = s32( mat4.size() );
	for (s32 i = 0; i < n; ++i ) {
		MMatrix resultMMat = Convert ( mat4[i] );
		arrayBuilder.addElement ( i ).set ( resultMMat );
		arrayBuilder.addElement ( i ).setClean ();
	}
	arrayHandle.set ( arrayBuilder );
	arrayHandle.setAllClean ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Xfo[] ÅÀ MMatrix[] port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mMat4, OUT MDataBlock& dataBlock, const std::vector<Xfo> &xfo ){
	MArrayDataHandle arrayHandle = dataBlock.outputArrayValue ( mMat4 );
	MArrayDataBuilder arrayBuilder = arrayHandle.builder ();
	s32 n = s32( xfo.size() );
	for (s32 i = 0; i < n; ++i){
		MMatrix resultMMat = Convert ( Mat4(xfo[i]) );
		arrayBuilder.addElement ( i ).set ( resultMMat );
		arrayBuilder.addElement ( i ).setClean ();
	}
	arrayHandle.set ( arrayBuilder );
	arrayHandle.setAllClean ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Vec3[] ÅÀ MVector [] port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mVec3, OUT MDataBlock& dataBlock, const std::vector<Vec3> &vec3 ) {
	MArrayDataHandle arrayHandle = dataBlock.outputArrayValue ( mVec3 );
	MArrayDataBuilder arrayBuilder = arrayHandle.builder ();
	s32 n = s32( vec3.size() );
	for (s32 i = 0; i < n; ++i) {
		MVector v = Convert ( vec3[i] );
		arrayBuilder.addElement ( i ).set ( v );
		arrayBuilder.addElement ( i ).setClean ();
	}
	arrayHandle.set ( arrayBuilder );
	arrayHandle.setAllClean ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Rad3[] ÅÀ MEulerRotation [] port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mEuler, OUT MDataBlock& dataBlock, const std::vector<Rad3> &rad3 ){
	MArrayDataHandle arrayHandle = dataBlock.outputArrayValue ( mEuler );
	MArrayDataBuilder arrayBuilder = arrayHandle.builder ();
	s32 n = s32( rad3.size() );
	for ( s32 i = 0; i < n; ++i ){
		MEulerRotation resultMEuler = Convert ( rad3[i] );
		MVector v = resultMEuler.asVector ();
		arrayBuilder.addElement ( i ).set ( v );
		arrayBuilder.addElement ( i ).setClean ();
	}
	arrayHandle.set ( arrayBuilder );
	arrayHandle.setAllClean ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Rad3[] ÅÀ MEulerRotation [] port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mEuler, OUT MDataBlock& dataBlock, const std::vector<Rad3d> &rad3 ) {
	MArrayDataHandle arrayHandle = dataBlock.outputArrayValue ( mEuler );
	MArrayDataBuilder arrayBuilder = arrayHandle.builder ();
	s32 n = s32( rad3.size() );
	for (s32 i = 0; i < n; ++i) {
		MEulerRotation resultMEuler = Convert ( rad3[i] );
		MVector v = resultMEuler.asVector ();
		arrayBuilder.addElement ( i ).set ( v );
		arrayBuilder.addElement ( i ).setClean ();
	}
	arrayHandle.set ( arrayBuilder );
	arrayHandle.setAllClean ();
}