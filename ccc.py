import maya.api.OpenMaya as om
import maya.cmds as cmds

def rename_shape_nodes_to_transform_plus_shape():
    """
    選択されたオブジェクトのシェープノードの名前を、
    トランスフォームノード名に "Shape" を付加した名前に変更する。
    """
    # 選択リストを取得
    selection_list = om.MGlobal.getActiveSelectionList()

    # 各選択オブジェクトについて処理
    for i in range(selection_list.length()):
        dag_path = selection_list.getDagPath(i)
        
        # トランスフォームノード名を取得
        transform_name = dag_path.fullPathName().split('|')[-1]
        
        # トランスフォームノードがシェープノードを持つ場合、シェープノードを取得
        try:
            dag_path.extendToShape()
            shape_node = dag_path.node()
        except RuntimeError:
            # シェープノードが存在しない場合はスキップ
            continue
        
        # 現在のシェープノード名を取得
        shape_name = dag_path.fullPathName().split('|')[-1]
        
        # 新しいシェープノード名を作成
        new_shape_name = f"{transform_name}Shape"
        
        # シェープノードの名前を変更
        if cmds.objExists(new_shape_name):
            print(f"名前 '{new_shape_name}' は既に存在します。スキップします。")
        else:
            cmds.rename(shape_name, new_shape_name)
            print(f"シェープノード '{shape_name}' の名前を '{new_shape_name}' に変更しました。")

# 使用例
if __name__ == "__main__":
    rename_shape_nodes_to_transform_plus_shape()
