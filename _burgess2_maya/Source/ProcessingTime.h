//******************************************************************************
//! @file	ProcessingTime.h
//! @brief	 �������Ԃ̌v��
//******************************************************************************
/** @defgroup Maya_Integration
*�@@{
*/
/**		@brief	�������Ԃ̌v�� <br>
*			#include <burgess2_maya/Integration.h> <br>
*			@code
*				{
*					ProcessingTime pt("TestName");
*					///
*						����
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

	///�v���̃��X�^�[�g
	void restart ()& {
		m_start = std::chrono::system_clock::now ();
		m_isActive = true;
	}

	///�v�����I�����A���ʂ�Maya�Ƀ��|�[�g�o��
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