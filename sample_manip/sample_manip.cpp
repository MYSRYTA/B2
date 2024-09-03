#include "sample_manip.h"

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// カスタムマニピュレータ

MTypeId MeshComponentScaleManip::id( 0x80021 );

MStatus MeshComponentScaleManip::createChildren(){
	MStatus stat = MStatus::kSuccess;

	// 既存のスケールマニピュレータをコンテナに登録する
	this->fScaleManip = this->addScaleManip("scaleManip", "sc");

	// ここからはスケール以外の組み込みマニピュレータを登録する（操作は出来るが処理はしない）
	this->fRotateManip = this->addRotateManip("rotateManip", "rot");
	this->fTranseManip = this->addFreePointTriadManip("transeManip", "tr");

	return stat;
}

MStatus MeshComponentScaleManip::connectToDependNode(const MObject &node){
	MStatus stat;
	
	// 処理対象の頂点リストが変更されていればメンバ変数をリフレッシュする
	// リフレッシュするかの判定をリストの数で行っているが、正確には頂点番号をチェックするべき
	MFnComponent componentFn(this->component);
	if (this->numComponents < componentFn.elementCount()){
		delete [] this->initialPositions;
		delete [] this->initialControlPoint;
		this->numComponents = componentFn.elementCount();
		this->initialPositions = new MPoint[this->numComponents];
		this->initialControlPoint = new MPoint[this->numComponents];
	}

	// 処理対象の頂点と対になるシェープノードのpntsアトリビュートの値をメンバ変数に格納していく
	// pntsアトリビュートをマニピュレータと紐づけする
	// マニピュレータを表示する位置(=処理対象の頂点群の中央）を計算する
	int i = 0;
	for (MItMeshVertex iter(this->surfaceDagPath, this->component); !iter.isDone(); iter.next(), i++){
		if (this->numComponents <= i)	{
			MGlobal::displayWarning("More components found than expected.");
			break;
		}

		// pntsプラグがあるか確認する
		MFnDependencyNode nodeFn(node);
		MPlug controlPointArrayPlug = nodeFn.findPlug("pnts",  true,  &stat);
		if (controlPointArrayPlug.isNull())	{
			MGlobal::displayError("mesh shape pnts plug not found on node.");
			return MS::kFailure;
		}

		// 念のため頂点番号と同じ配列番号でpntsプラグが有効かを確認する
		MPlug controlPointPlug = controlPointArrayPlug.elementByLogicalIndex(iter.index(), &stat);
		if (controlPointPlug.isNull()) {
			MGlobal::displayError("Control point element not found.");
			return MS::kFailure;
		}

		// 頂点位置を格納しておく
		this->initialPositions[i] = iter.position(MSpace::kWorld);

		// 中央位置を算出するために頂点位置を合算していく
		this->centroid += this->initialPositions[i];

		// pntsプラグの値を格納しておく
		this->initialControlPoint[i] = vectorPlugValue(controlPointPlug);

		// マニピュレータからpntsプラグへ値を受け渡す際にmanipToPlugConversion()を呼ぶ出すように設定する
		unsigned plugIndex = addManipToPlugConversion(controlPointPlug);

		// もしマニピュレータとpntsプラグの紐づけが失敗していた場合はエラー
		if (plugIndex != (unsigned)i){
			MGlobal::displayError("Unexpected plug index returned.");
			return MS::kFailure;
		}
	}
	// 中央位置を算出する
	this->centroid = this->centroid / this->numComponents;

	// pntsプラグからマニピュレータへ値を受け渡す際にplugToManipConversion()を呼ぶ出すように設定する
	MFnScaleManip scaleManipFn(this->fScaleManip);
	addPlugToManipConversion(scaleManipFn.scaleCenterIndex());

	// 終了宣言
	finishAddingManips();
	MPxManipContainer::connectToDependNode(node);
	return stat;
}

MManipData MeshComponentScaleManip::plugToManipConversion(unsigned index) {
	MObject obj = MObject::kNullObj;
	
	// マニピュレータの描画位置を更新する
	MFnScaleManip scaleManipFn(this->fScaleManip);
	if (index == scaleManipFn.scaleCenterIndex()){
		// マニピュレータの新しい描画位置を返す
		MFnNumericData numericData;
		obj = numericData.create( MFnNumericData::k3Double );
		numericData.setData(this->centroid.x,this->centroid.y,this->centroid.z);
		return MManipData(obj);
	}

	// エラーとなった場合に不正値が返らないようにしておく
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
		// マニピュレータの操作結果を取得する
		MVector scVal;
		getConverterManipValue(scaleManipFn.scaleIndex(), scVal);

		// マニピュレータの描画位置から頂点位置までの差分を算出する
		MVector positionVec = initialPositions[index] - centroid;

		// 差分をスケーリングする
		MVector newPosition(positionVec.x*scVal.x, positionVec.y*scVal.y, positionVec.z*scVal.z);

		// 初期値からの変化量を算出
		newPosition = newPosition - positionVec;

		// pntsアトリビュートの新しい値を返す
		newPosition += initialControlPoint[index];
		MFnNumericData numericData;
		obj = numericData.create( MFnNumericData::k3Double );
		numericData.setData(newPosition.x,newPosition.y,newPosition.z);
		return MManipData(obj);
	}

	// エラーとなった場合に不正値が返らないようにしておく
	MGlobal::displayError("Invalid index in scale changed callback!");
	MFnNumericData numericData;
	obj = numericData.create( MFnNumericData::k3Double );
	numericData.setData(0.0,0.0,0.0);
	return obj;
	
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// カスタムマニピュレータ用のコンテキストと起動コマンド

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
	
	// 登録済みのマニピュレータを一旦削除
	ComponentScaleContext * ctxPtr = (ComponentScaleContext *) data;
	ctxPtr->deleteManipulators(); 

	// 現在の頂点選択の状態でマニピュレータを作り直す
	MSelectionList list;
	stat = MGlobal::getActiveSelectionList(list);
	MItSelectionList iter(list, MFn::kInvalid, &stat);

	if (MS::kSuccess == stat) {
		for (; !iter.isDone(); iter.next()) {
			// コンポーネントが選択されていない場合は警告を出してスキップ
			MDagPath dagPath;
			MObject components;
			iter.getDagPath(dagPath, components);
			if (components.isNull() || !components.hasFn(MFn::kComponent)) {
				MGlobal::displayWarning("Object in selection list is not a component.");
				continue;
			}

			// マニピュレータを新規作成
			MString manipName ("MeshComponentScaleManip");
			MObject manipObject;
			MeshComponentScaleManip* manipulator = (MeshComponentScaleManip *) MeshComponentScaleManip::newManipulator(manipName, manipObject);

			if (NULL != manipulator) {
				// 新規作成が成功した場合、コンテキストに登録する
				ctxPtr->addManipulator(manipObject);

				// パスとコンポーネントを変数に格納する
				manipulator->setSurfaceDagPath(dagPath);
				manipulator->setComponentObject(components);
				
				// マニピュレータを紐づける
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
// コンテキストとマニピュレータノードをセットでプラグイン登録する

MStatus initializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj, PLUGIN_COMPANY, "6.0", "Any");

	status = plugin.registerContextCommand(	"MeshComponentScaleContext",
											MeshComponentScaleContext::creator);
	// Undo/Redoを含むツールコマンドを紐づけする場合は引数3，4個目に追加

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

