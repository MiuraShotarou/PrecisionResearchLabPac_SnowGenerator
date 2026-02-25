using UnityEngine;
/// <summary> MeshからCubeオブジェクトを作成するスクリプト </summary>
public class BObjectCreate : MonoBehaviour
{
    public void GenerateCube()
    {
        GameObject cube = new GameObject("BCube"); //生成

        MeshFilter meshFilter = cube.AddComponent<MeshFilter>(); //メッシュの参照を保存している
        MeshRenderer meshRenderer = cube.AddComponent<MeshRenderer>(); //MeshFilterにある参照から実際に描画処理を施す

        meshFilter.mesh = CreateCubeMesh();
        meshRenderer.material = new Material(Shader.Find("Universal Render Pipeline/Lit"));
    }

    Mesh CreateCubeMesh()
    {
        Mesh mesh = new Mesh();

        // 頂点配列（各面に4頂点 × 6面 = 24頂点）
        Vector3[] vertices = {
            // Front
            new Vector3(-0.5f, -0.5f,  0.5f), new Vector3( 0.5f, -0.5f,  0.5f),
            new Vector3( 0.5f,  0.5f,  0.5f), new Vector3(-0.5f,  0.5f,  0.5f),
            // Back
            new Vector3( 0.5f, -0.5f, -0.5f), new Vector3(-0.5f, -0.5f, -0.5f),
            new Vector3(-0.5f,  0.5f, -0.5f), new Vector3( 0.5f,  0.5f, -0.5f),
            // Left
            new Vector3(-0.5f, -0.5f, -0.5f), new Vector3(-0.5f, -0.5f,  0.5f),
            new Vector3(-0.5f,  0.5f,  0.5f), new Vector3(-0.5f,  0.5f, -0.5f),
            // Right
            new Vector3( 0.5f, -0.5f,  0.5f), new Vector3( 0.5f, -0.5f, -0.5f),
            new Vector3( 0.5f,  0.5f, -0.5f), new Vector3( 0.5f,  0.5f,  0.5f),
            // Top
            new Vector3(-0.5f,  0.5f,  0.5f), new Vector3( 0.5f,  0.5f,  0.5f),
            new Vector3( 0.5f,  0.5f, -0.5f), new Vector3(-0.5f,  0.5f, -0.5f),
            // Bottom
            new Vector3(-0.5f, -0.5f, -0.5f), new Vector3( 0.5f, -0.5f, -0.5f),
            new Vector3( 0.5f, -0.5f,  0.5f), new Vector3(-0.5f, -0.5f,  0.5f),
        };

        // 三角形インデックス（各面2三角形）
        int[] triangles = new int[36];
        for (int i = 0; i < 6; i++)
        {
            int v = i * 4;
            int t = i * 6;
            triangles[t + 0] = v;     triangles[t + 1] = v + 2; triangles[t + 2] = v + 1;
            triangles[t + 3] = v;     triangles[t + 4] = v + 3; triangles[t + 5] = v + 2;
        }

        mesh.vertices = vertices;
        mesh.triangles = triangles;
        mesh.RecalculateNormals();

        return mesh;
    }
}