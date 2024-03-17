//******************************************************************************
//! @file	LogMessage.h
//! @brief	 �f�o�b�O/Report_Print
//******************************************************************************
/**
*	@defgroup Maya_Integration
*	@brief �f�o�b�O/Report_Print <br>
*	#include <burgess2_maya/Integration.h><br>
*	@{
*/
#pragma once
#include <burgess2_main/Math.h>
#include <maya/MGlobal.h>

	/**
	*	@defgroup Report_Print
	*	@brief Maya�Ƀ��O���b�Z�[�W���o�� <br>
	*	�P�̂̏ꍇ�́@\b "title : �f�[�^�^�C�v�F�l" <br>
	*	�z��̏ꍇ�́@\b "title : �f�[�^�^�C�v�F�z�񐔁F�l" <br>
	*	#include <burgess2_maya/Integration.h><br>
	*	@code
	*		Vec3 v( 0.0, 1.2, -5.2 );
	*		report( v );�@
	*		print( v,"AAAAA" );
	*		
	*		// �o�͌���
	*		// [Burgess log message] ::::: Vec3:[0.0, 1.2, -5.2]
	*		// [Burgess log message] ::::: AAAAA : Vec3:[0.0, 1.2, -5.2]
	*	@endcode
	*	@{	*/

template<class ClassT>
static void report ( const ClassT data, const String title = "" );

template<class ClassT>
static void report ( const std::vector<ClassT> data, const String title = "", u32 size = 10 );

/*template<class ClassT>
static void print ( const ClassT data, const String title = "" );

template<class ClassT>
static void print ( const std::vector<ClassT> data, const String title = "", u32 size = 10 );
*/
#include "LogMessage.inl"

	/** @} */
/** @} */
//******************************************************************************
//! End of File
//******************************************************************************