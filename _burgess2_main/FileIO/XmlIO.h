#pragma once
#include <burgess2_main/Core/base.h>
#include <cereal/archives/xml.hpp>

//******************************************************************************
//! @file	XmlIO.h
//! @brief	 xml�t�@�C���̏������݁A�ǂݍ���
//******************************************************************************
/** @defgroup FileIO
*�@@{
*/
/**		@brief	�N���X�f�[�^��xml�t�@�C���֏������݁A�ǂݍ���<br>
*			#include <burgess2_main/FileIO.h> <br>
*
*			@code
* �@�@�@�@�@�@�@�@// Burgess2�ȊO�̃X�g���N�g��N���X��FileIO�ň��������ꍇ�́A��`��cereal�̃Z�b�g�A�b�v���L�q����
*				// �����o�ϐ������̂܂܈����̂ł����serialize()��OK�iBinaryIO.h�̃T���v���R�[�h���Q�Ɓj
*				// �ۑ��f�[�^�̏����⃍�[�h���̏����A�ۑ��f�[�^�����ʂŐݒ肵�����ꍇ��save()/load()�֐���ݒ�iVecT<T>�̃\�[�X�R�[�h���Q�Ɓj
*				Xfo val;
*				val.tr = ....;
*				val.ori = ....;
*
*				// �ۑ����o�^����
*				String path = "D:/temp/file.xml";
*				XmlIO <Xfo> xmlIO ( path );
*
*				// ��������
*				xmlIO.setWritable (); // �������݃��[�h�ɂ���
*				xmlIO.write ( val ); // �f�[�^�̏�������
*
*				// Xml��IO�����̏I����錾����
*				xmlIO.close();
*
*				///////////////////////////////////////////////////
*
*				// ���[�h���o�^����
*				String path2 = "D:/temp/file.xml";
*				XmlIO <Xfo> xmlIO2 ( path2 );
*				//�@�ǂݍ���
*				Xfo val2; // �ǂݍ��ݐ�̃f�[�^������
*				xmlIO2.setReadable (); // �ǂݍ��݃��[�h�ɂ���
*				xmlIO2.read ( val2 ); // Xml�t�@�C������f�[�^�����[�h����
*
*				// JSON��IO�����̏I����錾����
*				xmlIO2.close();
*			@endcode
*/
template<class C>
class XmlIO{
	String path;
	u8 mode = u8 ( 0 ); // 0=����, 1=��������, 2=�ǂݍ���
public:
	std::ifstream fin;
	std::ofstream fout;

	XmlIO (){};
	XmlIO ( const String &path );
	virtual  ~XmlIO (){}

	void setPath ( const String &path );
	Bool setWritable ();
	Bool setReadable ();
	void close ();

	void write ( const C &val );
	void read ( OUT C &val );
};

#include "XmlIO.inl"
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************