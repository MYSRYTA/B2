#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Math.h>
#include <burgess2_main/Containers/LArray.h>
#include <burgess2_main/Geometry.h>

//******************************************************************************
//! @file	Dijkstra.h
//! @brief	 ダイクストラ
//******************************************************************************
/** @defgroup Algorithm
*　@{
*/
/**	@brief	経路探索：ダイクストラ アルゴリズム<br>
*		参照先　
*		@ref http://qiita.com/edo_m18/items/0588d290a19f2afc0a84 <br>
*/
/**		@defgroup Dijkstra
*　		@{
*/
/**			@brief	ダイクストラグラフを構成するノード <br>
*/

template<typename T>
struct DijkstraNodeT {
	LArrayT<16, u32> surroundingNodeID; // 隣接するノードID
	LArrayT<16, T> surroundingNodeCost; // 隣接ノードへのコスト（距離）
	Bool done = false; // このノードが確定の場合はture
	T cost = T ( -1.0 ); // このノードへの最小コスト値を一時保管場所する変数

	Bool assign = false;

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( surroundingNodeID ),
			CEREAL_NVP ( surroundingNodeCost ),
			CEREAL_NVP ( done ),
			CEREAL_NVP ( cost ),
			CEREAL_NVP ( assign )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

/**		@brief	ダイクストラ アルゴリズム<br>
*			#include <burgess2_main/Algorithm.h> <br><br>
*			#alias <br> \b : Dijkstra = DijkstraT<float> <br> \b : Dijkstrad = DijkstraT<double> <br>
*			参照先　
*			@ref http://qiita.com/edo_m18/items/0588d290a19f2afc0a84 <br>
*			@code
*				// メッシュの頂点[4]から各頂点への経路探索を行う
*				PolygonMesh cube;
*				cube.createCube( Xfo(), 1.0f );
*
*				Dijkstra dij;
*				dij.setFromPolygonMesh( cube, false ); // メッシュをダイクストラに登録する
*				dij.recomputeRouteCost( 4, 1.1f ); // ４番頂点から各頂点への移動コスト(移動距離)を計算する
*				std::vector<f32> cost = dij.getRouteCost(); // 計算結果を取得する
*			@endcode
*/
template<typename T>
class DijkstraT {
	std::vector<DijkstraNodeT<T>> node;
	T maxCost;
	Bool initialized = false;
	Bool computed = false;
public:
	DijkstraT () {};
	virtual ~DijkstraT () {}

	void setFromPolygonMesh ( const PolygonMeshT<T> &mesh, Bool includeAllPolygonPoints );

	void recomputeRouteCost ( std::vector<u32> startNodeIDs, T maxDistance = T(BURGESS2::F32_INFINITE) );
	std::vector<T> getRouteCost ( u8 returnValueType = 0 );

	/// @cond
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( node ),
			CEREAL_NVP ( maxCost ),
			CEREAL_NVP ( initialized ),
			CEREAL_NVP ( computed )
		);
	}
	/***************************************************************************************************** FileIO用の仕込み ドキュメントには載らない*/
	/// @endcond
};

#include "Dijkstra.inl"
	/** @} */
/** @} */
using Dijkstra = DijkstraT<f32>;
using Dijkstrad = DijkstraT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************