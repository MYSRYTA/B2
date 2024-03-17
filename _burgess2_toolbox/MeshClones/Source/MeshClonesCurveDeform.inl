//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    ソースメッシュ情報を登録する
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void MeshClonesCurveDeformT<T>::set ( const PolygonMeshT<T> &sourceMesh
	, u32 size
	, const std::vector<GeometryDeltaT<T>> &sourceDelta
	, T _sourceCurveLength
	,Bool checkBorderEdge ){
	assert ( sourceDelta.size () == 0 || sourceMesh.pointCount () == sourceDelta.size () );
	for ( u32 i = 0, n = sourceDelta.size (); i<n; ++i ) { if ( sourceDelta [i].isValid () == false ) { return; } }

	this->MeshClonesT<T>::set ( sourceMesh, size );
	this->sourceDelta = sourceDelta;
	this->sourceCurveLengthInv = T(1.0) / _sourceCurveLength;
	if ( checkBorderEdge ) {
		std::set<u32> poiIDs;
		for ( u32 edge = 0, n = sourceMesh.edgeCount (); edge<n; ++edge ) {
			if ( sourceMesh.isPolygonBorder ( edge ) ) {
				Vec2u edgePoi = sourceMesh.getEdgePointIDs ( edge );
				poiIDs.insert ( edgePoi.x );
				poiIDs.insert ( edgePoi.y );
			}
		}
		this->sourceBorderPointIDs.resize ( poiIDs.size () );
		u32 i = 0;
		for ( auto itr = poiIDs.begin (); itr != poiIDs.end (); ++itr ) {
			this->sourceBorderPointIDs [i] = *itr;
			i++;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    カーブデフォーム (クローン数==カーブ数)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void MeshClonesCurveDeformT<T>::applayCurveDeform ( const BSplineCurvesT<T> &curves ) {
	if ( this->cloneCount () != curves.curveCount () || this->cloneCount () == 0 || curves.curveCount ()  == 0 ) { return; }

	std::vector<CurveSectionsT<T>> sectionsList ( curves.curveCount () );
	for ( u32 i = 0, n = curves.curveCount (); i<n; ++i ) {
		sectionsList [i].add ( T ( 0.0 ), T ( 1.0 ) );
	}
	this->applayCurveDeform ( curves, sectionsList );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    カーブの一定区間内でデフォーム (クローン数==カーブセクションの総数)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void MeshClonesCurveDeformT<T>::applayCurveDeform ( const BSplineCurvesT<T> &curves, const std::vector<CurveSectionsT<T>> &sectionsList ) {
	if ( this->sourceDelta.size () != this->sourceMesh.pointCount () ) { return; }
	std::vector<u32> secCnt(1);
	secCnt [0] = 0;
	for ( u32 i = 0, n = sectionsList.size (); i<n; ++i ) {
		secCnt.push_back( secCnt[i] + sectionsList [i].sectionCount () );
	}
	if ( secCnt[secCnt.size()-1] != this->cloneCount () || this->cloneCount () == 0 || curves.curveCount ()  == 0 || curves.curveCount() != sectionsList.size() ) { return; }
	Vec3Attribute &poiPos = this->mesh.getPointPositionAttribute ();
	Vec3Attribute &verNormal = this->mesh.getNormalAttribute ();
	u32 sourceMeshPoiCnt = this->sourceMesh.pointCount ();
	u32 sourceMeshVerCnt = this->sourceMesh.vertexCount ();
	
	std::vector<T> scale ( curves.curveCount () );
	for ( u32 i = 0, n = scale.size (); i<n; ++i ) { scale [i] = curves.getCurveLength ( i ) * this->sourceCurveLengthInv * T(0.5); }
	u32 n = poiPos.size ();
	#pragma omp parallel for
		for ( s32 i = 0; i<n; ++i ) {
			u32 cloneID = u32 ( i / sourceMeshPoiCnt );
			u32 origPoiID = i - ( sourceMeshPoiCnt * cloneID ) ;

			/*sectionsListの配列番号(=カーブID)とsectionの番号を取得*/
			u32 secID;
			u32 secArrayID = Math::TreeInterpolate ( secCnt, cloneID, secID );
			
			/*Deltaをセクション範囲で再現する*/
			T sourceLenRatio = this->sourceDelta [origPoiID].getCurveLengthRatio ();
			T reRatio = sectionsList [secArrayID].changeRange ( secID, sourceLenRatio );
			XfoT<T> locXfo = curves.getXfoAtLengthRatio ( secArrayID, reRatio );
			poiPos [i] = locXfo * ( this->sourceDelta [origPoiID].getDelta () * scale[secArrayID] * sectionsList [secArrayID].getLengthRatio ( secID ) );
			
			/* 法線 (Deltaに法線が登録されている場合のみ処理を実行する) */
			L16Array<u32> ver = this->sourceMesh.getSharedVertexIDs ( origPoiID );
			L16Array<Vec3T<T>> norDelta = this->sourceDelta [origPoiID].getNormalDeltas ();
			if ( ver.size () == norDelta.size () ) {
				locXfo.tr = Vec3T<T> ( T ( 0.0 ) );
				u32 offset = sourceMeshVerCnt * cloneID;
				for ( u32 k = 0, m = ver.size (); k<m; ++k ) {
					verNormal [ver [k]+offset] = locXfo * norDelta [k];
				}
			}
		}
}