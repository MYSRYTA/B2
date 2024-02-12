#pragma once
#include "CurveData.h"
#include <burgess2_main/Containers.h>

//******************************************************************************
//! @file	CurvesBaseT.h
//! @brief	 Curves基底クラス
//******************************************************************************
/** @defgroup Geometry
*	  @{
*/
/**		@defgroup Curves
*			@{
*/
/**				@defgroup CurveCore
*					@{
*/
/**				@brief ***Curvesクラスの基底クラス <br>
*					
*/

template<typename T>
class CurvesBaseT {
public:
	std::vector<CurveDataT<T>> data;
	
	CurvesBaseT () {}
	virtual ~CurvesBaseT () {}

	void setCurveCount ( u32 count );
	void resetAllPointPositions ( u32 curveID, const std::vector<Vec3T<T>> &positions );
	void setAllPointPositions ( u32 curveID, const std::vector<Vec3T<T>> &positions );
	void setPointPosition ( u32 curveID, u32 index, const Vec3T<T> & position );
	void resetAllNormals ( u32 curveID, const std::vector<Vec3T<T>> &normal );
	void computeNormals(u32 curveID, u32 sample = 1);
	void computePTFNormals ( u32 curveID, const Vec3T<T> &upvector, u32 sample = 1);
	void computePTFNormals2 ( u32 curveID, const Vec3T<T>& upvector, u32 sample = 1);
	void computeFrenetFrameNormals( u32 curveID, const Vec3T<T>& upvector, u32 sample = 1);
	void computeDoubleReflectionNormals(u32 curveID, const Vec3T<T>& upvector, u32 sample = 1);

	void recomputeAllNormals(u32 sample = 1);

	//void setAllNormals ( u32 curveID, const std::vector<Vec3T<T>> &normal );
	//void setAllNormal ( u32 curveID, u32 index, const Vec3T<T> &normal );
	void setMirroring(u32 curveID, Vec3T<T> normal);

	std::vector<Vec3T<T>> getAllPointPositions ( u32 curveID ) const;
	std::vector<Vec3T<T>> getCurveFormAllPointPositions ( u32 curveID ) const;
	Vec3T<T> getPointPosition ( u32 curveID, u8 index ) const;
	u8 getForm ( u32 curveID )const;

	u32 curveCount ()const;
	u32 pointCount ( u32 curveID ) const;
	u32 curveFormPointCount ( u32 curveID ) const;
	u32 allPointCount () const;
	u32 normalCount ( u32 curveID )const;
	u32 curveFormNormalCount ( u32 curveID ) const;

	void applyTransform ( u32 curveID, const Mat4T<T> &transform );
	void applyTransform ( u32 curveID, const XfoT<T> &transform );

	Bool isOpen ( u32 curveID )const;
	Bool isClose ( u32 curveID )const;
	Bool isPeriodic(u32 curveID)const;

	Bool paramLengthTableIsDirty ( u32 curveID )const;
	void paramLengthTableCached ( u32 curveID );
	void incrementParamLengthTableVirsion ( u32 curveID );

	BoolAttributeT<T>& getOrCreateBoolAttribute ( u32 curveID,  const String &_name  ) ;
	F32AttributeT<T>& getOrCreateF32Attribute ( u32 curveID,  const String &_name  ) ;
	F64AttributeT<T>& getOrCreateF64Attribute ( u32 curveID, const String &_name );
	S32AttributeT<T>& getOrCreateS32Attribute ( u32 curveID,  const String &_name  );
	U32AttributeT<T>& getOrCreateU32Attribute ( u32 curveID,  const String &_name  ) ;
	Vec2AttributeT<T>& getOrCreatVec2Attribute ( u32 curveID,  const String &_name  );
	Vec3AttributeT<T>& getOrCreatVec3Attribute ( u32 curveID,  const String &_name  );
	Vec4AttributeT<T>& getOrCreatVec4Attribute ( u32 curveID,  const String &_name  );

	Vec3AttributeT<T>& getPointPositionAttribute ( u32 curveID ) const ;
	Vec3AttributeT<T>& getNormalAttribute ( u32 curveID ) const;
	BoolAttributeT<T>& getBoolAttribute ( u32 curveID,  const String &_name  ) const ;
	F32AttributeT<T>& getF32Attribute ( u32 curveID,  const String &_name  ) const ;
	F64AttributeT<T>& getF64Attribute ( u32 curveID,  const String &_name  ) const;
	S32AttributeT<T>& getS32Attribute ( u32 curveID,  const String &_name  ) const ;
	U32AttributeT<T>& getU32Attribute ( u32 curveID,  const String &_name  ) const ;
	Vec2AttributeT<T>& getVec2Attribute ( u32 curveID,  const String &_name  ) const ;
	Vec3AttributeT<T>& getVec3Attribute ( u32 curveID,  const String &_name  ) const ;
	Vec4AttributeT<T>& getVec4Attribute ( u32 curveID,  const String &_name  ) const ;

	Bool has( u32 curveID, const String &_name ) const;

	/*! @name virtual :: value on curve*/
	/* @{ */
	virtual Vec3T<T> getPositionAtParam ( u32 curveID, T param ) const{ return Vec3T<T>(); }
	virtual Vec3T<T> getTangentAtParam ( u32 curveID, T param ) const{ return Vec3T<T>(); }
	virtual Vec3T<T> getNormalAtParam ( u32 curveID, T param ) const{ return Vec3T<T>(); }
	virtual XfoT<T> getXfoAtParam ( u32 curveID, T param, u8 directionType = 0 ) const{ return XfoT<T>(); }

	virtual Vec3T<T> getPositionAtNormalizedParam ( u32 curveID, T nParam ) const{ return Vec3T<T>(); }
	virtual Vec3T<T> getTangentAtNormalizedParam ( u32 curveID, T nParam ) const{ return Vec3T<T>(); }
	virtual Vec3T<T> getNormalAtNormalizedParam ( u32 curveID, T nParam ) const{ return Vec3T<T>(); }
	virtual XfoT<T> getXfoAtNormalizedParam ( u32 curveID, T nParam, u8 directionType = 0 ) const{ return XfoT<T>(); }

	virtual Vec3T<T> getPositionAtLengthRatio ( u32 curveID, T ratio ) const{ return Vec3T<T>(); }
	virtual Vec3T<T> getTangentAtLengthRatio ( u32 curveID, T ratio ) const{ return Vec3T<T>(); }
	virtual Vec3T<T> getNormalAtLengthRatio ( u32 curveID, T ratio ) const{ return Vec3T<T>(); }
	virtual XfoT<T> getXfoAtLengthRatio ( u32 curveID, T ratio, u8 directionType = 0 ) const{ return XfoT<T>(); }
	//** @} */
};

#include "CurvesBase.inl"
		/** @} */
	/** @} */
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************