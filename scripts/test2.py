import maya.api.OpenMaya as om
from maya import cmds
import pickle

def select_hierarchy_tip_nodes(root_obj):
    cmds.SelectHierarchy(root_obj)
    selection = om.MGlobal.getActiveSelectionList()
    
    dag_iter=om.MItSelectionList(selection, om.MFn.kDagNode)
    tip_nodes = []
    while not dag_iter.isDone():
        dag_path = dag_iter.getDagPath()
        if dag_path.hasFn(om.MFn.kTransform):
            child_count = dag_path.childCount()
            if child_count == 1:
                tip_nodes.append(dag_path)
        dag_iter.next()

    result_selection_list = om.MSelectionList()
    for node in tip_nodes:
        result_selection_list.add(node)

    om.MGlobal.setActiveSelectionList(result_selection_list, om.MGlobal.kReplaceList)
    
#--------------------------------------------------------------------------------------

def save_custom_data(filename, custom_data):
    # カスタムデータをバイト列に変換
    data_bytes = pickle.dumps(custom_data)

    # ファイルに書き込む
    with open(filename, 'wb') as f:
        f.write(data_bytes)

def load_custom_data(filename):
    # ファイルからバイト列を読み込む
    with open(filename, 'rb') as f:
        data_bytes = f.read()

    # バイト列をカスタムデータに復元
    custom_data = pickle.loads(data_bytes)
    return custom_data

#--------------------------------------------------------------------------------------

def get_translation(node_name, space):
    sel_list = om.MSelectionList()
    sel_list.add(node_name)
    dag_path = sel_list.getDagPath(0)

    fn_transform = om.MFnTransform(dag_path)
    return fn_transform.translation(space)
    
def get_rotation(node_name, space):
    sel_list = om.MSelectionList()
    sel_list.add(node_name)
    dag_path = sel_list.getDagPath(0)

    fn_transform = om.MFnTransform(dag_path)
    return fn_transform.rotation(space, True)

def get_selection_list(fullPathName=True):
    selected_objects = om.MGlobal.getActiveSelectionList()
    iter = om.MItSelectionList(selected_objects)
    node_name = []
    while not iter.isDone():
        dag_path = iter.getDagPath()
        if fullPathName:
            node_name.append(dag_path.fullPathName())
        else:
            node_name.append(dag_path.partialPathName())
        iter.next()
    return node_name
    
def select_nodes(node_names):
    selection_list = om.MSelectionList()

    for node_name in node_names:
        try:
            selection_list.add(node_name)
        except RuntimeError:
            print("Node '{}' not found.".format(node_name))

    om.MGlobal.setActiveSelectionList(selection_list, om.MGlobal.kReplaceList)

def to_list(data):
    result = []
    if type(data) == om.MVector:
        result = [data.x, data.y, data.z]
    if type(data) == om.MQuaternion:
        result = [data.x, data.y, data.z, data.w]
    return result
   
#--------------------------------------------------------------------------------------

class DagPoseData:
    def __init__(self, name):
        self.name = name
        self.count = 0
        self.l_tr = []
        self.l_rot = []
        self.g_tr = []
        self.g_rot = []
    
    def pose_count(self):
        return self.count
        
class DagPoseDataPack:
    def __init__(self, alc_name):
        self.alc_name = alc_name
        self.start_flame = 0
        self.step_flame = 10
        self.data = [] # DagPoseData[]
    
    def dag_count(self):
        return len(self.data)

    def record_pose(self, node_names, step_flame, world_tr=True, world_rot=False, local_tr=False, local_rot=False):
        self.start_flame = cmds.playbackOptions (q=True, min=True)
        self.step_flame = step_flame
        for name in node_names:
            data = DagPoseData(name.split(":")[-1])
            
            current_flame = self.start_flame
            while current_flame < cmds.playbackOptions (q=True, max=True):
                cmds.currentTime (current_flame)
                
                if world_tr:
                    tr_val = get_translation(name, om.MSpace.kWorld)
                    data.g_tr.append( to_list(tr_val) )
                if world_rot:
                    rot_val = get_rotation(name, om.MSpace.kWorld)
                    data.g_rot.append( to_list(rot_val) )
                if local_tr:
                    tr_val = get_translation(name, om.MSpace.kTransform)
                    data.l_tr.append( to_list(tr_val) )
                if local_rot:
                    rot_val = get_rotation(name, om.MSpace.kTransform)
                    data.l_rot.append( to_list(rot_val) )
                
                data.count = data.count + 1
                current_flame = current_flame + self.step_flame
    
            self.data.append(data)
    
    def pose_compare(self, target_name_space):
        for data in self.data:
            name = data.name if target_name_space == "" else target_name_space + ":" + data.name
            if not cmds.objExists(name): return False
            
            for i in range(data.pose_count()):
                current_flame = self.start_flame + self.step_flame * i
                cmds.currentTime (current_flame)
                
                if data.g_tr:
                    tr_val = get_translation(name, om.MSpace.kWorld)
                    if data.g_tr[i] != to_list(tr_val) : return False
                if data.g_rot:
                    rot_val = get_rotation(name, om.MSpace.kWorld)
                    if data.g_rot[i] != to_list(rot_val) : return False
                if data.l_tr:
                    tr_val = get_translation(name, om.MSpace.kTransform)
                    if data.l_tr[i] != to_list(tr_val) : return False
                if data.l_rot:
                    rot_val = get_rotation(name, om.MSpace.kTransform)
                    if data.l_rot[i] != to_list(rot_val) : return False
        return True
        
def aa(source_anim_curve, target_anim_curve):
    # ソースアニメーションカーブのキー情報を取得
    source_keys = cmds.keyframe(source_anim_curve, q=True, vc=True)
    source_times = cmds.keyframe(source_anim_curve, q=True, tc=True)
    source_cycle_pri = cmds.setInfinity (source_anim_curve, pri=True, q=True)[0]
    source_cycle_poi = cmds.setInfinity (source_anim_curve, poi=True, q=True)[0]

    # ターゲットアニメーションカーブをクリア
    cmds.cutKey(target_anim_curve)

    # ソースアニメーションカーブのキー情報をターゲットアニメーションカーブに設定
    for time, value in zip(source_times, source_keys):
        cmds.setKeyframe(target_anim_curve, time=time, value=value)
        cmds.selectKey(target_anim_curve, k = True, t =(time,time))
        cmds.keyframe(absolute=True, valueChange=value)
        
    # ターゲットアニメーションカーブのサイクルタイプを設定
    cmds.setInfinity(target_anim_curve, pri=source_cycle_pri, poi=source_cycle_poi)
    
def overwrite_animation_curve(source_anim_curve, target_anim_curve):
    # アニメーションカーブキーを複製
    cmds.copyKey(source_anim_curve)
    cmds.cutKey(target_anim_curve, clear=True)
    cmds.pasteKey(target_anim_curve)
    
    # ターゲットアニメーションカーブのインフィニティ―タイプを複製
    pri_poi_type = cmds.setInfinity (source_anim_curve, pri=True, poi=True, q=True)
    cmds.setInfinity(target_anim_curve, pri=pri_poi_type[0], poi=pri_poi_type[1])
            
def create_cycle_keyframes(anim_attr, pri_count, post_count):    
    # アニメーションカーブのインフィニティ―情報を取得する
    source_pri_poi = cmds.setInfinity (anim_attr, q=True, pri=True, poi=True)
    if not "cycle" in source_pri_poi[0] and not "cycle" in source_pri_poi[1] : return
    
    # アニメーションカーブの情報を取得する
    source_times = cmds.keyframe(anim_attr, q=True, tc=True)
    source_keys = cmds.keyframe(anim_attr, q=True, vc=True)
    time_range = source_times[-1] - source_times[0]
    value_offset = source_keys[-1] - source_keys[0]

    # スタート・エンドのキーのタンジェントを固定する
    for i in range(2):
        if not "cycle" in source_pri_poi[i] : continue
        
        time = source_times[0] if i == 0 else source_times[-1]
        cmds.selectKey(anim_attr, k=True, t=(time,time))
        td = cmds.keyTangent(anim_attr, t=(time,time), q=True, inAngle=True, inWeight=True, outAngle=True, outWeight=True)
        cmds.keyTangent(anim_attr, e=True, t =(time,time), inAngle=td[0], inWeight=td[2], outAngle=td[1], outWeight=td[3])
    
    # アニメーションカーブをクリップボードにコピーする
    cmds.copyKey(anim_attr) 
    
    cycle_count_pri_poi = [pri_count, post_count]
    for i in range(2):
        if not "cycle" in source_pri_poi[i] : continue

        cycle_count = cycle_count_pri_poi[i]
        pri_or_poi = -1 if i == 0 else 1
        for cycle_i in range(1,cycle_count+1):
            if source_pri_poi[i] == "cycleRelative":
                # 「オフセット付きサイクル」タイプの場合
                o_time = time_range * cycle_i * pri_or_poi
                o_value = value_offset * cycle_i * pri_or_poi 
                cmds.pasteKey(anim_attr, timeOffset=o_time, valueOffset=o_value, option="replace")
            else:
                # 「サイクル」タイプの場合
                o_time = ( time_range * cycle_i + 0.01) * pri_or_poi          
                cmds.pasteKey(anim_attr, timeOffset=o_time, option="replace")
                for source_time in source_times:           
                    new_time = source_time + o_time
                    cmds.selectKey(anim_attr, k = True, t =(new_time,new_time))
                    cmds.keyframe(absolute=True, timeChange=int(new_time))

def replace_animetion_curve(source_curve, target_curve, range_list, anim_layer=None):
    if not range_list : return
    
    for range in range_list:
        if len(range) != 2 : continue
        if range[1] <= range[0] : continue
        s = range[0]
        e = range[1]
        cmds.setKeyframe(target_curve, preserveCurveShape=True, time=(s,s))
        cmds.setKeyframe(source_curve, preserveCurveShape=True, time=(e,e))
        cmds.setKeyframe(target_curve, preserveCurveShape=True, time=(s-1,s-1))
        cmds.setKeyframe(target_curve, preserveCurveShape=True, time=(e+1,e+1))
    
        cmds.copyKey( source_curve, time=(s,e), option="curve" )
        cmds.pasteKey(target_curve, option="replace")
        
#replace_animetion_curve("locator1_translateX", "locator1_translateZ", [[20, 30], [40,70]])
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------------

# 階層の末端のオブジェクトを選択する
#selection_obj = test2.get_selection_list()
#tip_obj = test2.select_hierarchy_tip_nodes(selection_obj)
#
## 選択されたオブジェクトの名前を取得
#node_names = test2.get_selection_list(False)
#
## 
#pose_pack = test2.DagPoseDataPack("BaseAnimation")
#pose_pack.record_pose(node_names, 50)
#
## ポーズデータを外部データに保存する
#bin_path = "C:/D/saveCustomData.bin"
##test2.save_custom_data(bin_path, pose_pack)
#
#source_pose_pack = test2.load_custom_data(bin_path)
#print (source_pose_pack.pose_compare("ABC"))
#
#
## オブジェクトの選択状態をスクリプト実行時に戻す
#test2.select_nodes(selection_obj)