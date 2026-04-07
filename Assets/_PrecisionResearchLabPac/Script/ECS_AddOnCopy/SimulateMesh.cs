using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using Unity.Mathematics;
using Cysharp.Threading.Tasks;

public class SimulateMesh
{
    private Mesh mesh;
    public Mesh GetMesh => mesh;

    public SimulateMesh(Mesh mesh)
    {
        this.mesh = mesh;
    }

    public async UniTask Execute(SystemData data)
    {
        var meshData = await UniTask.RunOnThreadPool(() => MakeMeshData(data));
        var vertices  = meshData.vertices;
        var triangles = meshData.triangles;

        this.mesh.indexFormat = UnityEngine.Rendering.IndexFormat.UInt32;
        this.mesh.SetVertices(Enumerable.Range(0, vertices.GetLength(0))
            .Select(i => new Vector3(vertices[i, 0], vertices[i, 1], vertices[i, 2]))
            .ToList());
        this.mesh.SetTriangles(
            Enumerable.Range(0, triangles.GetLength(0) * 3)
                .Select(i => triangles[i / 3, i % 3])
                .ToList(), 0);
        this.mesh.RecalculateNormals();
        this.mesh.RecalculateBounds();
    }

    private (float[,] vertices, int[,] triangles) MakeMeshData(SystemData data)
    {
        int rows   = data.rows;
        int cols   = data.cols;
        int size   = rows; // 正方形グリッド前提
        float cos30 = math.cos(math.radians(30f));

        // vertex_mask
        var vertex_mask = new bool[rows * cols];
        for (int i = 0; i < rows * cols; i++)
            vertex_mask[i] = data.a[i];

        // triangles生成
        var triList = new List<(int, int, int)>();
        for (int i = 0; i < rows - 1; i++)
        for (int j = 0; j < cols - 1; j++)
        {
            int v00 = i * cols + j;
            int v10 = (i + 1) * cols + j;
            int v01 = i * cols + (j + 1);
            int v11 = (i + 1) * cols + (j + 1);
            triList.Add((v00, v10, v01));
            triList.Add((v10, v11, v01));
        }

        // triangle_mask
        var validTris = new List<(int, int, int)>();
        foreach (var (a, b, c) in triList)
        {
            if (vertex_mask[a] && vertex_mask[b] && vertex_mask[c])
                validTris.Add((a, b, c));
        }

        // vertex_indices_new（cumsum - self）
        var vertex_indices_new = new int[rows * cols];
        int sum = 0;
        for (int i = 0; i < rows * cols; i++)
        {
            vertex_indices_new[i] = sum;
            if (vertex_mask[i]) sum++;
        }
        int vertCount = sum;

        // 頂点座標計算（表面 + ミラー）
        var vertices = new float[vertCount * 2, 3];
        int vi = 0;
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            int idx = i * cols + j;
            if (!data.a[idx]) continue;

            float xi = i - rows / 2f;
            float yi = j - cols / 2f;
            xi = (xi + yi / 2f) / cos30;

            // 境界判定（6方向）
            bool isBoundary = false;
            if (i > 0)               isBoundary |= !data.a[(i-1)*cols+j];
            if (i > 0 && j < cols-1) isBoundary |= !data.a[(i-1)*cols+j+1];
            if (j > 0)               isBoundary |= !data.a[i*cols+j-1];
            if (j < cols-1)          isBoundary |= !data.a[i*cols+j+1];
            if (i < rows-1 && j > 0) isBoundary |= !data.a[(i+1)*cols+j-1];
            if (i < rows-1)          isBoundary |= !data.a[(i+1)*cols+j];
            float zi = isBoundary ? 0f : data.c[idx];

            // 表面
            vertices[vi, 0] = xi;
            vertices[vi, 1] = yi;
            vertices[vi, 2] = zi;
            // ミラー（Z反転）
            vertices[vertCount + vi, 0] = xi;
            vertices[vertCount + vi, 1] = yi;
            vertices[vertCount + vi, 2] = -zi;
            vi++;
        }

        // trianglesリマップ（表面 + ミラー、巻き順逆）
        var triangles = new int[validTris.Count * 2, 3];
        for (int i = 0; i < validTris.Count; i++)
        {
            int a = vertex_indices_new[validTris[i].Item1];
            int b = vertex_indices_new[validTris[i].Item2];
            int c = vertex_indices_new[validTris[i].Item3];
            // 表面
            triangles[i, 0] = a;
            triangles[i, 1] = b;
            triangles[i, 2] = c;
            // ミラー（巻き順逆）
            triangles[validTris.Count + i, 0] = a + vertCount;
            triangles[validTris.Count + i, 1] = c + vertCount;
            triangles[validTris.Count + i, 2] = b + vertCount;
        }

        return (vertices, triangles);
    }
}

// using System;
// using System.Collections.Generic;
// using System.Linq;
// using UnityEngine;
// using Unity.Mathematics;
// using Cysharp.Threading.Tasks;
//
// public class SimulateMesh
// {
//     private Mesh mesh;
//     public Mesh GetMesh => mesh;
//     public SimulateMesh(Mesh mesh){
//         this.mesh = mesh;
//     }
//     
//     public async UniTask Execute(SystemData data)
//     {
//         var meshData = await UniTask.RunOnThreadPool(() => MakeMeshData(data));
//         var vertices = meshData.vertices;
//         var triangles = meshData.triangles;
//         this.mesh.indexFormat = UnityEngine.Rendering.IndexFormat.UInt32;
//         this.mesh.SetVertices(Enumerable.Range(0, vertices.GetLength(0))
//             .Select(i => new Vector3(vertices[i, 0], vertices[i, 1], vertices[i, 2]))
//             .ToList());
//         this.mesh.SetTriangles(Numpy.Ravel<int>(triangles).ToList(), 0);
//         this.mesh.RecalculateNormals(); //ポリゴンの頂点が共有されていればここでスムーズシェードの機能が使えるらしい
//         this.mesh.RecalculateBounds();
//     }
//     private (float[,] vertices, int[,] triangles) MakeMeshData(SystemData data)
//     {
//         int size = data.size;
//         float cos30 = math.cos(math.radians(30f));
//         // vertex_mask
//         bool[] vertex_mask = new bool[size * size];
//         for (int i = 0; i < size; i++)
//         for (int j = 0; j < size; j++)
//             vertex_mask[i * size + j] = data.a[i, j];
//         // triangles生成
//         var triList = new List<(int, int, int)>();
//         for (int i = 0; i < size - 1; i++)
//         for (int j = 0; j < size - 1; j++)
//         {
//             int v00 = i * size + j;
//             int v10 = (i + 1) * size + j;
//             int v01 = i * size + (j + 1);
//             int v11 = (i + 1) * size + (j + 1);
//             triList.Add((v00, v10, v01));
//             triList.Add((v10, v11, v01));
//         }
//         // triangle_mask
//         var validTris = new List<(int, int, int)>();
//         foreach (var (a, b, c) in triList)
//         {
//             if (vertex_mask[a] && vertex_mask[b] && vertex_mask[c])
//                 validTris.Add((a, b, c));
//         }
//         // vertex_indices_new
//         int[] vertex_indices_new = new int[size * size];
//         int sum = 0;
//         for (int i = 0; i < size * size; i++)
//         {
//             vertex_indices_new[i] = sum;
//             if (vertex_mask[i]) sum++;
//         }
//         int vertCount = sum;
//         // 頂点座標計算（表面 + ミラー）
//         var vertices = new float[vertCount * 2, 3];
//         int vi = 0;
//         for (int i = 0; i < size; i++)
//         for (int j = 0; j < size; j++)
//         {
//             if (!data.a[i, j]) continue;
//             float xi = i - size / 2f;
//             float yi = j - size / 2f;
//             xi = (xi + yi / 2f) / cos30;
//             bool isBoundary = false;
//             if (i > 0)               isBoundary |= !data.a[i-1, j];
//             if (i > 0 && j < size-1) isBoundary |= !data.a[i-1, j+1];
//             if (j > 0)               isBoundary |= !data.a[i,   j-1];
//             if (j < size-1)          isBoundary |= !data.a[i,   j+1];
//             if (i < size-1 && j > 0) isBoundary |= !data.a[i+1, j-1];
//             if (i < size-1)          isBoundary |= !data.a[i+1, j];
//             float zi = isBoundary ? 0f : data.c[i, j];
//
//             // 表面
//             vertices[vi, 0] = xi;
//             vertices[vi, 1] = yi;
//             vertices[vi, 2] = zi;
//             // ミラー（Z反転）
//             vertices[vertCount + vi, 0] = xi;
//             vertices[vertCount + vi, 1] = yi;
//             vertices[vertCount + vi, 2] = -zi;
//             vi++;
//         }
//         // trianglesリマップ（表面 + ミラー、巻き順逆）
//         var triangles = new int[validTris.Count * 2, 3];
//         for (int i = 0; i < validTris.Count; i++)
//         {
//             int a = vertex_indices_new[validTris[i].Item1];
//             int b = vertex_indices_new[validTris[i].Item2];
//             int c = vertex_indices_new[validTris[i].Item3];
//             // 表面
//             triangles[i, 0] = a;
//             triangles[i, 1] = b;
//             triangles[i, 2] = c;
//             // ミラー（巻き順逆）
//             triangles[validTris.Count + i, 0] = a + vertCount;
//             triangles[validTris.Count + i, 1] = c + vertCount;
//             triangles[validTris.Count + i, 2] = b + vertCount;
//         }
//         return (vertices, triangles);
//     }
// }
