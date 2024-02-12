//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    スムース
//!	    @brief スムース<br>
//!		事前に@ref getOrCreateSurroundingPointIDsAttribute "getOrCreateSurroundingPointIDsAttribute()"で隣接頂点IDのアトリビュートを準備しておくことで高速化できる
//!		@param[in] iteration 反復回数
//!		@param[in] t 一回の計算でスムースする強さ(0.0～1.0)
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void PolygonMeshT<T>::smooth ( u8 iteration, T t ){
	if ( 0 == iteration ){ return; }
	s32 poiCnt = this->pointCount ();

	if(this->hasAttribute("surroundingPointIDs")){
		L16ArrayU32AttributeT<T> &restSurroundingPoints = this->getL16ArrayU32Attribute ( "surroundingPointIDs" );
		for ( u8 ite = 0; ite < iteration; ++ite ){
			std::vector < Vec3T<T> >& positionsBuffer = this->getAllPointPositions ();
			#pragma omp parallel for
			for ( s32 index = 0; index < poiCnt; ++index ){
				L16Array< u32 > &poiIDs = restSurroundingPoints[index];
				if ( poiIDs.size () == 0 ){ continue; }
				
				Vec3T<T> add{ T ( 0.0 ) };
				for ( u32 j = 0; j < poiIDs.size (); ++j ){
					add += positionsBuffer[poiIDs[j]] - positionsBuffer[index];
				}
				add = add / f32 ( poiIDs.size () ) * t;
				this->setPointPosition ( index, positionsBuffer[index] + add );
			}
		}
	}else{
		std::vector< L16Array< u32 > > restSurroundingPoints ( poiCnt );
		#pragma omp parallel for
		for ( s32 index = 0; index < poiCnt; ++index ){
			restSurroundingPoints[index] = this->getPointSurroundingPoints ( index, false );
		}

		for ( u8 ite = 0; ite < iteration; ++ite ){
			std::vector < Vec3T<T> >& positionsBuffer = this->getAllPointPositions ();
			
			#pragma omp parallel for
			for ( s32 index = 0; index < poiCnt; ++index ){
				L16Array< u32 >& poiIDs = restSurroundingPoints[index];
				if ( poiIDs.size () == 0 ){ continue; }

				Vec3T<T> add{ T ( 0.0 ) };
				for ( u32 j = 0; j < poiIDs.size (); ++j ){
					add += positionsBuffer[poiIDs[j]] - positionsBuffer[index];
				}
				add = add / f32 ( poiIDs.size () ) * t;
				this->setPointPosition ( index, positionsBuffer[index] + add );
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    スムース
//!	    @brief スムース
//!		@param[in] iteration 反復回数
//!		@param[in] t 一回の計算でスムースする強さ(0.0～1.0)
//!		@param[in] keepBorder 境界エッジをスムースしない場合はtrue
//!		@param[in] type スムース計算方法　0=エッジで繋がった頂点を使用, 1=フェースで繋がった頂点を使用, 2=DualMeshを使用
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<typename T>
void PolygonMeshT<T>::smooth ( u8 iteration, T t, Bool keepBorder, u8 type ){
	if ( iteration == 0 || Burgess2MathFn::AlmostEqual ( t, T ( 0.0 ) ) || 2 < type ){ return; }
	
	std::vector<Bool> border ( this->pointCount (), false );
	if ( keepBorder ){
		#pragma omp parallel for
		for ( s32 i = 0; i < this->pointCount (); ++i ){
			border[i] = this->isBorderPoint(i);
		}
	}

	if ( type == 0 || type == 1 ){
		Bool r = true;
		std::vector<Vec3T<T>> restPos ( this->pointCount () );
		for ( u8 ite = 0; ite < iteration; ++ite ){
			#pragma omp parallel for
			for ( s32 i = 0; i < this->pointCount (); ++i ){
				if ( border[i] ){
					if ( r ){
						restPos[i] = this->getPointPosition ( i );
					} else{
						this->setPointPosition ( i, restPos[i] );
					}
				} else{
					Vec3T<T> cPos;
					L16Array<u32> sPoiIDs = this->getPointSurroundingPoints ( i, type == u8 ( 1 ) );
					for ( u32 j = 0; j < sPoiIDs.size (); ++j ){
						if ( r ){
							cPos += this->getPointPosition ( sPoiIDs[j] );
						} else{
							cPos += restPos[sPoiIDs[j]];
						}
					}
					cPos = cPos / T ( sPoiIDs.size () );
					if ( r ){
						restPos[i] = this->getPointPosition ( i ).lerp ( cPos, t );
					} else{
						this->setPointPosition ( i, restPos[i].lerp ( cPos, t ) );
					}
				}
			}
			if ( ite < iteration - 1 ){
				r = r == false;
			}
		}
		if ( r ){ this->setAllPointPositions ( restPos ); }
	} else if( type == 2 ){
		for ( u8 ite = 0; ite < iteration; ++ite ){
			std::vector<u32> emptyMask;
			std::vector<s32> dualFaceToSourcePoint;
			PolygonMeshT<T> dMesh = this->createDualMesh ( T ( 1.0 ), emptyMask, dualFaceToSourcePoint );
			#pragma omp parallel for
			for ( s32 i = 0; i < dMesh.pointCount (); ++i ){
				Vec3T<T> cPos;
				L16Array<u32> sPoiIDs = dMesh.getPointSurroundingPoints ( i, false );
				for ( u32 j = 0; j < sPoiIDs.size (); ++j ){
					cPos += dMesh.getPointPosition ( sPoiIDs[j] );
				}
				cPos *= T ( 0.333333 );
				Vec3T<T> smoothPos = dMesh.getPointPosition ( i ).lerp ( cPos, t );
				dMesh.setPointPosition ( i, smoothPos );
			}
			#pragma omp parallel for
			for ( s32 i = 0; i < dMesh.polygonCount (); ++i ){
				u32 sourcePoiID = dualFaceToSourcePoint[i];
				if ( border[sourcePoiID] ){ continue; }

				L8Array<u32> poiIDs = dMesh.getPolygonPointIDs ( i );
				Vec3T<T> cPos;
				for ( u32 j = 0; j < poiIDs.size (); ++j ){
					cPos += dMesh.getPointPosition ( poiIDs[j] );
				}
				cPos /= T ( poiIDs.size () );
				this->setPointPosition ( sourcePoiID, cPos );
			}
		}
	}
}*/