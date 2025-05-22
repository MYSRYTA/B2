__version__ = '2.3'
import os
from maya import mel
from . import common as cmn, UI, paintfrost, manipulator

print("uhhhh holoooooooooooooooooo")
os.environ['MAYA_SCRIPT_PATH'] = f"{cmn.MP}/mel;{os.environ['MAYA_SCRIPT_PATH']}"
BIF_LIB_PATH = cmn.MP + "/BEP_Compounds/bifrost_lib_config.json"
if os.path.isfile(BIF_LIB_PATH):
    os.environ['BIFROST_LIB_CONFIG_FILES'] = f"{os.environ['BIFROST_LIB_CONFIG_FILES']};{BIF_LIB_PATH}"


def sourceMel():
    mel.eval(f'source "bifrostGraphMenu.mel"')
    mel.eval(f'source "bifrostEditorPlus_defaultObjects.mel"')
    mel.eval(f'source "PaintFrost.mel"')


def currentNode():
    return UI.WIN_MNGR.current_node_path


def currentGraph():
    cmn.updateBifContainer()
    return cmn.BIF_CONTAINER


def reloadPalette():
    UI.WIN_MNGR.plw.widget().populateTree()


def resetFeedback():
    cmn.resetFeedback()


sourceMel()
