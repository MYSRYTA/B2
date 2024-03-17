#pragma once
#include <burgess2_main/Core/base.h>

template<typename T> class Vec3T;
template<typename T> class GeometryLocationT;

//******************************************************************************
//! @file	GeometryDelta.h
//! @brief	 GeometryLocation + ��ԏ�̍������
//******************************************************************************
/**@defgroup Containers
*	 @{
*/
/**		@brief	GeometryLocationT + ��ԏ�̍������ <br>
*			#include <burgess2_main/Containers.h> <br><br>
*			#alias <br> \b : GeometryDelta <br> \b : GeometryDeltad <br>
*/

template<typename T>
class GeometryDeltaT: public GeometryLocationT<T> {
	Vec3T<T> direction = Vec3T<T> ( T ( 0.0 ) );
	T distance;
	L16Array<Vec3T<T>> normals; // ���_���\������S�Ẵo�[�e�b�N�X�̖@�����
public:
	GeometryDeltaT (){}
	~GeometryDeltaT () {}

	void set ( const GeometryLocationT<T> &_location, const Vec3T<T> &_direction, T _distance );
	void set ( const GeometryLocationT<T> &_location, const Vec3T<T> &_direction, T _distance, const std::vector<Vec3T<T>> &normalDelta );
	void reset ( const Vec3T<T> &_direction, T _distance );

	void setLocationValue ( const GeometryLocationT<T> &_location );
	void setDeltaValues ( const XfoT<T> &targetXfo, const Vec3T<T> &pointPosition );
	void setDeltaValues ( const XfoT<T> &targetXfo, const Vec3T<T> &pointPosition, const L16Array<Vec3T<T>> &pointNormal );

	Vec3T<T> getDelta ()const;
	Vec3T<T> getDirection ()const;
	T getDistance ()const;
	L16Array<Vec3T<T>> getNormalDeltas ()const;
	Vec3T<T> getNormalDelta ( u8 id )const;

	u32 normalCount ()const;

	Bool isValid ()const override;
	Bool hasNormalValue ()const;

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			cereal::base_class<GeometryLocationT<T>> ( this ),
			CEREAL_NVP ( direction ),
			CEREAL_NVP ( distance ),
			CEREAL_NVP ( normals )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

#include "GeometryDelta.inl"
/** @} */

using GeometryDelta = GeometryDeltaT<f32>;
using GeometryDeltad = GeometryDeltaT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************