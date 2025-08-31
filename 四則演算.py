import maya.cmds as cmds

# 文字列の数式 expStr を解析して、四則演算を行うコンパウンドを作成する
def createExpCompound(bifShapeName, nodePath, expStr):
    ### ------ トークン分割　入力文字列を分解する ------
    # 例：A+B*C →　[A, +, B, *, C]
    def tokenize():        
        tokens = []
        current = ""
        for ch in expStr:
            if ch.isalnum():  # 変数 (A, B, C ...)
                current += ch
            else:
                if current:
                    tokens.append(current)
                    current = ""
                if ch.strip():  # 空白は無視
                    tokens.append(ch)
        if current:
            tokens.append(current)
        return tokens
        
    ### ------ Shunting-yardアルゴリズムで逆ポーランド記法に変換 -------
    # 例: A+B*C → [A, B, C, *, +]
    # 例：(A＋B)×(C－D) →　[A, B, ＋, C, D, ー, ×]
    def shuntingYard(tokens):
        rpn = []
        stack = []
        precedence = {'+':1, '-':1, '*':2, '/':2}
    
        for token in tokens:
            if token.isalnum():  # 変数なら出力
                rpn.append(token)
            elif token in precedence:
                while stack and stack[-1] in precedence and precedence[stack[-1]] >= precedence[token]:
                    rpn.append(stack.pop())
                stack.append(token)
            elif token == '(':
                stack.append(token)
            elif token == ')':
                while stack and stack[-1] != '(':
                    rpn.append(stack.pop())
                stack.pop()
        while stack:
            rpn.append(stack.pop())
        return rpn
        
    ### ------ exp用のコンパウンドを作る -------
    def prepareExpCompound():
        compName = cmds.vnnCompound(bifShapeName, nodePath, create="exp_compound")
        cmds.vnnNode(bifShapeName, compName, setMetaDataFromString="NodeValueDisplay={show=1;format=\"exp_{exp}\"}")
        cmds.vnnCompound(bifShapeName, compName, createInputPort=("exp", "string"))
        cmds.vnnNode(bifShapeName, compName, setPortDefaultValues=["exp", expStr])
        return compName
        
    ### ------ 逆ポーランド記法を評価してBifrostのオペレータノードグラフを構築する ------
    # 例: A+B*C → [A, B, C, *, +] → B*C+A
    def buildExpGraph(compName, rpn):
        # 下流でのノード接続用に渡す情報　(ノード名, 変数(ポート)名, 親ポート名)
        stack = []
        var = []    
        
        for token in rpn:
            if token.isalnum(): # 変数(インプットポート)の作成               
                if token not in var:                    
                    cmds.vnnCompound(bifShapeName, f"{compName}", createInputPort=(token, "auto"))
                    var.append(token)    
                stack.append((compName, token, token))  # (ノード名, 変数(ポート)名, 親ポート名)を配列の末尾に保存する
            else: # 演算子ノードの取得と接続              
                if token == "+":
                    opNodeName = cmds.vnnCompound(bifShapeName, compName, addNode="BifrostGraph,Core::Math,add")[0]                    
                elif token == "-":
                    opNodeName = cmds.vnnCompound(bifShapeName, compName, addNode="BifrostGraph,Core::Math,subtract")[0]                    
                elif token == "*":
                    opNodeName = cmds.vnnCompound(bifShapeName, compName, addNode="BifrostGraph,Core::Math,multiply")[0]                    
                elif token == "/":
                    opNodeName = cmds.vnnCompound(bifShapeName, compName, addNode="BifrostGraph,Core::Math,divide")[0]
                    
                b = stack.pop()
                a = stack.pop()
                bPortName = b[1]             
                aPortName = f"{a[1]}_" if a[1] == b[1] else a[1]                
                cmds.vnnNode(bifShapeName, f"{compName}/{opNodeName}", createInputPort=(a[1], "auto"))
                cmds.vnnConnect(bifShapeName, f"{a[0]}.{a[2]}", f"{compName}/{opNodeName}.{aPortName}")
                cmds.vnnNode(bifShapeName, f"{compName}/{opNodeName}", createInputPort=(bPortName, "auto"))
                cmds.vnnConnect(bifShapeName, f"{b[0]}.{b[2]}", f"{compName}/{opNodeName}.{bPortName}")
                stack.append((f"{compName}/{opNodeName}", f"{a[1]}_{b[1]}", f"output"))
        # 最終ノードを出力ポートへ接続して終了       
        lastToken = stack.pop()
        cmds.vnnNode(bifShapeName, f"{compName}/output", createInputPort=("result", "auto"))
        cmds.vnnConnect(bifShapeName, f"{lastToken[0]}.{lastToken[2]}", f"{compName}.result")
    
    # 入力文字をトークン分割
    tokens = tokenize()
    # トークンを逆ポーランド記法に変換
    rpn = shuntingYard(tokens)
    # コンパウンドを用意する
    compName = prepareExpCompound()  
    # 逆ポーランド記法を評価してコンパウンド内にノードグラフを構築
    buildExpGraph(compName, rpn)

bifShape = cmds.vnnCompoundEditor(query=True, dgContainer=True)
nodePath = cmds.vnnCompoundEditor(query=True, currentCompound=True, name='bifrostGraphEditorControl')
expStr = "(Z+B)-(C-D)/(A+B*(C+D))" 
createExpCompound(bifShape, nodePath, expStr)
