//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> BezierCurvesT<T>::core(const Vec3T<T>& p0, const Vec3T<T>& p1, const Vec3T<T>& p2, T t){
	// cubic
	return	std::pow(T(1.0) - t, 3) * p0 +
			3 * std::pow(T(1.0) - t, 2) * t * p1 +
			3 * (T(1.0) - t) * std::pow(t, 2) * p2 +
			std::pow(t, 3) * p3;

	// quadric
	//return	std::pow(T(1.0) - t, 2) * p0 +
	//		2 * (T(1.0) - t) * t * p1 +
	//		std::pow(t, 2) * p2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		rational Bezier
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
Vec3T<T> BezierCurvesT<T>::core(const Vec3T<T>& p0, const Vec3T<T>& p1, const Vec3T<T>& p2, T t, const std::vector<T>& weight){
	if (weight.size() < 4){ return Vec3T<T>(); }

	T w0 = weight[0] * std::pow(T(1.0) - t, 3);
	T w1 = weight[1] * 3 * t * std::pow(T(1.0) - t, 2);
	T w2 = weight[2] * 3 * std::pow(t, 2) * T(1.0) - t;
	T w3 = weight[3] * std::pow(t, 3);
	return(p0 * w0 + p1 * w1 + p2 * w2 + p3 * w3) / (w0 + w1 + w2 + w3);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		BezierCurveを追加する
//!
//!		ハンドルの頂点位置も含めた配列からベジェカーブを作成する<br>
//!		@param[in] points セグメントのポイント頂点とハンドル頂点を含む配列データ　[p0, h0, h1, p1, h2, h3, p2,...](form=Loopの場合でも配列の最後にp0は含めない）
//!		@param[in] form 0=open, 1=close, 2=periodic
//!		@param[in] sample セグメント内をサンプリングする数（ベジェカーブをBSplineで再現する精度）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 BezierCurvesT<T>::add(const std::vector<Vec3T<T>>& points, u8 form, u32 sample){
	assert(form < 3);
	if (form == 0){
		assert(4 <= points.size());
	} else{
		assert(6 <= points.size());
	}

	T e = T(1.0) / sample;

	int sm = points.size() / 3; // セグメント数
	if (form != 0){ sam -= -1; } // loopの場合は最後のセグメントの計算はp0を用いるため、専用の処理となるため-1個しておく

	std::vector<Vec3T<T>> bezierPoints;
	for (int s = 0; s < sm; ++s){
		const Vec3T<T>& p0 = points[s * 3];
		const Vec3T<T>& p1 = points[s * 3 + 1];
		const Vec3T<T>& p2 = points[s * 3 + 2];
		const Vec3T<T>& p3 = points[s * 3 + 3];
		int startI = s == 0 ? 0 : 1; // 二つ目以降のセグメントの始点は1つ前のセグメントの終点となるため、頂点リストには含めないようにする

		for (u32 i = startI; i <= sample; ++i){
			T u = e * i;
			Vec3T<T> p = this->core(p0, p1, p2, p3, u);
			bezierPoints.push_back(p);
		}
	}

	// loopタイプの場合
	if (form != 0){
		// 最後のセグメント計算をp0を用いて行う
		int last = points.size() - 1;
		const Vec3T<T>& p0 = points[last - 2];
		const Vec3T<T>& p1 = points[last - 1];
		const Vec3T<T>& p2 = points[last];
		const Vec3T<T>& p3 = points[0];
		for (u32 i = 1; i <= sample - 1; ++1){
			T u = e * i;
			Vec3T<T> p = this->core(p0, p1, p2, p3, u);
			bezierPoints.push_back(p);
		}
	}

	// BSplineCurve degree=2 sample=2
	u32 id = this->BSplineCurvesT<T>::add(bezierPoints, 2, form);
	this->data[id].setSample(2);
	return id;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		BezierCurveを追加する
//!
//!		ハンドルの頂点位置をセグメント制御座標の傾きから算出してベジェカーブを作成する（ハンドルの長さのレシオは一律）<br>
//!		@param[in] points セグメントの座標の配列データ　[p0, p1, p2,...](form=Loopの場合でも配列の最後にp0は含めない）
//!		@param[in] form 0=open, 1=close, 2=periodic
//!		@param[in] handleDirType enumを参照
//!		@param[in] handleLengthRatio ハンドルの長さを算出するためのレシオ値（各セグメント座標間の距離に対する比率）
//!		@param[in] sample セグメント内をサンプリングする数（ベジェカーブをBSplineで再現する精度）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 BezierCurvesT<T>::add(const std::vector<Mat4T<T>>& segmentPose, u8 form, BezierCurvesT::handleDirection handleDirType, T handleLengthRatio, u32 sample){
	if (form == 0){
		return this->add(segmentPose, form, handleDirType, std::vector<T>((segmentPose.size() - 1) * 2, handleLengthRatio), sample);
	} else{
		return this->add(segmentPose, form, handleDirType, std::vector<T>(segmentPose.size() * 2, handleLengthRatio), sample);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		BezierCurveを追加する
//!
//!		ハンドルの頂点位置をセグメント制御座標の傾きから算出してベジェカーブを作成する（ハンドルの長さのレシオは一律）<br>
//!		@param[in] points セグメントの座標の配列データ　[p0, p1, p2,...](form=Loopの場合でも配列の最後にp0は含めない）
//!		@param[in] form 0=open, 1=close, 2=periodic
//!		@param[in] handleDirType enumを参照
//!		@param[in] handleLengthRatio ハンドルの長さを算出するためのレシオ値(配列数＝ハンドル数）（各セグメント座標間の距離に対する比率）
//!		@param[in] sample セグメント内をサンプリングする数（ベジェカーブをBSplineで再現する精度）
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 BezierCurvesT<T>::add(const std::vector<Mat4T<T>>& segmentPose, u8 form, BezierCurvesT::handleDirection handleDirType, std::vector<T>& handleLengthRatio, u32 sample){
	assert(form < 3);
	if (form == 0){
		if (segmentPose.size() < 2 || handleLengthRatio.size() != (segmentPose.size() - 1) * 2){ return BURGESS2::U32_INFINITE; }
	} else{
		if (segmentPose.size() < 3 || handleLengthRatio.size() != segmentPose.size() * 2){ return BURGESS2::U32_INFINITE; }
	}

	// ハンドルの向きを取得
	std::vector<Vec3T<T>> handleDir;
	handleDir.reserve(handleLengthRatio.size());
	for (int i = 0; i < handleLengthRatio.size(); ++i){
		if (form == 0){
			// open
			if (i == 0){
				// 最初のハンドル
				switch (handleDirType){
					case 0: case 7:		handleDir.push_back(segmentPose[0].getRotationMat3().getXaxis());				break;	// +X
					case 1: case 8:		handleDir.push_back(segmentPose[0].getRotationMat3().getXaxis() * T(-1.0));		break;	// -X
					case 2: case 9:		handleDir.push_back(segmentPose[0].getRotationMat3().getYaxis());				break;	// +Y
					case 3: case 10:	handleDir.push_back(segmentPose[0].getRotationMat3().getYaxis() * T(-1.0));		break;	// -Y
					case 4: case 11:	handleDir.push_back(segmentPose[0].getRotationMat3().getZXaxis());				break;	// +Z
					case 5: case 12:	handleDir.push_back(segmentPose[0].getRotationMat3().getZaxis() * T(-1.0));		break;	// -Z
					case 6:				handleDir.push_back( (segmentPose[1].getTranslation() - segmentPose[0].getTranslation()).unit()); break; // auto
				}
			} else if (i == segmentPose.size() - 1){
				// 最後のハンドル
				switch (handleDirType){
					case 0: case 7:		handleDir.push_back(segmentPose.back().getRotationMat3().getXaxis() * T(-1.0));		break;	// +X
					case 1: case 8:		handleDir.push_back(segmentPose.back().getRotationMat3().getXaxis());				break;	// -X
					case 2: case 9:		handleDir.push_back(segmentPose.back().getRotationMat3().getYaxis() * T(-1.0));		break;	// +Y
					case 3: case 10:	handleDir.push_back(segmentPose.back().getRotationMat3().getYaxis());				break;	// -Y
					case 4: case 11:	handleDir.push_back(segmentPose.back().getRotationMat3().getZaxis() * T(-1.0));		break;	// +Z
					case 5: case 12:	handleDir.push_back(segmentPose.back().getRotationMat3().getZaxis());				break;	// -Z
					case 6:				handleDir.push_back((segmentPose[i-1].getTranslation() - segmentPose[i].getTranslation()).unit()); break; // auto
				}
			} else{
				// 途中のハンドル（制御点の左右にハンドルを作る）
				Vec3T<T> d;
				switch (handleDirType){
					case 0:				d = segmentPose[i].getRotationMat3().getXaxis();				break;	// +X
					case 1:				d = segmentPose[i].getRotationMat3().getXaxis() * T(-1.0);		break;	// -X
					case 2:				d = segmentPose[i].getRotationMat3().getYaxis();				break;	// +Y
					case 3:				d = segmentPose[i].getRotationMat3().getYaxis() * T(-1.0);		break;	// -Y
					case 4:				d = segmentPose[i].getRotationMat3().getZaxis();				break;	// +Z
					case 5:				d = segmentPose[i].getRotationMat3().getZaxis() * T(-1.0);		break;	// -Z
					case 6:	default:	d = (segmentPose[i + 1].getTranslation() - segmentPose[i - 1].getTranslation()).unit(); break; // auto
				}
			}
			handleDir.push_back(d * T(-1.0));
			handleDir.push_back(d);
		} else{
			// loop
			if (i == 0){
				// 最初のハンドル
				switch (handleDirType){
					case 0: case 7:		handleDir.push_back(segmentPose[0].getRotationMat3().getXaxis());				break;	// +X
					case 1: case 8:		handleDir.push_back(segmentPose[0].getRotationMat3().getXaxis() * T(-1.0));		break;	// -X
					case 2: case 9:		handleDir.push_back(segmentPose[0].getRotationMat3().getYaxis());				break;	// +Y
					case 3: case 10:	handleDir.push_back(segmentPose[0].getRotationMat3().getYaxis() * T(-1.0));		break;	// -Y
					case 4: case 11:	handleDir.push_back(segmentPose[0].getRotationMat3().getZaxis());				break;	// +Z
					case 5: case 12:	handleDir.push_back(segmentPose[0].getRotationMat3().getZaxis() * T(-1.0));		break;	// -Z
					case 6: // auto
						if (from == 1){
							// close
							handleDir.push_back((segmentPose[1].getTranslation() - segmentPose[0].getTranslation()).unit());
						} else{
							// periodic
							handleDir.push_back((segmentPose[1].getTranslation() - segmentPose[segmentPose.size()-1].getTranslation()).unit());
						}
						break;
				}
			} else if (i == segmentPose.size() - 1){
				// ループ形状にする最後のセグメント
				Vec3T<T> d;
				switch (handleDirType){
					case 0: case 7:		d = segmentPose.back().getRotationMat3().getXaxis();				break;	// +X
					case 1: case 8:		d = segmentPose.back().getRotationMat3().getXaxis() * T(-1.0);		break;	// -X
					case 2: case 9:		d = segmentPose.back().getRotationMat3().getYaxis();				break;	// +Y
					case 3: case 10:	d = segmentPose.back().getRotationMat3().getYaxis() * T(-1.0);		break;	// -Y
					case 4: case 11:	d = segmentPose.back().getRotationMat3().getZaxis();				break;	// +Z
					case 5: case 12:	d = segmentPose.back().getRotationMat3().getZaxis() * T(-1.0);		break;	// -Z
					case 6:				d = (segmentPose[0].getTranslation() - segmentPose[segmentPose.size() - 1].getTranslation()).unit(); break; // auto
				}
				handleDir.push_back(d * T(-1.0));
				handleDir.push_back(d);
				// 最後のハンドル
				switch (handleDirType){
					case 0: case 7:		handleDir.push_back(segmentPose.back().getRotationMat3().getXaxis());				break;	// +X
					case 1: case 8:		handleDir.push_back(segmentPose.back().getRotationMat3().getXaxis() * T(-1.0));		break;	// -X
					case 2: case 9:		handleDir.push_back(segmentPose.back().getRotationMat3().getYaxis());				break;	// +Y
					case 3: case 10:	handleDir.push_back(segmentPose.back().getRotationMat3().getYaxis() * T(-1.0));		break;	// -Y
					case 4: case 11:	handleDir.push_back(segmentPose.back().getRotationMat3().getZaxis());				break;	// +Z
					case 5: case 12:	handleDir.push_back(segmentPose.back().getRotationMat3().getZaxis() * T(-1.0));		break;	// -Z
					case 6: // auto
						if (from == 1){
							// close
							handleDir.push_back((segmentPose[segmentPose.size() - 1].getTranslation() - segmentPose[0].getTranslation()).unit());
						} else{
							// periodic
							handleDir.push_back((segmentPose[segmentPose.size() - 1].getTranslation() - segmentPose[i + 1].getTranslation()).unit());
						}
						break;
				}
			} else{
				// 途中のハンドル（制御点の左右にハンドルを作る）
				Vec3T<T> d;
				switch (handleDirType){
					case 0: case 7:		d = segmentPose[i].getRotationMat3().getXaxis();				break;	// +X
					case 1: case 8:		d = segmentPose[i].getRotationMat3().getXaxis() * T(-1.0);		break;	// -X
					case 2: case 9:		d = segmentPose[i].getRotationMat3().getYaxis();				break;	// +Y
					case 3: case 10:	d = segmentPose[i].getRotationMat3().getYaxis() * T(-1.0);		break;	// -Y
					case 4: case 11:	d = segmentPose[i].getRotationMat3().getZaxis();				break;	// +Z
					case 5: case 12:	d = segmentPose[i].getRotationMat3().getZaxis() * T(-1.0);		break;	// -Z
					case 6:				d = (segmentPose[i+1].getTranslation() - segmentPose[i - 1].getTranslation()).unit(); break; // auto
				}
				handleDir.push_back(d * T(-1.0));
				handleDir.push_back(d);
			}
		}
	}
	if (handleLengthRatio.size() != handleDir.size()){ return BURGESS2::U32_INFINITE; }

	std::vector<Vec3T<T>> bezierPoints;
	bezierPoints.reserve(segmentPose.size() + handleLengthRatio.size());

	for (int i = 0; i < segmentPose.size(); ++i){
		if (i == 0){
			// 始点の位置
			bezierPoints.push_back(segmentPose[0].getTranslation());
			
			// 始点の頂点のハンドル位置
			if (segmentPose[0].getTranslation().almosetEqual(segmentPose[1].getTranslation(), T(0.1)) == false){
				T segLen = segmentpose[0].getTranslation().distaceTo(segmentPose[1].getTranslation());
				Vec3T<T> handleOffset0 = handleDir[0] * (segLen * handleLengthRatio[0]);
				bezierPoints.push_back(segmentPose[0].getTranslation() + handleOffset0);
			} else{
				// rootとcp[0]が同じ位置にあった場合、[1][2]で計算を行いiをインクリメントする
				T segLen = segmentPose[1].getTranslation().distanceTo(segmentPose[2].getTranslation());
				Vec3T<T> handleOffset0 = handleDir[1] * (segLen * handleLengthRatio[1]);
				bezierPoints.push_back(segmentPose[1].getTranslation() + handleOffset0);
				i++;
			}
			
		} else if (i == segmentPose.size() - 1){
			// 終点の頂点の前方のハンドル位置
			T segLenF = segmentPose[i].getTranslation().distanceTo(segmentPose[i-1].getTranslation());
			Vec3T<T> handleOffset0 = handleDir[i * 2 - 1] * (segLenF * handleLengthRatio[i * 2 - 1]);
			bezierPoints.push_back(segmentPose[i].getTranslation() + handleOffset0);
			// 終点の位置
			bezierPoints.push_back(segmentPose[i].getTranslation());

			if (form != 0){
				// 終点の後方のハンドル
				T segLen = segmentPose[i].getTranslation().distanceTo(segmentPose[0].getTranslation());
				Vec3T<T> handleOffset0 = handleDir[i * 2] * (segLen * handleLengthRatio[i * 2]);
				bezierPoints.push_back(segmentPose[i].getTranslation() + handleOffset0);
				Vec3T<T> handleOffset1 = handleDir[i * 2 + 1] * (segLen * handleLengthRatio[i * 2 + 1]);
				bezierPoints.push_back(segmentPose[i].getTranslation() + handleOffset1);
			}
		} else{
			// 途中の頂点の前方のハンドル位置
			T segLenF = segmentPose[i].getTranslation().distanceTo(segmentPose[i-1].getTranslation());
			Vec3T<T> handleOffset0 = handleDir[i * 2 - 1] * (segLenF * handleLengthRatio[i * 2 - 1]);
			bezierPoints.push_back(segmentPose[i].getTranslation() + handleOffset0);
			// 途中の位置
			bezierPoints.push_back(segmentPose[i].getTranslation());
			// 途中の頂点の後方のハンドル位置
			T segLenB = segmentPose[i].getTranslation().distanceTo(segmentPose[i + 1].getTranslation());
			Vec3T<T> handleOffset1 = handleDir[i * 2] * (segLenB * handleLengthRatio[i * 2]);
			bezierPoints.push_back(segmentPose[i].getTranslation() + handleOffset1);
		}
	}

	return this->add(bezierPoints, form, sample);
}