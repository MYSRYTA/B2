# シーン変更イベントを設定
        self.scene_event_id = om.MSceneMessage.addCallback(
            om.MSceneMessage.kAfterNew, self.close_on_scene_change
        )
        self.open_event_id = om.MSceneMessage.addCallback(
            om.MSceneMessage.kAfterOpen, self.close_on_scene_change
        )
        
        # Perforce管理下かどうかを確認し、処理を分岐
            if self.is_under_perforce_control(text_file_path):
                # ファイルが存在するかチェック
                if os.path.exists(text_file_path):
                    # ファイルが存在する場合、チェックアウトして上書き
                    self.perforce_checkout(text_file_path)
                else:
                    # ファイルが存在しない場合、新規に追加
                    self.perforce_add(text_file_path)
        
    def close_on_scene_change(self, *args):
        """シーンが新規作成または読み込まれた際にウィンドウを閉じる"""
        self.close()
        
    def export_text_data(self):
        # 現在のシーンのファイルパスを取得
        scene_path = cmds.file(q=True, sn=True)
        
        if scene_path:
            # シーンのディレクトリとファイル名を取得
            scene_dir = os.path.dirname(scene_path)
            scene_name = os.path.splitext(os.path.basename(scene_path))[0]
            
            # 出力するテキストファイルのパス
            text_file_path = os.path.join(scene_dir, f"{scene_name}.txt")

            # 出力データ（例としてシンプルなテキスト）
            text_data = "This is sample text data for the scene."

            # テキストデータの書き込み
            with open(text_file_path, "w") as file:
                file.write(text_data)
            
            # 完了メッセージを表示
            QtWidgets.QMessageBox.information(self, "Export Complete", f"Text data has been saved to:\n{text_file_path}")
        else:
            # シーンが保存されていない場合の警告
            QtWidgets.QMessageBox.warning(self, "Error", "Please save the scene before exporting text data.")
    
    def is_under_perforce_control(self, file_path):
        """指定されたファイルがPerforceの管理下かどうかを確認"""
        try:
            result = subprocess.check_output(["p4", "fstat", file_path], stderr=subprocess.STDOUT)
            return b"not on client" not in result  # クライアント上にない場合はFalse
        except subprocess.CalledProcessError:
            return False

    def perforce_checkout(self, file_path):
        """ファイルをチェックアウト"""
        try:
            subprocess.check_call(["p4", "edit", file_path])
        except subprocess.CalledProcessError:
            QtWidgets.QMessageBox.warning(self, "Perforce Error", f"Failed to check out file:\n{file_path}")

    def perforce_add(self, file_path):
        """ファイルをPerforceに追加"""
        try:
            subprocess.check_call(["p4", "add", file_path])
        except subprocess.CalledProcessError:
            QtWidgets.QMessageBox.warning(self, "Perforce Error", f"Failed to add file to Perforce:\n{file_path}")
    def checkin_text_data(self):
        """テキストデータをPerforceにチェックイン"""
        if hasattr(self, 'text_file_path') and os.path.exists(self.text_file_path):
            try:
                # 変更リストを作成
                change_output = subprocess.check_output(["p4", "change", "-o"])
                change_spec = change_output.decode().replace("<enter description here>", "Automated check-in for text data")

                # 一時ファイルに変更リストを書き込み
                with open("temp_changelist.txt", "w") as temp_file:
                    temp_file.write(change_spec)
                
                # 変更リストを更新
                change_output = subprocess.check_output(["p4", "change", "-i"], input=change_spec.encode())
                change_number = change_output.decode().split()[1]

                # 変更リストを使用してチェックイン
                subprocess.check_call(["p4", "submit", "-c", change_number])

                # 完了メッセージを表示
                QtWidgets.QMessageBox.information(self, "Check-In Complete", "Text data has been successfully checked in.")
            except subprocess.CalledProcessError:
                QtWidgets.QMessageBox.warning(self, "Perforce Error", "Failed to check-in text data.")
            finally:
                # 一時ファイルを削除
                if os.path.exists("temp_changelist.txt"):
                    os.remove("temp_changelist.txt")
        else:
            QtWidgets.QMessageBox.warning(self, "Error", "Text file does not exist or has not been exported.")

def get_boundary_vertices_with_high_u_value(mesh_name, u_threshold=0.8):
    # メッシュのシェイプノードを取得
    sel_list = om.MSelectionList()
    sel_list.add(mesh_name)
    dag_path = sel_list.getDagPath(0)
    mesh_fn = om.MFnMesh(dag_path)

    # 境界頂点を格納するセット
    boundary_vertices = set()

    # エッジイテレーターを使用して境界エッジを検出
    edge_it = om.MItMeshEdge(dag_path)
    while not edge_it.isDone():
        if edge_it.onBoundary():  # 境界エッジかどうかをチェック
            vtx1 = edge_it.vertexId(0)
            vtx2 = edge_it.vertexId(1)
            boundary_vertices.add(vtx1)
            boundary_vertices.add(vtx2)
        edge_it.next()

    # UVセットの取得（デフォルトで"map1"を使用）
    uv_set = mesh_fn.currentUVSetName()

    # 境界エッジにある頂点のうち、u値が閾値以上のものを特定
    high_u_vertices = []
    for vert_idx in boundary_vertices:
        try:
            u, v = mesh_fn.getUV(vert_idx, uv_set)
            if u > u_threshold:
                high_u_vertices.append(vert_idx)
        except RuntimeError:
            # UVが割り当てられていない頂点はスキップ
            continue

    return high_u_vertices
    
import maya.api.OpenMaya as om

def get_connected_uv_faces(mesh_name):
    # メッシュのシェイプノードを取得
    sel_list = om.MSelectionList()
    sel_list.add(mesh_name)
    dag_path = sel_list.getDagPath(0)
    mesh_fn = om.MFnMesh(dag_path)
    
    # UVごとに接続するフェースのマッピングを作成
    uv_face_map = {}
    
    # 全フェースをループして各フェースのUVを取得
    for face_id in range(mesh_fn.numPolygons):
        # フェースの頂点ごとにUVインデックスを取得
        uv_ids = mesh_fn.getPolygonUVids(face_id)
        
        for uv_id in uv_ids:
            # UVインデックスごとにフェースIDを追加
            if uv_id not in uv_face_map:
                uv_face_map[uv_id] = []
            uv_face_map[uv_id].append(face_id)
    
    # 接続しているフェースのリストを作成
    connected_face_groups = []
    visited_faces = set()
    
    for uv_id, faces in uv_face_map.items():
        connected_faces = []
        
        for face in faces:
            if face not in visited_faces:
                connected_faces.append(face)
                visited_faces.add(face)
        
        if connected_faces:
            connected_face_groups.append(connected_faces)
    
    return connected_face_groups

# 使用例
mesh_name = "pPlane1"  # メッシュの名前を指定
connected_faces = get_connected_uv_faces(mesh_name)
print("接続しているUVのフェース番号リスト:", connected_faces)

import maya.cmds as cmds
import maya.api.OpenMaya as om

def get_border_vertices(mesh_name):
    """メッシュの境界エッジにある頂点を取得し、左右の辺に分けて返します"""
    sel_list = om.MSelectionList()
    sel_list.add(mesh_name)
    dag_path = sel_list.getDagPath(0)
    
    # メッシュのエッジイテレーターを使って境界エッジを特定
    edge_it = om.MItMeshEdge(dag_path)
    left_border = []
    right_border = []
    
    while not edge_it.isDone():
        if edge_it.onBoundary():
            vtx1, vtx2 = edge_it.vertexId(0), edge_it.vertexId(1)
            pos1 = om.MFnMesh(dag_path).getPoint(vtx1, om.MSpace.kWorld)
            pos2 = om.MFnMesh(dag_path).getPoint(vtx2, om.MSpace.kWorld)
            
            # x座標に基づいて左と右の頂点を分類
            if pos1.x < 0:
                left_border.append(vtx1)
            else:
                right_border.append(vtx1)
            
            if pos2.x < 0:
                left_border.append(vtx2)
            else:
                right_border.append(vtx2)
        
        edge_it.next()
    
    # 重複を削除してソート
    left_border = sorted(set(left_border))
    right_border = sorted(set(right_border))
    return left_border, right_border

def transfer_skin_weights(source_vertices, target_vertices, mesh_name):
    """スキンウェイトを指定された頂点間で転写します"""
    # スキンクラスタの取得
    skin_cluster = cmds.ls(cmds.listHistory(mesh_name), type='skinCluster')[0]
    influences = cmds.skinCluster(skin_cluster, query=True, influence=True)

    # 各インフルエンスに対して、ウェイトを転写
    for i, src_vtx in enumerate(source_vertices):
        # ウェイトを取得
        weights = cmds.skinPercent(skin_cluster, f"{mesh_name}.vtx[{src_vtx}]", query=True, value=True)
        
        # 反対側の頂点にウェイトを適用
        tgt_vtx = target_vertices[i]
        for j, influence in enumerate(influences):
            cmds.skinPercent(skin_cluster, f"{mesh_name}.vtx[{tgt_vtx}]", transformValue=[(influence, weights[j])])

def main(mesh_name):
    # 境界頂点を取得し、左右の辺に分ける
    left_border, right_border = get_border_vertices(mesh_name)
    
    # 左から右へのスキンウェイト転写
    if len(left_border) == len(right_border):
        transfer_skin_weights(left_border, right_border, mesh_name)
        print("スキンウェイトの転写が完了しました。")
    else:
        print("エッジの頂点数が一致しません。手動で確認してください。")

# 使用例
mesh_name = "pPlane1"  # メッシュの名前を指定
main(mesh_name)
