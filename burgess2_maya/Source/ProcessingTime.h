//******************************************************************************
//! @file	ProcessingTime.h
//! @brief	 処理時間の計測
//******************************************************************************
/** @defgroup Maya_Integration
*　@{
*/
/**		@brief	処理時間の計測 <br>
*			#include <burgess2_maya/Integration.h> <br>
*			@code
*				{
*					ProcessingTime pt("TestName");
*					///
*						処理
*					///
*				}
*			@endcode
*/
#include <chrono>
#include "LogMessage.h"

class  ProcessingTime {
	std::string m_name;
	std::chrono::system_clock::time_point m_start;
	bool m_isActive;

public:
	ProcessingTime ( const std::string& name = "Process", bool start = true ) :
		m_name ( name ),
		m_isActive ( start ) {
		if (start) {
			this->restart ();
		}
	}
	~ProcessingTime () {
		this->stop ();
	}

	///計測のリスタート
	void restart ()& {
		m_start = std::chrono::system_clock::now ();
		m_isActive = true;
	}

	///計測を終了し、結果をMayaにレポート出力
	void stop ()& {
		if (!m_isActive)
			return;
		const auto end = std::chrono::system_clock::now ();
		const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds > ( end - m_start ).count();

		m_isActive = false;

		std::string result = std::to_string ( elapsed ) + "sc";
		report ( result, "Prosess:" + m_name );
	}
};

/** @} */
//******************************************************************************
//! End of File
//******************************************************************************