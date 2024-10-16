import maya.api.OpenMaya as om2
import maya.cmds as cmds
    
def set_locator_position(locator_name, position):
    # ロケータBに座標を設定
    cmds.setAttr(f'{locator_name}.translateX', position.x)
    cmds.setAttr(f'{locator_name}.translateY', position.y)
    cmds.setAttr(f'{locator_name}.translateZ', position.z)

def transferNeutralPose(sourceMeshName, sourceLocatorName, targetMeshName, outputRotByFaceNml=False):
    result = om2.MTransformationMatrix()
    
    # MFnMeshを取得
    selectionList = om2.MSelectionList()

    selectionList.clear() 
    selectionList.add(sourceMeshName)
    fnMeshA = om2.MFnMesh(selectionList.getDagPath(0))
    
    selectionList.clear() 
    selectionList.add(targetMeshName)
    fnMeshB = om2.MFnMesh(selectionList.getDagPath(0))
    
    # ロケータの位置を取得
    posA = cmds.xform(sourceLocatorName, q=True, ws=True, t=True)
    
    # ロケータの位置からソースメッシュ上の最も近い位置を取得してUV値とフェースIDに変換する
    point = om2.MPoint(posA[0], posA[1], posA[2])
    closestPoint, _ = fnMeshA.getClosestPoint(point, om2.MSpace.kWorld)
    uvAndFaceID = fnMeshA.getUVAtPoint(closestPoint, om2.MSpace.kWorld, "")
    closestUv = (uvAndFaceID[0], uvAndFaceID[1])
    closestFaceID = uvAndFaceID[2]
    
    # UV値の座標にメッシュBのUVが展開されているか確認する
    faceAtUv, pointsAtUv = fnMeshB.getPointsAtUV(closestUv[0], closestUv[1], om2.MSpace.kWorld, "")
    if not pointsAtUv:
        return result
    
    # 出力：位置
    result.setTranslation(om2.MVector(pointsAtUv[0].x, pointsAtUv[0].y, pointsAtUv[0].z), om2.MSpace.kWorld)
    
    # 出力：回転
    if outputRotByFaceNml:
        faceID = faceAtUv[0]
        
        facePointIDs = fnMeshB.getPolygonVertices(faceID)
        facePoints = [fnMeshB.getPoint(poiID, om2.MSpace.kWorld) for poiID in facePointIDs]
        min_x = min([p.x for p in facePoints])
        max_x = max([p.x for p in facePoints])
        p0 = om2.MPoint()
        p1 = om2.MPoint()
        for poi in facePoints:
            if min_x == poi.x:
                 p0 = poi
            if max_x == poi.x:
                 p1 = poi
        faceDir = om2.MVector(p1 - p0).normal()
        
        faceNml = fnMeshB.getPolygonNormal(faceID)
        
        ## Quatを作ってXFOにセットする
    
    return result

def getEyeballCenterLR(meshNames):
    rPoints = []
    lPoints = []
    selectionList = om2.MSelectionList()
    for meshName in meshNames:
        # MFnMeshを取得        
        selectionList.clear() 
        selectionList.add(meshName)
        fnMesh = om2.MFnMesh(selectionList.getDagPath(0))

        points = fnMesh.getPoints()
        for poi in points:
            if 0.0 == poi.x:
               rPoints.append(poi)
               lPoints.append(poi)
            elif 0.0 < poi.x:
                rPoints.append(poi)
            elif poi.x < 0.0:
                lPoints.append(poi)
    
    # min,maxを取得
    rMinX = min([p.x for p in rPoints])
    rMinY = min([p.y for p in rPoints])
    rMinZ = min([p.z for p in rPoints])

    rMaxX = max([p.x for p in rPoints])
    rMaxY = max([p.y for p in rPoints])
    rMaxZ = max([p.z for p in rPoints])
    
    lMinX = min([p.x for p in lPoints])
    lMinY = min([p.y for p in lPoints])
    lMinZ = min([p.z for p in lPoints])

    lMaxX = max([p.x for p in lPoints])
    lMaxY = max([p.y for p in lPoints])
    lMaxZ = max([p.z for p in lPoints])
    
    # 左右の中央値を返す
    return om2.MVector( (lMaxX+lMinX)*0.5, (lMaxY+lMinY)*0.5, (lMaxZ+lMinZ)*0.5), om2.MVector( (rMaxX+rMinX)*0.5, (rMaxY+rMinY)*0.5, (rMaxZ+rMinZ)*0.5)
    
def getTeethCenter(meshName, side):
    # MFnMeshを取得
    selectionList = om2.MSelectionList()
    selectionList.clear() 
    selectionList.add(meshName)
    fnMesh = om2.MFnMesh(selectionList.getDagPath(0))

    points = fnMesh.getPoints()
    
    # X値が0.0近辺の頂点だけで座標を算出する
    centetPoi = om2.MPoint()
    for p in points:
        if 0.1 < abs(p.x) :
            continue
        if side == "upper":
           centetPoi = p if p.y < centetPoi.y else centetPoi
        if side == "lower":
           centetPoi = p if centetPoi.y < p.y else centetPoi

    return om2.MVector( 0.0, centetPoi.y, centetPoi.z)

def getInfluencedPointIDs(meshName, minInfluences=2, minSkinWeight=0.001):
    # メッシュにバインドされているスキンクラスターを取得
    history = cmds.listHistory(meshName, pdo=True)
    skinClusterPathList = cmds.ls(history, type='skinCluster', long=True)

    if not skinClusterPathList:
        cmds.warning(f"{mesh_name}にはスキンバインドされていません")
        return None

    skinClusterPath = skinClusterPathList[0]

    # スキンクラスターをMFnSkinClusterで操作するためにMSelectionListで取得
    selectionList = om2.MSelectionList()
    selectionList.add(skinClusterPath)
    skinClusterNode = selectionList.getDependNode(0)
    fnSkinCluster = om2Anim.MFnSkinCluster(skinClusterNode)

    # メッシュのMObjectを取得
    selectionList.clear()
    selectionList.add(meshName)
    meshDagPath = selectionList.getDagPath(0)

    # スキンクラスターのウェイト情報を取得
    allWeightList, numInfluence = fnSkinCluster.getWeights(meshDagPath, om2.MObject())
    
    # メッシュの頂点数
    numPoint = int(len(allWeightList) / numInfluence)

    resultPoiIDs = []
    for poiID in range(numPoint):
        # ウェイトリストから頂点のウェイト値が記載された範囲を抜き出す
        poiWeightList = allWeightList[poiID*numInfluence:(poiID+1)*numInfluence]

        # minSkinWeightよりも大きいウェイト値を数える
        nonZeroWeightCount = len([poiWeight for poiWeight in poiWeightList if minSkinWeight < poiWeight])

        # 2つ以上のインフルエンスに影響される頂点をリストに追加
        if minInfluences <= nonZeroWeightCount:
            resultPoiIDs.append(poiID)

    return resultPoiIDs

def getFaceFromPoint(meshName, pointIDs):
    # フェースイテレーターを準備する
    selectionList = om2.MSelectionList()
    selectionList.add(meshName)
    meshDagPath = selectionList.getDagPath(0)
    faceIte = om2.MItMeshPolygon(meshDagPath)
    
    resultFaceIDs = []
    while not faceIte.isDone():
        # フェースを構成する頂点リストを取得
        facePoiIDs = faceIte.getVertices()  

        # 指定した頂点番号がフェースの頂点リストに含まれている場合はフェースの番号を出力リストに加える
        for poiID in facePoiIDs:
            if poiID in pointIDs:
                resultFaceIDs.append(faceIte.index())
                break

        faceIte.next()

    return resultFaceIDs
    
def main():
    # 選択されたオブジェクトを取得
    selection = cmds.ls(selection=True, long=True)
    if len(selection) != 4:
        cmds.error("4つのオブジェクトを選択してください (メッシュA、メッシュB、ロケータA、ロケータB)。")
        return
    
    meshAPath = selection[0]
    meshBPath = selection[1]
    locatorAPath = selection[2]
    locatorBPath = selection[3]
    
    closestXfo = transferNeutralPose(meshAPath, locatorAPath, meshBPath, True)
    set_locator_position(locatorBPath, closestXfo.translation(om2.MSpace.kWorld))
    print(closestXfo)

# スクリプト実行
cmds.select("pSphere1", r=True)
cmds.select("pSphere2", add=True)
cmds.select("locator1", add=True)
cmds.select("locator2", add=True)
main()

# 頂点を選択状態にするためのリストを作成
        vertex_selection = [f'{mesh_name}.vtx[{vtx}]' for vtx in influenced_vertices]
        
        # 頂点を選択
        cmds.select(vertex_selection)
        
////////////////////////////////////////////
import maya.api.OpenMaya as om2
import maya.cmds as cmds

def set_skin_weights(mesh_name, joint_name, weight_value=1.0):
    # 選択したメッシュのMFnMeshを取得
    selection_list = om2.MSelectionList()
    selection_list.add(mesh_name)
    mesh_dag_path = selection_list.getDagPath(0)
    mesh_fn = om2.MFnMesh(mesh_dag_path)

    # 頂点の位置を確認して、x値が0以上の頂点を抽出
    vertices_with_positive_x = []
    for i in range(mesh_fn.numVertices):
        point = mesh_fn.getPoint(i, om2.MSpace.kWorld)
        if point.x >= 0:
            vertices_with_positive_x.append(i)

    if not vertices_with_positive_x:
        cmds.warning(f"{mesh_name} には x >= 0 の頂点がありません")
        return

    # MELスクリプトを使用してスキンウェイトを変更
    for vtx in vertices_with_positive_x:
        mel_command = f'skinPercent -tv "{joint_name}" {weight_value} {mesh_name}.vtx[{vtx}]'
        cmds.mel.eval(mel_command)

    print(f"{len(vertices_with_positive_x)} 頂点に対して {joint_name} のウェイトを {weight_value} に設定しました。")

# スクリプトを実行するための例
mesh_name = "pSphere1"  # メッシュの名前
joint_name = "jointNameAA"  # インフルエンスにしたいジョイントの名前
set_skin_weights(mesh_name, joint_name, weight_value=1.0)

import maya.api.OpenMaya as om2
import maya.cmds as cmds

def set_skin_weights(mesh_name, joint_name_AA, joint_name_BB, weight_AA=0.1, weight_BB=0.9):
    # メッシュのMFnMeshを取得
    selection_list = om2.MSelectionList()
    selection_list.add(mesh_name)
    mesh_dag_path = selection_list.getDagPath(0)
    mesh_fn = om2.MFnMesh(mesh_dag_path)

    # 頂点の位置を確認して、x値が0以上の頂点を抽出
    vertices_with_positive_x = []
    for i in range(mesh_fn.numVertices):
        point = mesh_fn.getPoint(i, om2.MSpace.kWorld)
        if point.x >= 0:
            vertices_with_positive_x.append(i)

    if not vertices_with_positive_x:
        cmds.warning(f"{mesh_name} には x >= 0 の頂点がありません")
        return

    # MELスクリプトを使用してスキンウェイトを変更
    for vtx in vertices_with_positive_x:
        # jointNameAA に weight_AA のウェイトを設定
        mel_command_AA = f'skinPercent -tv "{joint_name_AA}" {weight_AA} {mesh_name}.vtx[{vtx}]'
        cmds.mel.eval(mel_command_AA)

        # jointNameBB に weight_BB のウェイトを設定
        mel_command_BB = f'skinPercent -tv "{joint_name_BB}" {weight_BB} {mesh_name}.vtx[{vtx}]'
        cmds.mel.eval(mel_command_BB)

    print(f"{len(vertices_with_positive_x)} 頂点に対して {joint_name_AA} に {weight_AA}、{joint_name_BB} に {weight_BB} のウェイトを設定しました。")

# スクリプトを実行するための例
mesh_name = "pSphere1"  # メッシュの名前
joint_name_AA = "jointNameAA"  # 1つ目のジョイントの名前
joint_name_BB = "jointNameBB"  # 2つ目のジョイントの名前
set_skin_weights(mesh_name, joint_name_AA, joint_name_BB, weight_AA=0.1, weight_BB=0.9)

import maya.api.OpenMaya as om2
import maya.cmds as cmds

def set_all_weights_to_joint(mesh_name, joint_name_AA):
    # メッシュのMFnMeshを取得
    selection_list = om2.MSelectionList()
    selection_list.add(mesh_name)
    mesh_dag_path = selection_list.getDagPath(0)
    mesh_fn = om2.MFnMesh(mesh_dag_path)

    # スキンクラスターを取得
    skin_clusters = cmds.ls(cmds.listHistory(mesh_name), type='skinCluster')
    if not skin_clusters:
        cmds.warning(f"{mesh_name} にスキンクラスターがありません")
        return
    skin_cluster = skin_clusters[0]

    # このスキンクラスターに影響を与えるすべてのジョイント（インフルエンス）を取得
    influences = cmds.skinCluster(skin_cluster, query=True, influence=True)

    # すべての頂点のウェイトをjointNameAAに1.0、それ以外のインフルエンスに0.0に設定
    for vtx_id in range(mesh_fn.numVertices):
        for joint in influences:
            if joint == joint_name_AA:
                cmds.skinPercent(skin_cluster, f"{mesh_name}.vtx[{vtx_id}]", transformValue=[(joint_name_AA, 1.0)])
            else:
                cmds.skinPercent(skin_cluster, f"{mesh_name}.vtx[{vtx_id}]", transformValue=[(joint, 0.0)])

    print(f"すべての頂点のウェイトを {joint_name_AA} に 1.0 にし、それ以外のジョイントに 0.0 に設定しました。")

# スクリプトを実行するための例
mesh_name = "pSphere1"  # メッシュの名前
joint_name_AA = "jointNameAA"  # ウェイトを設定するジョイントの名前
set_all_weights_to_joint(mesh_name, joint_name_AA)