from shiboken6 import wrapInstance
from PySide6.QtWidgets import QApplication, QMenu

def getBifrostEditorMenuActionNames():
    for widget in QApplication.topLevelWidgets():
        if "Bifrost Graph Editor" in widget.windowTitle():
            for menubar in widget.findChildren(QMenu):
                for action in menubar.actions():
                    print(action.text())

def getMenuAction(actionName):
    for widget in QApplication.topLevelWidgets():
        if "Bifrost Graph Editor" in widget.windowTitle():
            for menubar in widget.findChildren(QMenu):
                for action in menubar.actions():
                    if action.text().strip() == actionName:
                        return action
    return None

def setEnabledMenuAction(actionName, condition):
    action = getMenuAction(actionName)
    if action:
        action.setEnabled(condition)
 
def setNameMenuAction(actionName, newName):
    action = getMenuAction(actionName)
    if action:
        action.setText(newName)
 
def setHookMakeCompoundAction():
    def inline_confirmDialog():
        result = cmds.confirmDialog(
            title="警告",
            message="コンパウンドのリファレンスを解除すると参照元の更新が反映されなくなります。\n実行しますか？",
            button=["OK", "Cancel"],
            defaultButton="OK",
            cancelButton="Cancel",
            dismissString="Cancel",
            icon="warning"
        )
        if result == 'OK':
            cmds.vnnCompound("|bifrostGraph1|bifrostGraphShape1", "/", create="compound")
    action = getMenuAction("Make Compound	Ctrl+G")
    if action:
        action.triggered.disconnect()
        action.triggered.connect(inline_confirmDialog)
        return 
                        
# getBifrostEditorMenuActionNames（）で取得したエディターメニューのアクション名
# 引数として使用する場合はショートカット名まで含む
# New Graph
# Create
# Backdrop	Shift+B
# Sticky Note	Shift+S
# Watchpoint	W
# Publish
# Publish Compound	Ctrl+P
# Publish Graph...	Ctrl+Shift+P
# Select
# 
# Connect Selected Nodes	C
# Compound
# Make Compound Editable
# Explode Compound
# Make Compound	Ctrl+G
# 
# Convert Graph
# Select Graph in Maya	D
# 
# Apply Wedge ...
# Batch Execute ...
# Cancel Batch Execute ...
# 
# Pause Graph Execution	Ctrl+.
# 
# Reset Feedback State	Ctrl+R
# Upstream	,
# Downstream	.
# Up and Downstream	/
# BifrostBoard (DG node) to BifrostGraphShape
# BifrostGraphShape to BifrostBoard (DG node)
# Go Into Compound	I
# Go Out Of Compound	U
# Node Collapsing
# Collapse All	1
# Partially Collapse All	2
# Expand All	3
# Name Display Mode
# Show Node Names	N
# Show Node Types	T
# 
# Auto-Layout Selected	L
# Show Grid	G
# Snap to Grid	\
# Connection Style
# Connect Nodes on Creation
# Copy Input Connections on Paste
# Shift on Connect
# Connect on Drop
# Highlight Connections on Selected
# Resumable after Esc. Termination
# Bezier
# Straight Lines
# Corner Lines
# S-Shape
# Parameter Editor
# Information Panel
# Node Library
# Bifrost Help
# Bifrost Tutorials
# Bifrost Sample Graphs
# Bifrost Community Hub
# About Bifrost
# Edit
# Display
# Options
# Windows
# Help
