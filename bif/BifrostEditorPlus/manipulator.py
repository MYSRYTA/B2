import json
from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *
import maya.api.OpenMayaUI as omui
import maya.api.OpenMaya as om
from maya import mel, cmds as mc
from . import common as cmn, UI

MANIP_MODE = 0
DEG_MULT = 57.295779
UNIT_IS_DEG = True
FIRST_MANIP_CALLED = False


class ManipMenu(QMenu):
    def __init__(self, *args, **kwargs):
        super(self.__class__, self).__init__(*args, **kwargs)
        self.setTitle("Manipulators")

        action: QAction = self.addAction("Select on Create", lambda: cmn.togglePref('manip_allow_select'))
        action.setStatusTip("Auto select the manipulator when it's created.")
        action.setCheckable(True)
        action.setChecked(cmn.PREFS['manip_allow_select'])

        action: QAction = self.addAction("Hidden In Outliner", lambda: cmn.togglePref('manip_hidden_outliner'))
        action.setStatusTip("Hide manipulators in the outliner")
        action.setCheckable(True)
        action.setChecked(cmn.PREFS['manip_hidden_outliner'])

        action: QAction = self.addAction("Native Node Manipulators", lambda: nativeNodeManips(cmn.togglePref('manip_native_nodes')))
        action.setStatusTip("Add manipulators to certain native Bifrost nodes")
        action.setCheckable(True)
        action.setChecked(cmn.PREFS['manip_native_nodes'])

        # Graph selectable group
        self.addSection("Graph Shape Selectable")

        action: QAction = self.addAction("Auto Disable", lambda: cmn.togglePref('manip_auto_graph_selectable'))
        action.setStatusTip("Auto disables the Bifrost graph from being selectable from the viewport when manipulators are present")
        action.setCheckable(True)
        action.setChecked(cmn.PREFS['manip_auto_graph_selectable'])

        action = self.addAction("Disable For Current", lambda: setGraphSelectable(False, force=True))
        action.setStatusTip("Disable the current Bifrost graph from be selectable from the viewport (By setting display mode to 'Referenced')")

        # Mode Group
        self.addSection("Manip Creation Mode")

        grp = QActionGroup(self)
        action: QAction = self.addAction("Selected Node", lambda: setManipMode(1))
        action.setStatusTip("Created manipulators for the selected compound")
        action.setCheckable(True)
        action.setChecked(MANIP_MODE == 1)
        grp.addAction(action)

        action: QAction = self.addAction("Compound", lambda: setManipMode(2))
        action.setStatusTip("Create manipulators for all nodes at the current level (This option may be slow for large graphs)")
        action.setCheckable(True)
        action.setChecked(MANIP_MODE == 2)
        grp.addAction(action)

        action: QAction = self.addAction("Disabled", lambda: setManipMode(0))
        action.setStatusTip("Disable entire manipulator system.")
        action.setCheckable(True)
        action.setChecked(MANIP_MODE == 0)
        grp.addAction(action)
        self.addSeparator()

        action = self.addAction("Print Example", printExample)
        action.setStatusTip("Prints to script editor an example of how to add a manipulator to a Bifrost compound.")


def updateUnit():
    global UNIT_IS_DEG
    UNIT_IS_DEG = mc.currentUnit(a=True, q=True) == 'deg'


def setGraphSelectable(state, force=False):
    if cmn.CONTAINER_IS_BOARD:
        return

    if force or cmn.PREFS['manip_auto_graph_selectable']:
        if state:
            attr = cmn.BIF_CONTAINER + '.overrideEnabled'
            if mc.getAttr(attr):
                mc.setAttr(attr, 0)
        else:
            mc.setAttr(cmn.BIF_CONTAINER + '.overrideEnabled', 1)
            mc.setAttr(cmn.BIF_CONTAINER + '.overrideDisplayType', 2)


def setManipMode(mode):  # Change between showing manipulators for all nodes or only selected node
    global MANIP_MODE
    if MANIP_MODE != 0:
        if MANIP_MODE:
            UI.WIN_MNGR.selectionChanged.disconnect(activateManipSel)
        else:
            UI.WIN_MNGR.tabInited.disconnect(activateManips)

    cmn.setPref('manip_mode', mode)
    MANIP_MODE = mode
    if MANIP_MODE == 0:  # if None than manip creation is disabled
        deleteManips()

    else:
        if MANIP_MODE == 1:
            UI.WIN_MNGR.selectionChanged.connect(activateManipSel)
        elif MANIP_MODE == 2:
            UI.WIN_MNGR.tabInited.connect(activateManips)


def convertAndRotate(value, rotate_type, node):
    if rotate_type == 1:  # from degrees
        value = [item / DEG_MULT for item in value]
    elif rotate_type == 2:  # from quaternion
        value = om.MQuaternion(value).asEulerRotation()
    elif rotate_type == 3:  # from X-Vector
        vector = om.MVector(*value)
        value = om.MVector(1, 0, 0).rotateTo(vector).asEulerRotation()
    elif rotate_type == 4:  # from Y-Vector
        vector = om.MVector(*value)
        value = om.MVector(0, 1, 0).rotateTo(vector).asEulerRotation()
    elif rotate_type == 5:  # from Z-Vector
        vector = om.MVector(*value)
        value = om.MVector(0, 0, 1).rotateTo(vector).asEulerRotation()

    if mc.currentUnit(a=True,q=True) == 'deg':
        mc.rotate(value[0] * DEG_MULT, value[1] * DEG_MULT, value[2] * DEG_MULT, node, os=True)
    else:
        mc.rotate(*value, node, os=True)


def createBepLocator(container, node_path, t_port='', r_port='', s_port='', m_port='', rotate_type=0, triad=False, style_attrs: dict = None, style_attr_ports: dict = None, manip_parent=None, name=None, **kwargs):
    def convertPortValue(port_arg, _attr, default=0):  # get value from port or list of ports and return a single list of values
        arg_type = type(port_arg)
        if arg_type is str:
            return [float(x) for x in mc.vnnNode(container, node_path, qpv=port_arg)]
        elif arg_type is list or arg_type is tuple:
            values = []
            if port_arg[0]:
                values.append(float(mc.vnnNode(container, node_path, qpv=port_arg[0])))
            else:
                values.append(default)
                mc.setAttr(f"{transform}.{_attr}x", lock=True)

            if port_arg[1]:
                values.append(float(mc.vnnNode(container, node_path, qpv=port_arg[1])))
            else:
                values.append(default)
                mc.setAttr(f"{transform}.{_attr}y", lock=True)

            if port_arg[2]:
                values.append(float(mc.vnnNode(container, node_path, qpv=port_arg[2])))
            else:
                values.append(default)
                mc.setAttr(f"{transform}.{_attr}z", lock=True)

            return values

    def connectionCheck(port, type_check=True):
        if not port:
            return ""
        elif type_check and type(port) is list:
            return [connectionCheck(subport, type_check=False) for subport in port]
        elif port in connected_ports:
            return ""
        else:
            nonlocal has_unconnected
            has_unconnected = True
            return port

    # Check for ports that are connected and remove them
    connected_ports = mc.vnnNode(container, node_path, lp=True, c=True)
    if connected_ports:
        has_unconnected = False
        connected_ports = [port.split(".", 1)[1] for port in connected_ports]
        t_port = connectionCheck(t_port)
        r_port = connectionCheck(r_port)
        s_port = connectionCheck(s_port)
        m_port = connectionCheck(m_port)
        if not has_unconnected:
            return None

    # Create locator
    transform = mc.createNode('transform', ss=True, parent=manip_parent, name=name)
    locator = mc.createNode("BepLocator", parent=transform, ss=True, name=name + "Shape")
    if cmn.PREFS['manip_hidden_outliner']:
        mc.setAttr(transform + ".hiddenInOutliner", True)
    mc.setAttr(
        locator + ".args",
        json.dumps(dict(
            container=container,
            node_path=node_path,
            t_port=t_port,
            r_port=r_port,
            s_port=s_port,
            m_port=m_port,
            rotate_type=rotate_type
        )),
        type="string"
    )

    # Transform
    if m_port:
        matrix_list = convertPortValue(m_port, "")
        mc.xform(transform, matrix=matrix_list)
        mc.connectAttr(transform + ".worldMatrix", locator + ".srcm")

    else:
        if t_port:
            mc.move(*convertPortValue(t_port, "t"), transform, os=True)
            mc.connectAttr(transform + ".t", locator + ".srct")
        else:
            mc.setAttr(transform + ".t", lock=True)

        if r_port:
            convertAndRotate(convertPortValue(r_port, "t"), rotate_type, transform)
            mc.connectAttr(transform + ".r", locator + ".srcr")
        else:
            mc.setAttr(transform + ".r", lock=True)

        if s_port:
            mc.scale(*convertPortValue(s_port, "s", default=1), transform)
            mc.connectAttr(transform + ".s", locator + ".srcs")
        else:
            mc.setAttr(transform + ".s", lock=True)

    if triad:
        fn = omui.MFnFreePointTriadManip()
        mobj = om.MSelectionList().add(transform).getDependNode(0)
        plug = om.MFnDependencyNode(mobj).findPlug("translate", True)
        manip = fn.create("freepointmanip", "freepoint")
        fn.connectToPointPlug(plug)
        # parenting makes it so the triad deletes with object but also makes it behave weird when rotated
        om.MFnDagNode(mobj).addChild(manip)

    if style_attrs is not None:
        for attr, value in style_attrs.items():
            value_type = type(value)
            if value_type == str:
                mc.setAttr(locator + "." + attr, value, type="string")
            elif value_type == list:
                mc.setAttr(locator + "." + attr, value[0], value[1], value[2], type="double3")
            else:
                mc.setAttr(locator + "." + attr, value)

    if style_attr_ports is not None:
        for attr, port in style_attr_ports.items():
            value = mc.vnnNode(container, node_path, qpv=port)
            if type(value) == list:
                mc.setAttr(locator + "." + attr, float(value[0]), float(value[1]), float(value[2]), type="double3")
            else:
                if mc.vnnNode(container, node_path, qpt=port) == "string":
                    mc.setAttr(locator + "." + attr, value, type="string")
                else:
                    mc.setAttr(locator + "." + attr, float(value))

    return transform


def createManipsFromMetadata(node_path, counter=0):
    def createManipHierarchy(container, manip, parent=None):  # Recursively created children manips
        nonlocal counter
        counter += 1
        transform = createBepLocator(container, node_path, **manip, manip_parent=parent, name=manip_basename+str(counter))

        if "children" in manip:
            for child_manip in manip["children"]:
                createManipHierarchy(container, child_manip, parent=transform)
        return transform

    try:  # errors if node_path is not a compound
        md = mc.vnnCompound(cmn.BIF_CONTAINER, node_path, qmd='BEP_manipArgs')
    except RuntimeError:
        return None, counter

    manip_basename = node_path.rsplit('/')[1] + "_manip"
    result = None
    if md:
        manip_dicts = json.loads(md[0])
        for manip_dict in manip_dicts:
            result = createManipHierarchy(cmn.BIF_CONTAINER, manip_dict)
    return result, counter


def activateManips():
    if not cmn.BIF_CONTAINER or not cmn.CURRENT_COMPOUND:
        return
    firstManipCallCheck()
    updateUnit()
    transform = None

    with cmn.undoDisabler() as ud:
        try:
            deleteManips()
            nodes = mc.vnnCompound(cmn.BIF_CONTAINER, cmn.CURRENT_COMPOUND, ln=True)
            counter = 0
            for node in nodes:
                node_path = cmn.CURRENT_COMPOUND + node
                result, counter = createManipsFromMetadata(node_path, counter=counter)
                if result:
                    transform = result

            finishActivate(transform)

        except:
            mc.warning(f"Invalid manipulator metadata on: {cmn.CURRENT_COMPOUND}{node}")


def activateManipSel(node, comp_type):
    firstManipCallCheck()
    updateUnit()

    with cmn.undoDisabler() as ud:
        # try:
        deleteManips()
        transform, _ = createManipsFromMetadata(UI.WIN_MNGR.current_node_path)
        finishActivate(transform)

        # except:
        #     mc.warning(f"Invalid manipulator metadata on: {cmn.CURRENT_COMPOUND}{node}")


def finishActivate(transform):
    if transform:
        setGraphSelectable(False)
        if cmn.PREFS['manip_allow_select']:
            mc.select(transform)


def deleteManips(return_number=True):
    shapes = mc.ls(type="BepLocator")
    nodes = mc.listRelatives(shapes, parent=True)
    if nodes:
        setGraphSelectable(True)
        mc.delete(nodes)

        if return_number:
            highest = 0
            for node in nodes:
                number = int(node.rsplit("manip", 1)[1])
                highest = number if number > highest else highest
            return highest

    return 0


def nativeNodeManips(state=True):
    with cmn.undoDisabler() as ud:
        container = mc.createNode("bifrostBoard")
        manips = dict()

        manips["BifrostGraph,Modeling::Primitive,create_mesh_cube"] = [
            dict(
                t_port="position",
                r_port="up_axis",
                s_port=["width", "height", "length"],
                rotate_type=4,
                style_attrs=dict(shape=7, wire=False, xray=1, ove=1, ovrgbf=1, ovrgb=[1.2, 0.1, .1]),
            )
        ]

        manips["BifrostGraph,Modeling::Primitive,create_mesh_sphere"] = [
            dict(
                t_port="position",
                r_port="up_axis",
                s_port=["", "radius", ""],
                rotate_type=4,
                style_attrs=dict(shape=7, wire=False, ove=1, ovrgbf=1, ovrgb=[1.2, 0.1, .1]),
            )
        ]

        manips["BifrostGraph,Modeling::Primitive,create_mesh_plane"] = [
            dict(
                t_port="position",
                r_port="up_axis",
                s_port=["width", "", "length"],
                rotate_type=4,
                style_attrs=dict(shape=7, wire=False, ove=1, ovrgbf=1, ovrgb=[1.2, 0.1, .1]),
            )
        ]

        manips["BifrostGraph,Modeling::Primitive,create_mesh_torus"] = [
            dict(
                t_port="position",
                s_port="scale",
                r_port="up_axis",
                rotate_type=4,
                children=[
                    dict(
                        t_port=["major_radius", "", ""],
                        style_attrs=dict(shape=5, wire=False, ove=1, ovrgbf=1, ovrgb=[0.08, 0.5, 1.2]),
                        children=[
                            dict(
                                t_port=["minor_radius", "", ""],
                                style_attrs=dict(shape=5, wire=False, ove=1, ovrgbf=1, ovrgb=[0.08, 0.5, 1.2]),
                            )
                        ]
                    ),
                ],
                style_attrs=dict(shape=1, size=0.3, wire=False, ove=1, ovrgbf=1, ovrgb=[1.2, 0.1, .1])
            )
        ]

        manips["BifrostGraph,Modeling::Points,transform_points"] = [
            dict(
                m_port='transform',
                style_attrs=dict(shape=4, ove=1, ovrgbf=1, ovrgb=[1.2, 0.1, .1]),
            )
        ]

        for node_type, manip_list in manips.items():
            node_path = "/" + mc.vnnCompound(container, "/", addNode=node_type)[0]
            try:
                if state:
                    mc.vnnCompound(container, node_path, smd=["BEP_manipArgs", json.dumps(manip_list)])
                else:
                    mc.vnnCompound(container, node_path, rmd=["BEP_manipArgs", mc.vnnCompound(container, node_path, qmd="BEP_manipArgs")[0]])
            except:
                pass

        mc.delete(container)


def firstManipCallCheck():
    global FIRST_MANIP_CALLED
    if FIRST_MANIP_CALLED:
        return

    FIRST_MANIP_CALLED = True
    if not mc.pluginInfo("BepLocator", q=True, loaded=True):
        mc.loadPlugin(cmn.MP + "/modules/BepLocator.py")
    if cmn.PREFS['manip_native_nodes']:
        nativeNodeManips()


def printExample():
    print('''
#### Manipulator Example ##############################################################################################

# To use this example:
#       Create a "create_mesh_torus" node, make it editable to allow for adding metadata.
#       With the node selected execute this script to add manipulator metadata.

from maya import cmds
import json

# Manipulators are represented by a dict that is used to configure the manipulator when its created. No arguments are
# required so normally only what's relevant is included in the dict. For example a manipulator with all default
# attributes could be created using just 'dict()'. The example below includes all possible arguments and their usage.

# Manipulator for driving the cube's "position" port
manipulator = dict(
    t_port="position",  # port that receives the manip's translate values
    r_port="up_axis",   # port that receives rotation
    s_port="scale",     # port that receives scale
    m_port="",          # port that receives the manip's transform matrix (type must be 4x4 matrix)
    rotate_type=4,      # what form the manip's rotation is sent to Bifrost. 0: euler rad | 1: euler deg | 2: quaternion | 3, 4, 5: X, Y, or Z direction vector
    triad=False,        # create free point triad with manipulator
    children=[],        # list of children manipulator dicts
    style_attrs=dict(shape=1, size=0.3, wire=False, ove=1, ovrgbf=1, ovrgb=[1.2, 0.1, .1]),  # attr/value pairs that will be converted to a list of setAttr commands executed on creation
    style_attr_ports=None   # attr/port_name pairs. Similar to the previous argument but instead of a constant value, the setAttr will use the value of the named port from the compound.
)

# Manipulator to control the radius
radius_manip1 = dict(
    t_port=["major_radius", "", ""],  # Entering a list of 3 port names indicates the translate is controlling scalar ports, the 3 named ports will recieve the X, Y and Z values respectively.
    style_attrs=dict(shape=5, wire=False, ove=1, ovrgbf=1, ovrgb=[0.08, 0.5, 1.2]),
    children=[],
)

# Manuplator to control the second radius
radius_manip2 = dict(
    t_port=["minor_radius", "", ""],
    style_attrs=dict(shape=5, wire=False, ove=1, ovrgbf=1, ovrgb=[0.08, 0.5, 1.2]),
)


# Building the manipulator hierarchy 
radius_manip1["children"].append(radius_manip2)
manipulator["children"].append(radius_manip1)


# vnnNode command for adding a the manipulator to the selected compound's metadata.
cmds.vnnCompound(
    BifrostEditorPlus.currentGraph(),
    BifrostEditorPlus.currentNode(),
    smd=["BEP_manipArgs", json.dumps([manipulator])]
) 

#######################################################################################################################
''')


UI.WIN_MNGR.editorClosed.connect(deleteManips)
UI.WIN_MNGR.external_menus.append(ManipMenu)
setManipMode(cmn.PREFS["manip_mode"])
