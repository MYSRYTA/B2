#include "sample_manip.h"

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// �J�X�^���}�j�s�����[�^

MTypeId MeshComponentScaleManip::id( 0x80021 );

MStatus MeshComponentScaleManip::createChildren(){
	MStatus stat = MStatus::kSuccess;

	// �����̃X�P�[���}�j�s�����[�^���R���e�i�ɓo�^����
	this->fScaleManip = this->addScaleManip("scaleManip", "sc");

	// ��������̓X�P�[���ȊO�̑g�ݍ��݃}�j�s�����[�^��o�^����i����͏o���邪�����͂��Ȃ��j
	this->fRotateManip = this->addRotateManip("rotateManip", "rot");
	this->fTranseManip = this->addFreePointTriadManip("transeManip", "tr");

	return stat;
}

MStatus MeshComponentScaleManip::connectToDependNode(const MObject &node){
	MStatus stat;
	
	// �����Ώۂ̒��_���X�g���ύX����Ă���΃����o�ϐ������t���b�V������
	// ���t���b�V�����邩�̔�������X�g�̐��ōs���Ă��邪�A���m�ɂ͒��_�ԍ����`�F�b�N����ׂ�
	MFnComponent componentFn(this->component);
	if (this->numComponents < componentFn.elementCount()){
		delete [] this->initialPositions;
		delete [] this->initialControlPoint;
		this->numComponents = componentFn.elementCount();
		this->initialPositions = new MPoint[this->numComponents];
		this->initialControlPoint = new MPoint[this->numComponents];
	}

	// �����Ώۂ̒��_�Ƒ΂ɂȂ�V�F�[�v�m�[�h��pnts�A�g���r���[�g�̒l�������o�ϐ��Ɋi�[���Ă���
	// pnts�A�g���r���[�g���}�j�s�����[�^�ƕR�Â�����
	// �}�j�s�����[�^��\������ʒu(=�����Ώۂ̒��_�Q�̒����j���v�Z����
	int i = 0;
	for (MItMeshVertex iter(this->surfaceDagPath, this->component); !iter.isDone(); iter.next(), i++){
		if (this->numComponents <= i)	{
			MGlobal::displayWarning("More components found than expected.");
			break;
		}

		// pnts�v���O�����邩�m�F����
		MFnDependencyNode nodeFn(node);
		MPlug controlPointArrayPlug = nodeFn.findPlug("pnts",  true,  &stat);
		if (controlPointArrayPlug.isNull())	{
			MGlobal::displayError("mesh shape pnts plug not found on node.");
			return MS::kFailure;
		}

		// �O�̂��ߒ��_�ԍ��Ɠ����z��ԍ���pnts�v���O���L�������m�F����
		MPlug controlPointPlug = controlPointArrayPlug.elementByLogicalIndex(iter.index(), &stat);
		if (controlPointPlug.isNull()) {
			MGlobal::displayError("Control point element not found.");
			return MS::kFailure;
		}

		// ���_�ʒu���i�[���Ă���
		this->initialPositions[i] = iter.position(MSpace::kWorld);

		// �����ʒu���Z�o���邽�߂ɒ��_�ʒu�����Z���Ă���
		this->centroid += this->initialPositions[i];

		// pnts�v���O�̒l���i�[���Ă���
		this->initialControlPoint[i] = vectorPlugValue(controlPointPlug);

		// �}�j�s�����[�^����pnts�v���O�֒l���󂯓n���ۂ�manipToPlugConversion()���Ăԏo���悤�ɐݒ肷��
		unsigned plugIndex = addManipToPlugConversion(controlPointPlug);

		// �����}�j�s�����[�^��pnts�v���O�̕R�Â������s���Ă����ꍇ�̓G���[
		if (plugIndex != (unsigned)i){
			MGlobal::displayError("Unexpected plug index returned.");
			return MS::kFailure;
		}
	}
	// �����ʒu���Z�o����
	this->centroid = this->centroid / this->numComponents;

	// pnts�v���O����}�j�s�����[�^�֒l���󂯓n���ۂ�plugToManipConversion()���Ăԏo���悤�ɐݒ肷��
	MFnScaleManip scaleManipFn(this->fScaleManip);
	addPlugToManipConversion(scaleManipFn.scaleCenterIndex());

	// �I���錾
	finishAddingManips();
	MPxManipContainer::connectToDependNode(node);
	return stat;
}

MManipData MeshComponentScaleManip::plugToManipConversion(unsigned index) {
	MObject obj = MObject::kNullObj;
	
	// �}�j�s�����[�^�̕`��ʒu���X�V����
	MFnScaleManip scaleManipFn(this->fScaleManip);
	if (index == scaleManipFn.scaleCenterIndex()){
		// �}�j�s�����[�^�̐V�����`��ʒu��Ԃ�
		MFnNumericData numericData;
		obj = numericData.create( MFnNumericData::k3Double );
		numericData.setData(this->centroid.x,this->centroid.y,this->centroid.z);
		return MManipData(obj);
	}

	// �G���[�ƂȂ����ꍇ�ɕs���l���Ԃ�Ȃ��悤�ɂ��Ă���
	MGlobal::displayError("Invalid index in plugToManipConversion()!");	
	MFnNumericData numericData;
	obj = numericData.create( MFnNumericData::k3Double );
	numericData.setData(0.0,0.0,0.0);
	return obj;
}

MManipData MeshComponentScaleManip::manipToPlugConversion(unsigned index) {
	MObject obj = MObject::kNullObj;

	MFnScaleManip scaleManipFn(this->fScaleManip);
	if (index < (unsigned)this->numComponents){
		// �}�j�s�����[�^�̑��쌋�ʂ��擾����
		MVector scVal;
		getConverterManipValue(scaleManipFn.scaleIndex(), scVal);

		// �}�j�s�����[�^�̕`��ʒu���璸�_�ʒu�܂ł̍������Z�o����
		MVector positionVec = initialPositions[index] - centroid;

		// �������X�P�[�����O����
		MVector newPosition(positionVec.x*scVal.x, positionVec.y*scVal.y, positionVec.z*scVal.z);

		// �����l����̕ω��ʂ��Z�o
		newPosition = newPosition - positionVec;

		// pnts�A�g���r���[�g�̐V�����l��Ԃ�
		newPosition += initialControlPoint[index];
		MFnNumericData numericData;
		obj = numericData.create( MFnNumericData::k3Double );
		numericData.setData(newPosition.x,newPosition.y,newPosition.z);
		return MManipData(obj);
	}

	// �G���[�ƂȂ����ꍇ�ɕs���l���Ԃ�Ȃ��悤�ɂ��Ă���
	MGlobal::displayError("Invalid index in scale changed callback!");
	MFnNumericData numericData;
	obj = numericData.create( MFnNumericData::k3Double );
	numericData.setData(0.0,0.0,0.0);
	return obj;
	
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// �J�X�^���}�j�s�����[�^�p�̃R���e�L�X�g�ƋN���R�}���h

ComponentScaleContext::ComponentScaleContext()
{
	MString str("Plugin component scaling manipulator");
	setTitleString(str);
}


void ComponentScaleContext::toolOnSetup(MEvent &)
{
	MString str("Scale the selected components");
	setHelpString(str);

	updateManipulators(this);
	MStatus status;
	id1 = MModelMessage::addCallback(MModelMessage::kActiveListModified, updateManipulators, this, &status);
	if (!status) {
		MGlobal::displayError("Model addCallback failed");
	}
}


void ComponentScaleContext::toolOffCleanup()
{
	MStatus status;
	status = MModelMessage::removeCallback(id1);
	if (!status) {
		MGlobal::displayError("Model remove callback failed");
	}
	MPxContext::toolOffCleanup();
}


void ComponentScaleContext::updateManipulators(void * data)
{
	MStatus stat = MStatus::kSuccess;
	
	// �o�^�ς݂̃}�j�s�����[�^����U�폜
	ComponentScaleContext * ctxPtr = (ComponentScaleContext *) data;
	ctxPtr->deleteManipulators(); 

	// ���݂̒��_�I���̏�ԂŃ}�j�s�����[�^����蒼��
	MSelectionList list;
	stat = MGlobal::getActiveSelectionList(list);
	MItSelectionList iter(list, MFn::kInvalid, &stat);

	if (MS::kSuccess == stat) {
		for (; !iter.isDone(); iter.next()) {
			// �R���|�[�l���g���I������Ă��Ȃ��ꍇ�͌x�����o���ăX�L�b�v
			MDagPath dagPath;
			MObject components;
			iter.getDagPath(dagPath, components);
			if (components.isNull() || !components.hasFn(MFn::kComponent)) {
				MGlobal::displayWarning("Object in selection list is not a component.");
				continue;
			}

			// �}�j�s�����[�^��V�K�쐬
			MString manipName ("MeshComponentScaleManip");
			MObject manipObject;
			MeshComponentScaleManip* manipulator = (MeshComponentScaleManip *) MeshComponentScaleManip::newManipulator(manipName, manipObject);

			if (NULL != manipulator) {
				// �V�K�쐬�����������ꍇ�A�R���e�L�X�g�ɓo�^����
				ctxPtr->addManipulator(manipObject);

				// �p�X�ƃR���|�[�l���g��ϐ��Ɋi�[����
				manipulator->setSurfaceDagPath(dagPath);
				manipulator->setComponentObject(components);
				
				// �}�j�s�����[�^��R�Â���
				if (!manipulator->connectToDependNode(dagPath.node())) {
					MGlobal::displayWarning("Error connecting manipulator to object");
				}
			} 
		}
	}
}

///////////////////////////////////////////////////////////////////////
MVector vectorPlugValue(const MPlug& plug){
	if (plug.numChildren() == 3){
		double x, y, z;
		MPlug rx = plug.child(0);
		MPlug ry = plug.child(1);
		MPlug rz = plug.child(2);
		rx.getValue(x);
		ry.getValue(y);
		rz.getValue(z);
		MVector result(x, y, z);
		return result;
	} else{
		MGlobal::displayError("Expected 3 children for plug " + MString(plug.name()));
		MVector result(0, 0, 0);
		return result;
	}
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// �R���e�L�X�g�ƃ}�j�s�����[�^�m�[�h���Z�b�g�Ńv���O�C���o�^����

MStatus initializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj, PLUGIN_COMPANY, "6.0", "Any");

	status = plugin.registerContextCommand(	"MeshComponentScaleContext",
											MeshComponentScaleContext::creator);
	// Undo/Redo���܂ރc�[���R�}���h��R�Â�����ꍇ�͈���3�C4�ڂɒǉ�

	if (!status) {
		MGlobal::displayError("Error registering MeshComponentScaleContext command");
		return status;
	}

	status = plugin.registerNode(	"MeshComponentScaleManip", 
									MeshComponentScaleManip::id, 
									&MeshComponentScaleManip::creator, 
									&MeshComponentScaleManip::initialize,
									MPxNode::kManipContainer);
	if (!status) {
		MGlobal::displayError("Error registering MeshComponentScaleManip node");
		return status;
	}

	return status;
}


MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterContextCommand("MeshComponentScaleContext");
	if (!status) {
		MGlobal::displayError("Error deregistering MeshComponentScaleContext command");
		return status;
	}

	status = plugin.deregisterNode(MeshComponentScaleManip::id);
	if (!status) {
		MGlobal::displayError("Error deregistering MeshComponentScaleManip node");
		return status;
	}

	return status;
}

