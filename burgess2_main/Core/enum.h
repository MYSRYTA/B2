//******************************************************************************
//! @file	enum.h
//! @brief	 enum
//******************************************************************************
/**
*	@defgroup Base
*	@{
*/
#pragma once
#include <string>
#include <vector>

namespace BURGESS2{
	enum class Space : unsigned char{
		Local, Global
	};

	enum class RotationOrder : unsigned char{
		XYZ, YZX, ZXY, XZY, YXZ, ZYX
	};

	enum class Axis : unsigned char{
		X, Y, Z
	};

	enum class NormalAxis : unsigned char{
		X, neg_X, Y, neg_Y, Z, neg_Z
	};
}
/** @} */
//******************************************************************************
//! End of File
//******************************************************************************