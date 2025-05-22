import random, json
from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *
from maya import cmds as mc, mel
from . import common as cmn

INT_TYPES = ['long', 'ulong', 'int', 'uint', 'short', 'ushort', 'char', 'uchar']
INT_TYPES = INT_TYPES + [f'Math::{x}2' for x in INT_TYPES] + [f'Math::{x}3' for x in INT_TYPES] + [f'Math::{x}4' for x in INT_TYPES]
FLOAT_TYPES = ['float', 'double']
FLOAT_TYPES = FLOAT_TYPES + [f'Math::{x}2' for x in FLOAT_TYPES] + [f'Math::{x}3' for x in FLOAT_TYPES] + [f'Math::{x}4' for x in FLOAT_TYPES]


class ShufflerWidget(QWidget):
    scope = 0
    node = ''
    path = ''
    search_ints = True
    search_floats = True
    search_terms = ['seed']
    exact_match = True
    int_range = (0, 1000000)
    float_range = (0., 1.)
    float_precision = 4
    reset_feedback = False

    def __init__(self, *args, **kwargs):
        super(self.__class__, self).__init__(*args, **kwargs)

        layout = QVBoxLayout()
        layout.setContentsMargins(4,4,4,4)

        self.scroll = QScrollArea()
        self.scroll.setWidgetResizable(True)
        self.scroll.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        self.scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.mappings_scroll = QWidget(styleSheet='background: rgb(42,42,42)')
        self.mappings_scroll.setLayout(QVBoxLayout(alignment=Qt.AlignTop, spacing=4))
        self.scroll.setWidget(self.mappings_scroll)
        self.scroll.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(self.scroll)

        layout2 = QHBoxLayout(spacing=4)
        layout2.setContentsMargins(2, 0, 2, 0)
        self.setLayout(layout)
        layout.addLayout(layout2)

        btn = QPushButton('Shuffle')
        btn.clicked.connect(lambda: self.generateValueMappings(True))
        btn.clicked.connect(self.applyValueMappings)
        btn.clicked.connect(self.updateUI)
        layout2.addWidget(btn)
        btn = QPushButton('Apply')
        btn.clicked.connect(self.applyValueMappings)
        layout2.addWidget(btn)

        layout2.addStretch()

        btn = QPushButton('Preview')
        btn.clicked.connect(self.generateValueMappings)
        btn.clicked.connect(self.updateUI)
        layout2.addWidget(btn)

    def resizeEvent(self, event):
        super(self.__class__, self).resizeEvent(event)
        self.mappings_scroll.setFixedWidth(self.scroll.width()-16)

    def showShuffler(self, parent, node):
        self.path, self.node = cmn.CURRENT_COMPOUND, node
        parent.setCurrentIndex(3)
        parent.show()
        parent.setFixedHeight(180)
        self.updateUI()

    def querySettings(self):
        node_path = self.path + self.node
        self.scope = int(mc.vnnNode(cmn.BIF_CONTAINER, node_path, qpv='scope'))
        terms = mc.vnnNode(cmn.BIF_CONTAINER, node_path, qpv='search_terms').split(' ')
        self.search_terms = [term for term in terms if term]
        self.exact_match = int(mc.vnnNode(cmn.BIF_CONTAINER, node_path, qpv='exact_match'))
        self.search_ints = int(mc.vnnNode(cmn.BIF_CONTAINER, node_path, qpv='int_types'))
        self.search_floats = int(mc.vnnNode(cmn.BIF_CONTAINER, node_path, qpv='float_types'))
        values = mc.vnnNode(cmn.BIF_CONTAINER, node_path, qpv='int_range')
        self.int_range = (int(values[0]), int(values[1]))
        values = mc.vnnNode(cmn.BIF_CONTAINER, node_path, qpv='float_range')
        self.float_range = (float(values[0]), float(values[1]))
        self.float_precision = int(mc.vnnNode(cmn.BIF_CONTAINER, node_path, qpv='float_precision'))
        self.reset_feedback = int(mc.vnnNode(cmn.BIF_CONTAINER, node_path, qpv='reset_feedback'))

    def getPortState(self, node_path_port):
        md = mc.vnnNode(cmn.BIF_CONTAINER, self.path + self.node, qmd=node_path_port)
        if md:
            return False
        else:
            return True

    def setPortState(self, state, node_path_port):
        if state:
            mc.vnnNode(cmn.BIF_CONTAINER, self.path + self.node, cmd=node_path_port)
        else:
            mc.vnnNode(cmn.BIF_CONTAINER, self.path + self.node, smd=[node_path_port, 'disabled'])

    def randValue(self, is_int):
        if is_int:
            return random.randint(*self.int_range)
        else:
            return round(random.uniform(*self.float_range), self.float_precision)

    def isPortInTerms(self, port_name):
        if self.exact_match:
            return port_name in self.search_terms
        else:
            for term in self.search_terms:
                if term in port_name:
                    return True
        return False

    def valueStringFromPortType(self, port_type):
        if port_type in INT_TYPES:
            if not self.search_ints:
                return None
            is_int = True
        elif port_type in FLOAT_TYPES:
            if not self.search_floats:
                return None
            is_int = False
        else:
            return None
        try:
            num_dim = int(port_type[-1])
        except ValueError:
            return str(self.randValue(is_int))

        val_str = str([self.randValue(is_int) for x in range(num_dim)])
        return val_str.replace('[', '{').replace(']', '}')

    def generateValueMappings(self, randomize=False):
        shuffler_node_path = self.path + self.node
        self.querySettings()
        if self.scope == 2:
            root = '/'
        else:
            root = self.path
        node_paths = [root+node for node in mc.vnnCompound(cmn.BIF_CONTAINER, root, ln=True)]
        value_mappings = []

        # Recursively get all nodes
        if self.scope != 0:
            for path in node_paths:
                try:
                    if mc.vnnCompound(cmn.BIF_CONTAINER, path, qir=True):
                        continue
                    children = mc.vnnCompound(cmn.BIF_CONTAINER, path, ln=True)
                    full_paths = [f'{path}/{child}' for child in children]
                    node_paths.extend(full_paths)
                except RuntimeError:
                    continue

        # Search and generate values
        node_paths.remove(shuffler_node_path)
        for path in node_paths:
            ports = mc.vnnNode(cmn.BIF_CONTAINER, path, lp=True, connected=False)
            if ports is None:
                continue
            for port in ports:
                port_name = port.split('.')[1]
                if not self.isPortInTerms(port_name):
                    continue
                port_type = mc.vnnNode(cmn.BIF_CONTAINER, path, qpt=port_name)
                try:
                    if randomize and self.getPortState(f'{path}.{port_name}'):
                        val_str = self.valueStringFromPortType(port_type)
                    else:
                        val_str = str(mc.vnnNode(cmn.BIF_CONTAINER, path, qpv=port_name)).replace('[', '{').replace(']', '}').replace("'", '')
                    if val_str is None:
                        continue
                    value_mappings.append([path, port_name, val_str])
                except RuntimeError:  # Most likely caused by qpv on output, seems there's no other way to distinguish
                    continue

        mc.vnnNode(cmn.BIF_CONTAINER, shuffler_node_path, smd=['shufflerMappings', json.dumps(value_mappings)])
        return value_mappings

    def applyValueMappings(self):
        md = mc.vnnNode(cmn.BIF_CONTAINER, self.path + self.node, qmd='shufflerMappings')
        if not md:
            return
        mappings = json.loads(md[0])
        mc.undoInfo(ock=True)
        for node_path, port, value in mappings:
            node_path_port = f'{node_path}.{port}'
            if self.getPortState(node_path_port):
                try:
                    mc.vnnNode(cmn.BIF_CONTAINER, node_path, spv=[port, value])
                except RuntimeError:
                    mc.warning(f'Failed to apply mapping, port may have changed: {node_path_port}')
        mc.undoInfo(cck=True)

        if self.reset_feedback:
            cmn.resetFeedback()

    def updateUI(self):
        for child in self.mappings_scroll.children()[1:]:
            child.deleteLater()
        md = mc.vnnNode(cmn.BIF_CONTAINER, self.path + self.node, qmd='shufflerMappings')
        if not md:
            return
        mappings = json.loads(md[0])
        for node_path, port, value in mappings:
            node_path_port = f'{node_path}.{port}'
            checkbox = ElidingCheckbox(node_path_port, checked=self.getPortState(node_path_port))
            checkbox.stateChanged.connect(lambda x, y=f'{node_path}.{port}': self.setPortState(x, y))
            checkbox.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Minimum)
            layout = QHBoxLayout(spacing=0)
            layout.addWidget(checkbox)
            layout.addStretch()
            label = QLabel(fixedWidth=70)
            clipped_value = label.fontMetrics().elidedText(value, Qt.ElideRight, 70)
            label.setText(clipped_value)
            layout.addWidget(label)
            self.mappings_scroll.layout().addLayout(layout)


class ElidingCheckbox(QCheckBox):
    def __init__(self, label, checked=True):
        self.full_text = label
        super(self.__class__, self).__init__(label, checked=checked)

    def resizeEvent(self, event):
        super(self.__class__, self).resizeEvent(event)
        clipped_text = self.fontMetrics().elidedText(self.full_text, Qt.ElideLeft, self.parent().width()-115)
        self.setText(clipped_text + ' '*50)
