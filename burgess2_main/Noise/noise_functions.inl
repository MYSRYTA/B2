namespace Burgess2NoiseFn {
	///	cond
	template<typename T>
	T PerlinFade ( T t ){
		return t * t * t * (t * (t * T ( 6.0 ) - T ( 15.0 )) + T ( 10.0 ));
	}

	template<typename T>
	T PerlinLerp ( T t, T a, T b ){
		return a + t * (b - a);
	}

	template<typename T>
	T PerlinGrad ( s32 hash, T x, T y, T z ){
		s32 h = hash & 15;
		T	u = h < 8 ? x : y,
			v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
	/// encond

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	   パーリンノイズ：コード参照元　https://mrl.nyu.edu/~perlin/noise/
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	T Perlin ( T x, T y, T z ){
		const std::vector<T> p = { 151, 160, 137, 91, 90, 15, 131, 13, 201, 
			95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
			190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 
			57, 177, 33, 88, 237, 149, 56, 87, 174, 20,	125, 136, 171, 168, 68, 175, 74, 165, 71, 
			134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 
			105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 
			76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 
			198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 
			85,	212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183,	170, 213, 119, 
			248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 
			19, 98, 108, 110, 79, 113, 224,	232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 
			242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 
			49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 
			254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 
			156, 180, 151, 160,	137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
			36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,	120, 234, 75, 0, 26, 
			197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 
			125, 136, 171, 168, 68,	175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 
			111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 
			65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187,	208, 89, 18, 169, 200, 196, 135, 130, 
			116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 
			202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 
			42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163,	70, 221, 153, 101, 155, 167, 43, 
			172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 
			246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51,	145, 235, 
			249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 
			45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 
			78, 66, 215, 61, 156, 180 };

		s32 X = s32 ( std::floor ( x ) ) & 255;
		s32 Y = s32 ( std::floor ( y ) ) & 255;
		s32 Z = s32 ( std::floor ( z ) ) & 255;

		x -= std::floor ( x );
		y -= std::floor ( y );
		z -= std::floor ( z );

		T u = Burgess2NoiseFn::PerlinFade ( x );
		T v = Burgess2NoiseFn::PerlinFade ( y );
		T w = Burgess2NoiseFn::PerlinFade ( z );

		s32 A  = p[X] + Y,
			AA = p[A] + Z,
			AB = p[A + 1] + Z,
			B  = p[X + 1] + Y,
			BA = p[B] + Z,
			BB = p[B + 1] + Z;

		return
			Burgess2NoiseFn::PerlinLerp 
			( 
				w, Burgess2NoiseFn::PerlinLerp 
				( 
					v, Burgess2NoiseFn::PerlinLerp 
					( 
						u, Burgess2NoiseFn::PerlinGrad ( p[AA], x, y, z ), Burgess2NoiseFn::PerlinGrad ( p[BA], x - T(1.0), y, z ) 
					),
					Burgess2NoiseFn::PerlinLerp 
					( 
						u, Burgess2NoiseFn::PerlinGrad ( p[AB], x, y - T(1.0), z ), Burgess2NoiseFn::PerlinGrad ( p[BB], x - T ( 1.0 ), y - T ( 1.0 ), z ) 
					) 
				),
				Burgess2NoiseFn::PerlinLerp 
				( 
					v, Burgess2NoiseFn::PerlinLerp 
						( 
							u, 
							Burgess2NoiseFn::PerlinGrad ( p[AA + 1], x, y, z - T ( 1.0 ) ), 
							Burgess2NoiseFn::PerlinGrad ( p[BA + 1], x - T ( 1.0 ), y, z - T ( 1.0 ) ) 
						),
						Burgess2NoiseFn::PerlinLerp 
						( 
							u, 
							Burgess2NoiseFn::PerlinGrad ( p[AB + 1], x, y - T ( 1.0 ), z - T ( 1.0 ) ),
							Burgess2NoiseFn::PerlinGrad ( p[BB + 1], x - T ( 1.0 ), y - T ( 1.0 ), z - T ( 1.0 ) ) 
						) 
				) 
			);
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	   タービュランスノイズ（パーリンノイズを使用）
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> PerlinTurbulent ( T time,
		const Vec3T<T> &pos,
		const Vec3T<T> &center,
		const Vec3T<T> &amplitude,
		const Vec3T<T> &timeFrequency,
		const Vec3T<T> &spaceFrequency,
		Bool apply )
	{
		Vec3T<T> v = center + Vec3T<T> { time } * timeFrequency + spaceFrequency * pos;

		Vec3T<T> result;
		result.x() = amplitude.x() * Burgess2NoiseFn::Perlin ( v.x(), v.z(), v.y() );
		result.y() = amplitude.y() * Burgess2NoiseFn::Perlin ( v.y(), v.x(), v.z() );
		result.z() = amplitude.z() * Burgess2NoiseFn::Perlin ( v.z(), v.y(), v.x() );

		return apply ? pos + result : result;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//!	   振動ノイズ
	//------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Vec3T<T> Vibration ( T time, const Vec3T<T> &vibrationPow, T timeScale ){
		T sTime = time * timeScale;
		T sinTime = std::sin ( sTime );

		Vec3T<T> result;
		result.x() = (T ( 15.0 ) * vibrationPow.x() * sinTime + T ( 5.0 ) * vibrationPow.x() * std::sin ( T(1.0) * sTime ) + T ( 3.0 ) * vibrationPow.x() * sinTime + T ( 10.0 ) * vibrationPow.x() * sinTime);
		result.y() = (T ( 15.0 ) * vibrationPow.y() * sinTime + T ( 5.0 ) * vibrationPow.y() * std::sin ( T(1.0) * sTime ) + T ( 3.0 ) * vibrationPow.y() * sinTime + T ( 10.0 ) * vibrationPow.y() * sinTime);
		result.z() = (T ( 15.0 ) * vibrationPow.z() * sinTime + T ( 5.0 ) * vibrationPow.z() * std::sin ( T(1.0) * sTime ) + T ( 3.0 ) * vibrationPow.z() * sinTime + T ( 10.0 ) * vibrationPow.z() * sinTime);
		
		return result * vibrationPow * T ( 0.0001 );
	}
}