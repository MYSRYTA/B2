#pragma once
extern "C"
{
#include <CLAPACK/include/f2c.h>
#include <CLAPACK/include/clapack.h>
}

#include <burgess2_main/Core/base.h>
#include <burgess2_main/Math.h>
#include <burgess2_main/Math.h>





//******************************************************************************
//! @file	LUDecomposition.h
//! @brief	 LUï™â
//******************************************************************************
/** @defgroup Algorithm
*Å@@{
*/
/**		@brief	LUï™â <br>
*			#include <burgess2_main/Algorithm.h> <br><br>
*			LAPACKÇégóp
*			@ref https://github.com/Reference-LAPACK/lapack <br><br>
*			#alias <br> \b : LUDecomposition = LUDecompositionT<float> <br> \b : LUDecompositiond = LUDecompositionT<double> <br>
*/

template<typename T>
class LUDecompositionT {
public:
	MatT<T> LU;
	std::vector<s32> pivot;

	LUDecompositionT () {};
	LUDecompositionT ( const MatT<T> &A );
	virtual ~LUDecompositionT () {}

	VecT<T> solve ( const VecT<T> &B ) const;
};

#include "LUDecomposition.inl"

/** @} */

using LUDecomposition = LUDecompositionT<f32>;
using LUDecompositiond = LUDecompositionT<f64>;

/** @} */
//******************************************************************************
//! End of File
//******************************************************************************