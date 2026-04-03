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

public class SystemData
{
    // Params
    public float rho;
    public float beta;
    public float alpha;
    public float theta;
    public float kappa;
    public float mu   ;
    public float gamma;
    public float sigma;
    
    public int Autogrow;
    public bool[,] a;
    public float[,] b;
    public float[,] c;
    public float[,] d;
    //C#
    public int size => (int)Math.Sqrt(a.Length);
    public float[,] neighbors;
    /// <summary> a,b,c,dをいちいち初期化している </summary>
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
        a = new bool[size, size];
        b = new float[size, size];
        c = new float[size, size];
        d = new float[size, size];
        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            d[i, j] = 1f;
        int mid = size / 2;
        a[mid, mid] = true;
        c[mid, mid] = 1f;
        d[mid, mid] = 0f;
        neighbors = new float[size, size];
    }
}

public struct MoveComponent : IComponentData
{
    
}
