//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void SlidingPivotT<T>::setMirror ( Bool _mirror ) {
	this->mirror = _mirror;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    initialize
//!	    @brief initialize
//!		@param[in] pivotBase 基本ピボット
//!		@param[in] backPivot 後方側ピボット
//!		@param[in] frontPivot 前方側ピボット
//!		@param[in] leftPivot　左側ピボット
//!		@param[in] rightPivot 右側ピボット
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void SlidingPivotT<T>::initialize(
	const Mat4T<T>& pivotBase,
	const Mat4T<T>& backPivot,
	const Mat4T<T>& frontPivot,
	const Mat4T<T>& leftPivot,
	const Mat4T<T>& rightPivot
) {
	this->pivots[0] = pivotBase;
	this->pivots[1] = backPivot;
	this->pivots[2] = frontPivot;
	this->pivots[3] = leftPivot;
	this->pivots[4] = rightPivot;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Sliding Pivot
//!	    @brief Sliding Pivot
//!		@param[in] pitchAngle ピッチ値（Degree）
//!		@param[in] rollAngle ロール値（Degree）
//!		@return pivotBaseを複数ピボットで補正した結果
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Mat4T<T> SlidingPivotT<T>::solve (
	T pitchAngle,
	T rollAngle
	) const 
{
	XfoT<T> pivotBaseXfo	= XfoT<T> (this->pivots[0]);
	XfoT<T> backPivotXfo	= XfoT<T> (this->pivots[1]);
	XfoT<T> frontPivotXfo	= XfoT<T> (this->pivots[2]);
	XfoT<T> leftPivotXfo	= XfoT<T> (this->pivots[3]);
	XfoT<T> rightPivotXfo	= XfoT<T> (this->pivots[4]);

	XfoT<T> frontDeltaXfo	= backPivotXfo.inverse () * frontPivotXfo;
	XfoT<T> leftDeltaXfo	= frontPivotXfo.inverse () * leftPivotXfo;
	XfoT<T> rightDeltaXfo	= leftPivotXfo.inverse () * rightPivotXfo;
	XfoT<T> footAllDeltaXfo = rightPivotXfo.inverse () * pivotBaseXfo;

	// Back Rock
	XfoT<T> backPivotOutXfo( backPivotXfo );
	Quat backPivotOutOri;
	T backPitchAngle = Math::Rescale ( pitchAngle, T ( -1.0 ), T ( 0.0 ), T ( 1.0 ), T ( 0.0 ) );
	T backPivotRockAngle = Math::DegToRad ( Math::Clamp ( backPitchAngle, T(0.0), T(1.0)) * -90 );
	backPivotOutOri.setFromAxisAndAngle ( Vec3 ( 1, 0, 0 ), backPivotRockAngle );
	backPivotOutXfo.ori = backPivotXfo.ori * backPivotOutOri;

	// Front Rock
	XfoT<T> frontPivotOutXfo = backPivotOutXfo * frontDeltaXfo;
	Quat frontPivotOutOri;
	T frontPitchAngle = Math::Rescale ( pitchAngle, T ( 0.0 ), T ( 1.0 ), T ( 0.0 ), T ( 1.0 ) );
	T frontPivotRockAngle = Math::DegToRad ( Math::Clamp ( frontPitchAngle, T(0.0), T(1.0)) * 90 );
	frontPivotOutOri.setFromAxisAndAngle ( Vec3 ( 1, 0, 0 ), frontPivotRockAngle );
	frontPivotOutXfo.ori = frontPivotOutXfo.ori * frontPivotOutOri;

	// Left Bank
	T leftBankAngleMult = -1.0;
	if ( this->mirror ) {
		leftBankAngleMult = 1.0;
	}

	XfoT<T> leftBankOutXfo ( leftDeltaXfo );
	Quat leftBankOutOri;
	T leftRollAngle = Math::Rescale ( rollAngle, T ( -1.0 ), T ( 0.0 ), T ( 1.0 ), T ( 0.0 ) );
	T leftBankAngle = Math::DegToRad ( Math::Clamp ( leftRollAngle , T(0.0), T(1.0) ) * ( 90 * leftBankAngleMult ) );
	leftBankOutOri.setFromAxisAndAngle ( Vec3 ( 0, 0, 1 ), leftBankAngle );
	leftBankOutXfo.ori = leftDeltaXfo.ori * leftBankOutOri;

	// Right Bank
	T rightBankAngleMult = 1.0;
	if ( this->mirror ) {
		rightBankAngleMult = -1.0;
	}

	XfoT<T> rightBankOutXfo ( rightDeltaXfo );
	Quat rightBankOutOri;
	T rightRollAngle = Math::Rescale ( rollAngle, T ( 0.0 ), T ( 1.0 ), T ( 0.0 ), T ( 1.0 ) );
	T rightBankAngle = Math::DegToRad ( Math::Clamp ( rightRollAngle, T(0.0), T(1.0) ) * ( 90 * rightBankAngleMult ) );
	rightBankOutOri.setFromAxisAndAngle ( Vec3 ( 0, 0, 1 ), rightBankAngle );
	rightBankOutXfo.ori = rightDeltaXfo.ori * rightBankOutOri;

	return ( frontPivotOutXfo * leftBankOutXfo * rightBankOutXfo * footAllDeltaXfo ).toMat4 ();
}