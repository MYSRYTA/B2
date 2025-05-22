from maya import mel, cmds as mc
from . import common as cmn


def createInputPort(name, data_type, input_node, options=''):
    available = False
    x=0
    tryName = name
    while not available:
        try:
            mc.vnnNode(cmn.BIF_CONTAINER, '/', queryPortDataType=tryName)
            x+=1
            tryName = name + str(x)
        except:
            available = True
    mc.vnnCompound(cmn.BIF_CONTAINER, '/', createInputPort=[tryName, data_type], cpo=options)

    # create input node if it doesn't exist and move port to it
    try:
        mc.vnnNode(cmn.BIF_CONTAINER, '/' + input_node, qtn=1)
    except:
        new_input = mc.vnnCompound(cmn.BIF_CONTAINER, '/', addIONode=True)[0]
        mc.vnnCompound(cmn.BIF_CONTAINER, '/', rn=[new_input, input_node])

    mc.vnnCompound(cmn.BIF_CONTAINER, '/', movePortToIONode=[tryName, input_node])

    return tryName


def listInputs(data_type):
    cmn.updateBifContainer()
    ports = mc.vnnCompound(cmn.BIF_CONTAINER, '/', lp=True, ip=True)
    if ports:
        buf = list(ports)
        for port in buf:
            if mc.vnnCompound(cmn.BIF_CONTAINER, '/', qpt=port) != data_type:
                ports.remove(port)
    return ports


def listOutputs(data_type):
    cmn.updateBifContainer()
    ports = mc.vnnCompound(cmn.BIF_CONTAINER, '/', lp=True, op=True)
    if ports:
        buf = list(ports)
        for port in buf:
            if mc.vnnCompound(cmn.BIF_CONTAINER, '/', qpt=port) != data_type:
                ports.remove(port)
    return ports


def createDefaultMesh():
    path = cmn.MP + '/resources/default_mesh.obj'
    nodes = mc.file(path, i=True, type='OBJ', rnn=True, iv=True)
    parent = mc.rename(nodes[0], 'default_mesh')
    mc.select(parent)
    return mc.listRelatives(parent, shapes=True, pa=True)[0]


def newInputObject(src_attr):
    if src_attr in ('worldMesh', 'outMesh'):
        return createDefaultMesh()

    elif src_attr in ('worldSpace', 'local'):
        return mel.eval('BEP_createDefaultCurve()')

    elif src_attr in ('worldMatrix', 'matrix'):
        return mel.eval('BEP_createTransform()')

    # For pos/locator, for ws the shape needs to be returned, for local the transform
    elif src_attr == 'worldPosition':
        return mel.eval('BEP_createPosition()')[1]

    elif src_attr == 'translate':
        return mel.eval('BEP_createPosition()')[0]

    else: return


def getAttrDataType(node_attr):
    maya_dt = mc.getAttr(node_attr, type=True)
    if maya_dt == 'mesh' or maya_dt == 'nurbsCurve':
        return 'Object'
    elif maya_dt == 'matrix':
        return 'Math::float4x4'
    elif maya_dt == 'double3':
        return 'Math::float3'
    elif maya_dt == 'doubleLinear':
        return 'float'
    else:
        return None


def searchSelForAttr(attr, relatives):
    cmn.updateBifContainer()
    sel = mc.ls(sl=True)

    # remove self bifrost graph
    parent = mc.listRelatives(cmn.BIF_CONTAINER, p=True)
    if cmn.BIF_CONTAINER in sel:
        sel.remove(cmn.BIF_CONTAINER)
    if parent in sel:
        sel.remove(parent)

    if not sel:
        new_obj = newInputObject(attr)
        return [new_obj], getAttrDataType(f"{new_obj}.{attr}")

    if relatives:
        rel = mc.listRelatives(sel, s=True, path=True)
        if rel:
            sel += rel

    found, data_type = [], ''
    for item in sel:
        if mel.eval(f'attributeExists "{attr}" "{item}"'):
            found.append(item)

    if found:
        data_type = getAttrDataType(f"{found[0]}.{attr}")
        if len(found) > 1:
            data_type = f'array<{data_type}>'

    return found, data_type


def inputSel(attr, input_node, shape, input_port='', array=False):
    mc.undoInfo(ock=True)
    dag_nodes, data_type = searchSelForAttr(attr, shape)
    if not dag_nodes:
        return

    if len(dag_nodes) == 1 and not array:
        new_name = f"{dag_nodes[0].replace('Shape', '')}_{attr}"
        if not input_port:
            new_name = createInputPort(new_name, data_type, input_node)
            mc.connectAttr(f"{dag_nodes[0]}.{attr}", f"{cmn.BIF_CONTAINER}.{new_name}", f=True)
        else:
            mc.connectAttr(f"{dag_nodes[0]}.{attr}", f"{cmn.BIF_CONTAINER}.{input_port}", f=True)
            mc.vnnCompound(cmn.BIF_CONTAINER, '/', rp=[input_port, new_name])

    else:
        if not input_port:
            input_port = createInputPort(attr + '_array', data_type, input_node)
        starting_index = mc.getAttr(f"{cmn.BIF_CONTAINER}.{input_port}", s=True)
        for x, node in enumerate(dag_nodes):
            index = x + starting_index
            mc.connectAttr(f"{node}.{attr}", f"{cmn.BIF_CONTAINER}.{input_port}[{index}]")


def inputSelCurves(attr, input_node, input_port='', array=False):
    def breakCurveWsConnections(port):
        dst = cmn.BIF_CONTAINER + '.' + port
        srcs = mc.listConnections(dst, plugs=True, shapes=True)
        dst += '[%i]'
        for x, src in enumerate(srcs):
            mc.disconnectAttr(srcs[x], dst % x)

    dag_nodes, data_type = searchSelForAttr(attr, True)

    # filter non curves
    dag_nodes = mc.ls(dag_nodes, type='nurbsCurve')
    if not dag_nodes:
        return

    path = ' '.join(dag_nodes)
    options = "pathinfo={path=%s;setOperation=+;active=true;channels=*;computeTangents=true;computeLengths=false;" \
              "sampleMode=1;multiplier=1;evenlySpaced=false;targetSegLength=1;matchEnds=false}" % path

    if len(dag_nodes) == 1 and not array:
        new_name = f"{dag_nodes[0].replace('Shape', '')}_{attr}"
        if not input_port:
            input_port = createInputPort(new_name, data_type, input_node, options)
            if attr == 'local':
                mc.connectAttr(new_name, f"{cmn.BIF_CONTAINER}.{input_port}", f=True)
        else:
            mc.connectAttr(f"{dag_nodes[0]}.{attr}", f"{cmn.BIF_CONTAINER}.{input_port}", f=True)
            mc.vnnCompound(cmn.BIF_CONTAINER, '/', rp=[input_port, new_name])

    else:
        if not input_port:
            if attr == 'local':
                input_port = createInputPort('localCurve_array', data_type, input_node, options)
                breakCurveWsConnections(input_port)
                for x, node in enumerate(dag_nodes):
                    mc.connectAttr(f"{node}.{attr}", f"{cmn.BIF_CONTAINER}.{input_port}[{x}]")

            else:
                input_port = createInputPort('worldCurve_array', data_type, input_node, options)

        else:
            starting_index = mc.getAttr(f"{cmn.BIF_CONTAINER}.{input_port}", s=True)
            for x, node in enumerate(dag_nodes):
                index = x + starting_index
                mc.connectAttr(f"{node}.{attr}", f"{cmn.BIF_CONTAINER}.{input_port}[{index}]")


def inputCollection():
    sel = []
    comp_type = ''
    masks = [(31,'verts'), (34,'faces'), (32,'edges'), (35,'UVs'), (28,'CVs')]
    # check selection for [verts, faces, edges, UVs, CVs] first to return something wins
    for mask in masks:
        sel = mc.filterExpand(sm=mask[0])
        if sel:
            comp_type = mask[1]
            break
    if not sel:
        mc.warning('No supported components selected')
        return

    # build name and component string
    name = sel[0].split('.')[0] + '_' + comp_type
    if comp_type== 'edges':
        edges = mc.polyInfo(sel, ev=True)
        starts = []
        ends = []
        for edge in edges:
            split = edge.split()
            starts.append(split[2])
            ends.append(split[3])
        start_str = ' '.join(starts)
        endStr = ' '.join(ends)
        compound = open(cmn.MP + '/modules/edge_collection_compound.xml', 'r').read()
        cmn.safeCopy(compound % (name, start_str, endStr))
        cmn.bifPaste()

    else:
        newSel = []
        for c in sel:
            newSel.append(c.split('[', 1)[1])

        comps = ''.join(newSel)

        compound = open(cmn.MP + '/modules/collection_compound.xml', 'r').read()
        cmn.safeCopy(compound % (name, comps[:-1]))
        cmn.bifPaste()


def outputMesh(port):
    mc.undoInfo(ock=True)
    cmn.updateBifContainer()
    bifToMaya = mc.createNode('bifrostGeoToMaya')
    mc.setAttr(bifToMaya + '.properties', cmn.PREFS['out_properties'], type='string')
    mc.setAttr(bifToMaya + '.transferNormals', cmn.PREFS['out_transfer_normals'])
    mc.connectAttr(cmn.BIF_CONTAINER + '.' + port, bifToMaya + '.bifrostGeo')
    ports = mc.getAttr(bifToMaya + '.mayaMesh', mi=True)
    if not ports:
        mc.undoInfo(cck=True)
        mc.warning('Port is empty: ' + port)
        return

    for x in range(len(ports)):
        mesh = mc.createNode('mesh')
        mc.hyperShade(assign='lambert1')
        mc.connectAttr(bifToMaya + '.mayaMesh[%i]'%x, mesh + '.inMesh')
        mc.setAttr(mesh + '.motionVectorColorSet', cmn.PREFS['out_motion_vectors'], type='string')
        if cmn.PREFS['out_diplsay_property']:
            mc.setAttr(mesh + '.currentColorSet', cmn.PREFS['out_diplsay_property'], type='string')
            mc.setAttr(mesh + '.displayColors', True)

        if cmn.PREFS['out_generic_color_display']:
            mc.setAttr(mesh + '.displayColorChannel', 'None', type='string')

    if not cmn.CONTAINER_IS_BOARD:
        mc.setAttr(cmn.BIF_CONTAINER + '.displayOutputsInRenderer', 0)
        mc.setAttr(cmn.BIF_CONTAINER + '.displayOutputsInViewport', 0)

    mc.undoInfo(cck=True)


def updateSelComponents(node):
    cmn.updateBifContainer()
    sel = mc.ls(os=True, fl=True)
    if not sel:
        return None
    sep = ''
    is_edge = False
    for item in sel:
        split = item.split('.')
        if len(split) == 2:
            for test_sep in ['vtx[', 'e[', 'f[', 'map[', 'cv[']:
                if split[1].startswith(test_sep):
                    sep = test_sep
                    if sep == 'e[':
                        is_edge = True
                    break
    if not sep:
        return

    if is_edge:
        edges = mc.polyInfo(sel, ev=True)
        starts = []
        ends = []
        for edge in edges:
            split = edge.split()
            starts.append(split[2])
            ends.append(split[3])
        array_str1 = ']'.join(starts)
        array_str2 = ']'.join(ends)

    else:
        filtered_sel = []
        for item in sel:
            split = item.split(sep, 1)
            if len(split) == 2:
                filtered_sel.append(split[1])

        array_str1 = ''.join(filtered_sel)[:-1]
        array_str2 = ''

    mc.vnnNode(cmn.BIF_CONTAINER, node, setPortDefaultValues=['array_str1', array_str1])
    mc.vnnNode(cmn.BIF_CONTAINER, node, setPortDefaultValues=['array_str2', array_str2])
    mc.vnnNode(cmn.BIF_CONTAINER, node, setPortDefaultValues=['is_edge', int(is_edge)])

    if is_edge:
        mc.vnnNode(cmn.BIF_CONTAINER, node, setPortDataType=['out_type', 'array<Math::long2>'])
    else:
        mc.vnnNode(cmn.BIF_CONTAINER, node, setPortDataType=['out_type', 'array<long>'])


def toggleTrackSelOrder():
    state = mc.selectPref(q=True,  trackSelectionOrder=True) == 0
    mc.selectPref(trackSelectionOrder=state)
    if state:
        cmn.melPrint('Track Selection Order: Enabled')
    else:
        cmn.melPrint('Track Selection Order: Disabled')
