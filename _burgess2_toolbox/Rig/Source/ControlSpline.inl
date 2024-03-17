//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �X�v���C���̏����l��ݒ肷��
//!	    @brief �X�v���C���̏����l��ݒ肷��
//!		@param[in] splineType CurveDataT<T>::CurveType���Q��
//!		@param[in] splineLength ����
//!		@param[in] form CurveDataT<T>::CurveFrom���Q��
//!		@param[in] degree ���� (splineType��BSpline�̎��ɂ̂ݕK�v)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void ControlSplineT<T>::initialize ( u8 splineType, T splineLength, u8 _form, u8 _degree ){
	if ( splineType != CurveData::CurveType::Linear &&
		 splineType != CurveData::CurveType::BSpline &&
		 splineType != CurveData::CurveType::CatmullRom){ return; }
	if(splineLength <= T ( 0.0 )){ return; }

	this->type = splineType;
	this->neutralLength = splineLength;
	this->form = _form;
	this->degree = _degree;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �X�v���C���̏����l��ݒ肷��
//!	    @brief �X�v���C���̏����l��ݒ肷��
//!		@param[in] splineType CurveDataT<T>::CurveType���Q��
//!		@param[in] neutralCntrolPoint �����X�v���C����Global-Position�R���g���[���|�C���g
//!		@param[in] form CurveDataT<T>::CurveFrom���Q��
//!		@param[in] degree ���� (splineType��BSpline�̎��ɂ̂ݕK�v)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void ControlSplineT<T>::initialize ( u8 splineType, std::vector<Vec3T<T>> &neutralCntrolPoint, u8 _form, u8 _degree ){
	if ( splineType != CurveData::CurveType::Linear &&
		 splineType != CurveData::CurveType::BSpline &&
		 splineType != CurveData::CurveType::CatmullRom ){
		return;
	}
	if ( splineType == CurveData::CurveType::Linear ){
		if ( neutralCntrolPoint.size() < 2 ){ return; }
	} else{
		if ( neutralCntrolPoint.size() < 3 ){ return; }
	}

	T length;
	switch ( splineType ){
		case CurveData::CurveType::Linear:
			LinearCurvesT<T> lc;
			lc.add ( neutralCntrolPoint, this->form );
			length = lc.getCurveLength ( 0 );
			break;
		case CurveData::CurveType::BSpline:
			BSplineCurvesT<T> bs;
			bs.add ( neutralCntrolPoint, this->degree, this->form );
			length = bs.getCurveLength ( 0 );
			break;
		case CurveData::CurveType::CatmullRom:
			CatmullRomCurvesT<T> cr;
			cr.add ( neutralCntrolPoint, this->form );
			length = cr.getCurveLength ( 0 );
			break;
	}

	this->initialize ( splineType, length, _form, _degree );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �X�v���C���̏����l��ݒ肷��
//!	    @brief �X�v���C���̏����l��ݒ肷��
//!		@param[in] splineType CurveDataT<T>::CurveType���Q��
//!		@param[in] neutralCntrolPoint �����X�v���C����Global-Mat4�R���g���[���|�C���g
//!		@param[in] form CurveDataT<T>::CurveFrom���Q��
//!		@param[in] degree ���� (splineType��BSpline�̎��ɂ̂ݕK�v)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void ControlSplineT<T>::initialize ( u8 splineType, std::vector<Mat4T<T>> &neutralCntrolPoint, u8 _form, u8 _degree ){
	std::vector<Vec3T<T>> neutralCntrolPointPos ( neutralCntrolPoint.size () );
	for ( u32 i = 0; i < neutralCntrolPoint.size (); ++i ){
		neutralCntrolPointPos[i] = neutralCntrolPoint[i].getTranslation ();
	}
	this->initialize ( splineType, neutralCntrolPointPos, _form, _degree );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �X�v���C���ɃR���X�g���C���g����W���C���g����Ԃ�
//!	    @brief �X�v���C���ɃR���X�g���C���g����W���C���g����Ԃ�
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
u32 ControlSplineT<T>::jointCount ()const{
	return this->jointAttachRatio.size ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �X�v���C���ɃR���X�g���C���g����W���C���g����ݒ�
//!	    @brief �X�v���C���ɃR���X�g���C���g����W���C���g����ݒ�
//!		@param[in] jointSize �W���C���g��(jointSize+1���X�v���C���̏������̕�����)
//!		@param[in] startJoint �n�_�ɃW���C���g��z����ꍇ��true
//!		@param[in] endJoint�@��[�ɃW���C���g��z����ꍇ��true
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void ControlSplineT<T>::setJointSize ( u32 jointSize, Bool startJoint, Bool endJoint ){
	this->jointAttachRatio.resize ( 0 );
	if ( startJoint ){ this->jointAttachRatio.push_back ( T ( 0.0 ) ); }
	T e = T ( 1.0 ) / T ( jointSize + 1 );
	for ( u32 i = 0; i < jointSize; ++i ){
		this->jointAttachRatio.push_back ( e * T ( 1 + i ) );
	}
	if ( endJoint ){ this->jointAttachRatio.push_back ( T ( 1.0 ) ); }
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �S�̂̃X�P�[����ݒ肷��
//!	    @brief �S�̂̃X�P�[����ݒ肷��
//!		@param[in] _scale�@0.01 ~
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void ControlSplineT<T>::setScale ( T _scale ){ 
	this->scale = Math::Max ( _scale, T ( 0.01 ) );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �W���C���g�̉�]�l�̎Z�o�Ŏg�p����A�b�v�x�N�^�[�̃^�C�v��ݒ肷��
//!	    @brief �W���C���g�̉�]�l�̎Z�o�Ŏg�p����A�b�v�x�N�^�[�̃^�C�v��ݒ肷��
//!		@param[in] type
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void ControlSplineT<T>::setUpvectorType ( u8 type ){
	this->upvectorType = type;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �A�b�v�x�N�^�[��PTF���̃A���S���Y���ŎZ�o����ꍇ�̐��x��ݒ肷��
//!	    @brief �A�b�v�x�N�^�[��PTF���̃A���S���Y���ŎZ�o����ꍇ�̐��x��ݒ肷��
//!		@param[in] sample ���x�@1~10
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void ControlSplineT<T>::setUpvectorSample(u8 sample) {
	this->upvectorSample = Math::Clamp(sample, 1, 10);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �I�t�Z�b�g
//!	    @brief �I�t�Z�b�g
//!		@param[in] side 0=startSide, 1=endSide, 2=bothEnd
//!		@param[in] ratio
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void ControlSplineT<T>::setSlideOffset ( u8 side, T ratio ){
	this->offsetType = Math::Min( side, u8(3) );
	this->offsetRatio = ratio;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    �W���C���g���W�̎Z�o�ʒu���X�P�[������
//!	    @brief �W���C���g���W�̎Z�o�ʒu���X�P�[������
//!		@param[in] side �X�P�[���̎x�_�@0=startSide, 1=endSide, 2=centar
//!		@param[in] scale 0.01~
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void ControlSplineT<T>::setSlideScale ( u8 side, T _scale ){
	T scale = Math::Max ( _scale, T ( 0.01 ) );
	this->slideScale = scale;
	switch ( side ){
		case 0:
			for ( u32 i = 0; i < this->jointCount (); ++i ){
				this->jointAttachRatio[i] *= scale;
			}
			break;
		case 1:
			for ( u32 i = 0; i < this->jointCount (); ++i ){
				T d = this->jointAttachRatio[i] - T ( 1.0 );
				this->jointAttachRatio[i] = T ( 1.0 ) + d * scale;
			}
			break;
		default:
			for ( u32 i = 0; i < this->jointCount (); ++i ){
				T d = this->jointAttachRatio[i] - T ( 0.5 );
				this->jointAttachRatio[i] = T ( 0.5 ) + d * scale;
			}
			break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    solve
//!	    @brief solve
//!		@param[in] root Global-Mat4
//!		@param[in] controlPoint splineControlPoint��Global-Mat4
//!		@param[in] upvector upvector��Global-Mat4�iUpvectorType��UpvectorHandle�̎��ɂ̂ݎg�p�j
//!		@return �c���[�W���C���g��Local-Mat4
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<Mat4T<T>> ControlSplineT<T>::solve ( const Mat4T<T> &root, const std::vector<Mat4T<T>> &controlPoint, const Vec3T<T> &upvector ){
	std::vector<Mat4T<T>> result ( this->jointCount() );
	if ( this->neutralLength <= T(0.0) ){ return result; }

	u32 cpCnt = controlPoint.size ();
	if ( this->type == CurveData::CurveType::Linear ){
		if ( cpCnt < 2 ){ return result; }
	} else{
		if ( cpCnt < 3 ){ return result; }
	}

	std::vector<Vec3T<T>> cpPos ( cpCnt );
	Mat4T<T> invRoot = root.inverse ();
	for ( u32 i = 0; i < cpCnt; ++i ){
		if ( Math::AlmostEqual ( this->scale, T ( 1.0 ) ) ){
			cpPos[i] = controlPoint[i].getTranslation ();
		} else{
			Vec3T<T> locPos = invRoot * controlPoint[i].getTranslation ();
			cpPos[i] = root * (locPos * this->scale);
		}
	}

	std::vector<Vec3T<T>> nml;
	switch ( this->upvectorType ){
		case ControlSplineT::UpvectorType::UpvectorHandle:
			nml.resize(1);
			nml[0] = upvector;
			break;
		case ControlSplineT::UpvectorType::RootAndTipControlYaxis:
			nml.resize ( 2 );
			nml[0] = controlPoint[0].getUpperLeft ().getYaxis ();
			nml[1] = controlPoint[cpCnt - 1].getUpperLeft ().getYaxis ();
			break;
		case ControlSplineT::UpvectorType::AllControllerYaxis:
			nml.resize ( cpCnt );
			for ( u32 i = 0; i < cpCnt; ++i ){
				nml[i] = controlPoint[i].getUpperLeft ().getYaxis ();
			}
			break;
		case ControlSplineT::UpvectorType::rootYaxisPTF:
		case ControlSplineT::UpvectorType::rootYaxisPTF2:
		case ControlSplineT::UpvectorType::rootYaxisFrenetFrame:
		case ControlSplineT::UpvectorType::rootYaxisDoubleReflection:
		default:
			nml.resize(1);
			nml[0] = controlPoint[0].getUpperLeft().getYaxis();
			break;
	}

	switch ( this->type ){
		case CurveData::CurveType::Linear:
		{
			LinearCurvesT<T> lc;
			lc.add ( cpPos, this->form );
			switch ( this->upvectorType ){
				case ControlSplineT::UpvectorType::rootYaxisPTF:				lc.computePTFNormals(0, nml[0], 1); break;
				case ControlSplineT::UpvectorType::rootYaxisPTF2:				lc.computePTFNormals2(0, nml[0], 1); break;
				case ControlSplineT::UpvectorType::rootYaxisFrenetFrame:		lc.computeFrenetFrameNormals(0, nml[0], 1); break;
				case ControlSplineT::UpvectorType::rootYaxisDoubleReflection:	lc.computeDoubleReflectionNormals(0, nml[0], 1); break;
				default: lc.resetAllNormals ( 0, nml ); break;
			}
			T e = this->neutralLength / lc.getCurveLength ( 0 );
			for ( u32 i = 0; i < this->jointCount (); ++i ){
				T r = this->jointAttachRatio[i];
				switch ( this->offsetType ){
					case ControlSplineT::OffsetType::Start: r = r * e + this->offsetRatio; break;
					case ControlSplineT::OffsetType::End: r = r * e + T ( 1.0 ) - e - this->offsetRatio; break;
					case ControlSplineT::OffsetType::BothEnd:
						T r2 = Math::Min ( this->offsetRatio, T ( 0.5 ) );
						r = r * ( T ( 1.0 ) - r2 * 2.0 ) + r2;
				}
				XfoT<T> xfo = lc.getXfoAtLengthRatio ( 0, r );
				xfo.sc *= this->slideScale * this->scale;
				result[i] = xfo.toMat4 ();
			}
			break;
		}
		case CurveData::CurveType::BSpline:
		default:
		{
			BSplineCurvesT<T> bs;
			bs.add ( cpPos, this->degree, this->form );
			switch ( this->upvectorType ){
				case ControlSplineT::UpvectorType::rootYaxisPTF:				bs.computePTFNormals(0, nml[0], this->upvectorSample ); break;
				case ControlSplineT::UpvectorType::rootYaxisPTF2:				bs.computePTFNormals2(0, nml[0], this->upvectorSample); break;
				case ControlSplineT::UpvectorType::rootYaxisFrenetFrame:		bs.computeFrenetFrameNormals(0, nml[0], this->upvectorSample); break;
				case ControlSplineT::UpvectorType::rootYaxisDoubleReflection:	bs.computeDoubleReflectionNormals(0, nml[0], this->upvectorSample); break;
				default: bs.resetAllNormals ( 0, nml ); break;
			}
			T e = this->neutralLength / bs.getCurveLength ( 0 );
			for ( u32 i = 0; i < this->jointCount (); ++i ){
				T r = this->jointAttachRatio[i];
				switch ( this->offsetType ){
					case ControlSplineT::OffsetType::Start: r = r * e + this->offsetRatio; break;
					case ControlSplineT::OffsetType::End: r = r * e + T ( 1.0 ) - e - this->offsetRatio; break;
					case ControlSplineT::OffsetType::BothEnd:
						T r2 = Math::Min ( this->offsetRatio, T ( 0.5 ) );
						r = r * ( T ( 1.0 ) - r2 * 2.0 ) + r2;
				}
				XfoT<T> xfo = bs.getXfoAtLengthRatio ( 0, r );
				xfo.sc *= this->slideScale * this->scale;
				result[i] = xfo.toMat4 ();
			}
			break;
		}
		case CurveData::CurveType::CatmullRom:
		{
			CatmullRomCurvesT<T> cr;
			cr.add ( cpPos, this->form, 0.2, 5 );
			switch ( this->upvectorType ){
				case ControlSplineT::UpvectorType::rootYaxisPTF:				cr.computePTFNormals( 0, nml[0], this->upvectorSample); break;
				case ControlSplineT::UpvectorType::rootYaxisPTF2:				cr.computePTFNormals2(0, nml[0], this->upvectorSample); break;
				case ControlSplineT::UpvectorType::rootYaxisFrenetFrame:		cr.computeFrenetFrameNormals(0, nml[0], this->upvectorSample); break;
				case ControlSplineT::UpvectorType::rootYaxisDoubleReflection:	cr.computeDoubleReflectionNormals(0, nml[0], this->upvectorSample); break;
				default: cr.resetAllNormals ( 0, nml ); break;
			}
			T e = this->neutralLength / cr.getCurveLength ( 0 );
			for ( u32 i = 0; i < this->jointCount (); ++i ){
				T r = this->jointAttachRatio[i];
				switch ( this->offsetType ){
					case ControlSplineT::OffsetType::Start: r = r * e + this->offsetRatio; break;
					case ControlSplineT::OffsetType::End: r = r * e + T ( 1.0 ) - e - this->offsetRatio; break;
					case ControlSplineT::OffsetType::BothEnd:
						T r2 = Math::Min ( this->offsetRatio, T ( 0.5 ) );
						r = r * ( T ( 1.0 ) - r2 * 2.0 ) + r2;
				}
				XfoT<T> xfo = cr.getXfoAtLengthRatio ( 0, r );
				xfo.sc *= this->slideScale * this->scale;
				result[i] = xfo.toMat4 ();
			}
			break;
		}
	}
	return result;
}