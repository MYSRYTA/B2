#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class MatT;
template<typename T> class Mat4T;
template<typename T> class VecT;
template<typename T> class Vec3T;
template<typename T> class QuatT;

//******************************************************************************
//! @file	KrigingT.h
//! @brief	 球面クリギングソルバー
//******************************************************************************
/** @defgroup Algorithm
*　@{
*/
/** @brief 球面クリギングソルバー <br>
*	  参照コード提供元　Mukai Laboratory　
*	  @ref https://mukai-lab.org/ <br>
*/
/**	@defgroup Kriging
*　	@{
*/
/**	　@brief	DriverとDrivenのポーズセット<br>
*			#include <burgess2_main/Algorithm.h> <br><br>
*			#alias <br> \b : KrigingControlPoint = KrigingControlPointT<float> <br> \b : KrigingControlPointd = KrigingControlPointT<double><br>
*			第一引数がDriver(Quat型のみ),　第二引数がDriven  <br>
*			@code
*				std::vector<KrigingControlPoint> cp ( 3 );
*				cp [0] = KrigingControlPoint ( Quat ( 0.0f, -0.45f, -0.04f, 0.1f ),	 Vec3 ( 1.67f, 0.40f, 0.46f )	);
*				cp [1] = KrigingControlPoint ( Quat ( -0.90f, 0.99f, 0.21f, 0.2f ),	 Vec3 ( 2.84f, 1.54f, 1.54f )	);
*				cp [2] = KrigingControlPoint ( Quat ( 0.0f, 0.97f, 0.0f, 0.0f ),	 Vec3 ( 1.0f, 2.73f, 1.0f )		);
*
*				Kriring k ( cp );
*			@endcode
*/

template<typename T>
class KrigingControlPointT {
public:
	QuatT<T> driverRot;
	VecT<T> drivenValues;

	KrigingControlPointT () {}
	KrigingControlPointT ( const QuatT<T> &_driverRot, const VecT<T> &drivenValue )
		: driverRot{ _driverRot }
		, drivenValues{ drivenValue } {}
	KrigingControlPointT ( const QuatT<T> &_driverRot, const Vec3T<T> &drivenValue )
		: driverRot{ _driverRot }
		, drivenValues{ drivenValue.toVec() } {}
	KrigingControlPointT ( const QuatT<T> &_driverRot, const Mat4T<T> &drivenValue )
		: driverRot{ _driverRot }
		, drivenValues{ drivenValue.toVec () } {}
	KrigingControlPointT ( const QuatT<T> &_driverRot, const std::vector<Vec3T<T>> &_drivenValues );
	KrigingControlPointT ( const QuatT<T> &_driverRot, const std::vector<Mat4T<T>> &_drivenValues );
	virtual ~KrigingControlPointT () {}

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( driverRot ),
			CEREAL_NVP ( drivenValues )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

/**	　@brief	球面クリギングソルバー<br>
*		参照コード提供元　Mukai Laboratory　
*		@ref https://mukai-lab.org/ <br>
*			#include <burgess2_main/Algorithm/Algorithm.h> <br><br>
*			#alias <br> \b : Kriring = KriringT<float> <br> \b : Kriringd = KriringT<double><br>
*			@code
*				std::vector<KrigingControlPoint> cp ( 3 );
*				cp [0] = KrigingControlPoint ( Quat ( 0.0f, -0.45f, -0.04f, 0.1f ),	 Vec3 ( 1.67f, 0.40f, 0.46f )	);
*				cp [1] = KrigingControlPoint ( Quat ( -0.90f, 0.99f, 0.21f, 0.2f ),	 Vec3 ( 2.84f, 1.54f, 1.54f )	);
*				cp [2] = KrigingControlPoint ( Quat ( 0.0f, 0.97f, 0.0f, 0.0f ),		 Vec3 ( 1.0f, 2.73f, 1.0f )		);
*
*				Kriring k ( cp );
*
*				Quat rot ( -1.0f, 0.9f, 1.1f, -0.1f ) ;
*				Vec v = k.solve ( rot );
*				Vec3 result (v);
*			@endcode
*/

template<typename T>
class KrigingT {
public:
	KrigingT () {}
	KrigingT ( const std::vector<KrigingControlPointT<T>> &ctrlPoints );
	virtual ~KrigingT () {};

	VecT<T> solve ( const QuatT<T> &driverRot );
private:
	MatT<T> coefMatrix;
	VecT<T> distVec;
	std::vector<KrigingControlPointT<T>> ctrlPoints;

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( coefMatrix ),
			CEREAL_NVP ( distVec ),
			CEREAL_NVP ( ctrlPoints )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Kriging.inl"

/** @} */
/** @} */

using KrigingControlPoint = KrigingControlPointT<f32>;
using KrigingControlPointd = KrigingControlPointT<f64>;
using Kriging = KrigingT<f32>;
using Krigingd = KrigingT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************