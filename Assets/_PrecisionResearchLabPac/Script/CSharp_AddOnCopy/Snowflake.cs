using System;
using System.Linq;
using UnityEngine;
using Random = System.Random;

/// <summary>
/// StructData
/// </summary>
public struct SnowflakeParams
{
    public float rho;
    public float beta;
    public float alpha;
    public float theta;
    public float kappa;
    public float mu;
    public float gamma;
    public float sigma;
    
    //MyCode ※削除したい
    // public bool[,] a;
    // public float[,] b;
    // public float[,] c;
    // public float[,] d;
    public static readonly SnowflakeParams Default = new SnowflakeParams
    {
        rho   = 0.635f,
        beta  = 1.6f,
        alpha = 0.4f,
        theta = 0.025f,
        kappa = 0.005f,
        mu    = 0.015f,
        gamma = 0.0005f,
        sigma = 0.0f,
    };
}
/// <summary>
/// SnowflakeSystem → 格子計算など、論文のアルゴリズムを実装しているスクリプト。フィールドは計算用の値として使われる
/// </summary>
public class Snowflake
{
    public SnowflakeParams Params;
    public int Autogrow;
    public bool[,] a;
    public float[,] b;
    public float[,] c;
    public float[,] d;
    public Random rng = new ();
    //C#
    public int size => a.GetLength(0);
    public float[,] neighbors;
    
    public Snowflake(ObjectProperties properties, SnowflakeParams p, int autogrow = 10) {
        this.a = properties.a;
        this.b = properties.b;
        this.c = properties.c;
        this.d = properties.d;
        Params   = p;
        Autogrow = autogrow;
        // a = new bool[size,size];
        // b = new float[size,size];
        // c = new float[size,size];
        // d = new float[size,size];
        // for (int i = 0; i < size; i++)
        // for (int j = 0; j < size; j++)
        //     d[i, j] = 1f;
        // int mid = size / 2;
        // a[mid, mid] = true;
        // c[mid, mid] = 1f;
        // d[mid, mid] = 0f;
    }

    /// <summary>
    /// インスタンス呼び出しでOk
    /// </summary>
    public Array NbSum<T>(T[,] field, int cval = 0) where T : struct
    {
        return NumpyClass.Pad(
            NumpyClass.Reshape(
                NumpyClass.Ravel<T>(field.Slice(rowStop: -2, colStart: 1, colStop: -1))
                    .Zip(NumpyClass.Ravel<T>(field.Slice(rowStop: -2, colStart: 2)),              (a, b) => Convert.ToSingle(a) + Convert.ToSingle(b))
                    .Zip(NumpyClass.Ravel<T>(field.Slice(rowStart: 1, rowStop: -1, colStop: -2)), (c, d) => c + Convert.ToSingle(d))
                    .Zip(NumpyClass.Ravel<T>(field.Slice(rowStart: 1, rowStop: -1, colStart: 2)), (e, f) => e + Convert.ToSingle(f))
                    .Zip(NumpyClass.Ravel<T>(field.Slice(rowStart: 2, colStop: -2)),              (g, h) => g + Convert.ToSingle(h))
                    .Zip(NumpyClass.Ravel<T>(field.Slice(rowStart: 2, colStart: 1, colStop: -1)), (i, j) => i + Convert.ToSingle(j))
                    .ToArray(),
                field.GetLength(0) - 2,
                field.GetLength(1) - 2),
            1, ModeType.Constant, cval * 6);
    }

    void UpdateNeighbors(){
        this.neighbors = (float[,])NumpyClass.Where((bool[,])NumpyClass.Reshape(NumpyClass.Ravel<bool>(this.a).Select(v => !v).ToArray(),this.a.GetLength(0), this.a.GetLength(1)),
            NbSum((int[,])NumpyClass.Reshape(NumpyClass.Ravel<bool>(this.a).Select(v => v ? 1 : 0).ToArray(),this.a.GetLength(0), this.a.GetLength(1))),
            0);
    }

    void Melting()
    {
        for (int i = 0; i < d.GetLength(0); i++)
        for (int j = 0; j < d.GetLength(1); j++)
        {
            if (neighbors[i, j] > 0f)
            {
                d[i, j] += (b[i, j] * Params.mu + c[i, j] * Params.gamma) / Params.rho;
                b[i, j] -= b[i, j] * Params.mu;
                c[i, j] -= c[i, j] * Params.gamma;
            }
        }
    }

    void Diffusion()
    {
        Debug.Assert(this.d.BoolIndexExtract(this.a).Cast<float>().All(x => x == 0f));
        var outside = (bool[,])NumpyClass.Reshape(NumpyClass.Ravel<bool>(this.a).Select(A => !A).ToArray(), this.a.GetLength(0), this.a.GetLength(1));
        this.d = (float[,])PythonFunction.Div(
            PythonFunction.Add(
                PythonFunction.Mul(this.d, PythonFunction.Add(this.neighbors, NumpyClass.Full(this.d.Length, 1f, this.d.GetLength(0), this.d.GetLength(1)))),  // d * (1 + neighbors)
                NumpyClass.Where(outside, NbSum(this.d, 1), 0f)  // np.where(outside, nbsum(d, 1), 0)
            ),
            NumpyClass.Full(this.d.Length, 7f, this.d.GetLength(0), this.d.GetLength(1))  // / 7
        );
        Debug.Assert(this.d.BoolIndexExtract(this.a).Cast<float>().All(x => x == 0f));
    }

    void Freezing()
    {
        for (int i = 0; i < d.GetLength(0); i++)
        for (int j = 0; j < d.GetLength(1); j++)
        {
            if (neighbors[i, j] > 0f)
            {
                b[i, j] += d[i, j] * Params.rho * (1 - Params.kappa);
                c[i, j] += d[i, j] * Params.rho * Params.kappa;
                d[i, j] = 0f;
            }
        }
    }

    void Attachment()
    {
        var nearbyDiffusiveMass = (float[,])NbSum(this.d, 1).Mul(NumpyClass.Full(this.d.Length, Params.rho, this.d.GetLength(0), this.d.GetLength(1)));
        var boundary = ToBool(neighbors);
        var nbs = neighbors.BoolIndexExtract(boundary);
        var b = this.b.BoolIndexExtract(boundary);
        var a_mask = this.a.BoolIndexExtract(boundary); //結晶セルに隣接している非結晶セル
        for (int i = 0; i < a_mask.Length; i++)
        {
            float nbs_i = NumpyClass.Ravel<float>(nbs)[i], b_i = NumpyClass.Ravel<float>(b)[i], ndm_i = NumpyClass.Ravel<float>(nearbyDiffusiveMass.BoolIndexExtract(boundary))[i];
            if (((nbs_i == 1 || nbs_i == 2) && b_i >= Params.beta)
                ||
                (nbs_i == 3 && (b_i >= 1 || (b_i >= Params.alpha && ndm_i < Params.theta)))
                ||
                nbs_i >= 4)
            {
                a_mask[i] = true;
            }
        }
        this.a.BoolIndexSet(boundary, a_mask);
        var attached = (bool[,])NumpyClass.LogicalAnd(boundary, this.a);
        for (int i = 0; i < this.c.GetLength(0); i++)
        for (int j = 0; j < this.c.GetLength(1); j++)
            if (attached[i, j]) this.c[i, j] += this.b[i, j];
        this.b.BoolIndexSet(attached, 0f);
        this.d.BoolIndexSet(attached, 0f);
    }

    void Noise()
    {
        if (Params.sigma != 0)
        {
            var boundary = ToBool(neighbors);
            for (int i = 0; i < d.GetLength(0); i++)
            for (int j = 0; j < d.GetLength(1); j++)
            {
                if (boundary[i, j])
                {
                    float factor = (rng.NextDouble() < 0.5) ? 1f - Params.sigma : 1f + Params.sigma;
                    d[i, j] *= factor;
                }
            }
        }
    }

    void Grow()
    {
        if (Autogrow == 0){
            return;}
        float Eqdiff(float[,] d)
        {
            float max = 0f;
            foreach (float v in d)
            {
                float diff = Math.Abs(v - 1f);
                if (diff > max) max = diff;
            }
            return max;
        }
        var dmax = new float[]
        {
            Eqdiff((float[,])d.Slice(colStart: 1, colStop: 2)),
            Eqdiff((float[,])d.Slice(colStart: -2, colStop: -1)),
            Eqdiff((float[,])d.Slice(rowStart: 1, rowStop: 2)),
            Eqdiff((float[,])d.Slice(rowStart: -2, rowStop: -1))
        }.Max();
        if (dmax > 1e-2f)
        {
            this.a = (bool[,])NumpyClass.Pad(this.a, Autogrow);
            this.b = (float[,])NumpyClass.Pad(this.b, Autogrow);
            this.c = (float[,])NumpyClass.Pad(this.c, Autogrow);
            this.d = (float[,])NumpyClass.Pad(this.d, Autogrow, ModeType.Constant, 1f);
        }
    }

    void Symmetrize()
    {
        // オールAIコード
        int size = this.d.GetLength(0);
        var (i, j) = NumpyClass.Indices(size, size);
        float[,] ij = (float[,])PythonFunction.Add(i, j);
        int half = size / 2;
        bool[,] mask = new bool[size, size];
        for (int ri = 0; ri < size; ri++)
        for (int ci = 0; ci < size; ci++)
            mask[ri, ci] = ij[ri, ci] <= half || ij[ri, ci] >= size + half - 1;
        d.BoolIndexSet(mask, 1f);
    }
    
    public void Step()
    {
        UpdateNeighbors();
        Melting();
        Noise();
        Diffusion();
        Symmetrize();
        Freezing();
        Attachment();
        Grow();
    }
    
    private bool[,] ToBool(float[,] arr)
    {
        int rows   = arr.GetLength(0);
        int cols   = arr.GetLength(1);
        var result = new bool[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i, j] = arr[i, j] != 0f;
        return result;
    }
}
