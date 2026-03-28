using Unity.Entities;

// シミュレーション状態を保持するコンポーネント群

// パラメータ
public struct SnowflakeParamsComponent : IComponentData
{
    public float Rho;
    public float Beta;
    public float Alpha;
    public float Theta;
    public float Kappa;
    public float Mu;
    public float Gamma;
    public float Sigma;
    public float DeltaRho;
}

// 実行設定
public struct SnowflakeSettingsComponent : IComponentData
{
    public int   StepsPerFrame;
    public int   MaxSteps;
    public int   AutoGrow;
    public float Scale;
    public int   TotalSteps;
    public bool  Running;
}

// 格子データ（BlobAssetで保持）
public struct SnowflakeGridComponent : IComponentData
{
    public int Size;
    public BlobAssetReference<SnowflakeGridData> Grid;
}

public struct SnowflakeGridData
{
    public BlobArray<bool>  A; // 結晶フラグ
    public BlobArray<float> B; // 境界質量
    public BlobArray<float> C; // 結晶質量
    public BlobArray<float> D; // 拡散質量
}