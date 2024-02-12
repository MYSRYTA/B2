#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Math.h>
#include <burgess2_main/Math.h>
#include "LUDecomposition.h"

//******************************************************************************
//! @file	RBF.h
//! @brief	 RBF�\���o�[
//******************************************************************************
/** @defgroup Algorithm
*�@@{
*/
/**	�@@brief Radial Basis Function (���ˊ��֐�)<br>
*
*/
/**	@defgroup RBF
*�@	@{
*/
/**	�@@brief Driver��Driven�̃|�[�Y�Z�b�g<br>
*			#include <burgess2_main/Algorithm.h> <br><br>
*			#alias <br> \b : RBFControlPoint = RBFControlPointT<float> <br> \b : RBFControlPointd = RBFControlPointT<double> <br><br>
*			��������Driver,�@��������Driven <br>
*			sample�FVec3�^�̃|�[�Y(Driver)��Vec4�^�̒l(DrivenValue)��R�Â����|�[�Y�Z�b�g���R�쐬<br>
*			@code
*				// Driver = Vec3, DrivenValue = Vec4 �̑g�ݍ��킹��ControlPoint�iRBF�̃|�[�Y�j���쐬����
*				std::vector<RBFControlPoint> cp ( 3 );
*				cp [0] = RBFControlPoint ( Vec3 ( 0.02f, 1.42f, 1.04f ),	 Vec4 ( 1.67f, 0.40f, 0.46f, 0.0f )	);
*				cp [1] = RBFControlPoint ( Vec3 ( 1.91f, 0.99f, 2.12f ),	 Vec4 ( 2.84f, 1.54f, 1.54f, 1.0f )	);
*				cp [2] = RBFControlPoint ( Vec3 ( 0.01f, 3.97f, 0.02f ),	 Vec4 ( 1.00f, 2.73f, 1.01f, 0.2f )	);
*
*				// ��������ControlPoint��RBF���쐬����
*				RBF rbf ( cp, 1 ); // type : 1 = Linear
*
*			@endcode
*/

template<typename T>
class RBFControlPointT {
public:
	VecT<T> driverLocation;
	VecT<T> drivenValues;

	RBFControlPointT () {}
	RBFControlPointT ( const VecT<T> &_driverLocation, const VecT<T> &drivenValue )
		: driverLocation { _driverLocation }
		, drivenValues { drivenValue }{}
	RBFControlPointT ( const Vec3T<T> &_driverLocation, const Vec3T<T> &drivenValue )
		: driverLocation { _driverLocation.toVec() }
		, drivenValues { drivenValue.toVec () } {}
	RBFControlPointT ( const Vec3T<T> &_driverLocation, const Mat4T<T> &drivenValue )
		: driverLocation { _driverLocation.toVec () }
		, drivenValues { drivenValue.toVec () } {}
	RBFControlPointT ( const Vec4T<T> &_driverLocation, const Vec3T<T> &drivenValue )
		: driverLocation { _driverLocation.toVec () }
		, drivenValues { drivenValue.toVec () } {}
	RBFControlPointT ( const Vec4T<T> &_driverLocation, const Mat4T<T> &drivenValue )
		: driverLocation { _driverLocation.toVec () }
		, drivenValues { drivenValue.toVec () } {}
	RBFControlPointT ( const Vec3T<T> &_driverLocation, const std::vector<Vec3T<T>> &_drivenValues );
	RBFControlPointT ( const Vec3T<T> &_driverLocation, const std::vector<Mat4T<T>> &_drivenValues );
	RBFControlPointT ( const Vec4T<T> &_driverLocation, const std::vector<Vec3T<T>> &_drivenValues );
	RBFControlPointT ( const Vec4T<T> &_driverLocation, const std::vector<Mat4T<T>> &_drivenValues );
	virtual ~RBFControlPointT () {}

	String toString ( const String title = "" )const;

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( driverLocation ),
			CEREAL_NVP ( drivenValues )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

/**
/**	�@@brief	RBF�\���o�[<br>
*			#include <burgess2_main/Algorithm/Algorithm.h> <br><br>
*			#alias <br> \b : RBF = RBFT<float> <br> \b : RBFd = RBFT<double> <br><br>
*			solve()�̈����̃f�[�^�^��RBFControlPoint��Driver�̃f�[�^�^�ƍ��킹��K�v������<br>
*			type: 1=Linear, 2=Cubic, 3=MultiQuadric, 4=Quintic, 5=Gaussian, 6=ThinPlate<br>
*			<br>
*			@code
*				// Driver = Vec3, DrivenValue = Vec4 �̑g�ݍ��킹��RBF�Ƀ|�[�Y�o�^�����Ď��s����
*
*				std::vector<RBFControlPoint> cp ( 7 );
*				cp [0] = RBFControlPoint ( Vec3 ( 0.01f, 0.11f, 0.21f ), Vec4 ( 1.0f, 2.0f, 3.0f, 0.0f ));
*				cp [1] = RBFControlPoint ( Vec3 ( 0.89f, 0.38f, 2.34f ), Vec4 ( 0.7f, 0.7f, 0.4f, 0.0f ));
*				cp [2] = RBFControlPoint ( Vec3 ( 1.31f, -0.4f, 1.31f ), Vec4 ( 2.0f, 0.3f, 1.0f, 0.0f ));
*				cp [3] = RBFControlPoint ( Vec3 ( -1.6f, 0.73f, 0.01f ), Vec4 ( 2.2f, 2.2f, 2.2f, 0.0f ));
*				cp [4] = RBFControlPoint ( Vec3 ( 0.01f, -1.4f, -1.0f ), Vec4 ( 1.6f, 0.4f, 0.4f, 0.0f ));
*				cp [5] = RBFControlPoint ( Vec3 ( -1.9f, 0.99f, 2.11f ), Vec4 ( 2.8f, 1.5f, 1.5f, 0.0f ));
*				cp [6] = RBFControlPoint ( Vec3 ( 0.01f, 3.97f, 0.01f ), Vec4 ( 1.0f, 2.7f, 1.0f, 0.0f ));
*	
*				RBF rbf ( cp, 3 ); // type : 3 = MultiQuadric
*
*				Vec3 pos ( -1.0f, 0.9f, 1.1f ) ;
*				Vec v = rbf.solve ( pos );
*				Vec3 result (v);
*			@endcode
*/

template<typename T>
class RBFT {
public:
	MatT<T> P;
	MatT<T> W;
	MatT<T> V;
	/// RBFKernel
	u8 type;  /// �⊮�v�Z�̃^�C�v
	f32 epsilon; /// �K�E�X�֐��܂��̓}���`�l���֐��̒����\�Ȓ萔

	T norm ( T radius ) const;

	Bool initialized = false;

	RBFT (){}
	RBFT ( const std::vector<RBFControlPointT<T>> &ctrlPoints, u8 type = 1, T smooth = 0.0, T epsilon = 0.1);
	virtual ~RBFT () {};

	void setup( const std::vector<RBFControlPointT<T>> &ctrlPoints, u8 type = 1, T smooth = 0.0, T epsilon = 0.1 );

	VecT<T> solve ( const VecT<T> &driverPose ) const;
	VecT<T> solve ( const Vec3T<T> &driverPose ) const;
	VecT<T> solve ( const Vec4T<T> &driverPose ) const;

	String toString ( const String title = "" )const;

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( initialized ),
			CEREAL_NVP ( P ),
			CEREAL_NVP ( W ),
			CEREAL_NVP ( V ),
			CEREAL_NVP ( type ),
			CEREAL_NVP ( epsilon )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};
#include "RBF.inl"

/** @} */
/** @} */

using RBFControlPoint = RBFControlPointT<f32>;
using RBFControlPointd = RBFControlPointT<f64>;
using RBF = RBFT<f32>;
using RBFd = RBFT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************