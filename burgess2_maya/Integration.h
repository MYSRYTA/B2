//******************************************************************************
//! @file	Integration.h
//! @brief	★Maya Integration ヘッダー
//! @author	 Masanori Takashima.
//! @date 2023/8/29
//******************************************************************************
/** @defgroup Maya_Integration
*	   @{
*/

#pragma once
#include "Source/Converter.h"
#include "Source/IO.h"
#include "Source/LogMessage.h"
#include "Source/initializeHelper.h"
#include "Source/Burgess2MayaData.h"
#include "Source/ProcessingTime.h"

// MayaAPI
#include <maya/MEvaluationNode.h> // Maya2019以降でカスタムノードを並列モードで実行するために必要

/** @} */
//******************************************************************************
//! End of File
//******************************************************************************