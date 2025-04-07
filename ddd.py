from maya import cmds
from pxr import Usd, UsdGeom, UsdMaya, Sdf

def get_usd_stage_from_proxy(proxyShapeName):
    """
    指定されたProxyShapeのキャッシュIDからUSD Stageを取得する
    """
    if not cmds.objExists(proxyShapeName):
        raise ValueError(f"ProxyShape '{proxyShapeName}' が存在しません")

    # キャッシュIDを取得
    cacheId = cmds.getAttr(f"{proxyShapeName}.stageCacheId")
    if cacheId is None:
        raise RuntimeError(f"ProxyShape '{proxyShapeName}' に有効なキャッシュIDがありません")

    # USD Stageをキャッシュから取得
    stage = UsdMaya.StageCache.Get().Find(cacheId)
    if stage:
        return stage
    else:
        raise RuntimeError(f"キャッシュID {cacheId} に対応するUSD Stageが見つかりません")

def get_mesh_translate_values(stage, layerIdentifier):
    """
    指定したレイヤー内の全Meshのtranslate値を取得
    """
    # 指定レイヤーを取得
    layer = None
    for subLayer in stage.GetRootLayer().subLayerPaths:
        if layerIdentifier in subLayer:
            layer = Sdf.Layer.Find(subLayer)
            break
    
    if not layer:
        raise RuntimeError(f"レイヤー '{layerIdentifier}' がステージに見つかりません")

    translate_values = {}

    # Stage内の全Primsを走査
    for prim in stage.Traverse():
        if prim.IsA(UsdGeom.Mesh):  # Meshノードを探す
            xform = UsdGeom.Xformable(prim)
            translate_attr = xform.GetAttribute("xformOp:translate")

            if translate_attr:
                translate_val = translate_attr.Get()
                if translate_val is not None:
                    translate_values[prim.GetPath()] = translate_val

    return translate_values

# 使用例
proxyShapeName = "yourProxyShape"  # MayaのProxyShapeノード名
layerIdentifier = "your_layer.usda"  # 取得したいレイヤーの識別子

usd_stage = get_usd_stage_from_proxy(proxyShapeName)

if usd_stage:
    print("USD Stage取得成功:", usd_stage.GetRootLayer().identifier)

    mesh_translate_data = get_mesh_translate_values(usd_stage, layerIdentifier)
    for path, translate in mesh_translate_data.items():
        print(f"{path}: {translate}")

////////////////////////////////

#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MGlobal.h>
#include <maya/MMatrixArray.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnPlugin.h>

#include <pxr/usd/usd/stageCache.h>
#include <pxr/usd/usdGeom/xformable.h>
#include <pxr/usd/usdGeom/mesh.h>

PXR_NAMESPACE_USING_DIRECTIVE

class UsdMeshTransformNode : public MPxNode {
public:
    UsdMeshTransformNode() {}
    virtual ~UsdMeshTransformNode() {}
    
    virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

    static void* creator() { return new UsdMeshTransformNode(); }
    static MStatus initialize();

    static MTypeId id;
    static MObject inputStageCacheId;
    static MObject outputMeshTransform;
};

MTypeId UsdMeshTransformNode::id(0x0013B002);
MObject UsdMeshTransformNode::inputStageCacheId;
MObject UsdMeshTransformNode::outputMeshTransform;

MStatus UsdMeshTransformNode::compute(const MPlug& plug, MDataBlock& dataBlock) {
    if (plug != outputMeshTransform) {
        return MS::kUnknownParameter;
    }

    int stageCacheId = dataBlock.inputValue(inputStageCacheId).asInt();

    // USD Stage の取得
    UsdStageRefPtr stage = UsdMayaStageCache::Get().Find(stageCacheId);
    if (!stage) {
        MGlobal::displayWarning("USD Stageが見つかりません");
        return MS::kFailure;
    }

    // 出力用の配列
    MMatrixArray transformValues;
    
    // MeshノードのTransformを取得
    for (const UsdPrim& prim : stage->Traverse()) {
        if (prim.IsA<UsdGeomMesh>()) {
            UsdGeomXformable xformable(prim);
            GfVec3f translate(0.0f, 0.0f, 0.0f);
            GfVec3f rotate(0.0f, 0.0f, 0.0f);
            GfVec3f scale(1.0f, 1.0f, 1.0f);

            // Translate
            UsdAttribute translateAttr = xformable.GetAttribute(TfToken("xformOp:translate"));
            if (translateAttr) translateAttr.Get(&translate);

            // Rotate
            UsdAttribute rotateAttr = xformable.GetAttribute(TfToken("xformOp:rotateXYZ"));
            if (rotateAttr) rotateAttr.Get(&rotate);

            // Scale
            UsdAttribute scaleAttr = xformable.GetAttribute(TfToken("xformOp:scale"));
            if (scaleAttr) scaleAttr.Get(&scale);

            // MMatrix へ変換
            MTransformationMatrix mtx;
            mtx.setTranslation(MVector(translate[0], translate[1], translate[2]), MSpace::kTransform);
            double rot[3] = { rotate[0], rotate[1], rotate[2] };
            mtx.setRotation(rot, MTransformationMatrix::kXYZ);
            mtx.setScale(scale.GetArray(), MSpace::kTransform);

            transformValues.append(mtx.asMatrix());
        }
    }

    // Maya出力アトリビュートに設定
    MDataHandle outputHandle = dataBlock.outputValue(outputMeshTransform);
    outputHandle.setMMatrixArray(transformValues);
    outputHandle.setClean();

    return MS::kSuccess;
}

// ノードの属性を定義
MStatus UsdMeshTransformNode::initialize() {
    MFnNumericAttribute numericAttr;
    MFnMatrixAttribute matrixAttr;

    // 入力: USDのキャッシュID (int)
    inputStageCacheId = numericAttr.create("stageCacheId", "sid", MFnNumericData::kInt, 0);
    numericAttr.setKeyable(true);
    numericAttr.setStorable(true);
    addAttribute(inputStageCacheId);

    // 出力: MeshのTransformをMMatrix配列として出力
    outputMeshTransform = matrixAttr.create("meshTransform", "mxf");
    matrixAttr.setStorable(false);
    matrixAttr.setWritable(false);
    addAttribute(outputMeshTransform);

    // 入出力の関係を定義
    attributeAffects(inputStageCacheId, outputMeshTransform);

    return MS::kSuccess;
}

// プラグインの登録
MStatus initializePlugin(MObject obj) {
    MFnPlugin plugin(obj, "YourName", "1.0", "Any");
    return plugin.registerNode("UsdMeshTransformNode", UsdMeshTransformNode::id, 
                               UsdMeshTransformNode::creator, UsdMeshTransformNode::initialize);
}

// プラグインの解除
MStatus uninitializePlugin(MObject obj) {
    MFnPlugin plugin(obj);
    return plugin.deregisterNode(UsdMeshTransformNode::id);
}


import maya.api.OpenMaya as om
import maya.api.OpenMayaMPx as ompx

# 属性が変更された際に呼び出されるコールバック関数
def attributeChangedCallback(node, plug, data):
    # 変更された属性が "translate" かどうかを確認
    if plug.partialName() == "translate":
        print(f"{node.name()} xformOp:translate has been changed!")

# Maya のノードが変更されたときにコールバックを設定
def registerNodeListener(nodeName):
    # ノード名を選択リストに追加
    selectionList = om.MSelectionList()
    selectionList.add(nodeName)  # ノード名に合わせて変更

    # ノードを取得
    node = selectionList.getDependNode(0)

    # translate 属性の変更をリスニング
    om.MNodeMessage.addAttributeChangedCallback(node, attributeChangedCallback)

# 使用例
registerNodeListener("mayaUsdProxyShape1")  # 監視したいノード名に変更