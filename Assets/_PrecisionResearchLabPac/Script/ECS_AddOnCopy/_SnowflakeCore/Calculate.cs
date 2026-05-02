using System;
using System.Diagnostics;
using Unity.Mathematics;
using Cysharp.Threading.Tasks;

public class Calculate
{
    private SystemData data;
    public SystemData GetData => data;
    public Calculate(SystemData data) {
        this.data = data;
    }

    private bool isTenSteps = false;
    private Stopwatch stopwatch = new Stopwatch();

    public async UniTask Execute(int steps, float deltaRho, int i)
    {
        var total = Stopwatch.StartNew();
        await UniTask.RunOnThreadPool(() =>
        {
            for (int step = 0; step < steps; step++)
            {
                isTenSteps = step % 10 == 0;
                UnityEngine.Debug.Log($"i:{i}, step:{step}, time:{total.Elapsed.TotalSeconds}");
                Step();
                if (deltaRho != 0f)
                    data.rho *= 1 + deltaRho;
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

    void StartTimer() { stopwatch.Restart(); }
    double CurrentTime() => stopwatch.Elapsed.TotalSeconds;

    /// <summary> aの操作 </summary>
    void UpdateNeighbors()
    {
        if (isTenSteps) StartTimer();
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
        if (isTenSteps) UnityEngine.Debug.Log($"UpdateNeighbors:{CurrentTime()}");
    }

    void Melting()
    {
        if (isTenSteps) StartTimer();
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
        if (isTenSteps) UnityEngine.Debug.Log($"Melting:{CurrentTime()}");
    }

    void Diffusion()
    {
        if (isTenSteps) StartTimer();
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
        if (isTenSteps) UnityEngine.Debug.Log($"Diffusion:{CurrentTime()}");
    }

    void Freezing()
    {
        if (isTenSteps) StartTimer();
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
        if (isTenSteps) UnityEngine.Debug.Log($"Freezing:{CurrentTime()}");
    }

    void Attachment()
    {
        if (isTenSteps) StartTimer();
        int rows = data.a.GetLength(0);
        int cols = data.a.GetLength(1);
        var nearbyDiffusiveMass = new float[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            float nbSum = 6f;
            if (i > 0)               nbSum += data.d[i-1, j];
            if (i > 0 && j < cols-1) nbSum += data.d[i-1, j+1];
            if (j > 0)               nbSum += data.d[i,   j-1];
            if (j < cols-1)          nbSum += data.d[i,   j+1];
            if (i < rows-1 && j > 0) nbSum += data.d[i+1, j-1];
            if (i < rows-1)          nbSum += data.d[i+1, j];
            nearbyDiffusiveMass[i, j] = nbSum * data.rho;
        }
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
                data.a[i, j] = true;
        }
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            if (data.neighbors[i, j] <= 0f || !data.a[i, j]) continue;
            data.c[i, j] += data.b[i, j];
            data.b[i, j]  = 0f;
            data.d[i, j]  = 0f;
        }
        if (isTenSteps) UnityEngine.Debug.Log($"Attachment:{CurrentTime()}");
    }

    void Noise()
    {
        if (isTenSteps) StartTimer();
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
        if (isTenSteps) UnityEngine.Debug.Log($"Noise:{CurrentTime()}");
    }

    void Grow()
    {
        if (isTenSteps) StartTimer();
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
        if (isTenSteps) UnityEngine.Debug.Log($"Grow:{CurrentTime()}");
    }

    void Symmetrize()
    {
        if (isTenSteps) StartTimer();
        int size = data.d.GetLength(0);
        int half = size / 2;
        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            if (i + j <= half || i + j >= size + half - 1)
                data.d[i, j] = 1f;
        }
        if (isTenSteps) UnityEngine.Debug.Log($"Symmetrize:{CurrentTime()}");
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

// <ver.JobSystem>
// using System.Diagnostics;
// using Cysharp.Threading.Tasks;
// using Unity.Burst;
// using Unity.Collections;
// using Unity.Jobs;
// using Unity.Mathematics;
//
// public class Calculate
// {
//     private SystemData data;
//     public SystemData GetData => data;
//
//     private bool isTenSteps = false;
//     private Stopwatch stopwatch = new Stopwatch();
//
//     public Calculate(SystemData data)
//     {
//         this.data = data;
//     }
//
//     public async UniTask Execute(int steps, float deltaRho, int snowflakeIndex)
//     {
//         var total = Stopwatch.StartNew();
//         for (int step = 0; step < steps; step++)
//         {
//             isTenSteps = step % 10 == 0;
//             if (isTenSteps)
//                 UnityEngine.Debug.Log($"i:{snowflakeIndex}, step:{step}, time:{total.Elapsed.TotalSeconds}");
//             await Step();
//             if (deltaRho != 0f)
//                 data.rho *= 1 + deltaRho;
//         }
//     }
//
//     void StartTimer() { stopwatch.Restart(); }
//     double CurrentTime() => stopwatch.Elapsed.TotalSeconds;
//
//     private async UniTask Step()
//     {
//         int size = data.rows * data.cols;
//         var newD = new NativeArray<float>(size, Allocator.TempJob);
//
//         // 依存関係チェーンでJobを連結
//         // if (isTenSteps) StartTimer();
//         var h1 = new UpdateNeighborsJob
//         {
//             a         = data.a,
//             neighbors = data.neighbors,
//             rows      = data.rows,
//             cols      = data.cols
//         }.Schedule(size, 64);
//         
//         var h2 = new MeltingJob
//         {
//             neighbors = data.neighbors,
//             b         = data.b,
//             c         = data.c,
//             d         = data.d,
//             mu        = data.mu,
//             gamma     = data.gamma,
//             rho       = data.rho
//         }.Schedule(size, 64, h1);
//
//         var h3 = data.sigma != 0
//             ? new NoiseJob
//             {
//                 neighbors = data.neighbors,
//                 d         = data.d,
//                 sigma     = data.sigma,
//                 seed      = (uint)UnityEngine.Random.Range(1, int.MaxValue)
//             }.Schedule(size, 64, h2): h2;
//
//         var h4 = new DiffusionJob
//         {
//             a         = data.a,
//             d         = data.d,
//             neighbors = data.neighbors,
//             newD      = newD,
//             rows      = data.rows,
//             cols      = data.cols
//         }.Schedule(size, 64, h3);
//
//         var h5 = new SymmetrizeJob
//         {
//             d    = data.d,
//             rows = data.rows,
//             cols = data.cols
//         }.Schedule(size, 64, h4);
//
//         var h6 = new FreezingJob
//         {
//             neighbors = data.neighbors,
//             b         = data.b,
//             c         = data.c,
//             d         = data.d,
//             rho       = data.rho,
//             kappa     = data.kappa
//         }.Schedule(size, 64, h5);
//
//         // 1回のCompleteで全Job完了を待つ
//         h6.Complete();
//
//         // if (isTenSteps) UnityEngine.Debug.Log($"AllJobs:{CurrentTime()}");
//
//         // Diffusionの結果をコピーしてnewDを解放
//         NativeArray<float>.Copy(newD, data.d);
//         newD.Dispose();
//
//         // Attachment
//         // if (isTenSteps) StartTimer();
//         Attachment();
//         // if (isTenSteps) UnityEngine.Debug.Log($"Attachment:{CurrentTime()}");
//
//         // Grow
//         // if (isTenSteps) StartTimer();
//         Grow();
//         // if (isTenSteps) UnityEngine.Debug.Log($"Grow:{CurrentTime()}");
//
//         if (isTenSteps)
//             await UniTask.NextFrame();
//     }
//
//     // ===== Jobs =====
//
//     [BurstCompile]
//     struct UpdateNeighborsJob : IJobParallelFor
//     {
//         [ReadOnly] public NativeArray<bool> a;
//         [NativeDisableParallelForRestriction]
//         public NativeArray<float> neighbors;
//         public int rows, cols;
//
//         public void Execute(int index)
//         {
//             int i = index / cols;
//             int j = index % cols;
//             if (a[index]) { neighbors[index] = 0f; return; }
//             float sum = 0f;
//             if (i > 0)               sum += a[(i-1)*cols+j]   ? 1f : 0f;
//             if (i > 0 && j < cols-1) sum += a[(i-1)*cols+j+1] ? 1f : 0f;
//             if (j > 0)               sum += a[i*cols+j-1]     ? 1f : 0f;
//             if (j < cols-1)          sum += a[i*cols+j+1]     ? 1f : 0f;
//             if (i < rows-1 && j > 0) sum += a[(i+1)*cols+j-1] ? 1f : 0f;
//             if (i < rows-1)          sum += a[(i+1)*cols+j]   ? 1f : 0f;
//             neighbors[index] = sum;
//         }
//     }
//
//     [BurstCompile]
//     struct MeltingJob : IJobParallelFor
//     {
//         [ReadOnly] public NativeArray<float> neighbors;
//         [NativeDisableParallelForRestriction]
//         public NativeArray<float> b, c, d;
//         public float mu, gamma, rho;
//
//         public void Execute(int index)
//         {
//             if (neighbors[index] <= 0f) return;
//             d[index] += (b[index] * mu + c[index] * gamma) / rho;
//             b[index] -= b[index] * mu;
//             c[index] -= c[index] * gamma;
//         }
//     }
//
//     [BurstCompile]
//     struct NoiseJob : IJobParallelFor
//     {
//         [ReadOnly] public NativeArray<float> neighbors;
//         [NativeDisableParallelForRestriction]
//         public NativeArray<float> d;
//         public float sigma;
//         public uint seed;
//
//         public void Execute(int index)
//         {
//             if (neighbors[index] <= 0f) return;
//             var rng = new Unity.Mathematics.Random(seed + (uint)index);
//             d[index] *= rng.NextBool() ? 1f - sigma : 1f + sigma;
//         }
//     }
//
//     [BurstCompile]
//     struct DiffusionJob : IJobParallelFor
//     {
//         [ReadOnly] public NativeArray<bool>  a;
//         [ReadOnly] public NativeArray<float> d;
//         [ReadOnly] public NativeArray<float> neighbors;
//         [WriteOnly] public NativeArray<float> newD;
//         public int rows, cols;
//
//         public void Execute(int index)
//         {
//             int i = index / cols;
//             int j = index % cols;
//             if (a[index]) { newD[index] = 0f; return; }
//             float nbSum = 0f;
//             if (i > 0)               nbSum += d[(i-1)*cols+j];
//             if (i > 0 && j < cols-1) nbSum += d[(i-1)*cols+j+1];
//             if (j > 0)               nbSum += d[i*cols+j-1];
//             if (j < cols-1)          nbSum += d[i*cols+j+1];
//             if (i < rows-1 && j > 0) nbSum += d[(i+1)*cols+j-1];
//             if (i < rows-1)          nbSum += d[(i+1)*cols+j];
//             newD[index] = (d[index] * (1 + neighbors[index]) + nbSum) / 7f;
//         }
//     }
//
//     [BurstCompile]
//     struct SymmetrizeJob : IJobParallelFor
//     {
//         [NativeDisableParallelForRestriction]
//         public NativeArray<float> d;
//         public int rows, cols;
//
//         public void Execute(int index)
//         {
//             int i    = index / cols;
//             int j    = index % cols;
//             int half = rows / 2;
//             if (i + j <= half || i + j >= rows + half - 1)
//                 d[index] = 1f;
//         }
//     }
//
//     [BurstCompile]
//     struct FreezingJob : IJobParallelFor
//     {
//         [ReadOnly] public NativeArray<float> neighbors;
//         [NativeDisableParallelForRestriction]
//         public NativeArray<float> b, c, d;
//         public float rho, kappa;
//
//         public void Execute(int index)
//         {
//             if (neighbors[index] <= 0f) return;
//             b[index] += d[index] * rho * (1 - kappa);
//             c[index] += d[index] * rho * kappa;
//             d[index]  = 0f;
//         }
//     }
//
//     // ===== Job化不可メソッド =====
//
//     void Attachment()
//     {
//         int rows = data.rows;
//         int cols = data.cols;
//         var ndm  = new NativeArray<float>(rows * cols, Allocator.Temp);
//
//         for (int i = 0; i < rows; i++)
//         for (int j = 0; j < cols; j++)
//         {
//             int   idx   = i * cols + j;
//             float nbSum = 6f;
//             if (i > 0)               nbSum += data.d[(i-1)*cols+j];
//             if (i > 0 && j < cols-1) nbSum += data.d[(i-1)*cols+j+1];
//             if (j > 0)               nbSum += data.d[i*cols+j-1];
//             if (j < cols-1)          nbSum += data.d[i*cols+j+1];
//             if (i < rows-1 && j > 0) nbSum += data.d[(i+1)*cols+j-1];
//             if (i < rows-1)          nbSum += data.d[(i+1)*cols+j];
//             ndm[idx] = nbSum * data.rho;
//         }
//
//         for (int i = 0; i < rows; i++)
//         for (int j = 0; j < cols; j++)
//         {
//             int   idx = i * cols + j;
//             if (data.neighbors[idx] <= 0f) continue;
//             float nbs = data.neighbors[idx];
//             float b   = data.b[idx];
//             float n   = ndm[idx];
//             if (((nbs == 1 || nbs == 2) && b >= data.beta)
//                 || (nbs == 3 && (b >= 1 || (b >= data.alpha && n < data.theta)))
//                 || nbs >= 4)
//                 data.a[idx] = true;
//         }
//
//         for (int i = 0; i < rows; i++)
//         for (int j = 0; j < cols; j++)
//         {
//             int idx = i * cols + j;
//             if (data.neighbors[idx] <= 0f || !data.a[idx]) continue;
//             data.c[idx] += data.b[idx];
//             data.b[idx]  = 0f;
//             data.d[idx]  = 0f;
//         }
//
//         ndm.Dispose();
//     }
//
//     void Grow()
//     {
//         if (data.Autogrow == 0) return;
//         int rows = data.rows;
//         int cols = data.cols;
//
//         float Eqdiff(int rStart, int rEnd, int cStart, int cEnd)
//         {
//             float max = 0f;
//             for (int i = rStart; i < rEnd; i++)
//             for (int j = cStart; j < cEnd; j++)
//             {
//                 float diff = math.abs(data.d[i * cols + j] - 1f);
//                 if (diff > max) max = diff;
//             }
//             return max;
//         }
//
//         float dmax = math.max(
//             math.max(Eqdiff(0, rows, 1, 2),      Eqdiff(0, rows, cols-2, cols-1)),
//             math.max(Eqdiff(1, 2, 0, cols),       Eqdiff(rows-2, rows-1, 0, cols))
//         );
//
//         if (dmax > 1e-2f)
//             data.Pad(data.Autogrow);
//     }
// }