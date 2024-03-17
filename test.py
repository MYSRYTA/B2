import json

class AA:
    data = [1,2,3]
    dictionary = {
        "section1":{
            "key":"key1",
            "numberX": 1,
            "numberY": 1,
            "numberZ": 1
        },

        "section2":{
            "key":"key2",
            "numberX": 12.999999999999999999,
            "numberY": 13.0000000000000001,
            "numberZ": 14.3333333333333333
        }
    }

DATA = AA()

# 辞書型データをjsonファイルに書き出す
with open('C:/D/tmp.json', 'wt') as f:
    json.dump(DATA.dictionary, f, indent=2, ensure_ascii=False)
# jsonファイルを読み込んで辞書型データにする
with open('C:/D/tmp.json', "r") as f:
    json_load = json.load(f)
    print(json_load['section1']['key'])
    for v in json_load.values():
        print(v["numberX"])
        #cmds.setAttr("locator1.translate", v["numberX"], v["numberY"], v["numberZ"])
        
# -------------------------------------------------------------------------------------

# ディレクトリダイアログを開く
def aa(self):
    caption = "Select a file"
    dir = "C:/cygwin64/bin"
    fileName = QFileDialog.getExistingDirectory(
        None,
        caption,
        dir
    )
    print(fileName)
    
# -------------------------------------------------------------------------------------

# ユーザーレジストリに値を保存する
def bb(self):
    import winreg
    import json
    path = r'Software\Autodesk\Maya-2025\AAA\AT'
    pathAAA = r'Software\Autodesk\Maya-2025\AAA\AT'
    
    try: 
        winreg.OpenKeyEx(winreg.HKEY_CURRENT_USER, pathAAA)
        print ("BBBB")
    except FileNotFoundError:
        print ("AAAA")
    
    with winreg.OpenKeyEx(winreg.HKEY_CURRENT_USER, path) as key:
        data, _ = winreg.QueryValueEx(key, 'name1')
        print('読み取ったデータ：', data)
        aa = json.loads(data)
        print (aa["data"])

# ユーザーレジストリの値を取得する      
def cc(self):
    import winreg
    path = r'Software\Autodesk\Maya-2025\AAA\AT'
    with winreg.CreateKeyEx(winreg.HKEY_CURRENT_USER, path) as key:
        winreg.SetValueEx(key, 'name1', 0, winreg.REG_SZ, '{"data":987654321}')
        print('データを書き込み')