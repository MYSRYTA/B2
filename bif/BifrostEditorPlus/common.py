import os, json
from PySide2.QtWidgets import QWidget, QApplication, QMenuBar, QMessageBox
from PySide2.QtCore import QEventLoop, QPoint
from PySide2.QtGui import QCursor
from maya import cmds as mc, OpenMayaUI as omui, mel
from shiboken2 import wrapInstance
from .modules import pyperclip


class undoChunk():
    def __init__(self, name=''):
        self.name = name

    def __enter__(self):
        mc.undoInfo(ock=True, cn=self.name)
        return self

    def __exit__(self, type, value, traceback):
        mc.undoInfo(cck=True, cn=self.name)


class undoDisabler():
    def __enter__(self):
        mc.undoInfo(swf=False)
        return self

    def __exit__(self, type, value, traceback):
        mc.undoInfo(swf=True)


def processQt(ms=200):
    QApplication.processEvents(QEventLoop.AllEvents, ms)


def setUndo(state):
    mc.undoInfo(swf=state)


def melPrint(text):
    mel.eval(f'print "{text}"')


# copy text to clipboard, store existing clipboard to buffer to recover later
def safeCopy(text):
    global CB_BUFFER
    CB_BUFFER = pyperclip.paste()
    pyperclip.copy(text)


# returns clipboard text, then restores buffer to clipboard
def safePaste():
    global CB_BUFFER
    cb = pyperclip.paste()
    pyperclip.copy(CB_BUFFER)
    CB_BUFFER = ''
    return cb


def findControl(name, typ):
    swig = omui.MQtUtil.findControl(name)
    return wrapInstance(int(swig), typ)


def updateBifContainer():
    global BIF_CONTAINER
    global CURRENT_COMPOUND
    global CONTAINER_IS_BOARD
    tabWig = BIF_WINDOW.bep_tab_ref.children()[0]
    try:
        btn = tabWig.currentWidget().children()[1].children()[1].children()[4]
        BIF_CONTAINER = btn.text()
        CONTAINER_IS_BOARD = mc.nodeType(BIF_CONTAINER) == "bifrostBoard"
        CURRENT_COMPOUND = getCurrentCompound()
        if not CURRENT_COMPOUND.endswith('/'):
            CURRENT_COMPOUND = CURRENT_COMPOUND + '/'
    except AttributeError:
        BIF_CONTAINER = ''


def pathIsCompound(path):
    if path == '/':
        return False
    try:
        # does nothing will error if path is not a compound
        nt = mc.vnnCompound(BIF_CONTAINER, path, nt=True)
        return True
    except RuntimeError:
        return False


def compoundType(path):
    if path == '/':
        return None
    try:
        # does nothing will error if path is not a compound
        nt = mc.vnnCompound(BIF_CONTAINER, path, nt=True)
        return mc.vnnNode(BIF_CONTAINER, path, qtn=True)
    except RuntimeError:
        return None


def bifPaste():
    mc.vnnCompoundEditor(name='bifrostGraphEditorControl', sk=[86, 67108864])
    safePaste()


def getCurrentCompound():
    return mc.vnnCompoundEditor(q=True, cc=True, name='bifrostGraphEditorControl')


def resetFeedback():
    edit_menu = findControl('vnnUI::CEMenuBar', QMenuBar).children()[2]
    edit_menu.popup(QPoint(0, 0))
    edit_menu.hide()
    edit_menu.actions()[13].trigger()


def reloadBifrostPlugin():
    dia = QMessageBox()
    dia.setWindowTitle('Reload Warning')
    dia.setText('Warning: Reloading the Bifrost plugin can be unstable, continue?')
    dia.addButton(QMessageBox.Yes)
    dia.addButton(QMessageBox.Cancel)
    pos = QCursor.pos() + QPoint(-30, -74)
    dia.move(pos)
    dia.exec_()
    if dia.result() != QMessageBox.Yes:
        return

    tmp = f'{MP}/resources/tmp.ma'
    sn = mc.file(q=True, sn=True)
    if not sn:
        sn = 'untitled.ma'

    mc.file(rename=tmp)
    mc.file(save=True, typ='mayaAscii', force=True)

    nodes = mc.ls(type='bifrostGraphShape') + mc.ls(type='bifrostBoard')
    mc.delete(nodes)
    mc.flushUndo()
    mc.unloadPlugin("bifrostGraph", f=True)
    mc.loadPlugin("bifrostGraph")

    mc.file(tmp, open=True, f=True)
    os.remove(tmp)

    mc.file(rename=sn)


def verifyDir(path):
    if not os.path.isdir(path):
        os.makedirs(path)
    return path


def readJson(path, default=None):
    if not os.path.isfile(path):
        return default
    with open(path) as file:
        data = json.load(file)
    return data


def writeJson(path, data):
    with open(path, 'w') as file:
        json.dump(data, file, indent=4)


def setPref(key, value):
    PREFS[key] = value
    writeJson(MP + '/userdata/preferences.json', PREFS)


def togglePref(key):
    new_state = 1 - PREFS[key]
    setPref(key, new_state)
    return new_state


def readPrefs():
    defaultPrefs = {
        "out_properties": 'point_color point_velocity',
        "out_transfer_normals": True,
        "out_diplsay_property": 'point_color',
        "out_generic_color_display": True,
        "out_motion_vectors": 'point_velocity',
        "replace_search": False,
        "reset_feedback_shortcut": True,
        "manip_mode": 0,
        "manip_allow_select": True,
        "manip_hidden_outliner": True,
        "manip_auto_graph_selectable": True,
        "manip_native_nodes": True
    }
    prefs = {}
    path = MP + '/userdata/preferences.json'
    if not os.path.exists(os.path.dirname(path)):
        os.makedirs(os.path.dirname(path))
    if os.path.isfile(path):
        json = readJson(path)
        if type(json) is dict:
            prefs = json
    for key in defaultPrefs.keys():
        if not key in prefs:
            prefs[key] = defaultPrefs[key]
    writeJson(path, prefs)
    return prefs


MP = os.path.normpath(os.path.dirname(__file__)).replace('\\', '/')
BIF_CONTAINER = ''
CURRENT_COMPOUND = ''
CONTAINER_IS_BOARD = False
CB_BUFFER = ''
BIF_WINDOW = None
PREFS = readPrefs()
