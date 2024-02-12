#pragma once
#include <burgess2_main/Core/base.h>
#include <cereal/archives/json.hpp>

//******************************************************************************
//! @file	JsonIO.h
//! @brief	 JSON�t�@�C���̏������݁A�ǂݍ���
//******************************************************************************
/** @defgroup FileIO
*�@@{
*/
/**		@brief	�N���X�f�[�^��JSON�t�@�C���֏������݁A�ǂݍ��� <br>
*			#include <burgess2_main/FileIO.h> <br>
*
*			@code
* �@�@�@�@�@�@�@�@// Burgess2�ȊO�̃X�g���N�g��N���X��FileIO�ň��������ꍇ�́A��`��cereal�̃Z�b�g�A�b�v���L�q����
*				// �����o�ϐ������̂܂܈����̂ł����serialize()��OK
*				// �ۑ��f�[�^�̏����⃍�[�h���̏����A�ۑ��f�[�^�����ʂŐݒ肵�����ꍇ��save()/load()�֐���ݒ�iVecT<T>�̃\�[�X�R�[�h���Q�Ɓj
*				struct ClassAAA{
*					Xfo xfo;
*					Vec3 v3;
*								
*					// cereal setup 
*					template<class cerealArchive>
*					void serialize ( cerealArchive &archive ){
*						archive (
*							CEREAL_NVP ( xfo ),
*							CEREAL_NVP ( v3 )
*						);
*					}
*				};
*				
*				ClassAAA val;
*				val.xfo = ....;
*				val.v3 = ....;
*
*				// �ۑ����o�^����
*				String path = "D:/temp/file.json";
*				JsonIO <ClassAAA> jsonIO ( path );
*
*				// ��������
*				jsonIO.setWritable (); // �������݃��[�h�ɂ���
*				jsonIO.write ( val ); // �f�[�^�̏�������
*				
*				// JSON��IO�����̏I����錾����
*				jsonIO.close();
*				
*				///////////////////////////////////////////////////
*				
*				// ���[�h���o�^����
*				String path2 = "D:/temp/file.json";
*				JsonIO <ClassAAA> jsonIO2 ( path2 );
*				//�@�ǂݍ���
*				ClassAAA val2; // �ǂݍ��ݐ�̃f�[�^������
*				jsonIO2.setReadable (); // �ǂݍ��݃��[�h�ɂ���
*				jsonIO2.read ( val2 ); // JSON�t�@�C������f�[�^�����[�h����
*
*				// JSON��IO�����̏I����錾����
*				jsonIO2.close();
*			@endcode
*/
template<class C>
class JsonIO{
	String path;
	u8 mode = u8 ( 0 ); // 0=����, 1=��������, 2=�ǂݍ���
public:
	std::ifstream fin;
	std::ofstream fout;

	JsonIO (){};
	JsonIO ( const String &path );
	virtual  ~JsonIO (){}

	void setPath ( const String &path );
	Bool setWritable ();
	Bool setReadable ();
	void close ();

	void write ( const C &val );
	void read ( OUT C &val );
};

#include "JsonIO.inl"
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************