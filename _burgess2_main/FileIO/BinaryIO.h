#pragma once
#include <burgess2_main/Core/base.h>
#include <cereal/archives/binary.hpp>

//******************************************************************************
//! @file	BinaryIO.h
//! @brief	 �o�C�i���t�@�C���̏������݁A�ǂݍ���
//******************************************************************************
/** @defgroup FileIO
*�@@{
*/
/**		@brief	struct/class�f�[�^���o�C�i���t�@�C���֏������݁A�ǂݍ��� <br>
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
*				String path = "D:/temp/file.bin";
*				BinaryIO <ClassAAA> binIO ( path );
*
*				// ��������
*				binIO.setWritable (); // �������݃��[�h�ɂ���
*				binIO.write ( val ); // �f�[�^�̏�������
*				
*				// �o�C�i����IO�����̏I����錾����
*				binIO.close();
*				
*				///////////////////////////////////////////////////
*				
*				// ���[�h���o�^����
*				String path2 = "D:/temp/file.bin";
*				BinaryIO <ClassAAA> binIO2 ( path2 );
*				//�@�ǂݍ���
*				ClassAAA val2; // �ǂݍ��ݐ�̃f�[�^������
*				binIO2.setReadable (); // �ǂݍ��݃��[�h�ɂ���
*				binIO2.read ( val2 ); // �o�C�i���t�@�C������f�[�^�����[�h����
*
*				// �o�C�i����IO�����̏I����錾����
*				binIO2.close();
*			@endcode
*/
template<class C>
class BinaryIO{
	String path;
	u8 mode = u8 ( 0 ); // 0=����, 1=��������, 2=�ǂݍ���
public:
	std::ifstream fin;
	std::ofstream fout;

	BinaryIO (){};
	BinaryIO ( const String &path );
	virtual  ~BinaryIO (){}

	void setPath ( const String &path );
	Bool setWritable ();
	Bool setReadable ();
	void close ();

	void write ( const C &val );
	void read ( OUT C &val );
};

#include "BinaryIO.inl"
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************