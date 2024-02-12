
namespace Constraint {
	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    2個のポジション間で位置を制御する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> Position2 ( const Vec3T<T> &pos0, const Vec3T<T> &pos1, T weight ) {
		return pos0.lerp ( pos1, weight );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    ３個のポジション間で位置を制御する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> Position3 ( const Vec3T<T> &pos0, const Vec3T<T> &pos1, const Vec3T<T> &pos2, const Vec3T<T> &weight ) {
		T sum = weight [0] + weight [1] + weight [2];
		Vec3T<T> b = weight;
		if ( Math::AlmostEqual ( sum, T ( 1.0 ) ) ) {
			T r = T ( 1.0 ) / sum;
			b *= r;
		}
		return pos0 * b.x + pos1 * b.y + pos2 * b.z;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    N個のポジション間で位置を制御する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> PositionN ( const std::vector<Vec3T<T>> &positions, const std::vector<T> &weights ) {
		Vec3T<T> result;
		if ( positions.size () != weights.size () ) { return result; }

		T sum = 0;
		for ( u32 i = 0, n = weights.size (); i<n; i++ ) {
			sum += weights [i];
		}
		T r = T ( 1.0 );
		if ( Math::AlmostEqual ( sum, T ( 1.0 ) ) ) { r = T ( 1.0 ) / sum; }
		
		for ( u32 i = 0, n = weights.size (); i<n; i++ ) {
			result += positions [i] * weights[i] * r;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    parentから一定距離を保つように制御する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> Distance ( const Mat4T<T> &parent, const Vec3T<T> &delta, T weight ) {
		Vec3T<T> resultPos = parent * delta;
		return parent.getTranslation ().lerp ( resultPos, weight );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    2個の回転値間で回転を制御する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	QuatT<T> Orientation ( const QuatT<T> &ori0, const QuatT<T> &ori1, T weight, const QuatT<T> &offset ) {
		QuatT<T> result = ori0.slerp ( ori1, weight );
		return offset * result;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    ２つのベクトル（向き、アップベクター）で回転を制御する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	QuatT<T> Direction ( const Vec3T<T> &inPosition, const Vec3T<T> &target, const Vec3T<T> &upVector, u8 axisType, const QuatT<T> &offset ) {
		QuatT<T> result;
		result.setFromDirectionAndUpvector ( target-inPosition, upVector, axisType );
		return offset * result;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    2つのベクトルの角度が一定値以下になるよう制限する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> LimitAngle ( const Vec3T<T> &inVector, const Vec3T<T> &baseVector, T maxDeg ) {
		Vec3T<T> inUVec = inVector.setUnit ();
		Vec3T<T> baseUVec = baseVector.setUnit ();
		T rad = inUVec.angleTo ( baseUVec );
		T maxRad = Math::DegToRad ( maxDeg );
		if ( maxRad < rad ) {
			Vec3T<T> v ( T ( 0.0 ), std::sqrt ( T ( 1.0 ) - SQ ( maxRad ) ), maxRad );
			QuatT<T> q;
			q.setFromDirectionAndUpvector ( baseUVec, inUVec );
			return q.rotateVector ( v ) * inVector.length ();
		} else {
			return inVector;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    特定の軸で反転した座標にする
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Mat4T<T> Mirroring ( const Mat4T<T> &inMat4, u8 mirrorNormal ) {
		XfoT<T> rest ( inMat4 );
		switch ( mirrorNormal ) {
			case 1: /* mirror plane normal Y */
				rest.tr.y *= T ( -1.0 );
				rest.ori.setMirror ( 1 );
				break;
			case 2: /* mirror plane normal Z */
				rest.tr.z *= T ( -1.0 );
				rest.ori.setMirror ( 2 );
				break;
			case 3: /* mirror X */
				rest.tr.x *= T ( -1.0 );
				rest.ori.setMirror2 ( 1 );
				break;
			case 4: /* mirror Y */
				rest.tr.y *= T ( -1.0 );
				rest.ori.setMirror2 ( 2 );
				break;
			case 5: /* mirror Z */
				rest.tr.z *= T ( -1.0 );
				rest.ori.setMirror2 ( 3 );
				break;
			default: /* mirror plane normal X */
				rest.tr.x *= T ( -1.0 );
				rest.ori.setMirror ( 0 );
		}
		return Mat4T<T> ( rest );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    カーブ上で位置を制御する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> Curve ( const CurvesBaseT<T> &curve, u32 curveID, T val, u8 valType ) {
		if (valType == 0) { /* curve param */
			return curve.getPositionAtParam ( curveID, val );
		}
		else { /* length ratio */
			return curve.getPositionAtLengthRatio ( curveID, val );
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	    カーブ上で位置と回転を制御する
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Mat4T<T> Curve ( const CurvesBaseT<T> &curve, u32 curveID, T val, u8 valType, Vec3T<T> &upVector, u8 axisType ) {
		XfoT<T> Xfo;
		Xfo.tr = Curve ( curve, curveID, val, valType );

		Vec3T<T> dir;
		if (valType == 0) { /* curve param */
			dir = curve.getTangentAtParam ( curveID, val );
		}
		else { /* length ratio */
			dir = curve.getTangentAtLengthRatio ( curveID, val );
		}
		Xfo.ori.setFromDirectionAndUpvector ( dir, upVector, axisType );

		return Mat4T<T> ( Xfo );
	}
}