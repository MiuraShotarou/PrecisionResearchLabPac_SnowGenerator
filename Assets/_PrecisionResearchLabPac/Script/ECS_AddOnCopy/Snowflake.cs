using System;
using System.Linq;
using UnityEngine;
/// <summary>
/// 定数コンテナ。インスタンス利用で良い
/// </summary>
public class SnowflakeParams
{
    public float rho      = 0.635f;
    public float beta     = 1.6f;
    public float alpha    = 0.4f;
    public float theta    = 0.025f;
    public float kappa    = 0.005f;
    public float mu       = 0.015f;
    public float gamma    = 0.0005f;
    public float sigma    = 0.0f;
}
/// <summary>
/// 格子計算など、論文のアルゴリズムを実装しているスクリプト
/// </summary>
public class Snowflake
{
    public SnowflakeParams Params;
    public int Autogrow;
    public bool[,] a;
    public float[,] b;
    public float[,] c;
    public float[,] d;
    public System.Random rng;
    //C#
    public int size => a.GetLength(0);
    public float[,] neighbors;
    public Snowflake(SnowflakeParams p, int size = 11, int autogrow = 10) {
        Params   = p;
        Autogrow = autogrow;
        // 初期化のみリファクタ
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
        rng = new System.Random();
    }

    /// <summary>
    /// インスタンス呼び出しでOk
    /// </summary>
    public Array NbSum<T>(T[,] field, int cval = 0) where T : struct
    {
        return Numpy.Pad(
            Numpy.Reshape(
                Numpy.Ravel<T>(field.Slice(rowStop: -2, colStart: 1, colStop: -1))
                    .Zip(Numpy.Ravel<T>(field.Slice(rowStop: -2, colStart: 2)),              (a, b) => Convert.ToSingle(a) + Convert.ToSingle(b))
                    .Zip(Numpy.Ravel<T>(field.Slice(rowStart: 1, rowStop: -1, colStop: -2)), (c, d) => c + Convert.ToSingle(d))
                    .Zip(Numpy.Ravel<T>(field.Slice(rowStart: 1, rowStop: -1, colStart: 2)), (e, f) => e + Convert.ToSingle(f))
                    .Zip(Numpy.Ravel<T>(field.Slice(rowStart: 2, colStop: -2)),              (g, h) => g + Convert.ToSingle(h))
                    .Zip(Numpy.Ravel<T>(field.Slice(rowStart: 2, colStart: 1, colStop: -1)), (i, j) => i + Convert.ToSingle(j))
                    .ToArray(),
                field.GetLength(0) - 2,
                field.GetLength(1) - 2),
            1, ModeType.Constant, cval * 6);
    }

    void UpdateNeighbors(){
        this.neighbors = (float[,])Numpy.Where((bool[,])Numpy.Reshape(Numpy.Ravel<bool>(this.a).Select(v => !v).ToArray(),this.a.GetLength(0), this.a.GetLength(1)),
            NbSum((int[,])Numpy.Reshape(Numpy.Ravel<bool>(this.a).Select(v => v ? 1 : 0).ToArray(),this.a.GetLength(0), this.a.GetLength(1))),
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
        var outside = (bool[,])Numpy.Reshape(Numpy.Ravel<bool>(this.a).Select(A => !A).ToArray(), this.a.GetLength(0), this.a.GetLength(1));
        this.d = (float[,])PythonFunction.Div(
            PythonFunction.Add(
                PythonFunction.Mul(this.d, PythonFunction.Add(this.neighbors, Numpy.Full(this.d.Length, 1f, this.d.GetLength(0), this.d.GetLength(1)))),  // d * (1 + neighbors)
                Numpy.Where(outside, NbSum(this.d, 1), 0f)  // np.where(outside, nbsum(d, 1), 0)
            ),
            Numpy.Full(this.d.Length, 7f, this.d.GetLength(0), this.d.GetLength(1))  // / 7
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
        var nearbyDiffusiveMass = (float[,])NbSum(this.d, 1).Mul(Numpy.Full(this.d.Length, Params.rho, this.d.GetLength(0), this.d.GetLength(1)));
        var boundary = ToBool(neighbors);
        var nbs = neighbors.BoolIndexExtract(boundary);
        var b = this.b.BoolIndexExtract(boundary);
        var a_mask = this.a.BoolIndexExtract(boundary); //結晶セルに隣接している非結晶セル
        for (int i = 0; i < a_mask.Length; i++)
        {
            float nbs_i = Numpy.Ravel<float>(nbs)[i], b_i = Numpy.Ravel<float>(b)[i], ndm_i = Numpy.Ravel<float>(nearbyDiffusiveMass.BoolIndexExtract(boundary))[i];
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
        var attached = (bool[,])Numpy.LogicalAnd(boundary, this.a);
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
            this.a = (bool[,])Numpy.Pad(this.a, Autogrow);
            this.b = (float[,])Numpy.Pad(this.b, Autogrow);
            this.c = (float[,])Numpy.Pad(this.c, Autogrow);
            this.d = (float[,])Numpy.Pad(this.d, Autogrow, ModeType.Constant, 1f);
        }
    }

    void Symmetrize()
    {
        // オールAIコード
        int size = this.d.GetLength(0);
        var (i, j) = Numpy.Indices(size, size);
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
