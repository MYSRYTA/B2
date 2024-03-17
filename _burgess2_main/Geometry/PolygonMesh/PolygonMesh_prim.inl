//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    キューブメッシュを生成する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::createCube ( const XfoT<T> &xfo, T sideLength ) {
	PolygonMeshT<T> result;
	result.beginStructureChanges ();
	
	T val = sideLength * T ( 0.5 );
	std::vector<Vec3T<T>> poiVec3 ( 8 );
	poiVec3 [0] = Vec3T<T> ( -val, -val, val );
	poiVec3 [1] = Vec3T<T> ( val, -val, val );
	poiVec3 [2] = Vec3T<T> ( -val, val, val );
	poiVec3 [3] = Vec3T<T> ( val, val, val );
	poiVec3 [4] = Vec3T<T> ( -val, val, -val );
	poiVec3 [5] = Vec3T<T> ( val, val, -val );
	poiVec3 [6] = Vec3T<T> ( -val, -val, -val );
	poiVec3 [7] = Vec3T<T> ( val, -val, -val );
	result.resetPoints ( poiVec3 );

	std::vector<u32> polygonSizes = { 4, 4, 4, 4, 4, 4 };
	std::vector<u32> packedPointIDs = { 0,1,3,2,2,3,5,4,4,5,7,6,6,7,1,0,1,7,5,3,6,0,2,4 };
	result.resetTopology ( polygonSizes, packedPointIDs, false );

	std::vector<Vec3T<T>> normal;
	for ( int i = 0; i < 4; i++ ) { normal.push_back ( Vec3T<T> ( T(0.0), T(1.0), T(0.0) ) ); }
	for ( int i = 0; i < 4; i++ ) { normal.push_back ( Vec3T<T> ( T(0.0), T(-1.0), T(0.0) ) ); }
	for ( int i = 0; i < 4; i++ ) { normal.push_back ( Vec3T<T> ( T(0.0), T(0.0), T(1.0) ) ); }
	for ( int i = 0; i < 4; i++ ) { normal.push_back ( Vec3T<T> ( T(1.0), T(0.0), T(0.0) ) ); }
	for ( int i = 0; i < 4; i++ ) { normal.push_back ( Vec3T<T> ( T(0.0), T(0.0), T(-1.0) ) ); }
	for ( int i = 0; i < 4; i++ ) { normal.push_back ( Vec3T<T> ( T(-1.0), T(0.0), T(0.0) ) ); }
	result.resetNormals ( normal );

	result.endStructureChanges ();

	result.addUVMap ( "map1" );
	std::vector<Vec2> uv ( 14 );
	uv [0] = Vec2T<T> { T ( 0.4 ), T ( 0.0 ) };
	uv [1] = Vec2T<T> { T ( 0.6 ), T ( 0.0 ) };
	uv [2] = Vec2T<T> { T ( 0.4 ), T ( 0.2 ) };
	uv [3] = Vec2T<T> { T ( 0.6 ), T ( 0.2 ) };
	uv [4] = Vec2T<T> { T ( 0.4 ), T ( 0.4 ) };
	uv [5] = Vec2T<T> { T ( 0.6 ), T ( 0.4 ) };
	uv [6] = Vec2T<T> { T ( 0.4 ), T ( 0.6 ) };
	uv [7] = Vec2T<T> { T ( 0.6 ), T ( 0.6 ) };
	uv [8] = Vec2T<T> { T ( 0.4 ), T ( 0.8 ) };
	uv [9] = Vec2T<T> { T ( 0.6 ), T ( 0.8 ) };
	uv [10] = Vec2T<T> { T ( 0.8 ), T ( 0.0 ) };
	uv [11] = Vec2T<T> { T ( 0.8 ), T ( 0.2 ) };
	uv [12] = Vec2T<T> { T ( 0.2 ), T ( 0.0 ) };
	uv [13] = Vec2T<T> { T ( 0.2 ), T ( 0.2 ) };
	std::vector<u32> packedUVPointIDs = { 0,1,3,2,2,3,5,4,4,5,7,6,6,7,9,8,1,10,11,3,12,0,2,13 };
	result.resetAllUVValues ( "map1", uv, packedUVPointIDs );

	result.applyTransform ( xfo );
	*this = result;
}
