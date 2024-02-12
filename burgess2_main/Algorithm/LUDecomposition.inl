//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     コンストラクタ
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
::LUDecompositionT<T>::LUDecompositionT ( const MatT<T> &A ) {
	assert ( this->LU.isSquare () );

	integer colSize = A.colSize ();
	integer rowSize = colSize;
	//long* iPiv = new long[colSize];
	int matLength = colSize * colSize;

	// 引数AをCLAPACKライブラリMatrix(Column-majourの行列)に変換する
	double* lapack_mat = new double[matLength];
	#pragma omp parallel for
	for (s32 i = 0; i < matLength; i++) {
		s32 cID = i / colSize;
		s32 rID = i - (cID * colSize);
		lapack_mat[i] = A.get ( rID, cID );
	}
	
	integer INFO;
	long* piv = new long[matLength];
	dgetrf_ ( &colSize, &rowSize, lapack_mat, &colSize, piv, &INFO );

	if (INFO == 0) {
		// CLAPACKスタイルのpivotをBurgessスタイルに変更
		this->pivot.resize ( colSize );
		#pragma omp parallel for
		for (s32 i = 0; i < colSize; i++) {
			this->pivot[i] = i; // 0~colSizeの昇順配列を準備
		}
		for (s32 i = 0; i < colSize; i++) {
			std::swap ( this->pivot[i], this->pivot[piv[i] - 1] );
		}

		// CLAPACKライブラリMatrix(Column-majourの行列)をBURGESS2::Mat(Row-majour)に変換する
		this->LU.reset ( colSize, rowSize );
		#pragma omp parallel for
		for (s32 i = 0; i < matLength; i++) {
			s32 cID = i / colSize;
			s32 rID = i - (cID * colSize);
			this->LU.setValue ( rID, cID, lapack_mat[i] );
		}
	} else {
		// CLAPACKのdgetrf_()が正常に行えなかった場合はjama版のLUDecompositionを使用する（処理速度が非常に遅い）
		// PSDriver = false 原因不明
		// PSWrap = true

		this->LU = A;
		u32 colSize = this->LU.colSize();

		this->pivot.resize ( colSize );
		for ( u32 i = 0; i < colSize; i++ ) {
			this->pivot[i] = i;
		}

		VecT<T> restRowValues{ this->LU.rowSize () };

		for ( u32 col = 0; col < colSize; col++ ) {
			for ( u32 row = 0; row < col; row++ ) {
				restRowValues = this->LU.getRowValue ( row );
				T sum = restRowValues[col];
				for ( u32 i = 0; i < row; i++ ) {
					sum -= restRowValues[i] * this->LU.get ( i, col );
				}
				this->LU.setValue ( row, col, sum );
			}

			u32 diagonalCheck = col;
			T checker = -T ( BURGESS2::F32_INFINITE );
			for ( u32 row = col; row < colSize; row++ ) {
				restRowValues = this->LU.getRowValue ( row );
				T sum = restRowValues[col];
				for ( u32 i = 0; i < col; i++ ) {
					sum -= restRowValues[i] * this->LU.get ( i, col );
				}
				this->LU.setValue ( row, col, sum );

				if ( checker < abs ( sum ) ) { // std::abs
					diagonalCheck = row;
					checker = abs ( sum ); // std::abs
				}
			}

			if ( diagonalCheck != col ) {
				this->LU.swapRows ( diagonalCheck, col );
				std::swap ( this->pivot[diagonalCheck], this->pivot[col] );
			}

			T cc = this->LU.get ( col, col );
			if ( Burgess2MathFn::AlmostEqual ( cc, T ( 0.0 ) ) == false ) {
				for ( u32 row = col + 1; row < colSize; row++ ) {
					this->LU.setValue ( row * colSize + col, this->LU.get ( row, col ) / cc );
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     連立方程式を解く
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
VecT<T> LUDecompositionT<T>::solve ( const VecT<T> &B ) const {
	u32 pSize = u32(this->pivot.size ());
	VecT<T> result ( B.size () );
	//assert ( 0 < pSize && this->LU.getColCount () == B.size () );
	assert ( 0 < pSize );

	for ( u32 i = 0; i < pSize; i++ ) {
		result [i] = B [this->pivot [i]];
	}

	// Solve L*Y = B
	for ( u32 k = 0; k < pSize; k++ ) {
		for ( u32 i = k + 1; i < pSize; i++ ) {
			result [i] -= result [k] * this->LU.get ( i, k );
		}
	}

	// Solve U*X = Y
	for ( s32 k = s32 ( pSize ) - 1; k >= 0; k-- ) {
		T diagonalVal = this->LU.get ( k, k );
		if ( diagonalVal != T ( 0.0 ) ) {
			result [k] /= diagonalVal;
		}
		for ( s32 i = 0; i < k; i++ ) {
			result [i] -= result [k] * this->LU.get ( i, k );
		}
	}
	return result;
}

