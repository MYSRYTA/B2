//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ：複数のVec3型ドリブン値を登録する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
KrigingControlPointT<T>::KrigingControlPointT ( const QuatT<T> &driverRot, const std::vector<Vec3T<T>> &drivenValues )
	: driverRot { driverRot }
{
	for (u32 i = 0; i<drivenValues.size (); i++) {
		this->drivenValues.combine ( drivenValues[i].toVec () );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ：複数のMat4型ドリブン値を登録する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
KrigingControlPointT<T>::KrigingControlPointT ( const QuatT<T> &driverRot, const std::vector<Mat4T<T>> &drivenValues )
	: driverRot{ driverRot }
{
	for (u32 i = 0; i<drivenValues.size (); i++) {
		this->drivenValues.combine ( drivenValues[i].toVec () );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ：Krigingを構築する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
KrigingT<T>::KrigingT ( const std::vector<KrigingControlPointT<T>> &ctrlPoints )
	: ctrlPoints { ctrlPoints }
{
	s32 numData = s32(ctrlPoints.size ());
	this->coefMatrix.resize ( numData + 1, numData + 1 );
	this->coefMatrix.setAllValue( T ( 1.0 ) );
	this->coefMatrix.setValue ( numData, numData, 0.0 );
	for(u8 i=0; i<numData; i++){
		QuatT<T> qi = ctrlPoints[i].driverRot;
		for(u8 j=i; j<numData; j++){
			if(i==j){
				this->coefMatrix.setValue ( i, j, 0.0 );
				continue;
			}
			QuatT<T> qj = ctrlPoints[j].driverRot;
			T v = qi.dist ( qj );
			this->coefMatrix.setValue ( i, j, v );
			this->coefMatrix.setValue ( j, i, v );
		}
	}
	this->coefMatrix = this->coefMatrix.inverse();
	this->distVec.resize ( numData + 1 );
	this->distVec.setAll ( T ( 1.0 ) );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	    Kriging補間を実行する
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> KrigingT<T>::solve ( const QuatT<T> &driverRot ) {
	s32 numData = s32(this->ctrlPoints.size ());
	for(u8 i=0; i<numData; i++){
		this->distVec[i] = driverRot.dist ( this->ctrlPoints[i].driverRot );
	}
	VecT<T> weightVec = this->coefMatrix * this->distVec;

	VecT<T> result = weightVec[0] * this->ctrlPoints[0].drivenValues;
	for(u8 i=1; i<numData; i++){
		result += weightVec[i] * this->ctrlPoints[i].drivenValues;
	}

	return result;
}