using UnityEngine;

/// <summary>
/// Blenderのミラーモディファイア（Z軸）を再現するテスト用スクリプト
/// 適当なMeshFilterを持つGameObjectにアタッチして使用
/// </summary>
[ExecuteAlways]
[RequireComponent(typeof(MeshFilter))]
public class MirrorModifierTest : MonoBehaviour
{
    // ミラー化するには頂点とノルムに全部-1をかけてvertices, normalsそれぞれに追加し、triangles(Index)配列を二倍に増やせば良い。ただし、頂点座標は0を基準に計算しておく必要がある
    [SerializeField] bool useAxisX = false;
    [SerializeField] bool useAxisY = false;
    [SerializeField] bool useAxisZ = true;

    MeshFilter meshFilter;
    Mesh originalMesh;

    void OnEnable()
    {
        meshFilter = GetComponent<MeshFilter>();
        originalMesh = meshFilter.sharedMesh;
        ApplyMirror();
    }

    void OnValidate()
    {
        if (meshFilter == null) meshFilter = GetComponent<MeshFilter>();
        if (originalMesh == null) originalMesh = meshFilter.sharedMesh;
        ApplyMirror();
    }

    void ApplyMirror()
    {
        if (originalMesh == null) return;

        var srcVertices  = originalMesh.vertices;
        var srcTriangles = originalMesh.triangles;
        var srcNormals   = originalMesh.normals;

        var newVertices  = new Vector3[srcVertices.Length * 2];
        var newNormals   = new Vector3[srcNormals.Length * 2];
        var newTriangles = new int[srcTriangles.Length * 2];

        // 元の頂点をそのままコピー
        for (int i = 0; i < srcVertices.Length; i++)
        {
            newVertices[i] = srcVertices[i];
            newNormals[i]  = srcNormals[i];
        }

        // ミラーした頂点を追加
        for (int i = 0; i < srcVertices.Length; i++)
        {
            var v = srcVertices[i];
            newVertices[srcVertices.Length + i] = new Vector3(
                useAxisX ? -v.x : v.x,
                useAxisY ? -v.y : v.y,
                useAxisZ ? -v.z : v.z
            );
            var n = srcNormals[i];
            newNormals[srcNormals.Length + i] = new Vector3(
                useAxisX ? -n.x : n.x,
                useAxisY ? -n.y : n.y,
                useAxisZ ? -n.z : n.z
            );
        }

        // 元のトライアングルをそのままコピー
        for (int i = 0; i < srcTriangles.Length; i++)
            newTriangles[i] = srcTriangles[i];

        // ミラーしたトライアングル（Index）を追加（巻き順を逆にする）
        for (int i = 0; i < srcTriangles.Length; i += 3)
        {
            int offset = srcTriangles.Length; //36
            int vOffset = srcVertices.Length; //24
            //srcTriangles頂点Index
            newTriangles[offset + i + 0] = srcTriangles[i + 0] + vOffset;
            newTriangles[offset + i + 1] = srcTriangles[i + 2] + vOffset; // 巻き順を逆に
            newTriangles[offset + i + 2] = srcTriangles[i + 1] + vOffset;
        }

        var mirroredMesh = new Mesh();
        mirroredMesh.vertices  = newVertices;
        mirroredMesh.triangles = newTriangles;
        mirroredMesh.normals   = newNormals;
        mirroredMesh.RecalculateBounds();

        meshFilter.mesh = mirroredMesh;
    }

    // エディタ上でオブジェクトが無効になった時に元のメッシュに戻す
    void OnDisable()
    {
        if (meshFilter != null && originalMesh != null)
            meshFilter.mesh = originalMesh;
    }
}