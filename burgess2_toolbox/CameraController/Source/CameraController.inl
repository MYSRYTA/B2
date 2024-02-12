//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    カメラタイプをフリーカメラにする
//!	    @brief カメラタイプをフリーカメラにする
//!		@param[in] _freeCamera true = free camera, false = interest camera
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CameraControllerT<T>::setFreeCamera ( Bool _freeCamera ) {
	this->freeCamera = _freeCamera;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     カメラタイプがフリーカメラの場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Bool CameraControllerT<T>::isFreeCamera ()const {
	return this->freeCamera;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     手振れシミュレーションで使用するメンバー変数を初期化する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CameraControllerT<T>::reset () {
	this->hShake = HandShakeT<T> ();
	this->wShake = WalkShakeT<T> ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     フリーカメラが内包している注視点からカメラまでの距離をスケールする
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CameraControllerT<T>::setOffsetScale ( T s ) {
	this->interestOffset0 *= s;
	this->interestOffset1 *= s;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    各種処理を実行し、カメラのポジション・注視点・アップベクターを取得する
//!	    @brief 各種処理を実行し、カメラのポジション・注視点・アップベクターを取得する
//!		@param[out] outCameraPosition カメラのポジション Local-Vec3
//!		@param[out] outInterestPosition カメラの注視点 Local-Vec3
//!		@param[out] outUpvector カメラのアップベクター Local-Vec3
//!		@param[in] cameraGlobalPose カメラのコントローラ Global-Mat4
//!		@param[in] InterestGlobalPosition 注視点の Global-Mat4
//!		@param[in] time カレントフレーム
//!		@param[in] roll カメラロール
//!		@param[in] handShake 手振れノイズのON/OFF
//!		@param[in] handShakePow 手振れの強さ（横・縦方向）
//!		@param[in] handShakeTimeScale 手振れのタイムスケール
//!		@param[in] vibration 振動ノイズのON/OFF
//!		@param[in] vibrationPow 振動の強さ（横・縦方向）
//!		@param[in] vibrationTimeScale 振動のタイムスケール
//!		@param[in] walkShake 歩きノイズのON/OFF
//!		@param[in] walkShakeCameraPow 歩きのカメラのノイズの強さ（横・縦方向）
//!		@param[in] walkShakeInterestPow 歩きの注視点のノイズの強さ（横・縦方向）
//!		@param[in] walkShakeRollPow 歩きのカメラロールのノイズの強さ
//!		@param[in] walkShakeTimeScale 歩きノイズのタイムスケール
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void CameraControllerT<T>::solve (
	OUT Vec3T<T> &outCameraPosition,
	OUT Vec3T<T> &outInterestPosition,
	OUT Vec3T<T> &outUpvector,
	const Mat4T<T> &cameraGlobalPose,
	const Vec3T<T> &InterestGlobalPosition,
	const T time,
	const T roll,
	const Bool handShake,
	const Vec2T<T> &handShakePow,
	const T handShakeTimeScale,
	const Bool vibration,
	const Vec2T<T> &vibrationPow,
	const T vibrationTimeScale,
	const Bool walkShake,
	const Vec2T<T> &walkShakeCameraPow,
	const Vec2T<T> &walkShakeInterestPow,
	const T walkShakeRollPow,
	const T walkShakeTimeScale ) 
{
	QuatT<T> cameraGlobalRot = cameraGlobalPose.getRotation ();
	// ノイズ生成用のカメラMat4を作成
	Vec3T<T> dir;// カメラ注視点
	if ( this->freeCamera ) {
		dir = cameraGlobalRot.rotateVector ( this->interestOffset0 ); // -Z軸にダミーの注視点を生成
	} else {
		dir = InterestGlobalPosition - cameraGlobalPose.getTranslation (); // 注視点コントローラの位置を使用
	}

	QuatT<T> rot;// カメラの回転値
	rot.setFromDirectionAndUpvector ( dir, cameraGlobalRot.rotateVector( Vec3T<T> { 0.0, 1.0, 0.0 } ));
	rot = rot.addAngle ( Deg3T<T>{0.0, 0.0, roll} ); // カメラロール値

	Mat4T<T> cameraPose;// ノイズ生成用のカメラMat4
	cameraPose.setTranslation ( cameraGlobalPose.getTranslation () );
	cameraPose.setUpperLeft ( Mat3T<T> ( rot ) );
	
	// 歩き手振れノイズ
	this->wShake.solve (
		outCameraPosition,
		outInterestPosition,
		outUpvector,
		walkShake,
		time,
		cameraPose,
		cameraPose * this->interestOffset1,
		walkShakeCameraPow,
		walkShakeInterestPow,
		walkShakeRollPow,
		walkShakeTimeScale );
	// 手振れ/振動ノイズを生成
	Vec2T<T> addShake;
	if ( handShake ) {
		// 手振れノイズ
		this->hShake.solve ( 
			addShake,
			time, 
			cameraPose.getTranslation(),
			handShakePow, 
			handShakeTimeScale );
	}
	if ( vibration ) {
		// 振動ノイズ
		Vec3T<T> vib = Noise::Vibration ( time, Vec3T<T> ( vibrationPow.x, vibrationPow.y, T ( 0.0 ) ), vibrationTimeScale );
		addShake.x += vib.x;
		addShake.y += vib.y;
	}
	// 手振れ/振動ノイズをカメラの注視点とアップベクターに加算
	outInterestPosition = rot.rotateVector ( Vec3T<T>{ outInterestPosition.x + addShake.x, outInterestPosition.y + addShake.y, T ( 10.0 ) } );
	outUpvector.x += addShake.x;
	outUpvector.y += addShake.y;
	// ローカル値に変換
	rot = cameraGlobalRot.inverse ();
	outInterestPosition = rot.rotateVector ( outInterestPosition );
	outUpvector = rot.rotateVector ( outUpvector );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    各種処理を実行し、カメラのLocal-Mat4を取得する
//!	    @brief 各種処理を実行し、カメラのLocal-Mat4を取得する
//!		@param[in] cameraGlobalPose カメラのコントローラ Global-Mat4
//!		@param[in] InterestGlobalPosition 注視点の Global-Mat4
//!		@param[in] time カレントフレーム
//!		@param[in] roll カメラロール
//!		@param[in] handShake 手振れノイズのON/OFF
//!		@param[in] handShakePow 手振れの強さ（横・縦方向）
//!		@param[in] handShakeTimeScale 手振れのタイムスケール
//!		@param[in] vibration 振動ノイズのON/OFF
//!		@param[in] vibrationPow 振動の強さ（横・縦方向）
//!		@param[in] vibrationTimeScale 振動のタイムスケール
//!		@param[in] walkShake 歩きノイズのON/OFF
//!		@param[in] walkShakeCameraPow 歩きのカメラのノイズの強さ（横・縦方向）
//!		@param[in] walkShakeInterestPow 歩きの注視点のノイズの強さ（横・縦方向）
//!		@param[in] walkShakeRollPow 歩きのカメラロールのノイズの強さ
//!		@param[in] walkShakeTimeScale 歩きノイズのタイムスケール
//!		@return カメラのポーズ Local-Mat4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat4T<T> CameraControllerT<T>::solve (
	const Mat4T<T> &cameraGlobalPose,
	const Vec3T<T> &InterestGlobalPosition,
	const T time,
	const T roll,
	const Bool handShake,
	const Vec2T<T> &handShakePow,
	const T handShakeTimeScale,
	const Bool vibration,
	const Vec2T<T> &vibrationPow,
	const T vibrationTimeScale,
	const Bool walkShake,
	const Vec2T<T> &walkShakeCameraPow,
	const Vec2T<T> &walkShakeInterestPow,
	const T walkShakeRollPow,
	const T walkShakeTimeScale ) {
	Vec3T<T> outCameraPosition, outInterestPosition, outUpvector;
	this->solve ( outCameraPosition, outInterestPosition, outUpvector,
		cameraGlobalPose,
		InterestGlobalPosition,
		time,
		roll,
		handShake,
		handShakePow,
		handShakeTimeScale,
		vibration,
		vibrationPow,
		vibrationTimeScale,
		walkShake,
		walkShakeCameraPow,
		walkShakeInterestPow,
		walkShakeRollPow,
		walkShakeTimeScale );

	Mat4T<T> result;
	QuatT<T> rot;
	rot.setFromDirectionAndUpvector ( outCameraPosition - outInterestPosition , outUpvector - outCameraPosition );
	result.setUpperLeft ( Mat3T<T> ( rot ) );
	result.setTranslation ( outCameraPosition );
	return result;
}