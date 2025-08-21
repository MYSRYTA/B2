import maya.api.OpenMaya as om


########################################################
def bilinear_weights_general(p_uv, uv0, uv1, uv2, uv3, tol=1e-8, max_iter=20):
    """
    歪んだUV四角形に対して双一次補間の重みを解く
    p_uv : (u,v) 補間したい位置
    uv0..uv3 : 四角形の頂点UV (MPoint2d)
    """
    u,v = p_uv
    Q0, Q1, Q2, Q3 = uv0, uv1, uv2, uv3

    # 初期値
    s, t = 0.5, 0.5

    for _ in range(max_iter):
        # f(s,t) = (1-s)(1-t)Q0 + s(1-t)Q1 + stQ2 + (1-s)tQ3 - P
        fx = (1-s)*(1-t)*Q0.x + s*(1-t)*Q1.x + s*t*Q2.x + (1-s)*t*Q3.x - u
        fy = (1-s)*(1-t)*Q0.y + s*(1-t)*Q1.y + s*t*Q2.y + (1-s)*t*Q3.y - v

        # 偏微分
        dfxds = -(1-t)*Q0.x + (1-t)*Q1.x + t*Q2.x - t*Q3.x
        dfxdt = -(1-s)*Q0.x - s*Q1.x + s*Q2.x + (1-s)*Q3.x
        dfyds = -(1-t)*Q0.y + (1-t)*Q1.y + t*Q2.y - t*Q3.y
        dfydt = -(1-s)*Q0.y - s*Q1.y + s*Q2.y + (1-s)*Q3.y

        # ヤコビ行列
        det = dfxds*dfydt - dfyds*dfxdt
        if abs(det) < tol:
            break

        ds = ( fx*dfydt - fy*dfxdt) / det
        dt = (-fx*dfyds + fy*dfxds) / det

        s -= ds
        t -= dt

        if abs(ds) < tol and abs(dt) < tol:
            break

    # 重み
    w0 = (1-s)*(1-t)
    w1 = s*(1-t)
    w2 = s*t
    w3 = (1-s)*t
    return [w0, w1, w2, w3]


def tri_area(a, b, c):
    """2D三角形の符号付き面積"""
    return ( (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x) ) * 0.5

def wachspress_weights(p_uv, uv0, uv1, uv2, uv3):
    """
    Wachspress座標を用いたUV補間
    p_uv: MPoint2d
    uv0..uv3: 四角形の頂点UV (MPoint2d)
    """
    verts = [uv0, uv1, uv2, uv3]
    w = []
    for i in range(4):
        j = (i+1)%4
        k = (i+2)%4
        l = (i+3)%4
        num = tri_area(p_uv, verts[j], verts[k]) * tri_area(p_uv, verts[k], verts[l]) * tri_area(p_uv, verts[l], verts[j])
        w.append(num)

    s = sum(w)
    w = [wi/s for wi in w]
    return w


####################################
def bilinear_weights(p, q0, q1, q2, q3, tol=1e-6, max_iter=10):
    """
    双一次補間の重みを求める
    p: MPoint
    q0,q1,q2,q3: 四角形の頂点 (MPoint)
    """
    # 初期推定値
    s, t = 0.5, 0.5

    for _ in range(max_iter):
        # f(s,t) = P(s,t) - p
        f = ((1-s)*(1-t))*q0 + (s*(1-t))*q1 + (s*t)*q2 + ((1-s)*t)*q3 - p

        # 偏微分
        dPds = (1-t)*(-q0 + q1) + t*(-q3 + q2)
        dPdt = (1-s)*(-q0 + q3) + s*(-q1 + q2)

        
        # ここは実際にはnumpyで解いた方が楽だが、maya純正でやるなら手書き解法が必要
        # 便宜上 X-Y 平面限定と仮定して2x2で解く
        det = dPds.x*dPdt.y - dPds.y*dPdt.x
        if abs(det) < tol:
            break

        ds = ( f.x*dPdt.y - f.y*dPdt.x) / det
        dt = (-f.x*dPds.y + f.y*dPds.x) / det

        s -= ds
        t -= dt

        if abs(ds) < tol and abs(dt) < tol:
            break

    w0 = (1-s)*(1-t)
    w1 = s*(1-t)
    w2 = s*t
    w3 = (1-s)*t

    return [w0, w1, w2, w3]


def area(a, b, c):
    """三角形の符号付き面積 (2D平面近似: XY平面)"""
    return ( (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x) ) * 0.5


def wachspress_weights(p, q0, q1, q2, q3):
    """
    Wachspress座標を計算 (凸四角形用)
    """
    verts = [q0, q1, q2, q3]
    w = []
    for i in range(4):
        j = (i+1)%4
        k = (i+2)%4
        l = (i+3)%4
        num = area(p, verts[j], verts[k]) * area(p, verts[k], verts[l]) * area(p, verts[l], verts[j])
        w.append(num)

    # 正規化
    s = sum(w)
    w = [wi/s for wi in w]
    return w