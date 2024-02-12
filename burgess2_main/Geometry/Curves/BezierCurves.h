#pragma once
#include "CurvesBase.h"

//******************************************************************************
//! @file	BezierCurves.h
//! @brief	 BezierCurves
//******************************************************************************
/** @defgroup Geometry
*		@{
*/
/**		@defgroup Curves
*			@{
*/
/**				@brief �����{��NURBS�������N���X <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : BezierCurves = BezierCurvesT<f32> <br> \b : BezierCurvesd = BezierCurvesT<f64> <br>
*					rationalWeight�l���S��1.0�̏ꍇ��BSplineCurves�̕����y��<br>
*					BSplineCurves�Ƃ̈Ⴂ��rationalWeight�̂݁i���݂�rationalWeight�����Ή��̂���BSplineCurves�Ɠ����̃N���X�j<br>
*					<br>
*					@par ��{�̃A�g���r���[�g - basic attributes
*						- <b> �R���g���[���|�C���g�ʒu - CP(control point) position </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "pointPositions"
*							- �z�� - array size = �|�C���g�� - cp count
*						- <b> �m�b�g�x�N�g���i�ҏW�֎~�j- knot vector. Do not edit.</b>
*							- type = @link F32AttributeT<T> @endlink
*							- attribute name = "knotVector"
*							- �z�� - array size = ���L�̃J�[�u�t�H�[���ɂ���ĈقȂ�<br>
*								open : CP count + degree + 1<br>
*								close :  CP count + degree + 2<br>
*								periodic : CP count + (degree * 2) + 1 / [0]=0 <br>
*						- <b> �J�[�u��̒l���擾����֐������������邽�߂̃A�g���r���[�g�i�ҏW�֎~�j </b> 
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "paramLengthTable"
*							- ���� - note : Vec3.x=param, .y=length, .z=invLengthRatio
*						- <b> �E�F�C�g - rational weight
*							- type = @link F32AttributeT<T> @endlink
*							- attribute name = "rationalWeight"
*							- �z�� - array size = �|�C���g�� - cp count
*					<br>

*					@par �I�v�V���� �A�g���r���[�g - sub attributes
*						- <b> �J�[�u�@�� - normals </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "normals"
*							- �z�� - array size = compute***Normals(u32 sample)��sample * point cpunt
*					<br>
*					@note �@���A�g���r���[�g�̒l�̐ݒ�͉��L�̊֐��ōs�� <br>
*						To set the value of the normals attribute, the following functions are used. <br>
*						&nbsp;&nbsp; computeNormals (u32 curveID, u32 sample=1) <br>
*						&nbsp;&nbsp; computePTFNormals (u32 curveID, const Vec3T< T > &upvector, u32 sample=1) <br>
*						&nbsp;&nbsp; computePTFNormals2 (u32 curveID, const Vec3T< T > &upvector, u32 sample=1) <br>
*						&nbsp;&nbsp; computeFrenetFrameNormals (u32 curveID, const Vec3T< T > &upvector, u32 sample=1) <br>
*						&nbsp;&nbsp; computeDoubleReflectionNormals (u32 curveID, const Vec3T< T > &upvector, u32 sample=1) <br>
*
*					<br>

*					@note �J�[�u�̎����̓����o�[�ϐ�".v[0]"�ɕێ� <br>
*						curve degree save on .v[0]. ".v" is menber arg.
*
*					<br>
*					@note Maya��Curve�f�[�^ <br>
*						<b> CP size </b><br>
*						&nbsp;&nbsp; open form = CP count <br>
*						&nbsp;&nbsp; periodic = CP count + degree<br>
*						<b> knot vector size </b><br>
*						&nbsp;&nbsp; open form = CP count + degree - 1<br>
*						&nbsp;&nbsp; periodic form = CP count + (degree * 2) - 1 / [0] = -degree + 1 <br>
*
*					<br>
*					@code
*						// prepar cp
*						std::vector<Vec3> cp ( 5 );
*						cp [0] = Vec3 ( 0.f, 0.f, 4.f );
*						cp [1] = Vec3 ( 10.f, 0.f, 2.f );
*						cp [2] = Vec3 ( 0.f, 0.f, 1.f );
*						cp [3] = Vec3 ( -10.f, 0.f, -2.f );
*						cp [4] = Vec3 ( 0.f, 0.f, -5.f );
*
*						u8 degree = 3; // 3��
*
*						BezierCurves curves;
*						std::vector<f32> weights2 = { 0.01, 0.3, 1.0, 0.9, 1.0 };
*						curves.add ( cp, degree, CurveData::CurveFrom::Periodic, weights2 );
*					@endcode
*/

template<typename T>
class BezierCurvesT : public BSplineCurvesT<T>{
	Vec3T<T> core(const Vec3T<T>& p0, const Vec3T<T>& p1, const Vec3T<T>& p2, T t);
	Vec3T<T> core(const Vec3T<T>& p0, const Vec3T<T>& p1, const Vec3T<T>& p2, T t, const std::vector<T>& weight);

public:
	enum handleDirection
	{
		X = u8(0),
		neg_X,
		Y,
		neg_Y,
		Z,
		neg_Z,
		Auto,
		Auto_BothEnds_X,
		Auto_BothEnds_neg_X,
		Auto_BothEnds_Y,
		Auto_BothEnds_neg_Y,
		Auto_BothEnds_Z,
		Auto_BothEnds_neg_Z
	};

	BezierCurvesT() {}
	~BezierCurvesT() {}

	u32 add(const std::vector<Vec3T<T>> &points, u8 form, u32 sample = 10 );
	u32 add(const std::vector<Mat4T<T>>& segmentPose, u8 form, BezierCurvesT::handleDirection handleDirType, T handleLengthRatio, u32 sample = 10);
	u32 add(const std::vector<Mat4T<T>>& segmentPose, u8 form, BezierCurvesT::handleDirection handleDirType, std::vector<T>& handleLengthRatio, u32 sample = 10);
};

#include "BezierCurves.inl"

/** @} */
/** @} */
using BezierCurves = BezierCurvesT<f32>;
using BezierCurvesd = BezierCurvesT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************