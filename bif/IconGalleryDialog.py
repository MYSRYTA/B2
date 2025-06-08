from PySide6 import QtWidgets, QtGui, QtCore
import os
import maya.cmds as cmds


class IconGalleryDialog(QtWidgets.QDialog):
    selectedIconPath = None

    def __init__(self, rootDirList, ignoreDirList = [], thumbnailSize = 30, parent = None):
        super().__init__(parent)
        self.setMinimumSize(600, 500)
        self.setWindowFlags(QtCore.Qt.FramelessWindowHint | QtCore.Qt.Popup)
        
        mainLayout = QtWidgets.QVBoxLayout(self)
        
        self.gridWidget = QtWidgets.QWidget()
        self.gridLayout = QtWidgets.QGridLayout(self.gridWidget)
        
        self.scrollArea = QtWidgets.QScrollArea()
        self.scrollArea.setWidgetResizable(True)
        self.scrollArea.setWidget(self.gridWidget)
        mainLayout.addWidget(self.scrollArea)

        self.setupGallery(rootDirList, thumbnailSize)

    def setupGallery(self, rootDirList, thumbnailSize):        
        valid_exts = ('.png', '.ico', '.svg')

        width = self.gridWidget.width()
        maxColumns = max(1, width // (thumbnailSize *2))
        row, col = 0, 0
        for rootDir in rootDirList:
            for root, dirnames, files in os.walk(rootDir):
                # ignoreDirListに一致しるフォルダを省いたリストで完全に上書きする
                dirnames[:] = [d for d in dirnames if not d in ignoreDirList]
                # アイコンファイルを順にボタンとして登録する
                for f in files:
                    if not f.lower().endswith(valid_exts):
                        continue
                        
                    iconPath = os.path.join(root, f)
                    thumbnailButton = QtWidgets.QPushButton()
                    thumbnailButton.setCheckable(True)
                    thumbnailButton.setIcon(QtGui.QIcon(iconPath))
                    thumbnailButton.setIconSize(QtCore.QSize(thumbnailSize, thumbnailSize))
                    thumbnailButton.clicked.connect(lambda b=thumbnailButton, p=iconPath: self.selected(path=p))
                    thumbnailButton.setStyleSheet(
                    """
                        QPushButton {border: none; background-color: transparent; padding: 2px;}
                        QPushButton:hover {background-color: rgba(10, 10, 30, 30%); border-radius: 5px;}
                    """)
                    
                    self.gridLayout.addWidget(thumbnailButton, row, col)
                    col += 1
                    if maxColumns <= col:
                        col = 0
                        row += 1

    def selected(self, path):
        self.selectedIconPath = path
        self.accept()
        
    def getSelectedIconPath(self):
        return self.selectedIconPath  
    
def showIconGalleryDialog(rootDirectoryList, ignoreDirList):
    result = None
    dialog = IconGalleryDialog(rootDirectoryList, ignoreDirList)
    dialog.move(QtGui.QCursor.pos())
    if dialog.exec() == QtWidgets.QDialog.Accepted:
        result = dialog.getSelectedIconPath()
    return result

def getBifrostPath():
    result = None
    pluginName = "bifrostGraph"
    if cmds.pluginInfo(pluginName, query=True, loaded=True):
        path = cmds.pluginInfo(pluginName, query=True, path=True)
        if "plug-ins" in path:
            result = path.split("/plug-ins")[0]
    return result
        


bifrostDir = getBifrostPath()
if bifrostDir:
    iconDirList = [f"{bifrostDir}/icons", f"{bifrostDir}/resources", f"{bifrostDir}/packs"] 
    ignoreDirList = ["graphs"]
    aa = showIconGalleryDialog(iconDirList, ignoreDirList)
    print(aa)