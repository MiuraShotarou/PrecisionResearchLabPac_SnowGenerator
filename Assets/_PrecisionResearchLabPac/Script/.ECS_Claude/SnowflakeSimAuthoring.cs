using Unity.Entities;
using UnityEngine;

public class SnowflakeSimAuthoring : MonoBehaviour
{
    [Header("シミュレーションパラメータ")]
    public float Rho      = 0.635f;
    public float Beta     = 1.6f;
    public float Alpha    = 0.4f;
    public float Theta    = 0.025f;
    public float Kappa    = 0.005f;
    public float Mu       = 0.015f;
    public float Gamma    = 0.0005f;
    public float Sigma    = 0.0f;
    public float DeltaRho = 0.0f;

    [Header("ランダム生成")]
    public bool RandomRho      = false;
    public bool RandomBeta     = false;
    public bool RandomAlpha    = false;
    public bool RandomTheta    = false;
    public bool RandomKappa    = false;
    public bool RandomMu       = false;
    public bool RandomGamma    = false;
    public bool RandomSigma    = false;
    public bool RandomDeltaRho = false;

    [Header("実行設定")]
    public int   StepsPerFrame = 1;
    public int   MaxSteps      = 1000;
    public int   AutoGrow      = 10;
    public float Scale         = 0.01f;

    class Baker : Baker<SnowflakeSimAuthoring>
    {
        public override void Bake(SnowflakeSimAuthoring authoring)
        {
            Entity entity = GetEntity(TransformUsageFlags.Dynamic);

            // パラメータをランダム化
            var p = new SnowflakeParamsComponent
            {
                Rho      = authoring.Rho,
                Beta     = authoring.Beta,
                Alpha    = authoring.Alpha,
                Theta    = authoring.Theta,
                Kappa    = authoring.Kappa,
                Mu       = authoring.Mu,
                Gamma    = authoring.Gamma,
                Sigma    = authoring.Sigma,
                DeltaRho = authoring.DeltaRho,
            };

            if (authoring.RandomRho)      p.Rho      = SampleNormal(0.6f,   0.2f);
            if (authoring.RandomBeta)     p.Beta     = SampleNormal(1.6f,   0.3f);
            if (authoring.RandomAlpha)    p.Alpha    = SampleNormal(0.15f,  0.07f);
            if (authoring.RandomTheta)    p.Theta    = SampleNormal(0.02f,  0.02f);
            if (authoring.RandomKappa)    p.Kappa    = SampleNormal(0.03f,  0.03f);
            if (authoring.RandomMu)       p.Mu       = SampleNormal(0.05f,  0.02f);
            if (authoring.RandomGamma)    p.Gamma    = SampleNormal(0.001f, 0.01f);
            if (authoring.RandomSigma)    p.Sigma    = UnityEngine.Random.Range(0f, 1f);
            if (authoring.RandomDeltaRho) p.DeltaRho = UnityEngine.Random.Range(0f, 0.00005f);

            AddComponent(entity, p);
            AddComponent(entity, new SnowflakeSettingsComponent
            {
                StepsPerFrame = authoring.StepsPerFrame,
                MaxSteps      = authoring.MaxSteps,
                AutoGrow      = authoring.AutoGrow,
                Scale         = authoring.Scale,
                TotalSteps    = 0,
                Running       = true,
            });

            // 初期格子データをBlobAssetで生成
            int size = 11;
            int mid  = size / 2;

            var builder  = new BlobBuilder(Unity.Collections.Allocator.Temp);
            ref var root = ref builder.ConstructRoot<SnowflakeGridData>();

            var aArr = builder.Allocate(ref root.A, size * size);
            var bArr = builder.Allocate(ref root.B, size * size);
            var cArr = builder.Allocate(ref root.C, size * size);
            var dArr = builder.Allocate(ref root.D, size * size);

            for (int i = 0; i < size * size; i++)
            {
                aArr[i] = false;
                bArr[i] = 0f;
                cArr[i] = 0f;
                dArr[i] = 1f;
            }

            int midIdx    = mid * size + mid;
            aArr[midIdx]  = true;
            cArr[midIdx]  = 1f;
            dArr[midIdx]  = 0f;

            var gridRef = builder.CreateBlobAssetReference<SnowflakeGridData>(
                Unity.Collections.Allocator.Persistent
            );
            builder.Dispose();

            AddComponent(entity, new SnowflakeGridComponent
            {
                Size = size,
                Grid = gridRef,
            });
        }

        // Box-Muller法による正規分布サンプリング
        private float SampleNormal(float mean, float stddev)
        {
            float u1 = 1f - UnityEngine.Random.value;
            float u2 = 1f - UnityEngine.Random.value;
            float z  = UnityEngine.Mathf.Sqrt(-2f * UnityEngine.Mathf.Log(u1))
                     * UnityEngine.Mathf.Cos(2f * UnityEngine.Mathf.PI * u2);
            return UnityEngine.Mathf.Max(mean + stddev * z, 0f);
        }
    }
}