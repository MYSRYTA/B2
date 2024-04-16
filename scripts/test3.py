import maya.cmds as cmds
import maya.mel as mel
import maya.api.OpenMaya as om

def get_skin_orig_shape(skin_mesh_name):
    skin_cluster = mel.eval('findRelatedSkinCluster ' + skin_mesh_name)
    
    # MFnDependencyNode を使用してノードを取得
    selection_list = om.MSelectionList()
    selection_list.add(skin_cluster)
    mobj = selection_list.getDependNode(0)
    dep_node = om.MFnDependencyNode(mobj)
    
    # originalGeometry[0]に接続されているオリジナルシェイプノードを取得する
    skin_cluster_plug = dep_node.findPlug("originalGeometry", False)
    skin_cluster_plug_element = skin_cluster_plug.elementByLogicalIndex(0)
    source_plug = skin_cluster_plug_element.source()
    return source_plug.node()


orig_shape = get_skin_orig_shape("pCube1")
print(orig_shape) 

