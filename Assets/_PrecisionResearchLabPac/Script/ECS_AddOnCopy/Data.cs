using System;
using System.Linq;
using Unity.Entities;
using UnityEngine;
using Unity.Collections;
using System.Collections.Generic;
using Random = System.Random;

/// <summary> IComponentData </summary>
// public class SnowflakeParams_ECS
// {
//     public float rho      = 0.635f;
//     public float beta     = 1.6f;
//     public float alpha    = 0.4f;
//     public float theta    = 0.025f;
//     public float kappa    = 0.005f;
//     public float mu       = 0.015f;
//     public float gamma    = 0.0005f;
//     public float sigma    = 0.0f;
// }
/// <summary> Snowflake固有パラメータ（ObjectProperties）。Mesh生成後の記録用 兼 読み取り専用</summary>
public class SnowflakeManagedComponent : IComponentData
{
    public float rho;
    public float beta;
    public float alpha;
    public float theta;
    public float kappa;
    public float mu;
    public float gamma;
    public float sigma;
    public float deltaRho; //
    public int steps; // 累計ステップ数
    // public bool exportAnimation;
    // public bool applyAnimation;
    // public int stepsPerFrame;
    // public bool growing;
    public SnowflakeManagedComponent()
    {
        rho = Mathf.Max(rng.Normal(0.6f, 0.2f), 0f);
        beta = Mathf.Max(rng.Normal(1.6f, 0.3f), 0f);
        alpha = Mathf.Max(rng.Normal(0.15f, 0.07f), 0f);
        theta = Mathf.Max(rng.Normal(0.02f, 0.02f), 0f);
        kappa = Mathf.Max(rng.Normal(0.03f, 0.03f), 0f);
        mu = Mathf.Max(rng.Normal(0.05f, 0.02f), 0f);
        gamma = Mathf.Max(rng.Normal(0.001f, 0.01f), 0f);
        sigma = UnityEngine.Random.Range(0f, 1f);
        deltaRho = 0.0f; //単純に0代入するべきではない
        steps = 0;
    }
    private static readonly System.Random rng = new System.Random();
    // Bakerいらないかも
}

// public class SystemData
// {
//     // Params
//     public float rho;
//     public float beta;
//     public float alpha;
//     public float theta;
//     public float kappa;
//     public float mu   ;
//     public float gamma;
//     public float sigma;
//     
//     public int Autogrow;
//     public bool[,] a;  //結晶化しているかどうかの判定
//     public float[,] b; //境界質量
//     public float[,] c; //結晶質量
//     public float[,] d; //拡散質量
//     //C#
//     public int size => (int)Math.Sqrt(a.Length);
//     public float[,] neighbors;
//     /// <summary> a,b,c,dをいちいち初期化している </summary>
//     public SystemData(SnowflakeManagedComponent component, int size = 11, int autogrow = 10)
//     {
//         rho   = component.rho;
//         beta  = component.beta;
//         alpha = component.alpha;
//         theta = component.theta;
//         kappa = component.kappa;
//         mu    = component.mu;
//         gamma = component.gamma;
//         sigma = component.sigma;
//         
//         Autogrow = autogrow;
//         a = new bool[size, size];
//         b = new float[size, size];
//         c = new float[size, size];
//         d = new float[size, size];
//         for (int i = 0; i < size; i++)
//         for (int j = 0; j < size; j++)
//             d[i, j] = 1f;
//         int mid = size / 2;
//         a[mid, mid] = true;
//         c[mid, mid] = 1f;
//         d[mid, mid] = 0f;
//         neighbors = new float[size, size];
//     }
// }

public class SystemData : IDisposable
{
    // Params
    public float rho;
    public float beta;
    public float alpha;
    public float theta;
    public float kappa;
    public float mu;
    public float gamma;
    public float sigma;

    public int Autogrow;
    public int rows;
    public int cols;
    public int size => rows;

    // NativeArray（1次元フラット）
    public NativeArray<bool>  a;
    public NativeArray<float> b;
    public NativeArray<float> c;
    public NativeArray<float> d;
    public NativeArray<float> neighbors;

    // インデックス変換
    public int Idx(int i, int j) => i * cols + j;

    public SystemData(SnowflakeManagedComponent component, int size = 11, int autogrow = 10)
    {
        rho   = component.rho;
        beta  = component.beta;
        alpha = component.alpha;
        theta = component.theta;
        kappa = component.kappa;
        mu    = component.mu;
        gamma = component.gamma;
        sigma = component.sigma;

        Autogrow = autogrow;
        rows = size;
        cols = size;

        int total = size * size;
        a         = new NativeArray<bool> (total, Allocator.Persistent);
        b         = new NativeArray<float>(total, Allocator.Persistent);
        c         = new NativeArray<float>(total, Allocator.Persistent);
        d         = new NativeArray<float>(total, Allocator.Persistent);
        neighbors = new NativeArray<float>(total, Allocator.Persistent);

        // 初期化
        for (int i = 0; i < total; i++)
            d[i] = 1f;

        int mid = size / 2;
        int midIdx = Idx(mid, mid);
        a[midIdx] = true;
        c[midIdx] = 1f;
        d[midIdx] = 0f;
    }

    /// <summary> Pad処理（グリッド拡張） </summary>
    public void Pad(int padWidth)
    {
        int newRows  = rows + padWidth * 2;
        int newCols  = cols + padWidth * 2;
        int newTotal = newRows * newCols;

        var newA = new NativeArray<bool> (newTotal, Allocator.Persistent);
        var newB = new NativeArray<float>(newTotal, Allocator.Persistent);
        var newC = new NativeArray<float>(newTotal, Allocator.Persistent);
        var newD = new NativeArray<float>(newTotal, Allocator.Persistent);

        // newDは1fで初期化
        for (int i = 0; i < newTotal; i++)
            newD[i] = 1f;

        // 元データをコピー
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            int srcIdx = i * cols + j;
            int dstIdx = (i + padWidth) * newCols + (j + padWidth);
            newA[dstIdx] = a[srcIdx];
            newB[dstIdx] = b[srcIdx];
            newC[dstIdx] = c[srcIdx];
            newD[dstIdx] = d[srcIdx];
        }

        // 古い配列を解放
        a.Dispose();
        b.Dispose();
        c.Dispose();
        d.Dispose();
        neighbors.Dispose();

        a    = newA;
        b    = newB;
        c    = newC;
        d    = newD;
        rows = newRows;
        cols = newCols;
        neighbors = new NativeArray<float>(newTotal, Allocator.Persistent);
    }

    public void Dispose()
    {
        if (a.IsCreated)         a.Dispose();
        if (b.IsCreated)         b.Dispose();
        if (c.IsCreated)         c.Dispose();
        if (d.IsCreated)         d.Dispose();
        if (neighbors.IsCreated) neighbors.Dispose();
    }
}

public struct MoveComponent : IComponentData
{
    
}
