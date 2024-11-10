import maya.cmds as cmds
import subprocess
import os

def get_audio_files_and_start_times_from_trax():
    audio_data = []
    # トラックエディタ内のオーディオトラックを取得
    tracks = cmds.ls(type="audioTrack")

    for track in tracks:
        # トラックに配置されている全てのクリップを取得
        clips = cmds.listConnections(track, type="audioClip")
        if clips:
            for clip in clips:
                # ファイルパスと開始位置を取得
                file_path = cmds.getAttr(f"{clip}.filename")
                start_time = cmds.getAttr(f"{clip}.startFrame")
                audio_data.append((file_path, start_time))
    
    return audio_data

def create_silent_audio(duration, output_file):
    # duration秒分の無音ファイルを生成
    command = ["ffmpeg", "-f", "lavfi", "-i", f"anullsrc=r=44100:cl=mono", "-t", str(duration), output_file]
    subprocess.run(command, check=True)

def merge_audio_files_with_timing(audio_data, output_file):
    temp_files = []

    for file_path, start_time in audio_data:
        # 開始位置までの無音を作成
        silent_file = f"silent_{start_time}.wav"
        create_silent_audio(start_time, silent_file)
        temp_files.append(silent_file)

        # 開始位置の無音とオリジナル音声を一時ファイルに結合
        temp_output = f"temp_{start_time}.wav"
        command = ["ffmpeg", "-i", f"concat:{silent_file}|{file_path}", "-c", "copy", temp_output]
        subprocess.run(command, check=True)
        temp_files.append(temp_output)

    # FFmpegに渡すリストファイルを作成
    list_file = "file_list.txt"
    with open(list_file, 'w') as f:
        for temp_file in temp_files:
            f.write(f"file '{temp_file}'\n")

    # 全ての音声を最終出力ファイルにマージ
    command = ["ffmpeg", "-f", "concat", "-safe", "0", "-i", list_file, "-c", "copy", output_file]
    subprocess.run(command, check=True)

    # 一時ファイルを削除
    os.remove(list_file)
    for temp_file in temp_files:
        os.remove(temp_file)

    print(f"Audio files merged with timing into {output_file}")

def add_audio_to_trax(output_file, start_time=0):
    # トラックエディタに音声ファイルを追加
    audio_node = cmds.createNode("audio", name="MergedAudio")
    cmds.setAttr(f"{audio_node}.filename", output_file, type="string")
    cmds.setAttr(f"{audio_node}.offset", start_time)
    return audio_node

def set_solo_mode_for_trax_audio(target_audio_node):
    # トラックエディタ内のすべてのオーディオトラックを取得
    audio_tracks = cmds.ls(type="audioTrack")

    for track in audio_tracks:
        # トラックに接続されたオーディオノードを取得
        connected_audio = cmds.listConnections(track, type="audio")
        if connected_audio:
            for audio_node in connected_audio:
                if audio_node == target_audio_node:
                    # ターゲットのオーディオノードをミュート解除
                    cmds.setAttr(f"{audio_node}.mute", 0)
                else:
                    # 他のオーディオノードをミュートにする
                    cmds.setAttr(f"{audio_node}.mute", 1)

# トラックエディタ上の音声ファイルとその開始位置を取得
audio_data = get_audio_files_and_start_times_from_trax()

# 出力ファイル名
output_file = "merged_audio_with_timing.wav"

# 結合処理を実行
merge_audio_files_with_timing(audio_data, output_file)

# マージした音声ファイルをトラックエディタに追加
merged_audio_node = add_audio_to_trax(output_file)

# マージした音声ファイルをソロモードに設定
set_solo_mode_for_trax_audio(merged_audio_node)
