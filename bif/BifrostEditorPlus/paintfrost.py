from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *
import maya.OpenMayaUI as omui
from shiboken2 import wrapInstance, getCppPointer

CURRENT_WIG: QWidget


class TabletEventFilter(QObject):
    tablet_data = [1, 0, 0, 0]
    pres_min = 0

    def eventFilter(self, obj, event):
        if event.type() == QEvent.TabletMove:
            self.tablet_data = [event.pressure(), event.xTilt(), event.yTilt(), int(event.pointerType()==QTabletEvent.Eraser)]

    def resetData(self):
        self.tablet_data = [1, 0, 0, 0]


def installEF():
    global CURRENT_WIG
    CURRENT_WIG = QApplication.widgetAt(QCursor.pos())
    CURRENT_WIG.installEventFilter(EF)


def removeEF():
    CURRENT_WIG.removeEventFilter(EF)


def getTablet():
    data = str(EF.tablet_data).replace('[', '{').replace(']', '}')
    return data


EF = TabletEventFilter()
QTabWidget
