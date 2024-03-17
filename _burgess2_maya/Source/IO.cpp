#include "IO.h"

#if _MSC_VER > 1922 && !defined(_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include <experimental/filesystem>

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Int port �� s32
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT s32 &s, MDataBlock& dataBlock, const MObject &mS32 ) {
	s = dataBlock.inputValue ( mS32 ).asInt ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Int[] port �� s32[]
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input( OUT std::vector<s32>& s, MDataBlock& dataBlock, const MObject& mS32 ){
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue( mS32 );
	s.resize( arrayHandle.elementCount() );
	for ( u32 i = 0; i < arrayHandle.elementCount(); ++i ){
		arrayHandle.jumpToElement( i );
		s[i] = arrayHandle.inputValue().asInt();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		float port �� f32
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT f32 &f, MDataBlock& dataBlock, const MObject &mF32 ) {
	f = dataBlock.inputValue ( mF32 ).asFloat ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		float[] port �� f32[]
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input( OUT std::vector<f32>& f, MDataBlock& dataBlock, const MObject& mF32 ){
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue( mF32 );
	f.resize( arrayHandle.elementCount() );
	for ( u32 i = 0; i < arrayHandle.elementCount(); ++i ){
		arrayHandle.jumpToElement( i );
		f[i] = arrayHandle.inputValue().asFloat();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		double port �� f64
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT f64 &f, MDataBlock& dataBlock, const MObject &mF64 ) {
	f = dataBlock.inputValue ( mF64 ).asDouble ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		double[] port �� f64[]
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input( OUT std::vector<f64>& f, MDataBlock& dataBlock, const MObject& mF64 ){
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue( mF64 );
	f.resize( arrayHandle.elementCount() );
	for ( u32 i = 0; i < arrayHandle.elementCount(); ++i ){
		arrayHandle.jumpToElement( i );
		f[i] = arrayHandle.inputValue().asDouble();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		boolean port �� Bool
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT Bool &b, MDataBlock& dataBlock, const MObject &mBool ) {
	b = dataBlock.inputValue ( mBool ).asBool ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		boolean[] port �� Bool[]
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input( OUT std::vector<Bool>& b, MDataBlock& dataBlock, const MObject& mBool ){
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue( mBool );
	b.resize( arrayHandle.elementCount() );
	for ( u32 i = 0; i < arrayHandle.elementCount(); ++i ){
		arrayHandle.jumpToElement( i );
		b[i] = arrayHandle.inputValue().asBool();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MString port �� String
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT String &s, MDataBlock& dataBlock, const MObject &mStr ) {
	s = Convert( dataBlock.inputValue ( mStr ).asString () );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MString port �� String
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input( OUT std::vector<String>& s, MDataBlock& dataBlock, const MObject& mStr ){
	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue( mStr );
	s.resize( arrayHandle.elementCount() );
	for ( u32 i = 0; i < arrayHandle.elementCount(); ++i ){
		arrayHandle.jumpToElement( i );
		s[i] = Convert( arrayHandle.inputValue().asString() );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Bool[] �� Maya bool [] port
//------------------------------------------------------------------------------------------------------------------------------------------------
void Output ( MObject &mBool, OUT MDataBlock& dataBlock, const std::vector<Bool> &b ){
	MArrayDataHandle arrayHandle = dataBlock.outputArrayValue ( mBool );
	MArrayDataBuilder arrayBuilder = arrayHandle.builder ();
	s32 n = s32( b.size() );
	for ( s32 i = 0; i < n; ++i ){
		arrayBuilder.addElement ( i ).set ( b[i] );
		arrayBuilder.addElement ( i ).setClean ();
	}
	arrayHandle.set ( arrayBuilder );
	arrayHandle.setAllClean ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		**Maya2018�ł�arrayInput�̓��I�|�[�g�Ƀo�O�����邽�ߎg�p�s�� Float2DArray(compoundAttr) port �� float 2DArray
//!		@param[out] val
//!		@param[in] dataBlock
//!		@param[in] compVal compoundAttr��MObject
//!		@param[in] arrayVal�@FloatArry��MObject
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT std::vector < std::vector<f32>> &val, 
			 MDataBlock& dataBlock, 
			 const MObject &compVal, 
			 const MObject &arrayVal )
{
	MStatus status{ MStatus::kFailure };
	val.resize ( 0 );

	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( compVal );
	u32 compSize = arrayHandle.elementCount ();
	u32 i{ 0 }, compCnt{ 0 }, valMaxSize{ 0 };
	while( compCnt < compSize ){
		std::vector<f32> rest;
		status = arrayHandle.jumpToElement ( i ); // �z��|�[�g�ɓ��͂����邩���Ƀ`�F�b�N���Ă���
		if ( status != MStatus::kSuccess ){ // ���͂�������΃X���[
			val.push_back ( rest );
			i++;
			continue; 
		}
		
		MDataHandle eHandle = arrayHandle.inputValue ().child ( arrayVal );
		MArrayDataHandle eArrayHandle ( eHandle ); // floatArray�̃|�[�g�z��ւ̃n���h�����擾
		u32 valSize = eArrayHandle.elementCount ();
		u32 j{ 0 }, valCnt{ 0 };
		while( valCnt < valSize ){
			status = eArrayHandle.jumpToElement ( j ); // �z��|�[�g�ɓ��͂����邩���Ƀ`�F�b�N���Ă���
			if ( status != MStatus::kSuccess ){
				rest.push_back ( 0.0f );
				j++;
				continue;
			} // ���͂�������΃X���[
			rest.push_back( eArrayHandle.inputValue ().asFloat () );
			valCnt++;
			j++;
		}
		val.push_back ( rest );
		valMaxSize = std::max ( valMaxSize, u32(rest.size () ));
		compCnt++;
		i++;
	}

	for(u32 i=0; i<val.size(); ++i){
		val[i].resize ( valMaxSize );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Float2DArray(compoundAttr) port �� SkinningAttribute
//!		@param[io] mesh
//!		@param[in] dataBlock
//!		@param[in] weightList weightList(compoundAttr)��MObject
//!		@param[in] weights weights(valueArray)��MObject
//!		@param[in] attrName SkinningAttribute�̖��O�imesh�ɓ����̃A�g���r���[�g�����݂��Ȃ���ΐV�K�쐬����j
//------------------------------------------------------------------------------------------------------------------------------------------------
void InputSkinWeightList ( IO PolygonMesh &mesh,
						   MDataBlock& dataBlock,
						   const MObject &weightList,
						   const MObject &weights,
						   const String &attrName )
{
	if ( mesh.pointCount () == 0 ){ return; }

	MStatus status{ MStatus::kFailure };
	SkinningAttribute &skinAttr = mesh.getOrCreateSkinningAttribute ( attrName );
	skinAttr.resize ( mesh.pointCount () );

	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( weightList );
	for ( u32 i = 0; i < skinAttr.size(); ++i ){
		status = arrayHandle.jumpToElement ( i ); // �z��|�[�g�ɓ��͂����邩���Ƀ`�F�b�N���Ă���
		if ( status != MStatus::kSuccess ){ i++; continue; } // ���͂�������΃X���[

		SkinningPairData pair;
		MDataHandle eHandle = arrayHandle.inputValue ().child ( weights );
		MArrayDataHandle eArrayHandle ( eHandle ); // floatArray�̃|�[�g�z��ւ̃n���h�����擾
		s32 elementSize = eArrayHandle.elementCount (), valCnt{ 0 };
		u32 j{ 0 };
		while ( valCnt < elementSize){
			status = eArrayHandle.jumpToElement ( j ); // �z��|�[�g�ɓ��͂����邩���Ƀ`�F�b�N���Ă���
			if ( status == MStatus::kSuccess ){ // ���͂�����΃A�g���r���[�g�ɓo�^
				f32 weight = eArrayHandle.inputValue ().asFloat ();
				if ( 0.00001 < weight ){ // maya2018�̓��I�z��|�[�g�̃o�O�Ή�
					pair.addPair ( j, weight ); // �W���C���g/�{�[���z��ԍ��@+�@�E�F�C�g�l�̃y�A��o�^
				}
				valCnt++;
			}
			j++;
		}
		skinAttr.setValue ( i, pair );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Float2DArray(compoundAttr) port �� float 2DArray
//!		@param[out] val
//!		@param[in] dataBlock
//!		@param[in] compVal compoundAttr��MObject
//!		@param[in] arrayVal�@FloatArry��MObject
//!		@param[in] compSize compVal�̔z�񐔁@val��1�����ڂ̔z�񐔂Ƃ��Ă��g�p
//!		@param[in] valSize arrayVal�̔z�񐔁@val��2�����ڂ̔z�񐔂Ƃ��Ă��g�p
//------------------------------------------------------------------------------------------------------------------------------------------------
void Input ( OUT std::vector < std::vector<f32>> &val,
			 MDataBlock& dataBlock,
			 const MObject &compVal,
			 const MObject &arrayVal,
			 u32 compSize,
			 u32 valSize )
{
	MStatus status{ MStatus::kFailure };
	val.resize ( compSize );

	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( compVal );
	for ( u32 i = 0; i < compSize; ++i ){
		std::vector<f32> rest( valSize, 0.0f );
		status = arrayHandle.jumpToElement ( i ); // �z��|�[�g�ɓ��͂����邩���Ƀ`�F�b�N���Ă���
		if ( status != MStatus::kSuccess ){ val[i] = rest; continue; }// ���͂�������΃X���[

		MDataHandle eHandle = arrayHandle.inputValue ().child ( arrayVal );
		MArrayDataHandle eArrayHandle ( eHandle ); // floatArray�̃|�[�g�z��ւ̃n���h�����擾
		for ( u32 j = 0; j < valSize; ++j ){
			status = eArrayHandle.jumpToElement ( j ); // �z��|�[�g�ɓ��͂����邩���Ƀ`�F�b�N���Ă���
			rest[j] = status == MStatus::kSuccess ? eArrayHandle.inputValue ().asFloat () : 0.0f;
		}
		val[i] = rest;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		Float2DArray(compoundAttr) port �� SkinningAttribute
//!		@param[io] mesh
//!		@param[in] dataBlock
//!		@param[in] weightList weightList(compoundAttr)��MObject
//!		@param[in] weights weights(valueArray)��MObject
//!		@param[in] jointSize �W���C���g��
//!		@param[in] attrName SkinningAttribute�̖��O�imesh�ɓ����̃A�g���r���[�g�����݂��Ȃ���ΐV�K�쐬����j
//------------------------------------------------------------------------------------------------------------------------------------------------
void InputSkinWeightList ( IO PolygonMesh &mesh,
						   MDataBlock& dataBlock,
						   const MObject &weightList,
						   const MObject &weights,
						   u32 jointSize,
						   const String &attrName ){
	if ( mesh.pointCount () == 0 ){ return; }

	MStatus status{ MStatus::kFailure };
	SkinningAttribute &skinAttr = mesh.getOrCreateSkinningAttribute ( attrName );
	skinAttr.resize ( mesh.pointCount () );

	MArrayDataHandle arrayHandle = dataBlock.inputArrayValue ( weightList );
	for ( u32 i = 0; i < skinAttr.size (); ++i ){
		status = arrayHandle.jumpToElement ( i ); // �z��|�[�g�ɓ��͂����邩���Ƀ`�F�b�N���Ă���
		if ( status != MStatus::kSuccess ){ continue; } // ���͂�������΃X���[

		SkinningPairData pair;
		MDataHandle eHandle = arrayHandle.inputValue ().child ( weights );
		MArrayDataHandle eArrayHandle ( eHandle ); // floatArray�̃|�[�g�z��ւ̃n���h�����擾
		for ( u32 j = 0; j < jointSize; ++j ){
			status = eArrayHandle.jumpToElement ( j ); // �z��|�[�g�ɓ��͂����邩���Ƀ`�F�b�N���Ă���
			if ( status != MStatus::kSuccess ){ continue; } // ���͂�������΃X���[
			pair.addPair ( j, eArrayHandle.inputValue ().asFloat () ); // �W���C���g/�{�[���z��ԍ��@+�@�E�F�C�g�l�̃y�A��o�^
		}
		skinAttr.setValue ( i, pair );
	}
}