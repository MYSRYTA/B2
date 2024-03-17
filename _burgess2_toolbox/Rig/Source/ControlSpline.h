#pragma once
#include <burgess2_main/Math.h>
#include <burgess2_main/Containers.h>
#include <burgess2_main/Geometry.h>

//******************************************************************************
//! @file	ControlSpline.h
//! @brief	 Control Spline
//******************************************************************************
/** @defgroup Rig
*　@{
*/
/**		@brief	ControlSpline (Linear, BSpline, CatmullRomCurveをサポート) <br> <br>
*			alias : \b ControlSpline = ControlSplineT<float> // \b ControlSplined = ControlSplineT<double> <br>
*			#include <burgess2_main/Rig/Rig.h> <br><br>
*			@code
*
*			@endcode
*/

template<typename T>
class ControlSplineT{
	u8 type;
	u8 degree;
	u8 form;
	T scale{ 1.0 };

	T neutralLength{ 0.0 };
	VecT<T> jointAttachRatio;
	u8 upvectorType{ 0 };
	u8 upvectorSample{ 2 };

	T slideScale{ 1.0 };
	u8 offsetType{ 0 };
	T offsetRatio{ 0.0 };
public:
	enum UpvectorType{
		UpvectorHandle = 0,
		RootAndTipControlYaxis,
		AllControllerYaxis,
		rootYaxisPTF,
		rootYaxisPTF2,
		rootYaxisFrenetFrame,
		rootYaxisDoubleReflection
	};
	enum OffsetType{
		Start = 0,
		End,
		BothEnd
	};

	ControlSplineT<T>(){}
	virtual ~ControlSplineT<T>(){}

	void initialize ( u8 splineType, T splineLength, u8 form, u8 degree = 2 );
	void initialize ( u8 splineType, std::vector<Vec3T<T>> &neutralCntrolPoint, u8 form, u8 degree = 2 );
	void initialize ( u8 splineType, std::vector<Mat4T<T>> &neutralCntrolPoint, u8 form, u8 degree = 2 );
	
	u32 jointCount ()const;

	void setJointSize ( u32 jointSize, Bool startJoint, Bool endJoint );
	void setUpvectorType ( u8 type );
	void setUpvectorSample(u8 type);
	void setScale ( T _scale );
	void setSlideOffset ( u8 side, T ratio );
	void setSlideScale ( u8 side, T scale );

	std::vector<Mat4T<T>> solve ( const Mat4T<T> &root, 
								  const std::vector<Mat4T<T>> &controlPoint, 
								  const Vec3T<T> &upvector = Vec3T<T>() );
};

#include "ControlSpline.inl"
/** @} */
using ControlSpline = ControlSplineT<f32>;
using ControlSplined = ControlSplineT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************