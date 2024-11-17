def getUvIslands(meshName):
    selList = om.MSelectionList()
    selList.add(meshName)
    meshDagPath = selList.getDagPath(0)
    meshFn = om.MFnMesh(meshDagPath)
    uvSetName = meshFn.currentUVSetName()
    
    # 全フェースをループして、UV間の接続情報を収集
    uvIslandDict = {}
    verIsLandDict = {}
    for faceID in range(meshFn.numPolygons):
        # フェースごとのUV-IDを取得
        restUvIDs = []
        polyVertexSize = meshFn.polygonVertexCount(faceID)
        for localID in range(polyVertexSize):
            uvID = meshFn.getPolygonUVid(faceID, localID, uvSetName)
            restUvIDs.append(uvID)
        
        # UV-IDの接続情報を相互に関連付け
        for i, uvID1 in enumerate(restUvIDs):
            for uvID2 in restUvIDs[i + 1:]:
                if uvID1 not in uvIslandDict:
                    uvIslandDict[uvID1] = set()
                if uvID2 not in uvIslandDict:
                    uvIslandDict[uvID2] = set()
                uvIslandDict[uvID1].add(uvID2)
                uvIslandDict[uvID2].add(uvID1)
    
    # 接続グループを探索
    visited = set()
    result = []
    def dfs(uvID, uvIsland): # Union-Findアルゴリズム 深さ優先探索
        if uvID in visited:
            return
        visited.add(uvID)
        uvIsland.add(uvID)
        for neighbor in uvIslandDict.get(uvID, []):
            dfs(neighbor, uvIsland)

    for uvID in uvIslandDict:
        if uvID not in visited:
            uvIsland = set()
            dfs(uvID, uvIsland)
            result.append(sorted(uvIsland))  # ソートしてリストに変換
    return result

def getFaceIDsFromUv(meshName, uvID):
    # UVで構成するフェース番号を返す
    selList = om.MSelectionList()
    selList.add(meshName)
    meshDagPath = selList.getDagPath(0)
    meshFn = om.MFnMesh(meshDagPath)

    result = []
    for faceID in range(meshFn.numPolygons): 
        for localID in range(meshFn.polygonVertexCount(faceID)):
            # 各フェースの頂点が持つUV番号を取得
            uvL = meshFn.getPolygonUVid(faceID, localID)
            if uvL == uvID:
                result.append(faceID)
                break
    return result
    
def getUvIslandFaceIDs(meshName, sort=True):
    uvIDs = getUvIslands(meshName)
    
    result = []
    for uv_group in uvIDs:
        resultSet = set()
        resultList = []
        for uv_id in uv_group:
            uvFaceIDs = getFaceIDsFromUv(meshName, uv_id)
            for f in uvFaceIDs:
                resultSet.add(f)
                if not f in resultList:
                    resultList.append(f)
        if sort:
            result.append(list(resultSet))
        else:
            result.append(resultList)
    return result
    
 def uvIdToVertexId(meshName, uvIslandFaceIDs):
    selList = om.MSelectionList()
    selList.add(meshName)
    meshDagPath = selList.getDagPath(0)
    meshFn = om.MFnMesh(meshDagPath)
    
    result = []
    for faceIDs in uvIslandFaceIDs:
        resultList = []
        for f in faceIDs:
            faceVertices = meshFn.getPolygonVertices(f)
            for v in faceVertices:
                resultList.append(v)
        result.append(resultList)
    return result   
    
uv_groups = getUvIslandFaceIDs("pPlane1")
aa = uvIdToVertexId("pPlane1", uv_groups)
   
//////////////////////////////////////////////////////////////////////////////////////////   
import maya.cmds as cmds

def set_interpolated_skin_weight(mesh_name, vertex_a, vertex_b, target_vertex, t=0.5):
    """
    頂点Aと頂点Bのスキンウェイトを線形補間してターゲット頂点に設定する。

    :param mesh_name: メッシュ名
    :param vertex_a: 頂点Aのインデックス
    :param vertex_b: 頂点Bのインデックス
    :param target_vertex: スキンウェイトを設定するターゲット頂点のインデックス
    :param t: 線形補間係数 (0.0 ～ 1.0)
    """
    # メッシュのスキンクラスタを取得
    skin_cluster = find_skin_cluster(mesh_name)
    if not skin_cluster:
        raise ValueError(f"メッシュ {mesh_name} にスキンクラスタが見つかりません。")
    
    # 頂点AとBのスキンウェイトを取得
    weights_a = get_skin_weights(skin_cluster, mesh_name, vertex_a)
    weights_b = get_skin_weights(skin_cluster, mesh_name, vertex_b)

    # 線形補間したスキンウェイトを計算
    interpolated_weights = {
        influence: weights_a.get(influence, 0.0) * (1 - t) + weights_b.get(influence, 0.0) * t
        for influence in set(weights_a.keys()).union(weights_b.keys())
    }

    # ターゲット頂点にスキンウェイトを設定
    set_skin_weights(skin_cluster, mesh_name, target_vertex, interpolated_weights)

def find_skin_cluster(mesh_name):
    """
    メッシュに関連付けられたスキンクラスタを探す。
    """
    history = cmds.listHistory(mesh_name)
    skin_clusters = cmds.ls(history, type='skinCluster')
    return skin_clusters[0] if skin_clusters else None

def get_skin_weights(skin_cluster, mesh_name, vertex_index):
    """
    指定した頂点のスキンウェイトを取得する。
    """
    weights = cmds.skinPercent(skin_cluster, f"{mesh_name}.vtx[{vertex_index}]", query=True, value=True)
    influence_names = cmds.skinCluster(skin_cluster, query=True, influence=True)
    return dict(zip(influence_names, weights))

def set_skin_weights(skin_cluster, mesh_name, vertex_index, weights):
    """
    指定した頂点にスキンウェイトを設定する。
    """
    for influence, weight in weights.items():
        cmds.skinPercent(
            skin_cluster,
            f"{mesh_name}.vtx[{vertex_index}]",
            transformValue=[(influence, weight)]
        )

# 使用例
mesh_name = "pPlane1"  # メッシュ名を指定
vertex_a = 0  # 頂点Aのインデックス
vertex_b = 1  # 頂点Bのインデックス
target_vertex = 2  # スキンウェイトを設定するターゲット頂点
t = 0.5  # 線形補間係数

set_interpolated_skin_weight(mesh_name, vertex_a, vertex_b, target_vertex, t)
print(f"頂点 {target_vertex} のスキンウェイトを補間して設定しました。")

//////////////////////////////////////////////////////////////////////////////////////////
import sys
import maya.OpenMayaUI as omui
from PySide6.QtWidgets import QMainWindow, QVBoxLayout, QWidget, QProgressBar, QPushButton
from PySide6.QtCore import QTimer
from shiboken6 import wrapInstance

# Mayaのウィンドウに統合するためのヘルパー関数
def maya_main_window():
    main_window_ptr = omui.MQtUtil.mainWindow()
    return wrapInstance(int(main_window_ptr), QWidget)

class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()

        self.setWindowTitle("プログレスバーの例")
        self.setGeometry(300, 300, 300, 100)

        # メインウィジェットを作成
        main_widget = QWidget(self)
        self.setCentralWidget(main_widget)

        # レイアウトを作成
        layout = QVBoxLayout()

        # プログレスバーを作成
        self.progress_bar = QProgressBar(self)
        self.progress_bar.setRange(0, 100)  # 進行範囲を設定
        self.progress_bar.setValue(0)       # 初期値
        self.progress_bar.setTextVisible(True)  # 数字を表示

        # プログレスバーの縦幅（高さ）を小さくする
        self.progress_bar.setFixedHeight(10)  # 高さを小さく設定

        # ボタンを作成
        self.start_button = QPushButton("開始", self)
        self.start_button.clicked.connect(self.start_progress)

        # レイアウトにプログレスバーとボタンを追加
        layout.addWidget(self.progress_bar)
        layout.addWidget(self.start_button)

        # メインウィジェットにレイアウトを設定
        main_widget.setLayout(layout)

    def start_progress(self):
        # プログレスバーを0から100まで進める
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_progress)
        self.timer.start(100)  # 100msごとに更新

    def update_progress(self):
        # プログレスバーの値を更新
        current_value = self.progress_bar.value()
        if current_value < 100:
            self.progress_bar.setValue(current_value + 1)
        else:
            self.timer.stop()  # 進行が終了したらタイマーを停止

# Mayaウィンドウ内でPySideアプリを実行
if __name__ == "__main__":
    try:
        # QApplication インスタンスはMayaがすでに作成しているので、何もする必要はありません。
        window = MainWindow()
        window.setParent(maya_main_window())  # Mayaウィンドウ内に配置
        window.show()
    except Exception as e:
        print(f"Error: {e}")
