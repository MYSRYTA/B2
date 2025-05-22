import os, math
import xml.etree.ElementTree as ET
from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *
from maya import cmds as mc, mel
from . import common as cmn, IO, seedShuffler


class TabsEventFilter(QObject):
    def __init__(self):
        super(self.__class__, self).__init__()
        self.owner = None

    def eventFilter(self, obj, event):
        if event.type() == QEvent.LayoutRequest:
            initTab()
        return False


class ParamPanelExtras(QStackedWidget):
    class InputButton(QPushButton):
        curve = False
        local_attr = ''
        world_attr = ''
        data_type = ''
        alwaysHelp = False
        shape = False
        is_output = False

        def __init__(self, *args, **kwargs):
            super(self.__class__, self).__init__(*args, **kwargs)
            rad = '18'
            if self.maximumHeight() < 46:
                rad = '8'
            self.setStyleSheet(BTN_STYLE.replace('%rad', rad))
            self.setIconSize(self.size())
            self.clicked.connect(lambda: self.inputSel(''))

        def contextMenuEvent(self, event):
            menu = QMenu(self.parent())

            ports = IO.listInputs(self.data_type)
            for port in ports:
                menu.addAction(port, lambda x=port: self.inputSel(x))

            menu.addSeparator().setText('array')

            ports = IO.listInputs(f'array<{self.data_type}>')
            for port in ports:
                menu.addAction(port, lambda x=port: self.inputSel(x, array=True))

            if len(menu.actions()) < 2:
                return

            menu.exec_(event.globalPos())

        def inputSel(self, port, array=False):
            if QApplication.keyboardModifiers() == Qt.AltModifier:
                attr = self.local_attr
            else:
                attr = self.world_attr

            with cmn.undoChunk("BEP_inputSel") as uc:
                if self.curve:
                    IO.inputSelCurves(attr, WIN_MNGR.current_node, input_port=port, array=array)
                else:
                    IO.inputSel(attr, WIN_MNGR.current_node, self.shape, input_port=port, array=array)

    class OutputButton(QPushButton):
        out_wig = None

        def __init__(self, *args, **kwargs):
            super(self.__class__, self).__init__(*args, **kwargs)
            rad = '18'
            if self.maximumHeight() < 46:
                rad = '8'
            self.setStyleSheet(BTN_STYLE.replace('%rad', rad))
            self.setIconSize(self.size())

            self.clicked.connect(lambda: self.contextMenuEvent(None))
            self.clicked.connect(lambda: WIN_MNGR.param_extras.updateGraphDisplayPanel())

        def contextMenuEvent(self, event):
            menu = QMenu(self.parent())
            data_types = ['Object', 'array<Object>']
            for data_type in data_types:
                ports = IO.listOutputs(data_type)
                if ports:
                    for port in ports:
                        menu.addAction(port, lambda x=port: IO.outputMesh(x))

            if len(menu.actions()) < 1:
                return False

            menu.exec_(QCursor.pos())

    def __init__(self):
        super(self.__class__,self).__init__()
        self.setContentsMargins(2,2,2,2)
        WIN_MNGR.selectionChanged.connect(self.setWigFromSelNode)

        iconpath = cmn.MP + '/resources/'
        size = 42
        wig = QWidget()
        self.addWidget(wig)
        wig.setLayout(QHBoxLayout())
        wig.layout().setContentsMargins(0, 0, 0, 0)

    # misc page
        wig = QWidget()
        self.addWidget(wig)
        self.script_btns_layout = QVBoxLayout(spacing=2)
        wig.setLayout(self.script_btns_layout)
        wig.layout().setContentsMargins(0, 0, 0, 0)

    # input buttons page
        wig = QWidget()
        self.addWidget(wig)
        wig.setLayout(QHBoxLayout(spacing=4))
        wig.layout().addStretch()
        wig.layout().setContentsMargins(0, 0, 0, 0)

        btn = self.InputButton(fixedWidth=size, fixedHeight=size)
        btn.setIcon(QIcon(iconpath + 'mesh.png'))
        btn.world_attr, btn.local_attr, btn.data_type = 'worldMesh', 'outMesh', 'Object'
        btn.shape = True
        wig.layout().addWidget(btn)

        btn = self.InputButton(fixedWidth=size, fixedHeight=size)
        btn.setIcon(QIcon(iconpath + 'strands.png'))
        btn.world_attr, btn.local_attr, btn.data_type = 'worldSpace', 'local', 'Object'
        btn.curve, btn.shape = True, True
        wig.layout().addWidget(btn)

        btn = self.InputButton(fixedWidth=size, fixedHeight=size)
        btn.setIcon(QIcon(iconpath + 'transform.png'))
        btn.world_attr, btn.local_attr, btn.data_type = 'worldMatrix', 'matrix', 'Math::float4x4'
        wig.layout().addWidget(btn)

        btn = self.InputButton(fixedWidth=size, fixedHeight=size)
        btn.setIcon(QIcon(iconpath + 'translate.png'))
        btn.world_attr, btn.local_attr, btn.data_type = 'translate', 'translate', 'Math::float3'
        wig.layout().addWidget(btn)

        btn = self.InputButton(fixedWidth=size, fixedHeight=size)
        btn.setIcon(QIcon(iconpath + 'locator.png'))
        btn.shape = True
        btn.world_attr, btn.local_attr, btn.data_type = 'worldPosition', 'worldPosition', 'Math::float3'
        wig.layout().addWidget(btn)

        wig.layout().addStretch()

    # output buttons page
        self.output_wig = QWidget()
        self.output_wig.setLayout(QVBoxLayout())
        self.output_wig.layout().setContentsMargins(0, 0, 0, 0)

        btn = self.OutputButton(fixedWidth=size, fixedHeight=size)
        btn.out_wig = self
        btn.setIcon(QIcon(iconpath + 'output.png'))
        self.output_wig.layout().addWidget(btn, alignment=Qt.AlignHCenter)

        form = QFormLayout()
        self.output_wig.layout().addLayout(form)

        self.out_props_line = QLineEdit(cmn.PREFS['out_properties'])
        self.out_props_line.textChanged.connect(lambda x: cmn.setPref('out_properties', x))
        form.addRow('Properties', self.out_props_line)
        self.generic_color_display = QCheckBox('Transfer Normals', checked=cmn.PREFS['out_transfer_normals'])
        self.generic_color_display.stateChanged.connect(lambda x: cmn.setPref('out_transfer_normals', bool(x)))
        form.addWidget(self.generic_color_display)
        self.color_channel_line = QLineEdit(cmn.PREFS['out_diplsay_property'])
        self.color_channel_line.textChanged.connect(lambda x: cmn.setPref('out_diplsay_property', x))
        form.addRow('Display Property', self.color_channel_line)
        self.generic_color_display = QCheckBox('Generic Color Display', checked=cmn.PREFS['out_generic_color_display'])
        self.generic_color_display.stateChanged.connect(lambda x: cmn.setPref('out_generic_color_display', x))
        form.addWidget(self.generic_color_display)
        self.mv_line = QLineEdit(cmn.PREFS['out_motion_vectors'])
        self.mv_line.textChanged.connect(lambda x: cmn.setPref('out_motion_vectors', x))
        form.addRow('Motion Vectors', self.mv_line)

        # line
        frame = QFrame()
        frame.setFrameShape(QFrame.HLine)
        frame.setFixedHeight(2)
        self.output_wig.layout().addWidget(frame)

        # display attrs panel
        self.graph_display_wig = QWidget()
        self.graph_display_wig.setLayout(QGridLayout(spacing=8))
        self.buildGraphDisplayPanel()

        # seed shuffler pannel
        self.shuffler = seedShuffler.ShufflerWidget()
        self.addWidget(self.shuffler)

        # parent wig
        wig = QWidget()
        wig.setLayout(QVBoxLayout())
        wig.layout().setContentsMargins(0, 0, 0, 0)
        wig.layout().addWidget(self.output_wig)
        wig.layout().addWidget(self.graph_display_wig)
        self.addWidget(wig)

    def setWigFromSelNode(self, node, comp_type):
        if comp_type != "!":  # normal compound
            if comp_type.endswith('BEP,seed_shuffler'):
                self.shuffler.showShuffler(self, node)
            else:
                self.showCompoundScripts(node, comp_type)
            return

        cmn.safeCopy('')
        try:
            mc.vnnCopy(cmn.BIF_CONTAINER, cmn.CURRENT_COMPOUND, sourceNode=node)
        except RuntimeError:  # bad path or node
            self.hide()
            return
        root = ET.fromstring(cmn.safePaste())
        try:
            node_flag = int(root[0][0][0].get('flag'))
            node_type = root[0][0][0].get('type')
        except IndexError:  # probably nothing selected
            node_flag = -1
            node_type = ''

        if node_flag == 0:  # generic node selected
            self.hide()
        elif node_flag == 8:  # input node
            self.show()
            self.setCurrentIndex(2)
            self.setFixedHeight(60)
        elif cmn.CURRENT_COMPOUND == '/' or node_flag == 16 or node_type == 'BifrostGraph,Core::Graph,terminal':  # output/terminal
            self.show()
            self.setCurrentIndex(4)
            self.updateGraphDisplayPanel()
            self.setMaximumHeight(2000)
            self.output_wig.setVisible(True)
        else:  # nothing selected but inside a compound
            self.showCompoundScripts('', cmn.compoundType(cmn.CURRENT_COMPOUND))

    def showCompoundScripts(self, node, comp_type):
        self.setCurrentIndex(1)
        for wig in self.script_btns_layout.parent().children()[1:]:
            wig.deleteLater()

        target = cmn.CURRENT_COMPOUND + node
        if comp_type == "!":
            target = cmn.CURRENT_COMPOUND
            if not (cmn.pathIsCompound(target) and target.split('/')[-1] == node):
                self.hide()
                return

        md = mc.vnnCompound(cmn.BIF_CONTAINER, target, qmd='scriptButtons')
        if not md:
            self.hide()
            return

        self.show()
        btn_names = md[0].split(';')
        self.setFixedHeight(4 + 30 * len(btn_names))
        for btn_name in btn_names:
            btn = QPushButton(btn_name, fixedHeight=24)
            btn.setStyleSheet(BTN_STYLE.replace('%radpx', '2px'))
            btn.clicked.connect(lambda x=None, y=target, z=btn_name: self.execScriptButton(y, z))
            self.script_btns_layout.addWidget(btn)

    def buildGraphDisplayPanel(self):
        layout = self.graph_display_wig.layout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(QLabel('Viewport'), 0, 1, alignment=Qt.AlignCenter)
        layout.addWidget(QLabel('Render'), 0, 2, alignment=Qt.AlignCenter)
        for x, display_type in enumerate(('Final', 'Proxy', 'Diagnostic', 'Outputs')):
            layout.addWidget(QLabel(display_type), 1 + x, 0, alignment=Qt.AlignRight)
            attr = f'.display{display_type}InViewport'
            check_box = QCheckBox()
            check_box.stateChanged.connect(lambda x, y=attr: mc.setAttr(cmn.BIF_CONTAINER + y, bool(x)))
            layout.addWidget(check_box, 1 + x, 1, alignment=Qt.AlignCenter)

            attr = f'.display{display_type}InRenderer'
            check_box = QCheckBox()
            check_box.stateChanged.connect(lambda x, y=attr: mc.setAttr(cmn.BIF_CONTAINER + y, bool(x)))
            layout.addWidget(check_box, 1 + x, 2, alignment=Qt.AlignCenter)

    def updateGraphDisplayPanel(self):
        if cmn.CONTAINER_IS_BOARD:
            self.graph_display_wig.hide()
            return
        else:
            self.graph_display_wig.show()
        layout = self.graph_display_wig.layout()
        for x, display_type in enumerate(('Final', 'Proxy', 'Diagnostic', 'Outputs')):
            state = mc.getAttr(f'{cmn.BIF_CONTAINER}.display{display_type}InViewport')
            layout.itemAtPosition(1 + x, 1).widget().setChecked(state)

            state = mc.getAttr(f'{cmn.BIF_CONTAINER}.display{display_type}InRenderer')
            layout.itemAtPosition(1 + x, 2).widget().setChecked(state)

    @staticmethod
    def execScriptButton(path, btn_name):
        md = mc.vnnCompound(cmn.BIF_CONTAINER, path, qmd=btn_name + '_script')
        if md:
            if QApplication.keyboardModifiers() == Qt.AltModifier:
                print(f'\n# {btn_name} {"-" * (79 - len(btn_name))}\n')
                print(md[0])
                print('\n# ' + "-" * 80 + '\n')
            else:
                mc.evalDeferred(md[0])


class ScriptBtnWindow(QWidget):
    def __init__(self, *args, **kwargs):
        super(self.__class__, self).__init__(*args, **kwargs)
        self.setWindowTitle('Script Button')
        self.setObjectName('ScriptBtnWindow')
        self.setLayout(QVBoxLayout())

        self.label_line = QLineEdit(placeholderText='Button Label')
        self.script_edit = QPlainTextEdit(placeholderText='Button Script')
        self.layout().addWidget(self.label_line)
        self.layout().addWidget(self.script_edit)

        layout = QHBoxLayout()
        self.layout().addLayout(layout)

        btn = QPushButton('Remove All')
        btn.clicked.connect(lambda: self.deleteScriptButtons(WIN_MNGR.current_node_path))
        layout.addWidget(btn)

        btn = QPushButton('Add')
        btn.clicked.connect(lambda: self.addScriptButton(WIN_MNGR.current_node_path, self.label_line.text(), self.script_edit.toPlainText()))
        layout.addWidget(btn)

    @staticmethod
    def addScriptButton(path, btn_name, script):
        if not cmn.pathIsCompound(path):
            mc.warning('The selected node is a special type, buttons can only be added to compounds.')
            return

        if mc.vnnCompound(cmn.BIF_CONTAINER, path, qir=True):
            mc.warning("Can't add buttons to referenced compound.")
            return

        mc.vnnCompound(cmn.BIF_CONTAINER, path, setMetaData=[btn_name + '_script', script])
        md = mc.vnnCompound(cmn.BIF_CONTAINER, path, qmd='scriptButtons')
        if md:
            btn_name = md[0] + ';' + btn_name
        mc.vnnCompound(cmn.BIF_CONTAINER, path, setMetaData=['scriptButtons', btn_name])

    @staticmethod
    def deleteScriptButtons(path):
        if mc.vnnCompound(cmn.BIF_CONTAINER, path, qir=True):
            mc.warning("Can't remove buttons from referenced compound.")
            return
        md = mc.vnnCompound(cmn.BIF_CONTAINER, path, qmd='scriptButtons')
        if not md:
            return

        btn_names = md[0].split(';')

        for btn_name in btn_names:
            mc.vnnCompound(cmn.BIF_CONTAINER, path, clearMetaData=btn_name + '_script')
        mc.vnnCompound(cmn.BIF_CONTAINER, path, clearMetaData='scriptButtons')


class GetSetWindow(QWidget):
    class DragList(QListWidget):
        def __init__(self, *args, **kwargs):
            super(self.__class__, self).__init__(*args, **kwargs)
            self.setStyleSheet(LIST_STYLE)
            self.setSelectionMode(QAbstractItemView.ExtendedSelection)
            self.setDragDropMode(QAbstractItemView.InternalMove)

        def dragEnterEvent(self, event):
            super(self.__class__, self).dragEnterEvent(event)
            event.acceptProposedAction()

        def dragMoveEvent(self, event):
            super(self.__class__, self).dragMoveEvent(event)
            event.acceptProposedAction()

        def dropEvent(self, event):
            super(self.__class__, self).dropEvent(event)
            self.parent().sortAndWrite()

        def mimeData(self, items):
            super(self.__class__, self).mimeData(items)
            if QApplication.keyboardModifiers() == Qt.AltModifier:
                self.setDragDropMode(QAbstractItemView.InternalMove)
            else:
                self.setDragDropMode(QAbstractItemView.DragOnly)

            props = []
            for item in items:
                props.append(item.prop)
            mime = QMimeData()
            mime.setText(' '.join(props))
            return mime

    def __init__(self, *args, **kwargs):
        super(self.__class__, self).__init__(*args, **kwargs)
        self.setWindowTitle('Get/Set')
        self.setObjectName('GetSetWindow')
        self.setLayout(QVBoxLayout(spacing=3))
        self.layout().setContentsMargins(2,2,2,2)

        self.list = self.DragList(self)
        self.layout().addWidget(self.list)

        self.properties = cmn.readJson(cmn.MP + '/userdata/getter_setter.json')
        if not self.properties:
            self.properties = {
                "point_position": {
                    "target": "point_component",
                    "type": "Math::float3",
                    "default": "{0,0,0}"
                },
                "point_velocity": {
                    "target": "point_component",
                    "type": "Math::float3",
                    "default": "{0,0,0}"
                },
                "point_color": {
                    "target": "point_component",
                    "type": "Math::float4",
                    "default": "{1,0,0,1}"
                },
                "color": {
                    "target": "point_component",
                    "type": "Math::float4",
                    "default": "{0,0,1}"
                },
                "point_size": {
                    "target": "point_component",
                    "type": "float",
                    "default": "1"
                }
            }
        self.populate()

    def keyPressEvent(self, event):
        key = event.key()
        if key == Qt.Key_Delete:
            sel = self.list.selectedItems()
            if sel:
                for item in sel:
                    del self.properties[item.prop]
                    self.list.takeItem(self.list.row(item))
                self.sortAndWrite()

    def contextMenuEvent(self, event):
        menu = QMenu(self.parent())
        menu.addAction('Add Selected', self.registerProperty)
        menu.exec_(event.globalPos())

    def populate(self):
        self.list.clear()
        for prop in self.properties:
            # Build widget
            wig = QWidget()
            wig.setLayout(QHBoxLayout(spacing=3))
            wig.layout().setContentsMargins(2,2,2,2)
            wig.layout().addWidget(QLabel(prop))
            btn1 = QPushButton('Get', fixedWidth = 42, fixedHeight = 18)
            btn1.setStyleSheet(BTN_STYLE.replace('%radpx', '2px'))
            wig.layout().addWidget(btn1)
            btn2 = QPushButton('Set', fixedWidth = 42, fixedHeight = 18)
            btn2.setStyleSheet(BTN_STYLE.replace('%radpx', '2px'))
            wig.layout().addWidget(btn2)

            if self.properties[prop]['target']:
                btn1.clicked.connect(lambda x=None, y=prop: self.getGeoProperty(y))
                btn2.clicked.connect(lambda x=None, y=prop: self.setGeoProperty(y))
                item = QListWidgetItem(prop)

            else:
                btn1.clicked.connect(lambda x=None, y=prop: self.getProperty(y))
                btn2.clicked.connect(lambda x=None, y=prop: self.setProperty(y))
                item = QListWidgetItem()

            item.prop = prop
            item.setToolTip(str(self.properties[prop]))
            self.list.addItem(item)
            self.list.setItemWidget(item, wig)
            item.setSizeHint(QSize(0,22))

    def getProperty(self, prop):
        path = addNode("BifrostGraph,Core::Object,get_property")
        mc.vnnNode(cmn.BIF_CONTAINER, path, spv=["key", prop])
        mc.vnnNode(cmn.BIF_CONTAINER, path, setPortDataType=['default_and_type', self.properties[prop]['type']])
        if self.properties[prop]['default']:
            mc.vnnNode(cmn.BIF_CONTAINER, path, spv=["default_and_type", self.properties[prop]['default']])

    def setProperty(self, prop):
        path = addNode("BifrostGraph,Core::Object,set_property")
        mc.vnnNode(cmn.BIF_CONTAINER, path, spv=["key", prop])
        mc.vnnNode(cmn.BIF_CONTAINER, path, setPortDataType=['value', self.properties[prop]['type']])
        if self.properties[prop]['default']:
            mc.vnnNode(cmn.BIF_CONTAINER, path, spv=["value", self.properties[prop]['default']])

    def getGeoProperty(self, prop):
        path = addNode("BifrostGraph,Geometry::Properties,get_geo_property")
        mc.vnnNode(cmn.BIF_CONTAINER, path, spv=["property", prop])
        mc.vnnNode(cmn.BIF_CONTAINER, path, setPortDataType=['type', f"array<{self.properties[prop]['type']}>"])
        if self.properties[prop]['default']:
            mc.vnnNode(cmn.BIF_CONTAINER, path, spv=["type", self.properties[prop]['default']])

    def setGeoProperty(self, prop):
        path = addNode("BifrostGraph,Geometry::Properties,set_geo_property")
        mc.vnnNode(cmn.BIF_CONTAINER, path, spv=["property", prop])
        mc.vnnNode(cmn.BIF_CONTAINER, path, setPortDataType=['data', self.properties[prop]['type']])
        if self.properties[prop]['default']:
            mc.vnnNode(cmn.BIF_CONTAINER, path, spv=["data", self.properties[prop]['default']])
            mc.vnnNode(cmn.BIF_CONTAINER, path, spv=["default", self.properties[prop]['default']])
        mc.vnnNode(cmn.BIF_CONTAINER, path, spv=["target", self.properties[prop]['target']])

    def registerProperty(self):
        path = WIN_MNGR.current_node_path
        type_name = mc.vnnNode(cmn.BIF_CONTAINER, path, qtn=True)
        if type_name == 'BifrostGraph,Core::Object,get_property':
            prop = mc.vnnNode(cmn.BIF_CONTAINER, path, qpv="key")
            target = ''
            port_type = mc.vnnNode(cmn.BIF_CONTAINER, path, qpt="default_and_type")
            default = mc.vnnNode(cmn.BIF_CONTAINER, path, qpv="default_and_type")

        elif type_name == 'BifrostGraph,Core::Object,set_property':
            prop = mc.vnnNode(cmn.BIF_CONTAINER, path, qpv="key")
            target = ''
            port_type = mc.vnnNode(cmn.BIF_CONTAINER, path, qpt="value")
            default = mc.vnnNode(cmn.BIF_CONTAINER, path, qpv="value")

        elif type_name == 'BifrostGraph,Geometry::Properties,get_geo_property':
            prop = mc.vnnNode(cmn.BIF_CONTAINER, path, qpv="property")
            target = 'point_component'
            port_type = mc.vnnNode(cmn.BIF_CONTAINER, path, qpt="type")[6:-1]
            default = ''

        elif type_name == 'BifrostGraph,Geometry::Properties,set_geo_property':
            prop = mc.vnnNode(cmn.BIF_CONTAINER, path, qpv="property")
            target = mc.vnnNode(cmn.BIF_CONTAINER, path, qpv="target")
            port_type = mc.vnnNode(cmn.BIF_CONTAINER, path, qpt="default")
            default = mc.vnnNode(cmn.BIF_CONTAINER, path, qpv="default")

        else:
            cmn.melPrint('Compound is not get/set property')
            return

        if type(default) is list:
            default = str(default).replace("'", "").replace("[", "{").replace("]", "}")
        self.properties[prop] = {"target": target, "type": port_type, "default": default}
        self.populate()
        self.sortAndWrite()

    def sortAndWrite(self):
        buffer = {}
        for x in range(self.list.count()):
            prop = self.list.item(x).prop
            buffer[prop] = self.properties[prop]

        self.properties = buffer
        cmn.writeJson(cmn.MP + '/userdata/getter_setter.json', self.properties)


class BookmarkWindow(QWidget):
    class DragList(QListWidget):
        def dropEvent(self, event):
            super(self.__class__, self).dropEvent(event)
            self.parent().sortAndWrite()

    def __init__(self, *args, **kwargs):
        super(self.__class__, self).__init__(*args, **kwargs)
        self.setStyleSheet(LIST_STYLE)
        self.setWindowTitle('Bookmarks')
        self.setObjectName('BookmarkWindow')
        self.setLayout(QVBoxLayout(spacing=3))
        self.layout().setContentsMargins(2,2,2,2)

        self.list = self.DragList(self)
        self.list.setTextElideMode(Qt.ElideLeft)
        self.list.setDragDropMode(QAbstractItemView.InternalMove)
        self.layout().addWidget(self.list)
        self.list.itemDoubleClicked.connect(self.goToBookmark)
        self.list.itemChanged.connect(self.sortAndWrite)
        WIN_MNGR.tabInited.connect(self.populate)

    def keyPressEvent(self, event):
        key = event.key()
        if key == Qt.Key_Delete:
            sel = self.list.selectedItems()
            if sel:
                for item in sel:
                    self.list.takeItem(self.list.row(item))
                self.sortAndWrite()

    def showEvent(self, event):
        super(self.__class__, self).showEvent(event)
        self.populate()

    @staticmethod
    def goToBookmark(item):
        if QApplication.keyboardModifiers() == Qt.AltModifier:
            return
        try:
            mc.vnnCompoundEditor(name='bifrostGraphEditorControl', cc=item.text())
        except RuntimeError:
            cmn.melPrint('BifrostEditorPlus: Path not found')

    def contextMenuEvent(self, event):
        menu = QMenu(self.parent())
        item = self.list.itemAt(event.pos())
        if item:
            menu.addAction('Edit', lambda: self.list.editItem(item))
            menu.addSeparator()

        menu.addAction('Add Current Path', self.addBookmark)
        menu.exec_(event.globalPos())

    def addBookmark(self):
        self.populateSingle(cmn.CURRENT_COMPOUND)
        self.sortAndWrite()

    def populate(self, first_init=False):
        self.list.clear()
        if not (self.parent().isVisible() and cmn.BIF_CONTAINER):
            return
        md = mc.vnnCompound(cmn.BIF_CONTAINER, '/', qmd='bookmarks')
        if md:
            bookmarks = md[0].split(';')
        else:
            return
        for path in bookmarks:
            self.populateSingle(path)

    def populateSingle(self, path):
        item = QListWidgetItem(path)
        item.setSizeHint(QSize(200, 16))
        item.setFlags(item.flags() | Qt.ItemIsEditable)
        self.list.addItem(item)

    def sortAndWrite(self):
        bookmarks = []
        for x in range(self.list.count()):
            bookmarks.append(self.list.item(x).text())
        mc.vnnCompound(cmn.BIF_CONTAINER, '/', smd=['bookmarks', ';'.join(bookmarks)])


class PaletteWindow(QWidget):
    lib_comps = {}
    macro_dir = cmn.MP + '/userdata/macros/'

    class LibTree(QTreeWidget):
        palette_wig = None

        def contextMenuEvent(self, event):
            menu = QMenu(self.palette_wig)
            menu.addAction('Macro From Selection', self.palette_wig.createMacro)
            menu.exec_(event.globalPos())

    class NodeList(QListWidget):
        palette_wig = None

        def contextMenuEvent(self, event):
            item = self.itemAt(event.pos())
            if item:
                menu = QMenu(self.palette_wig)
                menu.addAction('Add to Hotbox', lambda: self.palette_wig.hotbox_tree.addToHotbox(item))
                if item.toolTip() == 'Macros ':
                    menu.addSeparator()
                    menu.addAction('Delete Macro', lambda: self.palette_wig.deleteMacro(item.text()))
                menu.exec_(event.globalPos())

        def keyPressEvent(self, event):
            super(self.__class__, self).keyPressEvent(event)
            if event.key() in (Qt.Key_Return, Qt.Key_Enter):
                if self.count():
                    sel = self.selectedItems()
                    if sel:
                        self.itemClicked.emit(sel[0])

    class HotboxTree(QTreeWidget):
        palette_wig = None

        def __init__(self, *args, **kwargs):
            super(self.__class__, self).__init__(*args, **kwargs)
            self.setTextElideMode(Qt.ElideNone)
            self.setSelectionMode(QTreeWidget.ExtendedSelection)
            self.setDragDropMode(QTreeWidget.InternalMove)

        def contextMenuEvent(self, event):
            item = self.itemAt(event.pos())
            menu = QMenu(self.palette_wig)
            if item:
                menu.addAction('Remove from Hotbox', self.removeSel)
                menu.addSeparator()
            menu.addAction('Create Menu', self.createHotboxMenu)
            menu.exec_(event.globalPos())

        def dropEvent(self, event):
            super(self.__class__, self).dropEvent(event)
            self.sortAndWriteHotbox()

        def removeSel(self):
            sel: [QTreeWidgetItem] = self.selectedItems()
            if sel:
                for item in sel:
                    if item.parent():
                        item.parent().takeChild(item.parent().indexOfChild(item))
                    else:
                        self.takeTopLevelItem(self.indexOfTopLevelItem(item))
                self.sortAndWriteHotbox()

        def addToHotbox(self, item):
            hotbox_dict = self.readHotbox()
            hotbox_dict[item.text()] = item.toolTip()
            self.writeHotbox(hotbox_dict)
            self.palette_wig.populateList()

        def createHotboxMenu(self):
            dia = QInputDialog()
            dia.setWindowTitle('Hotbox submenu')
            dia.setLabelText('Menu Name:')
            pos = QCursor.pos() + QPoint(-30, -74)
            dia.move(pos)
            dia.exec_()
            if dia.result():
                name = dia.textValue()
                hotbox_dict = self.readHotbox()
                hotbox_dict[name] = {}
                self.writeHotbox(hotbox_dict)
                self.palette_wig.populateList()

        def sortAndWriteHotbox(self):
            hotbox = {}

            for x in range(self.topLevelItemCount()):
                top_item: QTreeWidgetItem = self.topLevelItem(x)
                if top_item.toolTip(0):
                    hotbox[top_item.text(0)] = top_item.toolTip(0)
                else:
                    hotbox[top_item.text(0)] = {}
                    for x in range(top_item.childCount()):
                        child = top_item.child(x)
                        if child.toolTip(0):
                            hotbox[top_item.text(0)][child.text(0)] = child.toolTip(0)
                        else:
                            mc.warning(f'Skipped saving "{child.text(0)}": Hotbox does not yet support nested menus.')

            self.writeHotbox(hotbox)

        def populateHotbox(self):
            self.clear()
            for key, value in cmn.readJson(cmn.MP + '/userdata/hotbox.json', {}).items():
                item = QTreeWidgetItem()
                item.setText(0, key)
                if type(value) == dict:
                    for k, v in value.items():
                        child = QTreeWidgetItem()
                        child.setText(0, k)
                        child.setToolTip(0, v)
                        item.addChild(child)
                else:
                    item.setFlags(item.flags() ^ Qt.ItemIsDropEnabled)
                    item.setToolTip(0, value)
                self.addTopLevelItem(item)

        @staticmethod
        def writeHotbox(hotbox_dict):
            cmn.writeJson(cmn.MP + '/userdata/hotbox.json', hotbox_dict)
            WIN_MNGR.hotbox.built = False

        @staticmethod
        def readHotbox():
            return cmn.readJson(cmn.MP + '/userdata/hotbox.json', {})

    class SearchLine(QLineEdit):
        list:QListWidget

        def keyPressEvent(self, event):
            if event.key() in (Qt.Key_Down, Qt.Key_Up):
                self.list.keyPressEvent(event)
            elif event.key() in (Qt.Key_Return, Qt.Key_Enter):
                if self.list.count():
                    sel = self.list.selectedItems()
                    if not sel:
                        sel = [self.list.item(0)]
                    self.list.itemClicked.emit(sel[0])

            else:
                super(self.__class__, self).keyPressEvent(event)

    def __init__(self, *args, **kwargs):
        super(self.__class__, self).__init__(*args, **kwargs)
        self.setWindowTitle('Palette')
        self.setObjectName('PaletteWindow')
        self.setLayout(QVBoxLayout(spacing=3))
        self.layout().setContentsMargins(2,2,2,2)

        self.search_line = self.SearchLine(fixedHeight=22)
        self.search_line.setClearButtonEnabled(True)
        self.search_line.textChanged.connect(self.searchNodes)
        self.layout().addWidget(self.search_line)
        self.search_list = self.NodeList(styleSheet=LIST_STYLE)
        self.search_list.palette_wig = self
        self.search_list.is_search = True
        self.search_list.setSpacing(1)
        self.search_list.itemClicked.connect(self.addClickedItem)
        self.search_list.hide()
        self.search_line.list = self.search_list
        self.layout().addWidget(self.search_list)

        self.splitter = QSplitter(self)
        self.splitter.setOrientation(Qt.Horizontal)
        self.splitter.setHandleWidth(3)
        self.layout().addWidget(self.splitter)

        self.lib_tree = self.LibTree(styleSheet=LIST_STYLE, indentation=8)
        self.lib_tree.palette_wig = self
        self.lib_tree.setHeaderHidden(True)
        self.lib_tree.setTextElideMode(Qt.ElideNone)
        self.lib_tree.setSelectionMode(QTreeWidget.ExtendedSelection)
        self.lib_tree.itemSelectionChanged.connect(self.populateList)
        self.splitter.addWidget(self.lib_tree)

        self.hotbox_tree = self.HotboxTree(styleSheet=LIST_STYLE, indentation=8, headerHidden=True)
        self.hotbox_tree.palette_wig = self

        self.comp_list = self.NodeList(styleSheet=LIST_STYLE)
        self.comp_list.palette_wig = self
        self.comp_list.itemClicked.connect(self.addClickedItem)
        self.comp_list.setSpacing(1)

        self.stack = QStackedWidget()
        self.stack.setContentsMargins(0,0,0,0)
        self.stack.addWidget(self.comp_list)
        self.stack.addWidget(self.hotbox_tree)
        self.splitter.addWidget(self.stack)

        self.populateTree()

        self.search_hk = QShortcut(cmn.BIF_WINDOW)
        self.search_hk.setAutoRepeat(False)
        self.search_hk.setContext(Qt.WidgetWithChildrenShortcut)
        self.search_hk.activated.connect(self.initSearch)
        self.setSearchKey()

    def populateTree(self):
        self.lib_tree.clear()

        item = QTreeWidgetItem()
        item.setText(0, 'Hotbox ')
        item.setToolTip(0, 'Hotbox ')
        item.setSizeHint(0, QSize(200, 24))
        self.lib_tree.addTopLevelItem(item)

        item = QTreeWidgetItem()
        item.setText(0, 'Macros ')
        item.setToolTip(0, 'Macros ')
        item.setSizeHint(0, QSize(200, 24))
        self.lib_tree.addTopLevelItem(item)
        self.listMacros()

        def searchChild(chld):
            for x in range(parent.childCount()):
                if parent.child(x).text(0) == chld:
                    return parent.child(x)

        root = QTreeWidgetItem()
        root.setText(0, 'Bifrost')
        root.setToolTip(0, '::')

        root.setSizeHint(0, QSize(200, 24))
        root.ns = None
        self.lib_comps = {'::': ['input', 'output']}
        for lib in mc.vnn(lib='BifrostGraph')[1:]:
            parent = root
            for substr in lib.split('::'):
                child = searchChild(substr)
                if not child:
                    child = QTreeWidgetItem()
                    child.setToolTip(0, lib)
                    child.setText(0, substr)
                    child.setSizeHint(0, QSize(200, 16))
                    parent.addChild(child)
                    self.lib_comps[lib] = mc.vnn(nd=['BifrostGraph', lib])
                parent = child

        self.lib_tree.addTopLevelItem(root)
        root.setExpanded(True)

    def populateList(self):
        self.stack.setCurrentIndex(0)
        self.comp_list.clear()
        sel = self.lib_tree.selectedItems()
        if not sel:
            return
        for item in sel:
            lib = item.toolTip(0)
            if lib == 'Hotbox ':
                if len(sel) > 1:
                    self.lib_tree.clearSelection()
                    self.lib_tree.setItemSelected(item, True)
                else:
                    self.stack.setCurrentIndex(1)
                    self.hotbox_tree.populateHotbox()
                return

            if lib == 'Macros ':
                self.listMacros()
            for comp in self.lib_comps[lib]:
                list_item = QListWidgetItem(comp)
                list_item.setToolTip(lib)
                self.comp_list.addItem(list_item)

    def listMacros(self):
        files = os.listdir(cmn.verifyDir(self.macro_dir))
        macros = []
        for file in files:
            split = os.path.splitext(file)
            if split[1] == '.xml':
                macros.append(split[0])
        self.lib_comps['Macros '] = macros

    def createMacro(self):
        cmn.safeCopy('')
        mc.vnnCompoundEditor(name='bifrostGraphEditorControl', sk=[67, 67108864])
        xml = cmn.safePaste()
        if not xml:
            mc.warning('Create Macro Failed: Selection empty')
            return

        dia = QInputDialog()
        dia.setWindowTitle('New Macro')
        dia.setLabelText('Macro Name:')
        pos = QCursor.pos() + QPoint(-30, -74)
        dia.move(pos)
        dia.exec_()
        if dia.result():
            with open(self.macro_dir + dia.textValue() + '.xml', 'w') as f:
                f.write(xml)
        self.populateList()

    def deleteMacro(self, macro):
        path = self.macro_dir + macro + '.xml'
        if os.path.isfile(path):
            os.remove(path)
        self.populateList()

    @staticmethod
    def addClickedItem(item):
        if item.toolTip() == 'Macros ':
            importMacro(item.text())
        else:
            node_type = f'BifrostGraph,{item.toolTip()},{item.text()}'
            addNode(node_type)

    def initSearch(self):
        self.parent().show()
        self.parent().raise_()
        self.search_line.setFocus()
        self.search_line.selectAll()

    def searchNodes(self, text: str):
        self.search_list.clear()
        if not text:
            self.search_list.hide()
            self.splitter.show()
            return
        self.splitter.hide()
        self.search_list.show()
        text = text.replace(' ', '_').lower()
        terms = text.split(',')
        for term in terms:
            if term:
                for lib, comps in self.lib_comps.items():
                    for comp in comps:
                        comp_lower = comp.lower()
                        if term in comp_lower or term in ''.join([x[0] for x in comp_lower.split('_')]):
                            list_item = QListWidgetItem(comp)
                            list_item.setToolTip(lib)
                            self.search_list.addItem(list_item)

    def setSearchKey(self):
        if cmn.PREFS['replace_search']:
            self.search_line.setPlaceholderText('Tab to search')
            self.search_hk.setKey(QKeySequence(Qt.Key_Tab))
        else:
            self.search_line.setPlaceholderText('Shift + Tab to search')
            self.search_hk.setKey(QKeySequence(Qt.SHIFT | Qt.Key_Tab))

    def toggleSearchKey(self):
        cmn.togglePref('replace_search')
        self.setSearchKey()


class HotboxWindow(QDialog):
    built = False
    btns = []
    btn_style = '''
    QPushButton {background: rgba(23,30,40,180); border-radius: 4; padding: 8; text-align:center;}
    QPushButton:hover {background: rgba(20,120,180,180); border-radius: 4;} 
    QPushButton:checked {background: rgba(20,120,180,180); border-radius: 4;}
    QPushButton:pressed {background: rgba(50,120,220,180); border-radius: 4;}
    '''
    center: QPoint

    class MenuButton(QPushButton):
        def __init__(self, *args, **kwargs):
            super(self.__class__, self).__init__(*args, **kwargs)
            self.setIconSize(QSize(14,14))
            self.setIcon(QIcon(cmn.MP + '/resources/menu.png'))
            self.btn_wig = QWidget(self.parent())
            self.btn_wig.setStyleSheet('background: rgba(20,80,150,180); border-radius: 4; padding: 8;')
            self.btn_wig.setLayout(QVBoxLayout(spacing=2))
            self.btn_wig.layout().setContentsMargins(2,2,2,2)
            self.btn_wig.leaveEvent = lambda x: self.btn_wig.hide()

        def mousePressEvent(self, event):
            self.btn_wig.show()
            self.btn_wig.raise_()

        def mouseReleaseEvent(self, event):
            if self.btn_wig.isVisible():
                btn = self.btn_wig.childAt(self.btn_wig.mapFromGlobal(QCursor.pos()))
                if btn:
                    btn.click()
            self.btn_wig.hide()

        def mouseMoveEvent(self, event):
            btn = self.btn_wig.childAt(self.btn_wig.mapFromGlobal(QCursor.pos()))
            if btn:
                btn.setChecked(True)
            else:
                for btn in self.btn_wig.children()[1:]:
                    btn.setChecked(False)

        def reposition(self):
            pos = self.pos() + QPoint(self.width(), self.height()) / 2
            pos -= QPoint(self.btn_wig.width(), self.btn_wig.height()) / 2
            self.btn_wig.move(pos)
            self.btn_wig.hide()

    def __init__(self, *args, **kwargs):
        super(self.__class__, self).__init__(*args, **kwargs)
        self.setWindowFlags(Qt.Window | Qt.FramelessWindowHint)
        self.setAttribute(Qt.WA_TranslucentBackground)
        self.setFixedSize(800, 600)
        self.setModal(True)

        hk = QShortcut(QKeySequence(Qt.Key_Space), cmn.BIF_WINDOW)
        hk.setAutoRepeat(False)
        hk.setContext(Qt.WidgetWithChildrenShortcut)
        hk.activated.connect(self.popup)

    def keyReleaseEvent(self, event):
        super(self.__class__, self).keyReleaseEvent(event)
        if event.isAutoRepeat(): return
        self.hide()

    def focusInEvent(self, event):
        super(self.__class__, self).focusInEvent(event)

    def popup(self):
        self.center = newNodePosition()
        self.move(QCursor.pos() - QPoint(self.width()/2, self.width()/2))
        if not self.built:
            self.createFloatingButtons()
            self.built = True
        self.show()

    def createFloatingButtons(self):
        for btn in self.btns:
            btn.deleteLater()
        self.btns = []
        center = QPoint(self.width() / 2, self.width() / 2)

        # Create buttons
        def createBtn(name, source):
            button = QPushButton(name, styleSheet=self.btn_style)
            if source == 'Macros ':
                button.clicked.connect(lambda y=None, x=name: importMacro(x, self.center))
                button.setIconSize(QSize(14, 14))
                button.setIcon(QIcon(cmn.MP + '/resources/macro.png'))
            else:
                button.clicked.connect(lambda y=None, x=f'BifrostGraph,{source},{name}': addNode(x, self.center))
            return button
        for key, value in PaletteWindow.HotboxTree.readHotbox().items():
            if type(value) == dict:
                menu_btn = self.MenuButton(self)
                menu_btn.setStyleSheet(self.btn_style)
                menu_btn.setText(key)
                self.btns.append(menu_btn)
                for k, v in value.items():
                    btn = createBtn(k, v)
                    btn.setCheckable(True)
                    menu_btn.btn_wig.layout().addWidget(btn)
                    pass
            else:
                btn = createBtn(key, value)
                btn.setParent(self)
                self.btns.append(btn)

        # Arrange buttons
        self.show()
        cmn.processQt()
        count = float(len(self.btns))
        if count % 2:
            count += 1
        for i, btn in enumerate(self.btns):
            btn.move(self.calcPosOnCircle(i, count, btn.width(), btn.height()) + center)
            if type(btn) == self.MenuButton:
                btn.reposition()
        self.hide()

    @staticmethod
    def calcPosOnCircle(i, count, w, h):
        count_y = int(count/2)
        x = math.cos(i/count * 2 * math.pi - 0.5 * math.pi) * (20 + count * 6)
        y = (abs(1 - i/count_y) - 0.5) * -32 * count_y - 0.5*h

        if abs(x) < 0.001:
            return QPoint(x - 0.5*w, y)
        if x > 0:
            return QPoint(x, y)
        else:
            return QPoint(x-w, y)


class WindowManager(QObject):
    hotbox: HotboxWindow
    swb: ScriptBtnWindow
    gsw: GetSetWindow
    bmw: BookmarkWindow
    plw: PaletteWindow
    param_extras: ParamPanelExtras
    current_node = ''
    current_node_path = ''
    selectionChanged = Signal(str, str)
    tabInited = Signal(bool)
    editorClosed = Signal()
    settings = QSettings(cmn.MP + '/userdata/layout.ini', QSettings.IniFormat)
    bep_menu: QMenu
    palette_key_action = QAction()
    external_menus = []

    def saveWindowState(self):
        self.settings.setValue('geo', cmn.BIF_WINDOW.saveGeometry())
        self.settings.setValue('state', cmn.BIF_WINDOW.saveState())
        self.settings.sync()

    def loadWindowState(self):
        cmn.BIF_WINDOW.restoreGeometry(self.settings.value('geo'))
        cmn.BIF_WINDOW.restoreState(self.settings.value('state'))

    def eventFilter(self, obj, event):
        # print(event.type(), event)
        if event.type() == QEvent.Show:
            self.current_node = obj.text()
            self.current_node_path = cmn.CURRENT_COMPOUND + self.current_node
            comp_type = cmn.compoundType(self.current_node_path)
            if comp_type is None:
                comp_type = "!"
            self.selectionChanged.emit(self.current_node, comp_type)
        return False

    def createMenu(self):
        def toggleResetShortcut(state):
            cmn.setPref('reset_feedback_shortcut', state)
            mc.warning('Reset feedback shortcut toggled, reopen editor for changes to take effect.')

        menu_bar: QMenuBar = cmn.findControl("VNNUI_VERSIONED_NS::CompoundEditor|vnnUI::CEMenuBar", QMenuBar)
        # if menu_bar.actions()[-1].text() == 'BEP':
        #     return

        self.hotbox = HotboxWindow(cmn.BIF_WINDOW)
        self.swb = self.wig_to_dock(ScriptBtnWindow)
        self.gsw = self.wig_to_dock(GetSetWindow)
        self.bmw = self.wig_to_dock(BookmarkWindow)
        self.plw = self.wig_to_dock(PaletteWindow)

        self.bep_menu: QMenu = menu_bar.addMenu('BEP')
        self.bep_menu.addAction('Bookmarks', self.bmw.show)
        self.bep_menu.addAction('Get/Set Window', self.gsw.show)
        self.bep_menu.addAction('Script Button Editor', self.swb.show)
        self.bep_menu.addAction('Palette', self.plw.show)

        for menu in self.external_menus:
            self.bep_menu.addMenu(menu())

        self.bep_menu.addSeparator()

        self.bep_menu.addAction('Save Layout', self.saveWindowState)
        if cmn.PREFS['reset_feedback_shortcut']:
            shortcut = QShortcut(
                QKeySequence(Qt.Key_R),
                cmn.BIF_WINDOW.bep_tab_ref,
                autoRepeat=False,
                context=Qt.WidgetWithChildrenShortcut
            )
            shortcut.activated.connect(cmn.resetFeedback)
            self.bep_menu.addAction('Disable Reset Shortcut', lambda: toggleResetShortcut(0))
        else:
            self.bep_menu.addAction('Enable Reset Shortcut', lambda: toggleResetShortcut(1))
        self.palette_key_action = self.bep_menu.addAction('Assign Tab to Palette', self.plw.widget().toggleSearchKey)
        self.palette_key_action.setCheckable(True)
        self.palette_key_action.setChecked(cmn.PREFS['replace_search'])
        self.bep_menu.addAction('Reload Palette', self.plw.widget().populateTree)
        self.bep_menu.addSeparator()
        self.bep_menu.addAction('Reload Bifrost Plugin', cmn.reloadBifrostPlugin)
        self.loadWindowState()

    @staticmethod
    def wig_to_dock(wig_type):
        dock = QDockWidget(cmn.BIF_WINDOW)
        dock.setWidget(wig_type(dock))
        dock.setObjectName(dock.widget().objectName() + '_dock')
        dock.setWindowTitle(dock.widget().windowTitle())
        cmn.BIF_WINDOW.addDockWidget(Qt.RightDockWidgetArea, dock)
        dock.hide()
        return dock

    def initParamPanel(self):
        try:
            if not cmn.BIF_WINDOW.isVisible():
                return
            child = cmn.BIF_WINDOW.bep_param_ref.children()[-1].children()[0].children()[2].children()[0].children()[1]
        except (AttributeError, IndexError, RuntimeError):
            return

        if len(child.children()) == 8:  # extra buttons are not present
            self.param_extras = ParamPanelExtras()
            child.layout().insertWidget(3, self.param_extras, alignment=Qt.AlignTop)
            label = cmn.findControl("VNNUI_VERSIONED_NS::CompoundEditor||||||nodeNameLabel", QLabel)
            label.installEventFilter(self)
            self.param_extras.destroyed.connect(lambda: mc.evalDeferred(self.initParamPanel))
            self.eventFilter(label, QShowEvent())


def initTab():  # Called when tab changed or editor opened
    first_init = cmn.BIF_WINDOW is None
    if first_init:
        mc.workspaceControl("bifrostGraphEditorControl", e=True, uiScript="BifrostEditorPlus.UI.uiScript()")
        cmn.BIF_WINDOW = cmn.findControl("VNNUI_VERSIONED_NS::CompoundEditor", QMainWindow)
        cmn.BIF_WINDOW.destroyed.connect(bifrostEditorClose)
        cmn.BIF_WINDOW.bep_tab_ref = cmn.findControl("VNNUI_VERSIONED_NS::CompoundEditor|VNN_Node_Tab_Widget", QTabWidget)
        cmn.BIF_WINDOW.bep_param_ref = cmn.findControl("VNNUI_VERSIONED_NS::CompoundEditor|Parameters", QDockWidget)
        WIN_MNGR.createMenu()

    if cmn.BIF_WINDOW.bep_tab_ref != TABS_EF.owner:
        TABS_EF.owner = cmn.BIF_WINDOW.bep_tab_ref
        cmn.BIF_WINDOW.bep_tab_ref.installEventFilter(TABS_EF)

    last_con, last_cc = cmn.BIF_CONTAINER, cmn.CURRENT_COMPOUND
    cmn.updateBifContainer()
    WIN_MNGR.initParamPanel()
    if first_init or last_con != cmn.BIF_CONTAINER or last_cc != cmn.CURRENT_COMPOUND:  # exec inside to prevent init spam
        WIN_MNGR.tabInited.emit(first_init)


def bifrostEditorClose():
    cmn.BIF_WINDOW = None
    WIN_MNGR.editorClosed.emit()


def addNode(node_type, pos=None):
    with cmn.undoChunk("BEP_addNode: " + node_type) as uc:
        if node_type == 'BifrostGraph,::,input':
            node = mc.vnnCompound(cmn.BIF_CONTAINER, cmn.CURRENT_COMPOUND, addIONode=True)[0]
        elif node_type == 'BifrostGraph,::,output':
            node = mc.vnnCompound(cmn.BIF_CONTAINER, cmn.CURRENT_COMPOUND, addIONode=False)[0]
        else:
            node = mc.vnnCompound(cmn.BIF_CONTAINER, cmn.CURRENT_COMPOUND, addNode=node_type)[0]

        if not pos:
            pos = newNodePosition()

        path = cmn.CURRENT_COMPOUND + node
        mc.vnnNode(cmn.BIF_CONTAINER, path, smd=['LayoutPos', str(pos.x()) + ' ' + str(pos.y())])

    return path


def importMacro(macro, pos=None):
    if not pos:
        pos = newNodePosition()

    xmlpath = PaletteWindow.macro_dir + macro + '.xml'
    if not os.path.isfile(xmlpath):
        mc.warning('Macro file does not exist: ' + xmlpath)
        return
    with open(xmlpath) as f:
        xml = ET.fromstring(f.read())

    ref = None
    for nodes in xml.findall('nodes'):
        for node in nodes:
            mds = node[0].find('metadatas')
            nmds = node[0].find('node_metadatas')
            if nmds:
                mds = list(mds) + list(nmds.find('metadatas'))
            for md in mds:
                if md.get('name') == 'LayoutPos':
                    split = md.find('value').text.split(' ')
                    new_pos = QPoint(float(split[0]), float(split[1]))
                    if not ref:
                        ref = pos
                    dst = new_pos - ref + pos
                    md.find('value').text = str(dst.x()) + ' ' + str(dst.y())

    cmn.safeCopy(ET.tostring(xml, encoding="unicode"))
    cmn.bifPaste()


def newNodePosition():
    globalpos = QCursor.pos()
    wig = QApplication.widgetAt(globalpos)

    if wig:
        parent = wig.parent()
        if type(parent) == QGraphicsView:
            return wig.parent().mapToScene(wig.mapFromGlobal(globalpos)).toPoint() - QPoint(100, 100)

    rect = mc.vnnCompound(cmn.BIF_CONTAINER, cmn.CURRENT_COMPOUND, qmd="ViewportRect")[0].split(' ')
    rect = [float(x) for x in rect]
    x = rect[0] + rect[2] / 2 - 100
    y = rect[1] + rect[3] / 2 - 100

    return QPoint(x, y)


def uiScript():
    mc.vnnCompoundEditor(name="bifrostGraphEditorControl", title="Bifrost Graph Editor", reload=True)
    mc.evalDeferred(lambda: mc.evalDeferred(initTab))


BTN_STYLE = '''
QPushButton {background: rgba(100,130,150,120); border-radius: %radpx;} 
QPushButton:hover {background: rgba(50,160,220,150); border-radius: %radpx;} 
QPushButton:pressed {background: rgba(50,100,150,200); border-radius: %radpx;}
QPushButton:checked {background: rgba(70,160,220,150); border-radius: %radpx;}
'''
LIST_STYLE = '''
QListView {outline: 0;}
QListView::item:selected {background-color: rgba(15,70,120,250);}
QTreeView {selection-background-color: rgba(15,70,120,250);}
'''
TABS_EF = TabsEventFilter()
PARAM_PANEL_EXTRAS: ParamPanelExtras
WIN_MNGR = WindowManager()
