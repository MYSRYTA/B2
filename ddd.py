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
