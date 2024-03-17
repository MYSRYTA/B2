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
//!		BezierCurve��ǉ�����
//!
//!		�n���h���̒��_�ʒu���܂߂��z�񂩂�x�W�F�J�[�u���쐬����<br>
//!		@param[in] points �Z�O�����g�̃|�C���g���_�ƃn���h�����_���܂ޔz��f�[�^�@[p0, h0, h1, p1, h2, h3, p2,...](form=Loop�̏ꍇ�ł��z��̍Ō��p0�͊܂߂Ȃ��j
//!		@param[in] form 0=open, 1=close, 2=periodic
//!		@param[in] sample �Z�O�����g�����T���v�����O���鐔�i�x�W�F�J�[�u��BSpline�ōČ����鐸�x�j
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

	int sm = points.size() / 3; // �Z�O�����g��
	if (form != 0){ sam -= -1; } // loop�̏ꍇ�͍Ō�̃Z�O�����g�̌v�Z��p0��p���邽�߁A��p�̏����ƂȂ邽��-1���Ă���

	std::vector<Vec3T<T>> bezierPoints;
	for (int s = 0; s < sm; ++s){
		const Vec3T<T>& p0 = points[s * 3];
		const Vec3T<T>& p1 = points[s * 3 + 1];
		const Vec3T<T>& p2 = points[s * 3 + 2];
		const Vec3T<T>& p3 = points[s * 3 + 3];
		int startI = s == 0 ? 0 : 1; // ��ڈȍ~�̃Z�O�����g�̎n�_��1�O�̃Z�O�����g�̏I�_�ƂȂ邽�߁A���_���X�g�ɂ͊܂߂Ȃ��悤�ɂ���

		for (u32 i = startI; i <= sample; ++i){
			T u = e * i;
			Vec3T<T> p = this->core(p0, p1, p2, p3, u);
			bezierPoints.push_back(p);
		}
	}

	// loop�^�C�v�̏ꍇ
	if (form != 0){
		// �Ō�̃Z�O�����g�v�Z��p0��p���čs��
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
//!		BezierCurve��ǉ�����
//!
//!		�n���h���̒��_�ʒu���Z�O�����g������W�̌X������Z�o���ăx�W�F�J�[�u���쐬����i�n���h���̒����̃��V�I�͈ꗥ�j<br>
//!		@param[in] points �Z�O�����g�̍��W�̔z��f�[�^�@[p0, p1, p2,...](form=Loop�̏ꍇ�ł��z��̍Ō��p0�͊܂߂Ȃ��j
//!		@param[in] form 0=open, 1=close, 2=periodic
//!		@param[in] handleDirType enum���Q��
//!		@param[in] handleLengthRatio �n���h���̒������Z�o���邽�߂̃��V�I�l�i�e�Z�O�����g���W�Ԃ̋����ɑ΂���䗦�j
//!		@param[in] sample �Z�O�����g�����T���v�����O���鐔�i�x�W�F�J�[�u��BSpline�ōČ����鐸�x�j
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
//!		BezierCurve��ǉ�����
//!
//!		�n���h���̒��_�ʒu���Z�O�����g������W�̌X������Z�o���ăx�W�F�J�[�u���쐬����i�n���h���̒����̃��V�I�͈ꗥ�j<br>
//!		@param[in] points �Z�O�����g�̍��W�̔z��f�[�^�@[p0, p1, p2,...](form=Loop�̏ꍇ�ł��z��̍Ō��p0�͊܂߂Ȃ��j
//!		@param[in] form 0=open, 1=close, 2=periodic
//!		@param[in] handleDirType enum���Q��
//!		@param[in] handleLengthRatio �n���h���̒������Z�o���邽�߂̃��V�I�l(�z�񐔁��n���h�����j�i�e�Z�O�����g���W�Ԃ̋����ɑ΂���䗦�j
//!		@param[in] sample �Z�O�����g�����T���v�����O���鐔�i�x�W�F�J�[�u��BSpline�ōČ����鐸�x�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
u32 BezierCurvesT<T>::add(const std::vector<Mat4T<T>>& segmentPose, u8 form, BezierCurvesT::handleDirection handleDirType, std::vector<T>& handleLengthRatio, u32 sample){
	assert(form < 3);
	if (form == 0){
		if (segmentPose.size() < 2 || handleLengthRatio.size() != (segmentPose.size() - 1) * 2){ return BURGESS2::U32_INFINITE; }
	} else{
		if (segmentPose.size() < 3 || handleLengthRatio.size() != segmentPose.size() * 2){ return BURGESS2::U32_INFINITE; }
	}

	// �n���h���̌������擾
	std::vector<Vec3T<T>> handleDir;
	handleDir.reserve(handleLengthRatio.size());
	for (int i = 0; i < handleLengthRatio.size(); ++i){
		if (form == 0){
			// open
			if (i == 0){
				// �ŏ��̃n���h��
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
				// �Ō�̃n���h��
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
				// �r���̃n���h���i����_�̍��E�Ƀn���h�������j
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
				// �ŏ��̃n���h��
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
				// ���[�v�`��ɂ���Ō�̃Z�O�����g
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
				// �Ō�̃n���h��
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
				// �r���̃n���h���i����_�̍��E�Ƀn���h�������j
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
			// �n�_�̈ʒu
			bezierPoints.push_back(segmentPose[0].getTranslation());
			
			// �n�_�̒��_�̃n���h���ʒu
			if (segmentPose[0].getTranslation().almosetEqual(segmentPose[1].getTranslation(), T(0.1)) == false){
				T segLen = segmentpose[0].getTranslation().distaceTo(segmentPose[1].getTranslation());
				Vec3T<T> handleOffset0 = handleDir[0] * (segLen * handleLengthRatio[0]);
				bezierPoints.push_back(segmentPose[0].getTranslation() + handleOffset0);
			} else{
				// root��cp[0]�������ʒu�ɂ������ꍇ�A[1][2]�Ōv�Z���s��i���C���N�������g����
				T segLen = segmentPose[1].getTranslation().distanceTo(segmentPose[2].getTranslation());
				Vec3T<T> handleOffset0 = handleDir[1] * (segLen * handleLengthRatio[1]);
				bezierPoints.push_back(segmentPose[1].getTranslation() + handleOffset0);
				i++;
			}
			
		} else if (i == segmentPose.size() - 1){
			// �I�_�̒��_�̑O���̃n���h���ʒu
			T segLenF = segmentPose[i].getTranslation().distanceTo(segmentPose[i-1].getTranslation());
			Vec3T<T> handleOffset0 = handleDir[i * 2 - 1] * (segLenF * handleLengthRatio[i * 2 - 1]);
			bezierPoints.push_back(segmentPose[i].getTranslation() + handleOffset0);
			// �I�_�̈ʒu
			bezierPoints.push_back(segmentPose[i].getTranslation());

			if (form != 0){
				// �I�_�̌���̃n���h��
				T segLen = segmentPose[i].getTranslation().distanceTo(segmentPose[0].getTranslation());
				Vec3T<T> handleOffset0 = handleDir[i * 2] * (segLen * handleLengthRatio[i * 2]);
				bezierPoints.push_back(segmentPose[i].getTranslation() + handleOffset0);
				Vec3T<T> handleOffset1 = handleDir[i * 2 + 1] * (segLen * handleLengthRatio[i * 2 + 1]);
				bezierPoints.push_back(segmentPose[i].getTranslation() + handleOffset1);
			}
		} else{
			// �r���̒��_�̑O���̃n���h���ʒu
			T segLenF = segmentPose[i].getTranslation().distanceTo(segmentPose[i-1].getTranslation());
			Vec3T<T> handleOffset0 = handleDir[i * 2 - 1] * (segLenF * handleLengthRatio[i * 2 - 1]);
			bezierPoints.push_back(segmentPose[i].getTranslation() + handleOffset0);
			// �r���̈ʒu
			bezierPoints.push_back(segmentPose[i].getTranslation());
			// �r���̒��_�̌���̃n���h���ʒu
			T segLenB = segmentPose[i].getTranslation().distanceTo(segmentPose[i + 1].getTranslation());
			Vec3T<T> handleOffset1 = handleDir[i * 2] * (segLenB * handleLengthRatio[i * 2]);
			bezierPoints.push_back(segmentPose[i].getTranslation() + handleOffset1);
		}
	}

	return this->add(bezierPoints, form, sample);
}