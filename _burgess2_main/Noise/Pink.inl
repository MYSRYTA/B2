//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     初期化
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
PinkT<T>::PinkT () {
	this->Z.resize ( 16 );
	for (s8 i = 0; i<16; ++i) {
		this->Z[i] = T(0.0);
	}

	this->K.resize ( 16 );
	this->K[15] = T(0.5);
	for (s8 i = 15; 0 < i; --i) {
		this->K[i - 1] = this->K[i] * T(0.25);
	}

	f64 q = Burgess2MathFn::RandomS32 ( 15, 108, s32 ( 0 ), s32 ( 100 ) ) & 1 ? f64 ( 1.0 ) : f64 ( -1.0 );;
	for (s8 i = 0; i < 16; ++i) {
		this->Z[i] = q * this->K[i] + ( 1.0 - this->K[i]) * this->Z[i];
		q = (q + this->Z[i]) * T(0.5);
	}
	this->t = T(0.75) * q + T(0.25) * this->t;
	this->val = this->t;
	this->previousVal = T(0.0);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ピンクノイズを生成する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T PinkT<T>::solveNoise ( u32 seed, T time, T strength, T timeScale ) {
	f64 q = Burgess2MathFn::RandomS32 ( seed, time * timeScale, s32 ( 0 ), s32 ( 100 ) ) & 1 ? f64 ( 1.0 ) : f64 ( -1.0 );
	for (s8 i = 0; i<16; ++i) {
		this->Z[i] = q * this->K[i] + ( T(1.0) - this->K[i]) * this->Z[i];
		q = (q + this->Z[i]) * T(0.5);
	}
	this->t = q * T(0.75) + this->t * T(0.25);
	return this->t * f64(strength) * T(1000.0);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     ピンクノイズを用いた振動ノイズを生成する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T PinkT<T>::applyNoise ( u32 seed, T time, T strength, T timeScale ) {
	f64 q = Burgess2MathFn::RandomS32 ( seed, time * timeScale, s32 ( 0 ), s32 ( 99 ) ) & 1 ? f64 ( 1.0 ) : f64 ( -1.0 );
	for (s8 i = 0; i<16; ++i) {
		this->Z[i] = q * this->K[i] + ( T(1.0) - this->K[i]) * this->Z[i];
		q = (q + this->Z[i]) * T(0.5);
	}

	this->t = q * T(0.75) + this->t * T(0.25);
	f64 v0 = this->t * f64(strength) * T(1000.0);
	if (Burgess2MathFn::AlmostEqual ( this->previousVal, v0 ) == false) {
		v0 = Burgess2MathFn::Lerp ( this->previousVal, v0, T(0.4) );
	}

	this->previousVal = v0 - this->val;
	return this->previousVal;
}