#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

// ...

MStatus MeshOffsetNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    if (plug != outMesh)
        return MS::kUnknownParameter;

    MStatus status;

    // 入力メッシュ
    MObject inMeshObj = dataBlock.inputValue(inMesh, &status).asMesh();
    CHECK_MSTATUS_AND_RETURN_IT(status);
    if (inMeshObj.isNull())
        return MS::kFailure;

    // オフセット値
    float offsetValue = dataBlock.inputValue(offset, &status).asFloat();
    CHECK_MSTATUS_AND_RETURN_IT(status);
    if (offsetValue == 0.0f)
    {
        // オフセットが 0 の場合は単純にコピー
        MDataHandle outHandle = dataBlock.outputValue(outMesh, &status);
        outHandle.setMObject(inMeshObj);
        dataBlock.setClean(plug);
        return MS::kSuccess;
    }

    MFnMesh inMeshFn(inMeshObj, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    // 頂点情報を取得
    MPointArray points;
    inMeshFn.getPoints(points, MSpace::kObject);

    // 法線を取得（非フェース法線）
    MFloatVectorArray normals;
    inMeshFn.getVertexNormals(false, normals, MSpace::kObject);

    // 出力メッシュデータを作成
    MFnMeshData meshDataFn;
    MObject newMeshData = meshDataFn.create(&status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    // トポロジを複製（構造のみ）
    MObject copyMeshObj = inMeshFn.copy(inMeshObj, newMeshData, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    MFnMesh outMeshFn(copyMeshObj);

    // 頂点を並列オフセット
    const unsigned int numVerts = points.length();
    tbb::parallel_for(tbb::blocked_range<unsigned int>(0, numVerts),
        [&](const tbb::blocked_range<unsigned int>& r)
        {
            for (unsigned int i = r.begin(); i < r.end(); ++i)
            {
                const MFloatVector& n = normals[i];
                points[i].x += n.x * offsetValue;
                points[i].y += n.y * offsetValue;
                points[i].z += n.z * offsetValue;
            }
        });

    // 頂点位置のみ更新（トポロジは保持）
    outMeshFn.setPoints(points);

    // 出力ハンドルへ
    MDataHandle outHandle = dataBlock.outputValue(outMesh, &status);
    outHandle.setMObject(newMeshData);
    dataBlock.setClean(plug);

    return MS::kSuccess;
}
