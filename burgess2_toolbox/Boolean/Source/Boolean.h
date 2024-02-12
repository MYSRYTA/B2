#pragma once
#include <burgess2_main/Geometry.h>
//#include "Boolean_EdgeList.h"

//******************************************************************************
//! @file	Boolean.h
//! @brief	 Boolean
//******************************************************************************
/** @defgroup Tools
*	   @{
*/
/**		@brief	メッシュ・ブーリアン<br>
*			<br>
*/
/**			@defgroup Boolean
*			@{
*/
/**				@brief Boolean <br>
*				Boolean <br>
*				#include <ToolBox/Boolean/Boolean.h><br>
*/

template<typename T>
struct BooleanCrossDataT {
	std::vector<L4Array<Vec3T<T>>> crossPos;
	std::vector<L4Array<Vec2s>> crossPoly;
	std::vector<L4Array<s32>> splitEdgeSPoiID;
	std::vector<L4Array<T>> splitRatio;

	BooleanCrossDataT( u32 size ) {
		this->resize( size );
	}

	void resize(u32 size) {
		this->crossPos.resize( size );
		this->crossPoly.resize( size );
		this->splitEdgeSPoiID.resize( size );
		this->splitRatio.resize( size );
	}

	void push( const u32 id, const Vec3T<T>& pos, const Vec2s& poly, const s32 edgeID, const T ratio );
};

template<typename T>
class BooleanT {
public:
	BooleanT() {};
	virtual ~BooleanT() {}

	Bool initalize ( const PolygonMeshT<T>& meshA, const PolygonMeshT<T>& meshB );
	Bool meshReset ( const PolygonMeshT<T>& mesh, Bool isMeshA );
	Bool core( OUT std::vector<PolygonMeshT<T>>& result, const PolygonMeshT<T>& meshA, const PolygonMeshT<T>& meshB );

private:
	Bool initalized = false;
	std::vector<u32> edgeListA;
	std::vector<u32> edgeListB;
	u32 pointSizeA = 0;
	u32 pointSizeB = 0;
	u32 polygonSizeA = 0;
	u32 polygonSizeB = 0;
	PolygonMeshSpatialQueryT<T> sqA;
	PolygonMeshSpatialQueryT<T> sqB;

	std::vector<u32> createEdgeList( const PolygonMeshT<T>& mesh )const;

	void CrossDataSet(
		IO BooleanCrossDataT<T>& crossData0,
		IO BooleanCrossDataT<T>& crossData1,
		const std::vector<u32>& edgeList0,
		const PolygonMeshT<T>& mesh0,
		const PolygonMeshT<T>& mesh1,
		const PolygonMeshSpatialQueryT<T>& sq1,
		const T tolerance
	);

	void MakeSplitPolygonsList(
		OUT std::vector<u32>& polygonIDs,
		OUT std::vector<u32>& pointIDs1,
		OUT std::vector<f32>& ratio1,
		OUT std::vector<u32>& pointIDs2,
		OUT std::vector<f32>& ratio2,
		OUT std::vector<std::vector<Vec3T<T>>>& spritPositionsList,
		const PolygonMeshT<T>& mesh0,
		const BooleanCrossDataT<T>& crossData0,
		const Bool hole
	);

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/*template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( delta )
		);
	}*/
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Boolean.inl"

	/** @} */
/** @} */

using Boolean = BooleanT<f32>;
using Booleand = BooleanT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************