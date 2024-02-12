//******************************************************************************
//! @file	base.h
//! @brief	 基底型定義
//! @date 2019/2/1
//! @date 2021/2/4 eigen追加
//! @date 2023/8/29 Burgess2開始
//******************************************************************************
/** 
*	@defgroup Base
*	@{
*/

#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <codecvt>
#include <cassert>
#include <locale>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <conio.h>

//#define EIGEN_INITIALIZE_MATRICES_BY_ZERO
#define EIGEN_NO_DEBUG
#define EIGEN_NO_STATIC_ASSERT 
#include <eigen/Eigen/Core>
#include <eigen/Eigen/Geometry>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>

#include "string.h"
#include "enum.h"

/**
*	@defgroup Data Type Alias
*	@{
*/
using s8 = char; // -128 ~ 127
using s16 = short; // -32768 ~ 32767
using s32 = int; // -2147483648 ~ 2147483647
using s64 = long; // -9223372036854775808 ~ 9223372036854775807

using u8 = unsigned char; // 0 ~ 255
using u16 = unsigned short; // 0 ~ 65535
using u32 = unsigned int; // 0 ~ 4294967295
using u64 = unsigned long; // 0 ~ 18446744073709551615

using f32 = float;
using f64 = double;

using Bool = bool;

using LocalIndex = u8; // PolygonMeshでポリゴンもしくはポイント内に含まれるVertexのローカルIDで使用

/** @} */

/**
*	@defgroup Macro
*	@{
*/
namespace BURGESS2 {
	/** @def TOLERANCE
	* 浮動小数点誤差値
	*/
	const f64 TOLERANCE = 10e-6;

	/** @def TOLERANCE0999
	* 限りなく1.0に近い0.999999
	*/
	const f64 TOLERANCE0999 = 1.0 - TOLERANCE;

	/** @def TOLERANCE0499
	* 限りなく0.5に近い0.499999
	*/
	const f64 TOLERANCE0499 = 0.5 - TOLERANCE;

	const f64 DIVTOLERANCE = 10e-20;

	const f64 PI = 3.14159265358979;

	const f64 HPI = 1.57079632679;

	const f64 PI_DIV_180 = 0.017453292;

	const f64 RAD = 57.29577950;

	const f32 F32_INFINITE = 3.40282e+38f;

	const f64 F64_INFINITE = 1.79769e+308;

	const s8 S8_INFINITE = 127;

	const s16 S16_INFINITE = 32767;

	const s32 S32_INFINITE = 2147483647;

	const s64 S64_INFINITE = 2147483647;// 9223372036854775807;

	const u8 U8_INFINITE = 255;

	const u16 U16_INFINITE = 65535;

	const u32 U32_INFINITE = 4294967295;

	const u64 U64_INFINITE = 4294967295;// 18446744073709551615;

};

/** @} */
/** @def SQ
* 二乗
*/
#define SQ( v )			( (v) * (v) )	

/** @def out
* 参照返し
*/
#define OUT  

/** @def io
* 参照渡し＋返し
*/
#define IO  

/** @} */
//******************************************************************************
//! End of File
//******************************************************************************