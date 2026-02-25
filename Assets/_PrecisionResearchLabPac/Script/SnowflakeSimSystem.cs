using Unity.Burst;
using Unity.Collections;
using Unity.Entities;
using Unity.Mathematics;

public partial struct SnowflakeSimSystem : ISystem
{
    public void OnCreate(ref SystemState state)
    {
        state.RequireForUpdate<SnowflakeSettingsComponent>();
    }

    // BurstCompileはBlobArray操作の制約上外している
    public void OnUpdate(ref SystemState state)
    {
        foreach (var (settings, paramsComp, grid) in
            SystemAPI.Query<
                RefRW<SnowflakeSettingsComponent>,
                RefRW<SnowflakeParamsComponent>,
                RefRW<SnowflakeGridComponent>>())
        {
            if (!settings.ValueRO.Running) continue;

            for (int s = 0; s < settings.ValueRO.StepsPerFrame; s++)
            {
                if (settings.ValueRO.TotalSteps >= settings.ValueRO.MaxSteps)
                {
                    settings.ValueRW.Running = false;
                    break;
                }

                Step(ref settings.ValueRW, ref paramsComp.ValueRW, ref grid.ValueRW);
                settings.ValueRW.TotalSteps++;
            }
        }
    }

    private void Step(
        ref SnowflakeSettingsComponent settings,
        ref SnowflakeParamsComponent   p,
        ref SnowflakeGridComponent     grid)
    {
        int size = grid.Size;
        ref var data = ref grid.Grid.Value;

        UpdateNeighbors(ref data, size);
        Melting        (ref data, size, ref p);
        Noise          (ref data, size, ref p);
        Diffusion      (ref data, size);
        Symmetrize     (ref data, size);
        Freezing       (ref data, size, ref p);
        Attachment     (ref data, size, ref p);

        if (p.DeltaRho != 0f)
            p.Rho *= 1f + p.DeltaRho;
    }

    // 隣接結晶数（6方向）
    private int NbCrystal(ref SnowflakeGridData data, int size, int i, int j)
    {
        int sum = 0;
        if (i > 0      && data.A[(i-1)*size+ j   ]) sum++;
        if (i > 0      && j < size-1 && data.A[(i-1)*size+(j+1)]) sum++;
        if (j > 0      && data.A[ i   *size+(j-1)]) sum++;
        if (j < size-1 && data.A[ i   *size+(j+1)]) sum++;
        if (i < size-1 && j > 0      && data.A[(i+1)*size+(j-1)]) sum++;
        if (i < size-1 && data.A[(i+1)*size+ j   ]) sum++;
        return sum;
    }

    // 拡散質量の隣接合計
    private float NbD(ref SnowflakeGridData data, int size, int i, int j)
    {
        float self = data.D[i * size + j];
        float sum  = 0f;

        sum += (i > 0)                   ? (data.A[(i-1)*size+ j   ] ? self : data.D[(i-1)*size+ j   ]) : self;
        sum += (i > 0 && j < size-1)     ? (data.A[(i-1)*size+(j+1)] ? self : data.D[(i-1)*size+(j+1)]) : self;
        sum += (j > 0)                   ? (data.A[ i   *size+(j-1)] ? self : data.D[ i   *size+(j-1)]) : self;
        sum += (j < size-1)              ? (data.A[ i   *size+(j+1)] ? self : data.D[ i   *size+(j+1)]) : self;
        sum += (i < size-1 && j > 0)     ? (data.A[(i+1)*size+(j-1)] ? self : data.D[(i+1)*size+(j-1)]) : self;
        sum += (i < size-1)              ? (data.A[(i+1)*size+ j   ] ? self : data.D[(i+1)*size+ j   ]) : self;

        return sum;
    }

    private void UpdateNeighbors(ref SnowflakeGridData data, int size) { /* 各ステップ内でNbCrystalを直接呼ぶため省略 */ }

    private void Melting(ref SnowflakeGridData data, int size, ref SnowflakeParamsComponent p)
    {
        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            int idx = i * size + j;
            if (data.A[idx]) continue;
            if (NbCrystal(ref data, size, i, j) <= 0) continue;

            data.D[idx] += (data.B[idx] * p.Mu + data.C[idx] * p.Gamma) / p.Rho;
            data.B[idx] -= data.B[idx] * p.Mu;
            data.C[idx] -= data.C[idx] * p.Gamma;
        }
    }

    private void Noise(ref SnowflakeGridData data, int size, ref SnowflakeParamsComponent p)
    {
        if (p.Sigma <= 0f) return;
        var rng = new Unity.Mathematics.Random((uint)System.DateTime.Now.Ticks);

        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            int idx = i * size + j;
            if (data.A[idx]) continue;
            if (NbCrystal(ref data, size, i, j) <= 0) continue;

            data.D[idx] *= rng.NextBool()
                ? (1f - p.Sigma)
                : (1f + p.Sigma);
        }
    }

    private void Diffusion(ref SnowflakeGridData data, int size)
    {
        var dNew = new NativeArray<float>(size * size, Allocator.Temp);

        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            int idx = i * size + j;
            if (data.A[idx]) continue;

            int   nbs   = NbCrystal(ref data, size, i, j);
            float nbSum = NbD(ref data, size, i, j);
            dNew[idx]   = (data.D[idx] * (1 + nbs) + nbSum) / 7f;
        }

        for (int idx = 0; idx < size * size; idx++)
            if (!data.A[idx]) data.D[idx] = dNew[idx];

        dNew.Dispose();
    }

    private void Symmetrize(ref SnowflakeGridData data, int size)
    {
        int half = size / 2;
        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            if (i + j <= half || i + j >= size + half - 1)
                data.D[i * size + j] = 1f;
        }
    }

    private void Freezing(ref SnowflakeGridData data, int size, ref SnowflakeParamsComponent p)
    {
        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            int idx = i * size + j;
            if (data.A[idx]) continue;
            if (NbCrystal(ref data, size, i, j) <= 0) continue;

            float dRho  = data.D[idx] * p.Rho;
            data.B[idx] += dRho * (1f - p.Kappa);
            data.C[idx] += dRho * p.Kappa;
            data.D[idx]  = 0f;
        }
    }

    private void Attachment(ref SnowflakeGridData data, int size, ref SnowflakeParamsComponent p)
    {
        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            int idx = i * size + j;
            if (data.A[idx]) continue;

            int   nbs  = NbCrystal(ref data, size, i, j);
            if (nbs <= 0) continue;

            float b    = data.B[idx];
            float near = NbD(ref data, size, i, j) * p.Rho;

            bool attach =
                ((nbs == 1 || nbs == 2) && b >= p.Beta)
                || (nbs == 3 && (b >= 1f || (b >= p.Alpha && near < p.Theta)))
                || (nbs >= 4);

            if (!attach) continue;

            data.A[idx]  = true;
            data.C[idx] += data.B[idx];
            data.B[idx]  = 0f;
            data.D[idx]  = 0f;
        }
    }
}