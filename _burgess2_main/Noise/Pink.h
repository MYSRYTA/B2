#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Containers/LArray.h>

//******************************************************************************
//! @file	PinkNoise.h
//! @brief	 �s���N�m�C�Y
//******************************************************************************
/** @defgroup Noise
*�@@{
*/
/**		@brief	�s���N�m�C�Y<br>
*			#include <burgess2_main/Noise.h> <br><br>
*			#alias <br> \b : Pink = PinkT<float> <br> \b : Pinkd = PinkT<double> <br>
*			�R�[�h�Q�ƌ��@
*			@ref http://www.finetune.co.jp/~lyuka/technote/pinknoise/ <br>
*/

template<typename T>
class PinkT
{
	L16Array<f64> K;
	L16Array<f64> Z;
	f64 t;
	f64 val;
	f64 previousVal;
public:
	PinkT ();
	~PinkT () {}

	T solveNoise ( u32 seed, T time, T strength, T timeScale = T ( 1.0 ) );
	T applyNoise ( u32 seed, T time, T strength, T timeScale = T ( 1.0 ) );

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( K ),
			CEREAL_NVP ( Z ),
			CEREAL_NVP ( t ),
			CEREAL_NVP ( val ),
			CEREAL_NVP ( previousVal )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

#include "Pink.inl"
/** @} */

using Pink = PinkT<f32>;
using Pinkd = PinkT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************