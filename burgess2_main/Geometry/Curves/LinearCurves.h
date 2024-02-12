#pragma once
#include "CurvesBase.h"

//******************************************************************************
//! @file	LinearCurves.h
//! @brief	 LinearCurves
//******************************************************************************
/** @defgroup Geometry
*		@{
*/
/**		@defgroup Curves
*			@{
*/
/**				@brief �����{�̃��j�A�J�[�u�������N���X <br>
*					#include <burgess2_main/Geometry.h> <br><br>
*					#alias <br> \b : LinearCurves = LinearCurvesT<f32> <br> \b : LinearCurvesd = LinearCurvesT<f64> <br>
*					<br>
*					@par ��{�̃A�g���r���[�g - basic attributes
*						- <b> �R���g���[���|�C���g�ʒu - CP(control point) position </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "pointPositions"
*							- �z�� - array size = �|�C���g�� - point count
*						- <b> �J�[�u��̒l���擾����֐������������邽�߂̃A�g���r���[�g�i�ҏW�֎~�j </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "paramLengthTable"
*							- ���� - note : Vec3.x=param, .y=length, .z=invLengthRatio
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
*					@code
*						// �J�[�u�̃R���g���[���|�C���g
*						std::vector<Vec3> cp ( 5 );
*						cp [0] = Vec3 ( 0.f, 0.f, 4.f );
*						cp [1] = Vec3 ( 10.f, 0.f, 2.f );
*						cp [2] = Vec3 ( 0.f, 0.f, 1.f );
*						cp [3] = Vec3 ( -10.f, 0.f, -2.f );
*						cp [4] = Vec3 ( 0.f, 0.f, -5.f );

*						LinearCurves curves;
*
*						curves.add ( cp, CurveData::CurveFrom::Open );	//  �I�[�v���t�H�[���J�[�u���쐬
*						curves.add ( BBox( Vec3( 0.0, -10.0, -5.2 ), Vec3( 2.5, 10.0, 10.0 ) ) ); //  BBox��X���̒����̃��C���J�[�u���쐬
*
*						Vec3 normal = curves.getPositionAtLengthRatio ( 0, 0.9f ); // 0�{�ڂ̃J�[�u��̖@�����擾����
*						Vec3 position = curves.getPositionAtNormalizedParam ( 1, 0.35f ); // 1�{�ڂ̃J�[�u��̃|�W�V�������擾����
*					@endcode
*/
template<typename T>
class LinearCurvesT : public CurvesBaseT<T>{

public:
	LinearCurvesT() {}
	~LinearCurvesT() {}

	u32 add ( const std::vector<Vec3T<T>> &points, u8 form );
	u32 add ( const BBoxT<T> &bBox, u8 axis = u8(1), const Vec3T<T> offsetPos = Vec3T<T> ( T ( 0.0 ) ) );

	u32 duplicate ( u32 curveID );
	u32 copyFrom ( const LinearCurvesT<T> &src );
	u32 copyCurveFrom ( const LinearCurvesT<T> &src, u32 curveID );

	/*! @name param / normalized param*/
	/* @{ */
	T getParamFromNormalizedParam ( u32 curveID, T nParam )const;
	T getNormalizedParamFromParam ( u32 curveID, T param )const;
	T getMaxParam ( u32 curveID )const;
	T getMinParam ( u32 curveID )const;

	std::vector<T> getEvenlyDivideParams ( u32 curveID, u32 count ) const;

	std::vector<T> getRegularIntervalParams ( u32 curveID, f32 _length ) const;

	std::vector<T> getDynamicIntervalParams ( u32 curveID, f32 _length ) const;

	std::vector<T> getRandomParams ( u32 curveID, u32 count, u32 seed ) const;

	/* @{ */

	/*! @name value on curve*/
	/* @{ */
	Vec3T<T> getPositionAtParam ( u32 curveID, T param ) const override;
	Vec3T<T> getTangentAtParam ( u32 curveID, T param ) const override;
	Vec3T<T> getNormalAtParam ( u32 curveID, T param ) const override;
	XfoT<T> getXfoAtParam ( u32 curveID, T param, u8 directionType = 0 ) const override;

	Vec3T<T> getPositionAtNormalizedParam ( u32 curveID, T nParam ) const override;
	Vec3T<T> getTangentAtNormalizedParam ( u32 curveID, T nParam ) const override;
	Vec3T<T> getNormalAtNormalizedParam ( u32 curveID, T nParam ) const override;
	XfoT<T> getXfoAtNormalizedParam ( u32 curveID, T nParam, u8 directionType = 0 ) const override;
	
	Vec3T<T> getPositionAtLengthRatio ( u32 curveID, T ratio ) const override;
	Vec3T<T> getTangentAtLengthRatio ( u32 curveID, T ratio ) const override;
	Vec3T<T> getNormalAtLengthRatio ( u32 curveID, T ratio ) const override;
	XfoT<T> getXfoAtLengthRatio ( u32 curveID, T ratio, u8 directionType = 0 ) const override;
	/* @{ */

	/*! @name length*/
	/* @{ */
	void recomputeParamLengthTable ( u32 curveID );
	T getCurveLength ( u32 curveID );
	T getCurveLength ( u32 curveID ) const;

	T getParamFromLength ( u32 curveID, T length )const;
	T getNormalizedParamFromLength ( u32 curveID, T length )const;

	T getLengthAtParam ( u32 curveID, T param )const;
	T getLengthAtNormalizedParam ( u32 curveID, T nParam )const;

	T getParamFromLengthRatio ( u32 curveID, T ratio )const;
	T getNormalizedParamFromLengthRatio ( u32 curveID, T ratio )const;

	T getLengthRatioAtParam ( u32 curveID, T param )const;
	T getLengthRatioAtNormalizedParam ( u32 curveID, T nParam )const;
	/* @{ */

	/*! @name spatial query*/
	/* @{ */
	LinesSpatialQueryT<T> toSpatialQuery ( u32 curveID )const;
	Vec3T<T> getPositionAtLocation ( const GeometryLocationT<T> &loc ) const;
	Vec3T<T> getPositionAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const;
	
	Vec3T<T> getTangentAtLocation ( const GeometryLocationT<T> &loc ) const;
	Vec3T<T> getTangentAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const;

	Vec3T<T> getNormalAtLocation ( const GeometryLocationT<T> &loc ) const;
	Vec3T<T> getNormalAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const;

	XfoT<T> getXfoAtLocation ( const GeometryLocationT<T> &loc ) const;
	XfoT<T> getXfoAtLocation ( u32 curveID, const GeometryLocationT<T> &loc ) const;
	/* @{ */
};

#include "LinearCurves.inl"
#include "LinearCurves_param.inl"
#include "LinearCurves_value_on_curve.inl"
#include "LinearCurves_length.inl"
#include "LinearCurves_spatial_query.inl"

/** @} */
/** @} */
using LinearCurves = LinearCurvesT<f32>;
using LinearCurvesd = LinearCurvesT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************