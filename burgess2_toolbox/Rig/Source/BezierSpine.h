#pragma once
#include <burgess2_main/Math.h>
#include <burgess2_main/Containers.h>

//******************************************************************************
//! @file	BezierSpine.h
//! @brief	 ベジェ曲線スパイン
//******************************************************************************
/** @defgroup Rig
*　@{
*/
/**		@brief	ベジェ曲線スパイン <br>
*			alias : \b BezierSpine = BezierSpineT<float> // \b BezierSpined = BezierSpineT<double> <br>
*			#include <burgess2_toolbox/Rig/Rig.h> <br><br>
*			@code
*
*			@endcode
*/

template<typename T>
class BezierSpineT {
	T neutralLength;
	T boneLength;
	T scale { 1.0 };
	u8 jointCnt;
	Mat4T<T> tipMat4; // solve()で使用するキャッシュ

	Vec3T<T> evalBezier2 ( const L4Array<Vec3T<T>> &coeffs, T t ) const;
	Vec3T<T> evalBezier3 ( const L4Array<Vec3T<T>> &coeffs, T t ) const;
	std::vector<Mat4T<T>> solveBezier2 ( const Mat4T<T> root, const std::vector<Mat4T<T>> cp );
	std::vector<Mat4T<T>> solveBezier3 ( const Mat4T<T> root, const std::vector<Mat4T<T>> cp );
public:
	String name;

	BezierSpineT () {}
	virtual ~BezierSpineT () {}

	void initialize ( T neutralLength, u8 jointSize );

	void setScale ( T scale );
	u8 jointCount ()const;

	std::vector<Mat4T<T>> solve ( const Mat4T<T> root, const std::vector<Mat4T<T>> cp );
};

#include "BezierSpine.inl"
/** @} */
using BezierSpine = BezierSpineT<f32>;
using BezierSpined = BezierSpineT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************