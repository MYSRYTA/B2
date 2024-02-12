namespace Math {
	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     点Pが三角形の面上に存在する場合はtrueを返す
	//!	    @brief	点Pが三角形の面上に存在する場合はtrueを返す
	//!		@param[in] P　点P
	//!		@param[in] triP0,1,2　三角形の頂点位置
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Bool TestInsideTriangle ( const Vec3T<T> &P, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, T tolerance ) {
		Vec3T<T> p0p2 = triP0 - triP2;
		Vec3T<T> p1p0 = triP1 - triP0;
		Vec3T<T> p2p1 = triP2 - triP1;
		Vec3T<T> pp0 = P - triP0;
		Vec3T<T> pp1 = P - triP1;
		Vec3T<T> pp2 = P - triP2;

		// 点Pが三角形の頂点上に存在する場合はtrueを返して終了 
		if ( P.almostEqual ( triP0, tolerance ) || P.almostEqual ( triP1, tolerance ) || P.almostEqual ( triP2, tolerance ) ) {
			return true;
		}

		// 点Pが三角形のエッジ上に存在する場合はtrueを返して終了
		Bool onEdge = false;
		if ( p1p0.almostSameDirection ( pp0 ) && pp0.lengthSquared () <= p1p0.lengthSquared () ) {
			onEdge = true;
		} else if ( p2p1.almostSameDirection ( pp1 ) && pp1.lengthSquared () < p2p1.lengthSquared () ) {
			onEdge = true;
		} else if ( p0p2.almostSameDirection ( pp2 ) && pp2.lengthSquared () < p0p2.lengthSquared () ) {
			onEdge = true;
		}
		if (onEdge) {
			Vec3T<T> w = Math::GetTriBarycentric ( P, triP0, triP1, triP2, tolerance, false );
			if (w.x < T ( -Burgess::TOLERANCE ) || w.y < T ( -Burgess::TOLERANCE ) || w.z < T ( -Burgess::TOLERANCE )) {
				return false;
			}
			return true;
		}
	
		Vec3T<T> a = p0p2.cross ( pp0 ).unit ();
		Vec3T<T> b = p1p0.cross ( pp1 ).unit ();
		Vec3T<T> c = p2p1.cross ( pp2 ).unit ();

		T d_ab = a.dot ( b );
		T d_bc = b.dot ( c );

		// 点Pが三角形内に存在すればtrueを返す 
		if ( T(1.0) - tolerance < d_ab && T( 1.0 ) - tolerance < d_bc ) {
			return true;
		}
		return false;		
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     三角形内において点Pの位置を表すウェイト値を取得する
	//!	    @brief	三角形内において点Pの位置を表すウェイト値を取得する<br>
	//!		@param[in] P 点P
	//!		@param[in] triP0,1,2　三角形の頂点位置
	//!		@return Vec3のx~zは各頂点0~2へのウェイト値 ( 各頂点の位置xウェイトの合計が点Pの座標になる )
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> GetTriBarycentric ( const Vec3T<T> &P, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, T tolerance, Bool normalize ) {
		Vec3T<T> pt0ToStart = P - triP0;
		Vec3T<T> pt0ToPt1 = triP1 - triP0;
		Vec3T<T> pt0ToPt2 = triP2 - triP0;

		Vec3T<T> triCross = pt0ToPt1.cross ( pt0ToPt2 );
		T triCrossSqLen = triCross.dot ( triCross );

		Vec3T<T> pt0ToInters = pt0ToStart - triCross * ( pt0ToStart.dot ( triCross ) / triCrossSqLen );
		T val1;
		T d00, d01, d11, d20, d21;
		d00 = pt0ToPt1.dot ( pt0ToPt1 );
		d01 = pt0ToPt1.dot ( pt0ToPt2 );
		d11 = pt0ToPt2.dot ( pt0ToPt2 );
		d20 = pt0ToInters.dot ( pt0ToPt1 );
		d21 = pt0ToInters.dot ( pt0ToPt2 );
		val1 = d00 * d11 - d01 * d01;

		T invDenom = T ( 1.0 ) / val1;
		T v = ( d11 * d20 - d01 * d21 ) * invDenom;
		T w = ( d00 * d21 - d01 * d20 ) * invDenom;
		T u = T ( 1.0 ) - v - w;
		if (normalize) {
			s32 neg[3];
			neg[0] = u < T ( 0.0 ) ? 1 : 0;
			neg[1] = v < T ( 0.0 ) ? 1 : 0;
			neg[2] = w < T ( 0.0 ) ? 1 : 0;

			if (neg[0] + neg[1] + neg[2]) {
				T ratio;
				if (neg[0]) {
					GetClosestPositionOnLineSegment ( pt0ToStart, pt0ToPt1, pt0ToPt2, ratio, tolerance );
					u = T ( 0.0 );
					if (T ( 1.0 ) <= ratio) {
						v = T ( 0.0 );
					} else {
						v = T ( 1.0 ) - ratio;
					}					
				}
				if (neg[1]) {
					GetClosestPositionOnLineSegment ( pt0ToStart, Vec3T<T> (), pt0ToPt2, ratio, tolerance );
					if (T ( 1.0 ) <= ratio) {
						u = T ( 0.0 );
					} else {
						u = T ( 1.0 ) - ratio;
					}
					v = T ( 0.0 );
				}
				if (neg[2]) {
					GetClosestPositionOnLineSegment ( pt0ToStart, Vec3T<T> (), pt0ToPt1, ratio, tolerance );
					if (T ( 1.0 ) <= ratio) {
						u = T ( 0.0 );
					} else {
						u = T ( 1.0 ) - ratio;
					}
					v = ratio;
				}
			}
		}

		T resultW;
		if (T ( 1.0 ) <= u + v) {
			resultW = T ( 0.0 );
		} else {
			resultW = T ( 1.0 ) - (u + v);
		}
		return Vec3T<T> { u, v, resultW };
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     点Pから三角形に対して最も近い位置を取得する
	//!	    @brief	点Pから三角形に対して最も近い位置を取得する
	//!		@param[in] P 点P
	//!		@param[in] triP0,1,2　三角形の頂点位置
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> GetClosestPositionOnTriangle ( const Vec3T<T> &P, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2 ) {
		Vec3T<T> result ( Burgess::F32_INFINITE );
		if ( triP0.almostEqual ( triP1 ) || triP0.almostEqual ( triP2 ) || triP1.almostEqual ( triP2 ) ) { return result; }

		Vec3T<T> edge0 = triP1 - triP0;
		Vec3T<T> edge1 = triP2 - triP0;
		Vec3T<T> p0 = P - triP0;

		T vector0 = edge0.dot ( edge0 );
		T vector1 = edge0.dot ( edge1 );
		T vector2 = edge1.dot ( edge1 );
		T vector3 = p0.dot ( edge0 ) * -1.0;
		T vector4 = p0.dot ( edge1 ) * -1.0;

		T d0 = vector0 * vector2 - vector1 * vector1;
		T t0 = vector1 * vector4 - vector2 * vector3;
		T t1 = vector1 * vector3 - vector0 * vector4;

		if ( t0 + t1 <= d0 ){
			if ( t0 < T(0.0) ){
				if ( t1 < T(0.0) ) {
					if ( vector3 < T(0.0) ){
						t0 = vector0 <= -vector3 ? T ( 1.0 ) : -vector3 / vector0;
						t1 = T(0.0);
					} else{
						t0 = T(0.0);
						if ( T ( 0.0 ) <= vector4 ) {
							t1 = T(0.0);
						} else if ( vector2 <= -vector4 ) {
							t1 = T(1.0);
						} else {
							t1 = -vector4 / vector2;
						}
					}
				} else {
					t0 = T(0.0);
					if ( T ( 0.0 ) <= vector4 ) {
						t1 = T(0.0);
					} else if ( vector2 <= -vector4 ) {
						t1 = T(1.0);
					} else {
						t1 = -vector4 / vector2;
					}
				}
			} else if ( t1 < T(0.0) ) {
				t1 = T(0.0);
				if ( T ( 0.0 ) <= vector3 ) {
					t0 = T(0.0);
				} else if ( vector0 <= -vector3 ) {
					t0 = T(1.0);
				} else {
					t0 = -vector3 / vector0;
				}
			} else {
				T invDet = T(1.0) / d0;
				t0 *= invDet;
				t1 *= invDet;
			}
		} else{
			if ( t0 < T(0.0) ) {
				T rest0 = vector1 + vector3;
				T rest1 = vector2 + vector4;
				if ( rest0 < rest1 ){
					T val0 = rest1 - rest0;
					T val1 = vector0 - 2.0 * vector1 + vector2;
					if ( val1 <= val0 ) {
						t0 = T(1.0);
						t1 = T(0.0);
					} else {
						t0 = val0 / val1;
						t1 = T(1.0) - t0;
					}
				} else{
					t0 = T(0.0);
					if ( rest1 <= T(0.0) ) {
						t1 = T(1.0);
					} else if ( T ( 0.0 ) <= vector4 ) {
						t1 = T(0.0);
					} else {
						t1 = -vector4 / vector2;
					}
				}
			} else if ( t1 < T(0.0) ) {
				T rest0 = vector1 + vector4;
				T rest1 = vector0 + vector3;
				if ( rest0 < rest1 ){
					T val0 = rest1 - rest0;
					T val1 = vector0 - 2.0 * vector1 + vector2;
					if ( val1 <= val0 ) {
						t1 = T(1.0);
						t0 = T(0.0);
					} else {
						t1 = val0 / val1;
						t0 = T(1.0) - t1;
					}
				} else{
					t1 = T(0.0);
					if ( rest1 <= T(0.0) ) {
						t0 = T(1.0);
					} else if ( T ( 0.0 ) <= vector3 ) {
						t0 = T(0.0);
					} else {
						t0 = -vector3 / vector0;
					}
				}
			} else {
				T val0 = vector2 + vector4 - vector1 - vector3;
				if ( val0 <= T(0.0) ) {
					t0 = T(0.0);
					t1 = T(1.0);
				} else{
					T val1 = vector0 - 2.0 * vector1 + vector2;
					if ( val1 <= val0 ) {
						t0 = T(1.0);
						t1 = T(0.0);
					} else {
						t0 = val0 / val1;
						t1 = T(1.0) - t0;
					}
				}
			}
		}
		return triP0 + edge0 * t0 + edge1 * t1;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    三角形の法線を取得する
	//!	    @brief	三角形の法線を取得する
	//!		@param[in] triP0,1,2　三角形の頂点位置
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> GetTriangleNormal ( const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2 ) {
		Vec3T<T> edge1 = triP1 - triP0;
		Vec3T<T> edge2 = triP2 - triP0;
		return edge1.cross ( edge2 ).unit();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    三角形の面積を取得する
	//!	    @brief	三角形の面積を取得する
	//!		@param[in] triP0,1,2　三角形の頂点位置
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	T GetTriangleArea ( const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2 ){
		if ( triP0.almostEqual( triP1 ) || triP1.almostEqual( triP2 ) || triP2.almostEqual( triP0 ) ) {
			return T( 0.0 );
		}

		Vec3T<T> edge1 = triP1 - triP0;
		Vec3T<T> edge2 = triP2 - triP0;
		if ( edge1.almostParallel( edge2 ) ) {
			return T( 0.0 );
		}
		return edge1.cross ( edge2 ).length() * T ( 0.5 );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     startPositionから点群への最も近い点の配列番号を取得する
	//!	    @brief	startPositionから点群への最も近い点の配列番号を取得する
	//!		@param[in] points 点群
	//!		@param[in] startPosition 検索スタート地点
	//!		@param[in] maxDistance 検索距離
	//!		@return 最も近い点の配列番号 (maxDistance内で点が見つからなかった場合はu32無限数を返す)
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename T>
	u32 GetClosestPointID ( const std::vector<Vec3T<T>> &points, const Vec3T<T> &startPosition, T maxDistance ) {
		s32 loopCnt = points.size ();
		if ( loopCnt == 0 ) { return Burgess::U32_INFINITE; }

		std::vector<T> dist ( loopCnt );
		#pragma omp parallel for
		for ( s32 i = 0; i<loopCnt; i++ ) {
			dist [i] = (startPosition - points [i] ).lengthSquared();
		}
		u32 result = Math::MinimumIndex ( dist );
		return dist[result] < SQ( maxDistance ) ? result : Burgess::U32_INFINITE;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     スフィア内に存在する全ての点の配列番号を取得する
	//!	    @brief	スフィア内に存在する全ての点の配列番号を取得する
	//!		@param[in] points 点群
	//!		@param[in] centerPosition スフィアの中心位置
	//!		@param[in] sphereRadius スフィアの半径
	//!		@param[in] sort centerPositionから近い順にソートする場合はtrue
	//!		@return 点の配列番号
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename T>
	std::vector<u32> GetPointIDsInSphere ( const std::vector<Vec3T<T>> &points, const Vec3T<T> &centerPosition, T sphereRadius, Bool sort ) {
		std::vector<u32> result(0);
		s32 loopCnt = points.size ();
		if ( loopCnt == 0 ) { return result; }

		std::vector<T> dist ( loopCnt );
		#pragma omp parallel for
		for ( s32 i = 0; i<loopCnt; i++ ) {
			dist[i] = ( centerPosition - points[i] ).lengthSquared ();// 
		}

		T sphereRadiusSQ = SQ ( sphereRadius );

		result.reserve ( loopCnt );
		if ( sort ) {
			std::vector<T> dist2;
			dist2.reserve ( loopCnt );
			for ( u32 i = 0; i < loopCnt; i++ ) {
				if ( dist[i] < sphereRadiusSQ ) {
					dist2.push_back ( dist[i] );
				} else {
					break;
				}
			}
			Math::Sort ( dist2, result );
		} else {
			for ( u32 i = 0; i < loopCnt; i++ ) {
				if ( dist[i] < sphereRadiusSQ ) {
					result.push_back ( i );
				}
			}
		}
		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     点Pから無限線への最も近い位置を取得する
	//!	    @brief	点Pから無限線への最も近い位置とレシオを取得する
	//!		@param[in] P 点P
	//!		@param[in] lineP0,1　無限線を表す2点
	//!		@return 点Pから無限線への最も近い位置
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename T>
	Vec3T<T> GetClosestPositionOnLine ( const Vec3T<T> &P, const Vec3T<T> &lineP0, const Vec3T<T> &lineP1, OUT T& segment_t ) {
		Vec3T<T> linVec = lineP1 - lineP0;
		Vec3T<T> pToP0 = P - lineP0;

		T dot1 = pToP0.dot ( linVec );
		T dot2 = linVec.dot ( linVec );
		T line_t;
		if ( dot2 < Burgess::TOLERANCE ) {
			line_t = T ( 0.0 );
		} else {
			line_t = dot1 / dot2;
		}

		segment_t = line_t;
		return lineP0 + linVec * line_t;
	}

	template <typename T>
	Vec3T<T> GetClosestPositionOnLine ( const Vec3T<T>& P, const Vec3T<T>& lineP0, const Vec3T<T>& lineP1 ) {
		T segment_t;
		return GetClosestPositionOnLine(P, lineP0, lineP1, segment_t );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     点Pから線分への最も近い位置とレシオを取得する
	//!	    @brief	点Pから線分への最も近い位置とレシオを取得する
	//!		@param[in] P 点P
	//!		@param[in] lineP0,1　線分の始点と終点
	//!		@param[out] segment_t　取得した位置をP0~P1までを0.0~1.0としたレシオで返す
	//!		@return 点Pから線分への最も近い位置
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename T>
	Vec3T<T> GetClosestPositionOnLineSegment ( const Vec3T<T> &P, const Vec3T<T> &segmentP0, const Vec3T<T> &segmentP1, OUT T &segment_t, T tolerance ) {
		Vec3T<T> linVec = segmentP1 - segmentP0;
		Vec3T<T> pToP0 = P - segmentP0;

		T dot1 = pToP0.dot ( linVec );
		if ( dot1 < T( 0.0 ) ) {
			segment_t = T( 0.0 );
		}else if ( Math::AlmostEqual( dot1, T( 0.0 ), tolerance ) ){
			segment_t = T( 0.0 );
		} else {
			T dot2 = linVec.dot ( linVec );
			if ( dot2 < dot1 ) {
				segment_t = T ( 1.0 );
			} else if ( Math::AlmostEqual( dot2, dot1, tolerance ) ) {
				segment_t = T ( 1.0 );
			} else {
				segment_t = dot1 / dot2;
			}
		}

		return segmentP0 + linVec * segment_t;
	}

	template <typename T>
	Vec3T<T> GetClosestPositionOnLineSegment ( const Vec3T<T>& P, const Vec3T<T>& segmentP0, const Vec3T<T>& segmentP1 ) {
		T segment_t;
		return GetClosestPositionOnLineSegment ( P, segmentP0, segmentP1, segment_t, Burgess::TOLERANCE);
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     点Pから無限線への最短距離を取得する
	//!	    @brief	点Pから無限線への最短距離を取得する
	//!		@param[in] P 点P
	//!		@param[in] lineP0,1　無限線を表す2点
	//!		@return 点Pから無限線への最短距離
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename T>
	T GetDistanceFromPointToLine ( const Vec3T<T> &P, const Vec3T<T> &lineP0, const Vec3T<T> &lineP1, OUT T& segment_t, T tolerance ) {
		Vec3T<T> locPos = GetClosestPositionOnLine ( P, lineP0, lineP1, segment_t );
		return P.distanceTo ( locPos );
	}

	template <typename T>
	T GetDistanceFromPointToLine ( const Vec3T<T>& P, const Vec3T<T>& lineP0, const Vec3T<T>& lineP1 ) {
		T segment_t;
		return GetDistanceFromPointToLine ( P, lineP0, lineP1, segment_t );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     点Pから線分への最短距離を取得する
	//!	    @brief	 点Pから線分への最短距離を取得する
	//!		@param[in] P 点P
	//!		@param[in] lineP0,1　線分の始点と終点
	//!		@return 点Pから線分への最短距離
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename T>
	T GetDistanceFromPointToLineSegment ( const Vec3T<T> &P, const Vec3T<T> &segmentP0, const Vec3T<T> &segmentP1, OUT T& segment_t, T tolerance ) {
		Vec3T<T> locPos = GetClosestPositionOnLineSegment ( P, segmentP0, segmentP1, segment_t, tolerance );
		return P.distanceTo ( locPos );
	}

	template <typename T>
	T GetDistanceFromPointToLineSegment ( const Vec3T<T>& P, const Vec3T<T>& segmentP0, const Vec3T<T>& segmentP1 ) {
		T segment_t;
		return GetDistanceFromPointToLineSegment ( P, segmentP0, segmentP1, segment_t );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     点PからRayへの最も近い位置とパラメータを取得する
	//!	    @brief	点PからRayへの最も近い位置とパラメータを取得する
	//!		@param[in] ray 
	//!		@param[in] P 点P
	//!		@param[out] rayParam　Rayのスタート位置から取得した位置までの距離
	//!		@return Ray上の最も近い位置
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> GetClosestPositionOnRay ( const Ray3T<T> ray, const Vec3T<T> &P, OUT T &rayParam ) {
		Vec3T<T> val = P - ray.getStartPosition();
		Vec3T<T> dir = ray.getDirection();
		Vec3T<T> start = ray.getStartPosition ();
		rayParam = val.dot ( dir ) / ray.getDirection().dot ( dir );

		Vec3T<T> result;
		if ( T ( 0.0 ) <= rayParam ) {
			result = start + ( dir * rayParam );
		} else {
			rayParam = T ( 0.0 );
			result = start;
		}
		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     2本の無限線の最短距離とレシオを取得する
	//!	    @brief	 2本の無限線の最短距離とレシオを取得する
	//!		@param[in] line1_p0,1  無限線1を表す2点
	//!		@param[in] line2_p0,1  無限線1を表す2点
	//!		@param[out] line1_t　
	//!		@param[out] line2_t　
	//!		@return 2本の無限線の最短距離
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename T>
	T GetDistanceFromLineToLine (
		const Vec3T<T> &line1_p0,
		const Vec3T<T> &line1_p1,
		const Vec3T<T> &line2_p0,
		const Vec3T<T> &line2_p1,
		OUT T &line1_t,
		OUT T &line2_t
	) {
		Vec3T<T> line1P1P0 = line1_p1 - line1_p0;
		Vec3T<T> line2P1P0 = line2_p1 - line2_p0;
		Vec3T<T> line1P0_line2P0 = line1_p0 - line2_p0;
		T dot0 = line1P1P0.dot ( line1P1P0 );
		T dot1 = line1P1P0.dot ( line2P1P0 );
		T dot2 = line2P1P0.dot ( line2P1P0 );
		T dot3 = line1P1P0.dot ( line1P0_line2P0 );
		T dot4 = line2P1P0.dot ( line1P0_line2P0 );
		if ( dot0 == T ( 0.0 ) && dot2 == T ( 0.0 ) ) { 
			line1_t = T ( 0.0 );
			line2_t = T ( 0.0 );
			return line1_p0.distanceTo ( line2_p0 ); 
		}
		if ( dot0 == T ( 0.0 ) ) { 
			line1_t = T ( 0.0 );
			return GetDistanceFromPointToLine ( line1_p0, line2_p0, line2_p1, line2_t );
		}
		if ( dot2 == T ( 0.0 ) ) { 
			line2_t = T ( 0.0 );
			return GetDistanceFromPointToLine ( line2_p0, line1_p0, line1_p1, line1_t );
		}

		T dotVal = dot0 * dot2 - dot1 * dot1;

		if ( dotVal < T ( 0.0000001 ) && dot1 > dot2 ) {
			line1_t = T ( 0.0 );
			line2_t = dot3 / dot1;
		} else {
			line1_t = ( dot1 * dot4 - dot2 * dot3 ) / dotVal;
			line2_t = ( dot0 * dot4 - dot1 * dot3 ) / dotVal;
		}
		Vec3T<T> res = line1P0_line2P0 + ( line1P1P0 * line1_t ) - ( line2P1P0 * line2_t );

		return res.length ();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Rayと無限平面が交差する場合はtrueを返す
	//!	    @brief	 Rayと無限平面が交差する場合はtrueを返す
	//!		@param[in] planePoint　無限平面の中央位置
	//!		@param[in] planeNormal　無限平面の法線方向
	//!		@return 交差する場合はtrue
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Bool TestRayIntersectPlane ( const Ray3T<T> &ray, const Vec3T<T> &planePoint, const Vec3T<T> &planeNormal, T tolerance ) {
		T dot = planeNormal.dot ( ray.getDirection() );
		//Rayがプレーンと並行の場合は接触なし
		if ( Math::AlmostEqual(dot, T(0.0), tolerance ) ) {
			return false;
		}
		Vec3T<T> delta = ray.getStartPosition() - planePoint;
		T normalDot = -planeNormal.dot ( delta );
		T dist = normalDot / dot;
		// 距離が0の場合は接触なし
		if ( dist < tolerance ) {
			return false;
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Rayと三角形が交差する場合はtrueを返す
	//!	    @brief	Rayと三角形が交差する場合はtrueを返す
	//!		@param[in] triP0,1,2　三角形の頂点位置
	//!		@param[in] doubleSide　Rayのマイナス方向も測定する場合はtrue
	//!		@return 交差するか否か
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Bool TestRayIntersectTriangle ( const Ray3T<T> &ray, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, Bool doubleSide, T tolerance ) {
		Vec3T<T> edge1 = triP1 - triP0;
		Vec3T<T> edge2 = triP2 - triP0;
		Vec3T<T> N = edge1.cross ( edge2 );
		T sDen = ray.getDirection().dot ( N );
		T D = triP0.dot ( N );
		if ( ( doubleSide && Math::AlmostEqual ( std::abs ( sDen ), T ( 0.0 ), tolerance ) == false ) ||
			( !doubleSide && sDen < T ( 0.0 ) ) ) {
			T sNum = D - ray.getStartPosition().dot ( N );
			T s = sNum / sDen;
			Vec3T<T> pos = ray.getPositionOnRay ( s );
			Vec3T<T> N2 = pos.almostEqual( ray.getStartPosition (), tolerance ) ? N * T(-1.0): ( pos - ray.getStartPosition() ).unit ();
			if ( doubleSide == false && T ( 0.0 ) < N.dot ( N2 ) ) {
				return false;
			}
			return TestInsideTriangle ( pos, triP0, triP1, triP2, tolerance );
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Rayと無限平面の交差位置を取得する
	//!	    @brief	Rayと無限平面の交差位置を取得する
	//!		@param[in] planePoint　無限平面の中央位置
	//!		@param[in] planeNormal　無限平面の法線方向
	//!		@return 交差位置
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> GetRayIntersectPositionToPlane ( const Ray3T<T> &ray, const Vec3T<T> &planePoint, const Vec3T<T> &planeNormal ) {
		Vec3T<T> delta = ray.getStartPosition() - planePoint;
		T dot = planeNormal.dot ( ray.getDirection() );
		if ( std::abs ( dot ) < Burgess::TOLERANCE ) {/* Rayがプレーンと並行の場合は接触なし */
			return Vec3T<T> ( T(Burgess::F32_INFINITE) );
		}
		T normalDot = -planeNormal.dot ( delta );
		T dist = normalDot / dot;
		if ( dist < Burgess::TOLERANCE ) {/* 距離が0の場合は接触なし*/
			return Vec3T<T> { T(Burgess::F32_INFINITE) };
		}
		return ray.getPositionOnRay ( dist );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Rayと三角形の交差位置を取得する
	//!	    @brief	Rayと三角形の交差位置を取得する
	//!		@param[in] triP0,1,2　三角形の頂点位置
	//!		@param[in] faceSide 0=bothFace,1=frontFace,2=backFace
	//!		@return 交差位置
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T>GetRayIntersectPositionToTriangle ( const Ray3T<T> &ray, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, u8 faceSide, T tolerance ) {
		Vec3T<T> edge1 = triP1 - triP0;
		Vec3T<T> edge2 = triP2 - triP0;
		Vec3T<T> N = edge1.cross ( edge2 );
		T sDen = ray.getDirection().dot ( N );
		T D = triP0.dot ( N );
		Bool side = false;
		switch ( faceSide ){
			case 0: side = sDen != T ( 0.0 ); break;
			case 1: side = sDen < T ( 0.0 ); break;
			case 2: side = sDen > T ( 0.0 ); break;
		}
		if ( side ) {
			T sNum = D - ray.getStartPosition().dot ( N );
			T s = sNum / sDen;
			Vec3T<T> pos = ray.getPositionOnRay ( s );
			Vec3T<T> N2 = ( pos.almostEqual ( ray.getStartPosition (), tolerance ) ) ? N * T ( -1.0 ) : ( pos - ray.getStartPosition () ).unit ();
			switch ( faceSide ){
				case 0:
					if ( Math::AlmostEqual( T ( 0.0 ), N.dot ( N2 ), tolerance ) ){
						return Vec3T<T> { T ( Burgess::F32_INFINITE ) };
					}
					break;
				case 1:
					if ( T ( 0.0 ) < N.dot ( N2 ) ){
						return Vec3T<T> { T ( Burgess::F32_INFINITE ) };
					}
					break;
				case 2:
					if ( T ( 0.0 ) > N.dot ( N2 ) ){
						return Vec3T<T> { T ( Burgess::F32_INFINITE ) };
					}
					break;
			}
			return TestInsideTriangle ( pos, triP0, triP1, triP2, tolerance ) ? pos : Vec3T<T> ( T(Burgess::F32_INFINITE) );
		}
		return Vec3T<T> ( T(Burgess::F32_INFINITE) );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Global空間の２つのMat4の差分(Local値)を取得する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Mat4T<T> GetChildLocalMat4 ( const Mat4T<T> &parentGlobalMat4, const Mat4T<T> &childGlobalMat4 ) {
		return parentGlobalMat4.inverse () * childGlobalMat4;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Global空間のMat4から差分(Local値)を加えたGlobalMatrixを取得する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Mat4T<T> GetChildGlobalMat4 ( const Mat4T<T> &parentGlobalMat4, const Mat4T<T> &childLocalMat4 ) {
		return parentGlobalMat4 * childLocalMat4;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     root空間におけるチェインのLocal-Mat4[]をGlobal値に変換した配列を取得する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	std::vector<Mat4T<T>> ChainToGlobalMat4 ( const Mat4T<T> &rootGMat4, const std::vector<Mat4T<T>> &chainLocalMat4 ){
		u32 cnt = chainLocalMat4.size ();
		std::vector<Mat4T<T>> result ( cnt );
		if ( 0 < cnt ) {
			result[0] = rootGMat4 * chainLocalMat4[0];
			for ( u32 i = 1; i < cnt; ++i ) {
				result[i] = result[i - 1] * chainLocalMat4[i];
			}
		}
		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     root空間におけるチェインのGlobal-Mat4[]をLocal値に変換した配列を取得する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	std::vector<Mat4T<T>> ChainToLocalMat4 ( const Mat4T<T>& rootGMat4, const std::vector<Mat4T<T>>& chainGlobalMat4 ) {
		u32 cnt = chainGlobalMat4.size ();
		std::vector<Mat4T<T>> result ( cnt );
		if ( 0 < cnt ) {
			result[0] = rootGMat4.inverse() * chainGlobalMat4[0];
			for ( u32 i = 1; i < cnt; ++i ) {
				result[i] = chainGlobalMat4[i - 1].inverse() * chainGlobalMat4[i];
			}
		}
		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     root空間におけるチェインの先端ジョイントのGlobal値を取得する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Mat4T<T> GetChainEndJointGlobalMat4 ( const Mat4T<T> &rootGMat4, const std::vector<Mat4T<T>> &chainLocalMat4 ){
		Mat4T<T> result = rootGMat4;
		for ( u32 i = 0; i < chainLocalMat4.size (); ++i ){
			result *= chainLocalMat4[i];
		}
		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Global空間の２つのXfoの差分(Local値)を取得する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	XfoT<T> GetChildLocalXfo ( const XfoT<T> &parentGlobalXfo, const XfoT<T> &childGlobalXfo ){
		return parentGlobalXfo.inverse () * childGlobalXfo;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Global空間のXfoから差分(Local値)を加えたGlobalXfoを取得する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	XfoT<T> GetChildGlobalXfo ( const XfoT<T> &parentGlobalXfo, const XfoT<T> &childLocalXfo ){
		return parentGlobalXfo * childLocalXfo;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     余弦定理で３辺の長さからコサインを算出する
	//!	    @brief	余弦定理で３辺の長さからコサインを算出する
	//!		@param[in] oppositeLength　対線の長さ
	//!		@param[in] hypotenuseLength　斜辺の長さ
	//!		@param[in] adjacentLength　隣辺の長さ
	//!		@return cosine
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	T CosineFormula ( T oppositeLength, T hypotenuseLength, T adjacentLength ){
		return ( SQ ( hypotenuseLength ) + SQ ( adjacentLength ) - SQ ( oppositeLength ) ) / ( T ( 2.0 ) * hypotenuseLength * adjacentLength );
	}

}; // namespace Math

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
QuatT<T> operator*( T val, QuatT<T> quat ) {
	return quat * val;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
VecT<T> operator * ( T val, VecT<T> vec3 ) {
	return vec3 * val;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec2T<T> operator * ( T val, Vec2T<T> vec3 ) {
	return vec3 * val;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> operator * ( T val, Vec3T<T> vec3 ) {
	return vec3 * val;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec4T<T> operator * ( T val, Vec4T<T> vec3 ) {
	return vec3 * val;
}