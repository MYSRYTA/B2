#include "CustomLocator.h"

MTypeId PGCustomLocator::id ( CustomLocator_id );

MObject PGCustomLocator::icon;
MObject PGCustomLocator::size;
MObject PGCustomLocator::color0;
MObject PGCustomLocator::color1;
MObject PGCustomLocator::offsetTr;
MObject PGCustomLocator::offsetRot;
MObject PGCustomLocator::wire;
MObject PGCustomLocator::face;
MString	PGCustomLocator::drawDbClassification ( "drawdb/geometry/PGCustomLocator"  );
MString	PGCustomLocator::drawRegistrantId ( "PGCustomLocatorPlugin" );

// �`�揈����ON/OFF�̂��߂�BBox
MBoundingBox PGCustomLocator::boundingBox () const
{
	MObject thisNode = thisMObject ();

	Bool wireD, faceD;
	MPlug wP ( thisNode, this->wire );
	if (!wP.isNull ()){ wireD = wP.asBool(); }
	MPlug fP ( thisNode, this->face );
	if (!fP.isNull ()){ faceD = fP.asBool (); }

	s32 _iconType;
	MPlug iP ( thisNode, this->icon );
	if (!iP.isNull ()){ _iconType = iP.asInt (); }
	// �o�E���f�B���O�{�b�N�X���쐬
	BBox bbox;
	if(wireD == false && faceD == false){
		// �����`�悵�Ȃ��ꍇ�̓o�E���f�B���O�{�b�N�X���쐬�����ɏI��
		return MBoundingBox ();
	} else{
		// �A�C�R���̃|�C���g���X�g����o�E���f�B���O�{�b�N�X���쐬����
		std::vector<Vec3> poi;
		switch (_iconType){
			case 0: poi = linePoint; break;
			case 1:	poi = rectPoint; break;
			case 2: poi = circlePoint; break;
			case 3: poi = circlePoint2; break;
			case 4: poi = circleHalfPoint; break;
			case 5: poi = rectArrowPoint; break;
			case 6: poi = trianglePoint; break;
			case 7: break;
			case 8: break;
			case 9: break;
			case 10: poi = boxPoint; break;
			case 11: poi = slopePoint; break;
			case 12: poi = trapezoidPoint; break;
			case 13: poi = pyramidPoint; break;
			case 14: poi = diamondPoint; break;
			case 15: poi = ringPoint; break;
			case 16: poi = gravityPoint; break;
			case 17: poi = windPoint; break;
			case 18: poi = solverPoint; break;
			default: poi = rectPoint; break;
		}
		// �I�t�Z�b�g�g�����X�t�H�[��
		Vec3 inTr, inSc;
		Rad3 inRot;
		MPlug scP ( thisNode, this->size );
		if (!scP.isNull ()){ for (u32 i = 0; i < 3; i++){ inSc[i] = float ( scP.child ( i ).asDouble () ); } }
		MPlug trP ( thisNode, this->offsetTr );
		if (!trP.isNull ()){ for (u32 i = 0; i < 3; i++){ inTr[i] = float ( trP.child ( i ).asDouble () ); } }
		MPlug rotP ( thisNode, this->offsetRot );
		if (!rotP.isNull ()){ for (u32 i = 0; i < 3; i++){ inRot[i] = float ( rotP.child ( i ).asDouble () ); } }
		Xfo offset ( inSc, Quat ( inRot ), inTr );
		for (u32 i = 0; i < poi.size (); i++){
			poi[i] = offset * poi[i];
		}
		bbox = BBox ( poi );
		return MBoundingBox ( ConvertMFloatVector ( bbox.min ), ConvertMFloatVector ( bbox.max ) );
	}
}

// �m�[�h�̓��̓|�[�g�̕ύX�̗L�����Ď�����
MStatus PGCustomLocator::setDependentsDirty ( const MPlug& plug, MPlugArray& )
{
	const MPlug dirtyPlug = plug.isChild () ? plug.parent () : plug;
	if (dirtyPlug == this->size || dirtyPlug == this->icon || dirtyPlug == color0 || dirtyPlug == color1) {
		this->valueDirty = true;
	}
	return MS::kSuccess;
}

// ���񏈗��̍ۂɃX���b�h�̃\�[�X�ƂȂ�|�[�g�̕ύX�̗L�����Ď�����
MStatus PGCustomLocator::preEvaluation (
	const MDGContext& context,
	const MEvaluationNode& evaluationNode )
{
	if (!this->valueDirty){
		for (MEvaluationNodeIterator it = evaluationNode.iterator (); !it.isDone (); it.next ()) {
			const MPlug dirtyPlug = it.plug ();
			if (dirtyPlug == this->size || dirtyPlug == this->icon || dirtyPlug == color0 || dirtyPlug == color1) {
				this->valueDirty = true;
				break;
			}
		}
	}

	return MStatus::kSuccess;
}

MStatus PGCustomLocator::compute ( const MPlug& plug, MDataBlock& data ){
	if(this->valueDirty) {
		Input ( this->iconType, data, icon );
		this->valueDirty = false;
	}

	return MS::kUnknownParameter;
};

// �A�g���r���[�g�쐬�@(initializePlugin()�Ŏg�p)
MStatus PGCustomLocator::initialize () {
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnMatrixAttribute mAttr;
	MFnCompoundAttribute cAttr;
	MFnUnitAttribute uAttr;

	// �A�g���r���[�g�̃f�[�^�^�Əڍאݒ�
	icon = nAttr.create ( "icon", "icon", MFnNumericData::kInt );
	nAttr.setDefault ( 1 );
	size = nAttr.create ( "size", "size", MFnNumericData::k3Double );
	nAttr.setDefault ( 1.0, 1.0, 1.0 );
	nAttr.setKeyable ( true );
	color0 = nAttr.createColor ( "color0", "color0" );
	nAttr.setDefault ( 0.5f, 1.0f, 0.0f );
	nAttr.setKeyable ( true );
	nAttr.setUsedAsColor ( true );
	color1 = nAttr.createColor ( "color1", "color1" );
	nAttr.setDefault ( 0.7f, 1.0f, 0.3f );
	nAttr.setKeyable ( true );
	nAttr.setUsedAsColor ( true );
	offsetTr = nAttr.create ( "offsetTr", "offsetTr", MFnNumericData::k3Double );
	nAttr.setKeyable ( true );
	offsetRot = CreateEulerPort ( cAttr, "offsetRot" );
	cAttr.setKeyable ( true );
	wire = nAttr.create ( "wire", "wire", MFnNumericData::kBoolean );
	nAttr.setDefault ( true );
	nAttr.setKeyable ( true );
	face = nAttr.create ( "face", "face", MFnNumericData::kBoolean );
	nAttr.setKeyable ( true );

	// �J�X�^���A�g���r���[�g�쐬
	addAttribute ( icon );
	addAttribute ( size );
	addAttribute ( color0 );
	addAttribute ( color1 );
	addAttribute ( offsetTr );
	addAttribute ( offsetRot );
	addAttribute ( wire );
	addAttribute ( face );

	return MS::kSuccess;
}

