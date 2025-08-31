import maya.cmds as cmds

# ������̐��� expStr ����͂��āA�l�����Z���s���R���p�E���h���쐬����
def createExpCompound(bifShapeName, nodePath, expStr):
    ### ------ �g�[�N�������@���͕�����𕪉����� ------
    # ��FA+B*C ���@[A, +, B, *, C]
    def tokenize():        
        tokens = []
        current = ""
        for ch in expStr:
            if ch.isalnum():  # �ϐ� (A, B, C ...)
                current += ch
            else:
                if current:
                    tokens.append(current)
                    current = ""
                if ch.strip():  # �󔒂͖���
                    tokens.append(ch)
        if current:
            tokens.append(current)
        return tokens
        
    ### ------ Shunting-yard�A���S���Y���ŋt�|�[�����h�L�@�ɕϊ� -------
    # ��: A+B*C �� [A, B, C, *, +]
    # ��F(A�{B)�~(C�|D) ���@[A, B, �{, C, D, �[, �~]
    def shuntingYard(tokens):
        rpn = []
        stack = []
        precedence = {'+':1, '-':1, '*':2, '/':2}
    
        for token in tokens:
            if token.isalnum():  # �ϐ��Ȃ�o��
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
        
    ### ------ exp�p�̃R���p�E���h����� -------
    def prepareExpCompound():
        compName = cmds.vnnCompound(bifShapeName, nodePath, create="exp_compound")
        cmds.vnnNode(bifShapeName, compName, setMetaDataFromString="NodeValueDisplay={show=1;format=\"exp_{exp}\"}")
        cmds.vnnCompound(bifShapeName, compName, createInputPort=("exp", "string"))
        cmds.vnnNode(bifShapeName, compName, setPortDefaultValues=["exp", expStr])
        return compName
        
    ### ------ �t�|�[�����h�L�@��]������Bifrost�̃I�y���[�^�m�[�h�O���t���\�z���� ------
    # ��: A+B*C �� [A, B, C, *, +] �� B*C+A
    def buildExpGraph(compName, rpn):
        # �����ł̃m�[�h�ڑ��p�ɓn�����@(�m�[�h��, �ϐ�(�|�[�g)��, �e�|�[�g��)
        stack = []
        var = []    
        
        for token in rpn:
            if token.isalnum(): # �ϐ�(�C���v�b�g�|�[�g)�̍쐬               
                if token not in var:                    
                    cmds.vnnCompound(bifShapeName, f"{compName}", createInputPort=(token, "auto"))
                    var.append(token)    
                stack.append((compName, token, token))  # (�m�[�h��, �ϐ�(�|�[�g)��, �e�|�[�g��)��z��̖����ɕۑ�����
            else: # ���Z�q�m�[�h�̎擾�Ɛڑ�              
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
        # �ŏI�m�[�h���o�̓|�[�g�֐ڑ����ďI��       
        lastToken = stack.pop()
        cmds.vnnNode(bifShapeName, f"{compName}/output", createInputPort=("result", "auto"))
        cmds.vnnConnect(bifShapeName, f"{lastToken[0]}.{lastToken[2]}", f"{compName}.result")
    
    # ���͕������g�[�N������
    tokens = tokenize()
    # �g�[�N�����t�|�[�����h�L�@�ɕϊ�
    rpn = shuntingYard(tokens)
    # �R���p�E���h��p�ӂ���
    compName = prepareExpCompound()  
    # �t�|�[�����h�L�@��]�����ăR���p�E���h���Ƀm�[�h�O���t���\�z
    buildExpGraph(compName, rpn)

bifShape = cmds.vnnCompoundEditor(query=True, dgContainer=True)
nodePath = cmds.vnnCompoundEditor(query=True, currentCompound=True, name='bifrostGraphEditorControl')
expStr = "(Z+B)-(C-D)/(A+B*(C+D))" 
createExpCompound(bifShape, nodePath, expStr)
