#pragma once
#include <burgess2_main/FileIO.h>

#include <maya/MPxData.h>
#include <maya/MTypeId.h>
#include <maya/MGlobal.h>

//******************************************************************************
//! @file	Burgess2MayaData.h
//! @brief	 C++�J�X�^���N���X�f�[�^�����̂܂܃m�[�h�v���O�ň������߂̃N���X
//******************************************************************************
/** @defgroup Maya_Integration
*�@@{
*/
/**		@brief	C++�J�X�^���N���X�f�[�^�����̂܂܃m�[�h�v���O�ň������߂̃N���X<br>
 *			C++�J�X�^���N���X�ɂ�ceral�̃A�[�J�C�u�Z�b�g�A�b�v���K�v<br>
*			#include <burgess2_maya/Integration.h><br>
*/

namespace{
	class Burgess2MayaData : public MPxData{
	public:
		std::stringstream data;

		static MString typeName;
		static MTypeId id;

		Burgess2MayaData (){}//
		virtual ~Burgess2MayaData (){}

		virtual MStatus readASCII ( const MArgList & argList, unsigned & idx ){ return MS::kSuccess; }
		virtual MStatus readBinary ( std::istream & in, unsigned length ){ return MS::kSuccess; }
		virtual MStatus writeASCII ( std::ostream & out ){ return MS::kSuccess; }
		virtual MStatus writeBinary ( std::ostream & out ){ return MS::kSuccess; }

		virtual MString name () const{ return this->typeName; }
		virtual MTypeId typeId () const{ return this->id; }
		static void * creator (){ return new Burgess2MayaData (); }

		void setVal ( const std::stringstream &val ){ this->data << val.rdbuf (); }
		void getVal ( OUT std::stringstream &val ){ val << this->data.rdbuf (); }
		virtual void copy ( const MPxData & other ){
			if ( &other != this ){
				const Burgess2MayaData& _other = ( const Burgess2MayaData & ) other;
				this->data << _other.data.rdbuf ();
			}
		}
	};

	MTypeId Burgess2MayaData::id ( 0x50907060 );
	MString Burgess2MayaData::typeName ( MString ( "customDataName" ) );
}

/** @} */
//******************************************************************************
//! End of File
//******************************************************************************