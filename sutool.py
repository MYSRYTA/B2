from maya import OpenMayaUI as omui
from PySide6 import QtWidgets, QtGui, QtCore
from shiboken6 import wrapInstance

def get_maya_main_window():
    """Mayaのメインウィンドウを取得する"""
    main_window_ptr = omui.MQtUtil.mainWindow()
    return wrapInstance(int(main_window_ptr), QtWidgets.QWidget)

class MeshItemGroup(QtWidgets.QGroupBox):   
    def __init__(self, title, iconDir, iconName, arg0):
        super(MeshItemGroup, self).__init__()
                
        main_layout = QtWidgets.QVBoxLayout()
        self.setLayout(main_layout)
        
        ############################################### アイコンとタイトルテキスト
        # アイコンを表示するQLabel
        icon_label = QtWidgets.QLabel()
        icon_pixmap = QtGui.QPixmap(iconDir + "/" + iconName)
        icon_pixmap = icon_pixmap.scaled(30, 30, QtCore.Qt.AspectRatioMode.KeepAspectRatio)
        icon_label.setPixmap(icon_pixmap)
        # タイトルテキストを表示するQLabel
        title_label = QtWidgets.QLabel(title)
        title_label.setStyleSheet("font-size: 18px;")  # タイトルのフォントサイズを設定
        # アイコンとタイトルラベルを横に並べるレイアウトを作成
        title_layout = QtWidgets.QHBoxLayout()
        title_layout.addWidget(icon_label)
        title_layout.addWidget(title_label, 100) # アイコンを配置した後にタイトルを左詰めで配置する
        # アイコンの幅に合わせてタイトルラベルを配置
        title_label.setFixedHeight(icon_pixmap.height())  # アイコンの高さに合わせる

        ############################################### テンプレートメッシュ情報
        # レイアウトを用意
        template_layout = QtWidgets.QHBoxLayout()
        # ラベル
        self.template_label = QtWidgets.QLabel("  テンプレート:")
        # テンプレートのメッシュ名を表示するテキストボックス（UIの見た目を統一するためにテキストボックスにするものの編集不可）
        self.textbox = QtWidgets.QLineEdit("face_skin")
        self.textbox.setEnabled(False)
        # テンプレートメッシュのUV名を表示するコンボボックス（UIの見た目を統一するためにコンボボックスにするものの編集不可）
        self.combobox = QtWidgets.QComboBox()
        self.combobox.addItems(["map1"])
        self.combobox.setEnabled(False)
        # ウィジェットを配置
        template_layout.addWidget(self.template_label)
        template_layout.addWidget(self.textbox)
        template_layout.addWidget(self.combobox)

        ############################################### キャラクターメッシュ情報
        # レイアウトを用意
        character_layout = QtWidgets.QHBoxLayout()
        # ラベル
        self.character_label = QtWidgets.QLabel("  キャラクター:")
        # リスト
        self.list_widget = QtWidgets.QListWidget()
        self.list_widget.setMinimumHeight(14)
        # リストへアイテムを追加するボタン
        self.addItem_button = QtWidgets.QPushButton("get")
        self.addItem_button.setIcon(QtGui.QIcon(iconDir + "/regist.png"))
        self.addItem_button.setIconSize(QtCore.QSize(15, 15))
        self.addItem_button.clicked.connect(self.addItem)
        self.reselect_button = QtWidgets.QPushButton("")
        self.reselect_button.setIcon(QtGui.QIcon(iconDir + "/reselect.png"))
        self.reselect_button.setIconSize(QtCore.QSize(15, 15))
        # ウィジェットを配置
        character_layout.addWidget(self.character_label)
        character_layout.addWidget(self.list_widget)
        character_layout.addWidget(self.addItem_button)
        character_layout.addWidget(self.reselect_button)

        ############################################### グループに各レイアウトを追加
        main_layout.addLayout(title_layout)
        main_layout.addLayout(template_layout)
        main_layout.addLayout(character_layout)
        
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
            
            delete_button = QtWidgets.QPushButton("Delete")
            delete_button.clicked.connect(lambda: self.deleteItem(item_widget))
            
            item_layout.addWidget(meshName_label)
            item_layout.addWidget(uv_comboBbox)
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

class MyWidget(QtWidgets.QWidget):
    NETWORK_NODE_NAME = "customToolNetwork"
    aa = []
    
    def __init__(self):
        super(MyWidget, self).__init__()

        # メインレイアウト
        main_layout = QtWidgets.QVBoxLayout(self)
        
        # タイトルリスト
        titleList = [f"顔メッシュ", f"眼球", f"まぶた(上)", f"まぶた(下)", f"歯(上)", f"歯(下)", f"舌"]
        # アイコンリスト
        iconDirectry = "C:/D/GitHub/CreateJointsOnMeshComponents/scripts/tools/icons"
        iconNameList = ["regist.png", "execute.png", "menu.png", "reselect.png", "regist.png", "execute.png", "menu.png"]
        
        # MeshItemGroupを複数インスタンス化
        for i in range(len(titleList)):  # 3つのFFFグループを追加
            meshItemGroup = MeshItemGroup(title=titleList[i], iconDir=iconDirectry, iconName=iconNameList[i], arg0 = self.aa)
            
            main_layout.addWidget(meshItemGroup)

        # メインレイアウトを設定
        self.setLayout(main_layout)    

# 実行
if __name__ == "__main__":
    maya_main_window = get_maya_main_window()
    widget = MyWidget()
    widget.setWindowTitle("My Custom Tool")
    widget.setGeometry(300, 300, 400, 300)  # ウィジェットのサイズと位置を設定
    widget.show()


//////////////////////////
from PySide6 import QtWidgets, QtGui
import shiboken6
import maya.cmds as cmds
import maya.OpenMayaUI as omui
import json

def get_maya_main_window():
    ptr = omui.MQtUtil.mainWindow()
    return shiboken6.wrapInstance(int(ptr), QtWidgets.QWidget)



class CustomToolUI(QtWidgets.QDialog):
    NETWORK_NODE_NAME = "customToolNetwork"
    FFF_temp_mesh = ""

    def __init__(self, parent=get_maya_main_window()):
        super(CustomToolUI, self).__init__(parent)
        
        self.setWindowTitle("Custom Tool UI")
        self.setFixedSize(600, 500)
        
        main_layout = QtWidgets.QVBoxLayout(self)
        self.setLayout(main_layout)

        fff_group = QtWidgets.QGroupBox()
        fff_layout = QtWidgets.QVBoxLayout(fff_group)
        #fff_group.setStyleSheet("QGroupBox { font-size: 18px; }")
        
        template_group = QtWidgets.QGroupBox("Face Skin Mesh")
        template_layout = QtWidgets.QHBoxLayout()
        
        self.template_label = QtWidgets.QLabel("template mesh")
        self.text_box = QtWidgets.QLineEdit()
        template_layout.addWidget(self.template_label)
        template_layout.addWidget(self.text_box)
        
        self.combo_box = QtWidgets.QComboBox()
        self.combo_box.addItems(["Option 1", "Option 2", "Option 3"])
        template_layout.addWidget(self.combo_box)
        
        self.other_button = QtWidgets.QPushButton("Other Button")
        self.other_button.clicked.connect(self.set_template_mesh)
        template_layout.addWidget(self.other_button)
        
        template_group.setLayout(template_layout)
        
        character_group = QtWidgets.QGroupBox("Character Mesh")
        character_layout = QtWidgets.QHBoxLayout()
        
        self.template_label = QtWidgets.QLabel("character mesh")
        
        self.list_widget = QtWidgets.QListWidget()
        character_layout.addWidget(self.list_widget)
        
        self.add_button = QtWidgets.QPushButton("Add Item")
        self.add_button.clicked.connect(self.add_item)
        character_layout.addWidget(self.add_button)
        
        character_group.setLayout(character_layout)
        
        fff_layout.addWidget(template_group)
        fff_layout.addWidget(character_group)
        main_layout.addWidget(fff_group)
        
        self.load_data_from_scene()
        self.setLayout(main_layout)

    def set_template_mesh(self):
        selected = cmds.ls(selection=True, long=True)
        if selected:
            CustomToolUI.FFF_temp_mesh = selected[0]
            self.text_box.setText(selected[0])

    def add_item(self):
        selected_meshes = cmds.ls(selection=True, shortNames=True, type="transform")
        
        for mesh_name in selected_meshes:
            item_widget = QtWidgets.QWidget()
            item_layout = QtWidgets.QHBoxLayout(item_widget)
            item_layout.setContentsMargins(5, 1, 1, 5) 
            
            label = QtWidgets.QLabel(mesh_name)
            item_layout.addWidget(label)
            
            combo_box = QtWidgets.QComboBox()
            uv_sets = cmds.polyUVSet(mesh_name, query=True, allUVSets=True) or []
            combo_box.addItems(uv_sets)
            item_layout.addWidget(combo_box)
            
            delete_button = QtWidgets.QPushButton("Delete")
            delete_button.clicked.connect(lambda: self.delete_item(item_widget))
            item_layout.addWidget(delete_button)
            
            list_item = QtWidgets.QListWidgetItem(self.list_widget)
            list_item.setSizeHint(item_widget.sizeHint())
            self.list_widget.addItem(list_item)
            self.list_widget.setItemWidget(list_item, item_widget)

    def delete_item(self, item_widget):
        for i in range(self.list_widget.count()):
            if self.list_widget.itemWidget(self.list_widget.item(i)) == item_widget:
                self.list_widget.takeItem(i)
                break

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


    def add_item_with_data(self, mesh_name, uv_set):
        item_widget = QtWidgets.QWidget()
        item_layout = QtWidgets.QHBoxLayout(item_widget)
        
        label = QtWidgets.QLabel(mesh_name)
        item_layout.addWidget(label)
        
        combo_box = QtWidgets.QComboBox()
        uv_sets = cmds.polyUVSet(mesh_name, query=True, allUVSets=True) or []
        combo_box.addItems(uv_sets)
        combo_index = combo_box.findText(uv_set)
        if combo_index >= 0:
            combo_box.setCurrentIndex(combo_index)
        item_layout.addWidget(combo_box)
        
        delete_button = QtWidgets.QPushButton("Delete")
        delete_button.clicked.connect(lambda: self.delete_item(item_widget))
        item_layout.addWidget(delete_button)
        
        list_item = QtWidgets.QListWidgetItem(self.list_widget)
        list_item.setSizeHint(item_widget.sizeHint())
        self.list_widget.addItem(list_item)
        self.list_widget.setItemWidget(list_item, item_widget)

    def closeEvent(self, event):
        self.save_data_to_scene()
        super(CustomToolUI, self).closeEvent(event)

try:
    custom_tool_ui.close()
except:
    pass

custom_tool_ui = CustomToolUI()
custom_tool_ui.show()


///////////////////////////////////////////////////////////
from maya import cmds
from PySide6 import QtWidgets, QtCore

class AAA(QtWidgets.QGroupBox):
    def __init__(self, title, instance_name, parent=None):
        super(AAA, self).__init__(parent)
        self.setTitle(title)
        self.instance_name = instance_name

        # レイアウト
        self.layout = QtWidgets.QVBoxLayout(self)

        # テキストボックスを追加
        self.textbox = QtWidgets.QLineEdit(self)
        self.layout.addWidget(self.textbox)

        # リストウィジェットを作成
        self.list_widget = QtWidgets.QListWidget(self)
        self.list_widget.setFixedHeight(1.5)  # 初期高さを1.5pxに設定
        self.layout.addWidget(self.list_widget)

        # 初期アイテムを追加
        self.add_initial_items()

        # アイテム数変更に応じてリストの高さを更新
        self.list_widget.itemChanged.connect(self.update_list_height)
        self.update_list_height()  # 初期アイテム追加後の高さ調整

        # データのロード
        self.load_data()

    def add_initial_items(self):
        """リストに固定アイテムを追加"""
        self.list_widget.addItem("Item 1")
        self.list_widget.addItem("Item 2")

    def update_list_height(self):
        """アイテム数に応じてリストの縦幅を調整"""
        item_count = self.list_widget.count()
        base_height = 1.5  # 初期の高さ
        self.list_widget.setFixedHeight(base_height + max(0, item_count - 1) * 1.5)

    def save_data(self):
        """Mayaシーンにデータを保存"""
        cmds.fileInfo(f"{self.instance_name}_textbox", self.textbox.text())

        # リストアイテムの保存
        for i in range(self.list_widget.count()):
            item_text = self.list_widget.item(i).text()
            cmds.fileInfo(f"{self.instance_name}_item_{i}", item_text)

    def load_data(self):
        """Mayaシーンからデータをロード"""
        textbox_data = cmds.fileInfo(f"{self.instance_name}_textbox", query=True)
        if textbox_data:
            self.textbox.setText(textbox_data)

        # リストアイテムのロード
        i = 0
        while True:
            item_data = cmds.fileInfo(f"{self.instance_name}_item_{i}", query=True)
            if not item_data:
                break
            self.list_widget.addItem(item_data)
            i += 1
        self.update_list_height()

class MyTool(QtWidgets.QWidget):
    def __init__(self):
        super(MyTool, self).__init__()
        self.layout = QtWidgets.QVBoxLayout(self)

        # AAAクラスのインスタンスを追加
        self.instance1 = AAA("AAA Instance 1", "instance1", self)
        self.instance2 = AAA("AAA Instance 2", "instance2", self)

        self.layout.addWidget(self.instance1)
        self.layout.addWidget(self.instance2)

        # 保存ボタン
        self.save_button = QtWidgets.QPushButton("Save Data", self)
        self.save_button.clicked.connect(self.save_all_data)
        self.layout.addWidget(self.save_button)

        # その他のUI要素の追加
        self.show()

    def save_all_data(self):
        """全インスタンスのデータを保存"""
        self.instance1.save_data()
        self.instance2.save_data()

# ツールを起動する関数
def launch_tool():
    try:
        cmds.deleteUI("myToolWindow")
    except:
        pass

    window = MyTool()
    window.setObjectName("myToolWindow")
    window.setWindowTitle("My Tool")
    window.show()

# ツールを起動
launch_tool()
