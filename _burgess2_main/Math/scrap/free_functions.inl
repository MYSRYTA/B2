namespace Math {
	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     �_P���O�p�`�̖ʏ�ɑ��݂���ꍇ��true��Ԃ�
	//!	    @brief	�_P���O�p�`�̖ʏ�ɑ��݂���ꍇ��true��Ԃ�
	//!		@param[in] P�@�_P
	//!		@param[in] triP0,1,2�@�O�p�`�̒��_�ʒu
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Bool TestInsideTriangle ( const Vec3T<T> &P, const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2, T tolerance ) {
		Vec3T<T> p0p2 = triP0 - triP2;
		Vec3T<T> p1p0 = triP1 - triP0;
		Vec3T<T> p2p1 = triP2 - triP1;
		Vec3T<T> pp0 = P - triP0;
		Vec3T<T> pp1 = P - triP1;
		Vec3T<T> pp2 = P - triP2;

		// �_P���O�p�`�̒��_��ɑ��݂���ꍇ��true��Ԃ��ďI�� 
		if ( P.almostEqual ( triP0, tolerance ) || P.almostEqual ( triP1, tolerance ) || P.almostEqual ( triP2, tolerance ) ) {
			return true;
		}

		// �_P���O�p�`�̃G�b�W��ɑ��݂���ꍇ��true��Ԃ��ďI��
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

		// �_P���O�p�`���ɑ��݂����true��Ԃ� 
		if ( T(1.0) - tolerance < d_ab && T( 1.0 ) - tolerance < d_bc ) {
			return true;
		}
		return false;		
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     �O�p�`���ɂ����ē_P�̈ʒu��\���E�F�C�g�l���擾����
	//!	    @brief	�O�p�`���ɂ����ē_P�̈ʒu��\���E�F�C�g�l���擾����<br>
	//!		@param[in] P �_P
	//!		@param[in] triP0,1,2�@�O�p�`�̒��_�ʒu
	//!		@return Vec3��x~z�͊e���_0~2�ւ̃E�F�C�g�l ( �e���_�̈ʒux�E�F�C�g�̍��v���_P�̍��W�ɂȂ� )
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
	//!	     �_P����O�p�`�ɑ΂��čł��߂��ʒu���擾����
	//!	    @brief	�_P����O�p�`�ɑ΂��čł��߂��ʒu���擾����
	//!		@param[in] P �_P
	//!		@param[in] triP0,1,2�@�O�p�`�̒��_�ʒu
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
	//!	    �O�p�`�̖@�����擾����
	//!	    @brief	�O�p�`�̖@�����擾����
	//!		@param[in] triP0,1,2�@�O�p�`�̒��_�ʒu
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> GetTriangleNormal ( const Vec3T<T> &triP0, const Vec3T<T> &triP1, const Vec3T<T> &triP2 ) {
		Vec3T<T> edge1 = triP1 - triP0;
		Vec3T<T> edge2 = triP2 - triP0;
		return edge1.cross ( edge2 ).unit();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    �O�p�`�̖ʐς��擾����
	//!	    @brief	�O�p�`�̖ʐς��擾����
	//!		@param[in] triP0,1,2�@�O�p�`�̒��_�ʒu
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
	//!	     startPosition����_�Q�ւ̍ł��߂��_�̔z��ԍ����擾����
	//!	    @brief	startPosition����_�Q�ւ̍ł��߂��_�̔z��ԍ����擾����
	//!		@param[in] points �_�Q
	//!		@param[in] startPosition �����X�^�[�g�n�_
	//!		@param[in] maxDistance ��������
	//!		@return �ł��߂��_�̔z��ԍ� (maxDistance���œ_��������Ȃ������ꍇ��u32��������Ԃ�)
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
	//!	     �X�t�B�A���ɑ��݂���S�Ă̓_�̔z��ԍ����擾����
	//!	    @brief	�X�t�B�A���ɑ��݂���S�Ă̓_�̔z��ԍ����擾����
	//!		@param[in] points �_�Q
	//!		@param[in] centerPosition �X�t�B�A�̒��S�ʒu
	//!		@param[in] sphereRadius �X�t�B�A�̔��a
	//!		@param[in] sort centerPosition����߂����Ƀ\�[�g����ꍇ��true
	//!		@return �_�̔z��ԍ�
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
	//!	     �_P���疳�����ւ̍ł��߂��ʒu���擾����
	//!	    @brief	�_P���疳�����ւ̍ł��߂��ʒu�ƃ��V�I���擾����
	//!		@param[in] P �_P
	//!		@param[in] lineP0,1�@��������\��2�_
	//!		@return �_P���疳�����ւ̍ł��߂��ʒu
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
	//!	     �_P��������ւ̍ł��߂��ʒu�ƃ��V�I���擾����
	//!	    @brief	�_P��������ւ̍ł��߂��ʒu�ƃ��V�I���擾����
	//!		@param[in] P �_P
	//!		@param[in] lineP0,1�@�����̎n�_�ƏI�_
	//!		@param[out] segment_t�@�擾�����ʒu��P0~P1�܂ł�0.0~1.0�Ƃ������V�I�ŕԂ�
	//!		@return �_P��������ւ̍ł��߂��ʒu
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
	//!	     �_P���疳�����ւ̍ŒZ�������擾����
	//!	    @brief	�_P���疳�����ւ̍ŒZ�������擾����
	//!		@param[in] P �_P
	//!		@param[in] lineP0,1�@��������\��2�_
	//!		@return �_P���疳�����ւ̍ŒZ����
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
	//!	     �_P��������ւ̍ŒZ�������擾����
	//!	    @brief	 �_P��������ւ̍ŒZ�������擾����
	//!		@param[in] P �_P
	//!		@param[in] lineP0,1�@�����̎n�_�ƏI�_
	//!		@return �_P��������ւ̍ŒZ����
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
	//!	     �_P����Ray�ւ̍ł��߂��ʒu�ƃp�����[�^���擾����
	//!	    @brief	�_P����Ray�ւ̍ł��߂��ʒu�ƃp�����[�^���擾����
	//!		@param[in] ray 
	//!		@param[in] P �_P
	//!		@param[out] rayParam�@Ray�̃X�^�[�g�ʒu����擾�����ʒu�܂ł̋���
	//!		@return Ray��̍ł��߂��ʒu
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
	//!	     2�{�̖������̍ŒZ�����ƃ��V�I���擾����
	//!	    @brief	 2�{�̖������̍ŒZ�����ƃ��V�I���擾����
	//!		@param[in] line1_p0,1  ������1��\��2�_
	//!		@param[in] line2_p0,1  ������1��\��2�_
	//!		@param[out] line1_t�@
	//!		@param[out] line2_t�@
	//!		@return 2�{�̖������̍ŒZ����
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
	//!	     Ray�Ɩ������ʂ���������ꍇ��true��Ԃ�
	//!	    @brief	 Ray�Ɩ������ʂ���������ꍇ��true��Ԃ�
	//!		@param[in] planePoint�@�������ʂ̒����ʒu
	//!		@param[in] planeNormal�@�������ʂ̖@������
	//!		@return ��������ꍇ��true
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Bool TestRayIntersectPlane ( const Ray3T<T> &ray, const Vec3T<T> &planePoint, const Vec3T<T> &planeNormal, T tolerance ) {
		T dot = planeNormal.dot ( ray.getDirection() );
		//Ray���v���[���ƕ��s�̏ꍇ�͐ڐG�Ȃ�
		if ( Math::AlmostEqual(dot, T(0.0), tolerance ) ) {
			return false;
		}
		Vec3T<T> delta = ray.getStartPosition() - planePoint;
		T normalDot = -planeNormal.dot ( delta );
		T dist = normalDot / dot;
		// ������0�̏ꍇ�͐ڐG�Ȃ�
		if ( dist < tolerance ) {
			return false;
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Ray�ƎO�p�`����������ꍇ��true��Ԃ�
	//!	    @brief	Ray�ƎO�p�`����������ꍇ��true��Ԃ�
	//!		@param[in] triP0,1,2�@�O�p�`�̒��_�ʒu
	//!		@param[in] doubleSide�@Ray�̃}�C�i�X���������肷��ꍇ��true
	//!		@return �������邩�ۂ�
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
	//!	     Ray�Ɩ������ʂ̌����ʒu���擾����
	//!	    @brief	Ray�Ɩ������ʂ̌����ʒu���擾����
	//!		@param[in] planePoint�@�������ʂ̒����ʒu
	//!		@param[in] planeNormal�@�������ʂ̖@������
	//!		@return �����ʒu
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> GetRayIntersectPositionToPlane ( const Ray3T<T> &ray, const Vec3T<T> &planePoint, const Vec3T<T> &planeNormal ) {
		Vec3T<T> delta = ray.getStartPosition() - planePoint;
		T dot = planeNormal.dot ( ray.getDirection() );
		if ( std::abs ( dot ) < Burgess::TOLERANCE ) {/* Ray���v���[���ƕ��s�̏ꍇ�͐ڐG�Ȃ� */
			return Vec3T<T> ( T(Burgess::F32_INFINITE) );
		}
		T normalDot = -planeNormal.dot ( delta );
		T dist = normalDot / dot;
		if ( dist < Burgess::TOLERANCE ) {/* ������0�̏ꍇ�͐ڐG�Ȃ�*/
			return Vec3T<T> { T(Burgess::F32_INFINITE) };
		}
		return ray.getPositionOnRay ( dist );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Ray�ƎO�p�`�̌����ʒu���擾����
	//!	    @brief	Ray�ƎO�p�`�̌����ʒu���擾����
	//!		@param[in] triP0,1,2�@�O�p�`�̒��_�ʒu
	//!		@param[in] faceSide 0=bothFace,1=frontFace,2=backFace
	//!		@return �����ʒu
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
	//!	     Global��Ԃ̂Q��Mat4�̍���(Local�l)���擾����
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Mat4T<T> GetChildLocalMat4 ( const Mat4T<T> &parentGlobalMat4, const Mat4T<T> &childGlobalMat4 ) {
		return parentGlobalMat4.inverse () * childGlobalMat4;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Global��Ԃ�Mat4���獷��(Local�l)��������GlobalMatrix���擾����
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Mat4T<T> GetChildGlobalMat4 ( const Mat4T<T> &parentGlobalMat4, const Mat4T<T> &childLocalMat4 ) {
		return parentGlobalMat4 * childLocalMat4;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     root��Ԃɂ�����`�F�C����Local-Mat4[]��Global�l�ɕϊ������z����擾����
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
	//!	     root��Ԃɂ�����`�F�C����Global-Mat4[]��Local�l�ɕϊ������z����擾����
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
	//!	     root��Ԃɂ�����`�F�C���̐�[�W���C���g��Global�l���擾����
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
	//!	     Global��Ԃ̂Q��Xfo�̍���(Local�l)���擾����
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	XfoT<T> GetChildLocalXfo ( const XfoT<T> &parentGlobalXfo, const XfoT<T> &childGlobalXfo ){
		return parentGlobalXfo.inverse () * childGlobalXfo;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     Global��Ԃ�Xfo���獷��(Local�l)��������GlobalXfo���擾����
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	XfoT<T> GetChildGlobalXfo ( const XfoT<T> &parentGlobalXfo, const XfoT<T> &childLocalXfo ){
		return parentGlobalXfo * childLocalXfo;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	     �]���藝�łR�ӂ̒�������R�T�C�����Z�o����
	//!	    @brief	�]���藝�łR�ӂ̒�������R�T�C�����Z�o����
	//!		@param[in] oppositeLength�@�ΐ��̒���
	//!		@param[in] hypotenuseLength�@�Εӂ̒���
	//!		@param[in] adjacentLength�@�וӂ̒���
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