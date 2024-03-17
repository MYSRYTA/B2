#include <ratio>

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
PointsSpatialQueryT<T>::PointsSpatialQueryT (const std::vector<Vec3T<T>>& vec3Array) {
	if (vec3Array.size () == 0) {
		this->initialized = false;
		return;
	}
	
	this->pointCloud.reset ( new NanoPointCloudT<T>( vec3Array ) );
	this->nTree3.reset ( new NanoFlannTree3T<T> ( 3, *this->pointCloud, nanoflann::KDTreeSingleIndexAdaptorParams ( 10 ) ) );
	this->nTree3->buildIndex ();
	
	this->initialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		最も近いポイントの配列番号を取得
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
s32 PointsSpatialQueryT<T>::getClosestIndex ( const Vec2T<T>& position, T maxDistance )const {
	T query_pt[2] = { position.x(), position.y() };
	size_t closestID = -1;
	T distance;
	this->nTree2->knnSearch ( query_pt, 1, &closestID, &distance );

	T maxDis = T ( BURGESS2::F32_INFINITE ) <= maxDistance ? maxDistance : SQ ( maxDistance );
	if (0 <= closestID && distance < maxDis) {
		return closestID;
	} else {
		return -1;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		最も近いポイントの配列番号を取得
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
s32 PointsSpatialQueryT<T>::getClosestIndex ( const Vec3T<T>& position, T maxDistance )const {
	T query_pt[3] = { position.x(), position.y(), position.z() };
	size_t closestID = -1;
	T distance;
	this->nTree3->knnSearch ( query_pt, 1, &closestID, &distance );

	T maxDis = T ( BURGESS2::F32_INFINITE ) <= maxDistance ? maxDistance : SQ ( maxDistance );
	if (0 <= closestID && distance < maxDis) {
		return closestID;
	} else {
		return -1;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		指定した値に近いポイントを複数個検索し、近い順に配列番号を取得
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<u32> PointsSpatialQueryT<T>::getNClosestIndexes ( const Vec2T<T>& position, u32 maxCount, T maxDistance )const {
	std::vector<std::pair<size_t, T> > ret_matches;
	T query_pt[2] = { position.x(), position.y() };

	nanoflann::SearchParams params;
	params.sorted = true;

	size_t nMatches = this->nTree2->radiusSearch ( query_pt, maxDistance, ret_matches, params );

	size_t cnt = nMatches < size_t ( maxCount ) ? nMatches : size_t ( maxCount );

	std::vector<u32> result;
	for (size_t i = 0; i < cnt; i++) {
		result.push_back ( ret_matches[i].first );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		指定した値に近いポイントを複数個検索し、近い順に配列番号を取得
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<u32> PointsSpatialQueryT<T>::getNClosestIndexes ( const Vec3T<T>& position, u32 maxCount, T maxDistance )const {
	std::vector<std::pair<size_t, T> > ret_matches;
	T query_pt[3] = { position.x(), position.y(), position.z() };

	nanoflann::SearchParams params;
	params.sorted = true;

	size_t nMatches = this->nTree3->radiusSearch ( query_pt, maxDistance, ret_matches, params );

	size_t cnt = nMatches < size_t(maxCount) ? nMatches : size_t(maxCount);

	std::vector<u32> result;
	for (size_t i = 0; i < cnt; i++) {
		result.push_back ( ret_matches[i].first );
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		指定した値から一定範囲内に存在するポイントを検索し、全ての配列番号を取得
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<u32> PointsSpatialQueryT<T>::getRadiusSearchIndexes ( const Vec2T<T>& position, T searchRadius, Bool sort )const {
	std::vector<std::pair<size_t, T> > ret_matches;
	T query_pt[2] = { position.x(), position.y() };

	nanoflann::SearchParams params;
	params.sorted = sort;

	size_t nMatches = this->nTree2->radiusSearch ( query_pt, searchRadius, ret_matches, params );

	std::vector<u32> result ( nMatches );
	for (size_t i = 0; i < nMatches; i++) {
		result[i] = ret_matches[i].first;
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		指定した値から一定範囲内に存在するポイントを検索し、全ての配列番号を取得
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<u32> PointsSpatialQueryT<T>::getRadiusSearchIndexes ( const Vec3T<T>& position, T searchRadius, Bool sort )const {
	std::vector<std::pair<size_t, T> > ret_matches;
	T query_pt[3] = { position.x(), position.y(), position.z() };

	nanoflann::SearchParams params;
	params.sorted = true;

	size_t nMatches = this->nTree3->radiusSearch ( query_pt, searchRadius, ret_matches, params );

	std::vector<u32> result;
	for (size_t i = 0; i < nMatches; i++) {
		if (ret_matches[i].first < this->pointCloud->points.size ()) {
			result.push_back ( ret_matches[i].first );
		}
	}
	return result;
}