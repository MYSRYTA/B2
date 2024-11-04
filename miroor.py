import maya.cmds as cmds

def invertAnimCurve(objName, attribute):
    # 指定したオブジェクトのアトリビュートにアニメーションがあるか確認
    animCurve = cmds.listConnections(f"{objName}.{attribute}", type="animCurve")
    
    if not animCurve:
        return
    else:
        animCurveName = animCurve[0]
        numKeys = cmds.keyframe(animCurveName, query=True, keyframeCount=True)
        
        # 全てのキーの値とハンドル情報を+-反転
        for i in range(numKeys):
            # キーの値
            keyVal = cmds.keyframe(animCurveName, index=(i, i), query=True, valueChange=True)[0]
            cmds.keyframe(animCurveName, index=(i, i), edit=True, valueChange=-keyVal)
            
            # ハンドルの傾き
            inTangent = cmds.keyTangent(animCurveName, index=(i, i), query=True, inAngle=True)[0]
            outTangent = cmds.keyTangent(animCurveName, index=(i, i), query=True, outAngle=True)[0]
            cmds.keyTangent(animCurveName, index=(i, i), edit=True, inAngle=-inTangent, outAngle=-outTangent)

invertAnimCurve("locator1", "translateX")

#################################################################################
import maya.api.OpenMaya as om
import maya.api.OpenMayaAnim as oma
import maya.cmds as cmds

def transfer_skin_weight_openmaya(mesh_name, source_influence, target_influence):
    # メッシュのシェイプノードを取得
    sel = om.MSelectionList()
    sel.add(mesh_name)
    dag_path = sel.getDagPath(0)
    
    # メッシュに接続されているスキンクラスターを取得
    skin_cluster_name = None
    history = cmds.listHistory(mesh_name)
    for node in history:
        if cmds.nodeType(node) == "skinCluster":
            skin_cluster_name = node
            break
    
    if not skin_cluster_name:
        om.MGlobal.displayError("スキンクラスターが見つかりません。")
        return
    
    # SkinClusterノードをOpenMayaで取得
    sel = om.MSelectionList()
    sel.add(skin_cluster_name)
    skin_cluster = sel.getDependNode(0)
    skin_fn = oma.MFnSkinCluster(skin_cluster)
    
    # インフルエンスリストとジオメトリ情報を取得
    influences = skin_fn.influenceObjects()
    dag_path, components = skin_fn.getGeometry()
    
    # ソースとターゲットのインフルエンスのインデックスを取得
    source_index = influences.index(cmds.ls(source_influence, long=True)[0])
    target_index = influences.index(cmds.ls(target_influence, long=True)[0])
    
    # ウェイトデータを取得して処理
    weights = skin_fn.getWeights(dag_path, components)
    for i in range(components.elementCount):
        # 各頂点のソースウェイトを取得してターゲットに移動
        source_weight = weights[i * len(influences) + source_index]
        weights[i * len(influences) + target_index] += source_weight
        weights[i * len(influences) + source_index] = 0.0  # ソースウェイトを0に設定
    
    # 新しいウェイトデータを設定
    skin_fn.setWeights(dag_path, components, influences, weights, normalize=True)

# 使用例
mesh_name = "pSphere1"  # 対象メッシュの名前
source_influence = "jointA"  # 移動元のインフルエンス
target_influence = "jointB"  # 移動先のインフルエンス
transfer_skin_weight_openmaya(mesh_name, source_influence, target_influence)

#################################################################################
import os
import json

from PySide6 import QtWidgets, QtGui, QtCore
import shiboken6

import maya.cmds as cmds
import maya.OpenMayaUI as omui
#################################################################################

def maya_main_window():
    main_window_ptr = omui.MQtUtil.mainWindow()
    return shiboken6.wrapInstance(int(main_window_ptr), QtWidgets.QWidget)


#################################################################################
class MeshItemGroup(QtWidgets.QGroupBox):
    iconDir = ""
    
    def __init__(self, title, iconDir, iconName, type=0):
        super(MeshItemGroup, self).__init__()
        
        self.iconDir = iconDir
        
        main_layout = QtWidgets.QVBoxLayout()
        self.setLayout(main_layout)
        
        ############################################### アイコン/タイトルテキスト/アイテム追加ボタン
        title_layout = QtWidgets.QHBoxLayout()
        main_layout.addLayout(title_layout)
        # アイコンを表示するQLabel
        icon_label = QtWidgets.QLabel()
        icon_pixmap = QtGui.QPixmap(iconDir + "/" + iconName)
        icon_pixmap = icon_pixmap.scaled(25, 25, QtCore.Qt.AspectRatioMode.KeepAspectRatio)
        icon_label.setPixmap(icon_pixmap)
        # タイトルテキストを表示するQLabel
        title_label = QtWidgets.QLabel(title)
        title_label.setStyleSheet("font-size: 18px;")  # タイトルのフォントサイズを設定
        title_label.setFixedHeight(icon_pixmap.height())  # アイコンの高さに合わせる
        # リストへアイテムを追加するボタン
        self.addItem_button = QtWidgets.QPushButton("get")
        self.addItem_button.setIcon(QtGui.QIcon(iconDir + "/regist.png"))
        self.addItem_button.setIconSize(QtCore.QSize(15, 15))
        self.addItem_button.clicked.connect(self.addItem)
        # 
        self.op_button = QtWidgets.QPushButton("exe")
        self.op_button.clicked.connect(self.op_exe)

        # 配置
        title_layout.addWidget(icon_label)
        title_layout.addWidget(title_label, 100) # アイコンを配置した後にタイトルを左詰めで配置する
        title_layout.addWidget(self.addItem_button)          

        ############################################### キャラクターメッシュ情報
        character_layout = QtWidgets.QVBoxLayout()
        main_layout.addLayout(character_layout)
        # リストを配置
        self.list_widget = QtWidgets.QListWidget()
        self.list_widget.setMinimumHeight(14)
        character_layout.addWidget(self.list_widget)
        
        if type == 1:
            character_layout.addWidget(self.op_button) 
        
    def addItem(self):
        selectedMeshes = cmds.ls(selection=True, long=True, type="transform")
        
        for meshNname in selectedMeshes:
            item_widget = QtWidgets.QWidget()
            item_layout = QtWidgets.QHBoxLayout(item_widget)
            item_layout.setContentsMargins(5, 1, 1, 5) 
            
            meshName_label = QtWidgets.QLabel(meshNname)
            
            uv_comboBbox = QtWidgets.QComboBox()
            uvSets = cmds.polyUVSet(meshNname, query=True, allUVSets=True) or []
            uv_comboBbox.addItems(uvSets)
            
            reselect_button = QtWidgets.QPushButton()
            icon = QtGui.QIcon(self.iconDir + "/reselect.png")
            reselect_button.setIcon(icon)
            reselect_button.setIconSize(QtCore.QSize(15, 15))
            reselect_button.setFixedSize(QtCore.QSize(35, 19))
            
            delete_button = QtWidgets.QPushButton()
            icon = QtGui.QIcon(self.iconDir + "/reselect.png")
            delete_button.setIcon(icon)
            delete_button.setIconSize(QtCore.QSize(15, 15))
            delete_button.setFixedSize(QtCore.QSize(35, 19))
            delete_button.clicked.connect(lambda: self.deleteItem(item_widget))
            
            item_layout.addWidget(meshName_label, 100)
            item_layout.addWidget(uv_comboBbox)
            item_layout.addWidget(reselect_button)
            item_layout.addWidget(delete_button)            
            
            list_item = QtWidgets.QListWidgetItem(self.list_widget)
            list_item.setSizeHint(item_widget.sizeHint())
            self.list_widget.addItem(list_item)
            self.list_widget.setItemWidget(list_item, item_widget)
            
    def deleteItem(self, item_widget):
        for i in range(self.list_widget.count()):
            if self.list_widget.itemWidget(self.list_widget.item(i)) == item_widget:
                self.list_widget.takeItem(i)
                break
                
    def op_exe(self):
        print("abc")

#################################################################################
class SubWindow(QtWidgets.QDialog):
    def __init__(self, main_window, sp, inf, infType):
        super(SubWindow, self).__init__(main_window)
        
        self.setWindowTitle("Subwindow")
        self.setFixedSize(500, 150)
        self.main_window = main_window
        
        # サブウィンドウのレイアウト
        main_layout = QtWidgets.QVBoxLayout(self)
        
        ###############################################
        sp_layout = QtWidgets.QVBoxLayout()
        main_layout.addLayout(sp_layout)
        
        if not os.path.isfile(inf):
            sp = ""
        self.spLabel = QtWidgets.QLabel("File: " + sp)
        sp_layout.addWidget(self.spLabel)
        
        # JSONファイル選択ボタン
        self.select_json_button = QtWidgets.QPushButton("Select JSON File")
        self.select_json_button.clicked.connect(self.select_json_file)
        sp_layout.addWidget(self.select_json_button, 10)

        ###############################################
        inf_layout = QtWidgets.QVBoxLayout()
        main_layout.addLayout(inf_layout)
        
        if not os.path.isfile(inf):
            inf = ""
        self.infLabel = QtWidgets.QLabel("Pose: " + inf)
        inf_layout.addWidget(self.infLabel)
        
        # TXTファイル選択ボタン
        self.select_txt_button = QtWidgets.QPushButton("Select TXT File")
        self.select_txt_button.clicked.connect(self.select_txt_file)
        inf_layout.addWidget(self.select_txt_button, 20)

        ###############################################
        infType_layout = QtWidgets.QHBoxLayout()
        main_layout.addLayout(infType_layout)
        #
        self.infTypeLabel = QtWidgets.QLabel("type")
        infType_layout.addWidget(self.infTypeLabel)
        #
        self.infTypeCombo = QtWidgets.QComboBox()
        self.infTypeCombo.addItems(["Over", "ref"])
        self.infTypeCombo.setCurrentIndex(infType)  # 初期状態を設定
        self.infTypeCombo.currentIndexChanged.connect(self.change_inf_type)  # 選択が変わったときに関数を実行する
        self.infTypeCombo.setEnabled(os.path.isfile(inf))            
        infType_layout.addWidget(self.infTypeCombo)
        
        # サブウィンドウが閉じられたときの処理
        self.finished.connect(self.on_close)

    def select_json_file(self):
        file_path, _ = QtWidgets.QFileDialog.getOpenFileName(self, "Select JSON File", "", "JSON Files (*.txt)")
        if file_path:
            self.spLabel.setText("File: " + file_path)
            self.main_window.sPath = file_path
            self.main_window.spLabel.setText("File: " + os.path.basename(file_path))

    def select_txt_file(self):
        file_path, _ = QtWidgets.QFileDialog.getOpenFileName(self, "Select TXT File", "", "Text Files (*.txt)")
        if file_path:
            self.infLabel.setText("Pose: " + file_path)
            self.main_window.inf = file_path
            self.main_window.infLabel.setText("Pose: " + os.path.basename(file_path).split(".")[0])
            self.infTypeCombo.setEnabled(True)
        else:
            self.infTypeCombo.setEnabled(False)
            
    def change_inf_type(self, index):
        self.main_window.infType = index

    def on_close(self):
        self.main_window.setEnabled(True)
        
#################################################################################
class TabbedToolUI(QtWidgets.QWidget):
    sPath = "XXX.CCC.aa.txt"
    inf = f"あああ"
    infType = -1
    NETWORK_NODE_NAME = "TabbedToolUIInfo"
    meshItemGroup0 = []
    meshItemGroup1 = []
    
    def __init__(self, parent=maya_main_window()):
        super(TabbedToolUI, self).__init__(parent)
        
        self.setWindowTitle("Tabbed Tool UI")
        self.setFixedSize(500, 1000)

        # メインレイアウト
        main_layout = QtWidgets.QVBoxLayout(self)

        # タブウィジェットを追加
        self.tab_widget = QtWidgets.QTabWidget()
        main_layout.addWidget(self.tab_widget)

        # タブ1の内容
        self.tab1 = QtWidgets.QWidget()
        tab1_layout = QtWidgets.QVBoxLayout(self.tab1)
        
        # タイトルリスト
        titleList = [f"AAA", f"BBB", f"CCC", f"DDD", f"EEE", f"FFF", f"GGG"]
        # アイコンリスト
        iconDirectry = "C:/D/GitHub/CreateJointsOnMeshComponents/scripts/tools/icons"
        iconNameList = ["regist.png", "execute.png", "menu.png", "reselect.png", "regist.png", "execute.png", "menu.png"]
        # MeshItemGroupを複数インスタンス化
        for i in range(len(titleList)):  # 3つのFFFグループを追加
            meshItemGroup = MeshItemGroup(title=titleList[i], iconDir=iconDirectry, iconName=iconNameList[i], type=0)
            self.meshItemGroup0.append(meshItemGroup)
            tab1_layout.addWidget(meshItemGroup)
        
        # 実行ボタンを追加        
        self.outer_button = QtWidgets.QPushButton("Outer Button")
        self.outer_button.setIcon(QtGui.QIcon(iconDirectry + "/execute.png"))
        self.outer_button.setIconSize(QtCore.QSize(30, 30))
        tab1_layout.addWidget(self.outer_button)
        
        #######################################################
        op_layout = QtWidgets.QHBoxLayout()
        tab1_layout.addLayout(op_layout)
        
        # sp
        self.spLabel = QtWidgets.QLabel("File: " + self.sPath.split(".")[-1])
        op_layout.addWidget(self.spLabel)
        # inf
        self.infLabel = QtWidgets.QLabel("Pose: " + self.inf)
        op_layout.addWidget(self.infLabel)
        
        # op 
        self.joint_option_button = QtWidgets.QPushButton()
        self.joint_option_button.setIcon(QtGui.QIcon(iconDirectry + "/menu.png"))
        self.joint_option_button.setFixedSize(20,20)
        self.joint_option_button.clicked.connect(self.show_sub_window)
        op_layout.addWidget(self.joint_option_button)   
        
        self.bb = QtWidgets.QPushButton("test")
        self.bb.clicked.connect(self.report)
        op_layout.addWidget(self.bb)  
        
        self.tab_widget.addTab(self.tab1, f"aaa")

        # タブ2の内容
        self.tab2 = QtWidgets.QWidget()
        tab2_layout = QtWidgets.QVBoxLayout(self.tab2)
        
        # タイトルリスト
        titleList2 = [f"AAA", f"BBB", f"CCC"]
        # アイコンリスト
        iconNameList2 = ["regist.png", "execute.png", "menu.png"]
        # MeshItemGroupを複数インスタンス化
        for i in range(len(titleList2)):  # 3つのFFFグループを追加
            meshItemGroup = MeshItemGroup(title=titleList2[i], iconDir=iconDirectry, iconName=iconNameList2[i], type=1)
            self.meshItemGroup1.append(meshItemGroup)
            tab2_layout.addWidget(meshItemGroup)
        
        mode_layout = QtWidgets.QVBoxLayout()
        tab2_layout.addLayout(mode_layout)
        self.mode_button = QtWidgets.QPushButton()
        self.mode_button.setIcon(QtGui.QIcon(iconDirectry + "/menu.png"))
        self.save_button = QtWidgets.QPushButton()
        self.save_button.setIcon(QtGui.QIcon(iconDirectry + "/menu.png"))
        mode_layout.addWidget(self.mode_button)
        mode_layout.addWidget(self.save_button)
        
        self.tab_widget.addTab(self.tab2, f"bbb")
        
    def show_sub_window(self):
        self.subwindow = SubWindow(main_window=self, sp=self.sPath, inf=self.inf, infType=self.infType)  # サブウィンドウのインスタンス化
        self.subwindow.show()
        self.setEnabled(False)  # メインウィンドウを無効にする
        self.subwindow.setEnabled(True)
        
    def report(self):
        for itemGroup in self.meshItemGroup0:
            for i in range(itemGroup.list_widget.count()):
                a = itemGroup.list_widget.itemWidget(itemGroup.list_widget.item(i))
                print(a)        
            
    
    def save_data_to_scene(self):
        if not cmds.objExists(self.NETWORK_NODE_NAME):
            cmds.createNode("network", name=self.NETWORK_NODE_NAME)
        
        list_items_data = []
        for i in range(self.list_widget.count()):
            item_widget = self.list_widget.itemWidget(self.list_widget.item(i))
            mesh_label = item_widget.findChild(QtWidgets.QLabel).text()
            uv_combo = item_widget.findChild(QtWidgets.QComboBox)
            selected_uv = uv_combo.currentText()
            list_items_data.append({"mesh": mesh_label, "uv_set": selected_uv})
        
        data = {
            "text_box": self.text_box.text(),
            "combo_box": self.combo_box.currentText(),
            "list_items": list_items_data
        }
        data_str = json.dumps(data)
        
        if not cmds.attributeQuery("customData", node=self.NETWORK_NODE_NAME, exists=True):
            cmds.addAttr(self.NETWORK_NODE_NAME, longName="customData", dataType="string")
        cmds.setAttr(f"{self.NETWORK_NODE_NAME}.customData", data_str, type="string")

    def load_data_from_scene(self):
        if cmds.objExists(self.NETWORK_NODE_NAME) and cmds.attributeQuery("customData", node=self.NETWORK_NODE_NAME, exists=True):
            data_str = cmds.getAttr(f"{self.NETWORK_NODE_NAME}.customData")
            if data_str:
                try:
                    data = json.loads(data_str)
                    
                    # テキストボックスとコンボボックスの値を設定
                    self.text_box.setText(data.get("text_box", ""))
                    
                    combo_value = data.get("combo_box", "Option 1")
                    index = self.combo_box.findText(combo_value)
                    if index >= 0:
                        self.combo_box.setCurrentIndex(index)
                    
                    # リストアイテムの設定を読み取り
                    list_items_data = data.get("list_items", [])
                    if isinstance(list_items_data, list):
                        for item in list_items_data:
                            mesh_name = item.get("mesh", "")
                            uv_set = item.get("uv_set", "")
                            self.add_item_with_data(mesh_name, uv_set)
                except json.JSONDecodeError:
                    print("データの読み込みエラー：データが正しい形式で保存されているか確認してください。")

def show_tool_ui():
    if cmds.window("TabbedToolUI", exists=True):
        cmds.deleteUI("TabbedToolUI", wnd=True)

    ui = TabbedToolUI()
    ui.setWindowFlags(ui.windowFlags() | QtCore.Qt.Window)
    ui.show()

show_tool_ui()











