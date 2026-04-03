using System;
using System.Linq;
using UnityEngine;
using Unity.Mathematics;
using Cysharp.Threading.Tasks;

public class Calculate
{
    private SystemData data;
    public SystemData GetData => data;
    public Calculate(SystemData data) {
        this.data = data;
    }

    /// <summary> Stepメソッドの繰り返し実行 </summary>
    public async UniTask Execute(int steps, float deltaRho, int i)
    {
        await UniTask.RunOnThreadPool(() =>
        {
            for (int step = 0; step < steps; step++)
            {
                Debug.Log($"i:{i}, step:{step}");
                Step();
                if (deltaRho != 0f)
                {
                    data.rho *= 1 + deltaRho;
                }
            }
        });
    }
    
    private void Step()
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

    void UpdateNeighbors()
    {
        int rows = data.a.GetLength(0);
        int cols = data.a.GetLength(1);
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            if (data.a[i, j]) { data.neighbors[i, j] = 0f; continue; }
            float sum = 0f;
            if (i > 0)               sum += data.a[i-1, j]   ? 1f : 0f;
            if (i > 0 && j < cols-1) sum += data.a[i-1, j+1] ? 1f : 0f;
            if (j > 0)               sum += data.a[i,   j-1] ? 1f : 0f;
            if (j < cols-1)          sum += data.a[i,   j+1] ? 1f : 0f;
            if (i < rows-1 && j > 0) sum += data.a[i+1, j-1] ? 1f : 0f;
            if (i < rows-1)          sum += data.a[i+1, j]   ? 1f : 0f;
            data.neighbors[i, j] = sum;
        }
    }

    void Melting()
    {
        System.Threading.Tasks.Parallel.For(0, data.d.GetLength(0), i =>
        {
            for (int j = 0; j < data.d.GetLength(1); j++)
            {
                if (data.neighbors[i, j] > 0f)
                {
                    data.d[i, j] += (data.b[i, j] * data.mu + data.c[i, j] * data.gamma) / data.rho;
                    data.b[i, j] -= data.b[i, j] * data.mu;
                    data.c[i, j] -= data.c[i, j] * data.gamma;
                }
            }
        });
    }

    void Diffusion()
    {
        int rows = data.d.GetLength(0);
        int cols = data.d.GetLength(1);
        var newD = new float[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            if (data.a[i, j]) continue;
            float nbSum = 0f;
            if (i > 0)               nbSum += data.d[i-1, j];
            if (i > 0 && j < cols-1) nbSum += data.d[i-1, j+1];
            if (j > 0)               nbSum += data.d[i,   j-1];
            if (j < cols-1)          nbSum += data.d[i,   j+1];
            if (i < rows-1 && j > 0) nbSum += data.d[i+1, j-1];
            if (i < rows-1)          nbSum += data.d[i+1, j];
            newD[i, j] = (data.d[i, j] * (1 + data.neighbors[i, j]) + nbSum) / 7f;
        }
        data.d = newD;
    }

    void Freezing()
    {
        for (int i = 0; i < data.d.GetLength(0); i++)
        for (int j = 0; j < data.d.GetLength(1); j++)
        {
            if (data.neighbors[i, j] > 0f)
            {
                data.b[i, j] += data.d[i, j] * data.rho * (1 - data.kappa);
                data.c[i, j] += data.d[i, j] * data.rho * data.kappa;
                data.d[i, j] = 0f;
            }
        }
    }

    void Attachment()
    {
        int rows = data.a.GetLength(0);
        int cols = data.a.GetLength(1);
        // nearbyDiffusiveMass = nbsum(d, 1) * rho
        var nearbyDiffusiveMass = new float[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            float nbSum = 6f; // cval * 6
            if (i > 0)               nbSum += data.d[i-1, j];
            if (i > 0 && j < cols-1) nbSum += data.d[i-1, j+1];
            if (j > 0)               nbSum += data.d[i,   j-1];
            if (j < cols-1)          nbSum += data.d[i,   j+1];
            if (i < rows-1 && j > 0) nbSum += data.d[i+1, j-1];
            if (i < rows-1)          nbSum += data.d[i+1, j];
            nearbyDiffusiveMass[i, j] = nbSum * data.rho;
        }
        // boundary が true のセルに対して条件判定
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            if (data.neighbors[i, j] <= 0f) continue;
            float nbs = data.neighbors[i, j];
            float b   = data.b[i, j];
            float ndm = nearbyDiffusiveMass[i, j];

            if (((nbs == 1 || nbs == 2) && b >= data.beta)
                || (nbs == 3 && (b >= 1 || (b >= data.alpha && ndm < data.theta)))
                || nbs >= 4)
            {
                data.a[i, j] = true;
            }
        }
        // attached = boundary & a
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            if (data.neighbors[i, j] <= 0f || !data.a[i, j]) continue;
            data.c[i, j] += data.b[i, j];
            data.b[i, j]  = 0f;
            data.d[i, j]  = 0f;
        }
    }

    void Noise()
    {
        if (data.sigma == 0) return;
        int rows = data.d.GetLength(0);
        int cols = data.d.GetLength(1);
        var rng = new System.Random();
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            if (data.neighbors[i, j] <= 0f) continue;
            float factor = rng.NextDouble() < 0.5 ? 1f - data.sigma : 1f + data.sigma;
            data.d[i, j] *= factor;
        }
    }

    void Grow()
    {
        if (data.Autogrow == 0) return;
        int rows = data.d.GetLength(0);
        int cols = data.d.GetLength(1);
        float Eqdiff(int rStart, int rEnd, int cStart, int cEnd)
        {
            float max = 0f;
            for (int i = rStart; i < rEnd; i++)
            for (int j = cStart; j < cEnd; j++)
            {
                float diff = math.abs(data.d[i, j] - 1f);
                if (diff > max) max = diff;
            }
            return max;
        }
        float dmax = math.max(
            math.max(Eqdiff(0, rows, 1, 2), Eqdiff(0, rows, cols-2, cols-1)),
            math.max(Eqdiff(1, 2, 0, cols), Eqdiff(rows-2, rows-1, 0, cols))
        );
        if (dmax > 1e-2f)
        {
            data.a = (bool[,])NpPad(data.a, data.Autogrow, null);
            data.b = (float[,])NpPad(data.b, data.Autogrow, null);
            data.c = (float[,])NpPad(data.c, data.Autogrow, null);
            data.d = (float[,])NpPad(data.d, data.Autogrow, 1f);
        }
        data.neighbors = new float[data.a.GetLength(0), data.a.GetLength(1)];
    }

    void Symmetrize()
    {
        int size = data.d.GetLength(0);
        int half = size / 2;
        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            if (i + j <= half || i + j >= size + half - 1)
                data.d[i, j] = 1f;
        }
    }
    
    private Array NpPad(Array arr, int padWidth, object padValue)
    {
        Type elemType = arr.GetType().GetElementType();
        int  rows     = arr.GetLength(0);
        int  cols     = arr.GetLength(1);
        int  newRows  = rows + padWidth * 2;
        int  newCols  = cols + padWidth * 2;
        var  result   = Array.CreateInstance(elemType, newRows, newCols); 
        
        if (padValue == null)
            padValue = elemType.IsValueType ? Activator.CreateInstance(elemType) : null;
        
        for (int i = 0; i < newRows; i++)
        for (int j = 0; j < newCols; j++)
            result.SetValue(padValue, new int[] { i, j });
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.SetValue(arr.GetValue(new int[] { i, j }), new int[]{i + padWidth, j + padWidth});
        return result;
    }
}
