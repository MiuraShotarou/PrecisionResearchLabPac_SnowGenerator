// SnowflakeParams.cs
using System;
using UnityEngine;

/// <summary>
/// 雪の結晶生成パラメータ
/// Gravner & Griffeath (2007) 論文に基づく
/// </summary>
[Serializable]
public class SnowflakeParams
{
    [Tooltip("蒸気密度")]
    public float rho   = 0.635f;

    [Tooltip("付着異方性")]
    public float beta  = 1.6f;

    [Tooltip("ナイフエッジ不安定性")]
    public float alpha = 0.4f;

    [Tooltip("ナイフエッジ不安定性（閾値）")]
    public float theta = 0.025f;

    [Tooltip("凍結強度")]
    public float kappa = 0.005f;

    [Tooltip("境界質量の融解強度")]
    public float mu    = 0.015f;

    [Tooltip("結晶質量の融解強度")]
    public float gamma = 0.0005f;

    [Tooltip("ランダム性（0=人工的, 1=自然発生的）")]
    public float sigma = 0.0f;

    [Tooltip("ステップごとの蒸気密度変化率")]
    public float deltaRho = 0.0f;

    public SnowflakeParams() { }

    // コピーコンストラクタ
    public SnowflakeParams(SnowflakeParams src)
    {
        rho      = src.rho;
        beta     = src.beta;
        alpha    = src.alpha;
        theta    = src.theta;
        kappa    = src.kappa;
        mu       = src.mu;
        gamma    = src.gamma;
        sigma    = src.sigma;
        deltaRho = src.deltaRho;
    }

    /// <summary>
    /// 各パラメータを正規分布・一様分布でランダム化する
    /// </summary>
    public void Randomize(
        bool rhoR      = false,
        bool betaR     = false,
        bool alphaR    = false,
        bool thetaR    = false,
        bool kappaR    = false,
        bool muR       = false,
        bool gammaR    = false,
        bool sigmaR    = false,
        bool deltaRhoR = false)
    {
        if (rhoR)      rho      = Mathf.Max(SampleNormal(0.6f,   0.2f),  0f);
        if (betaR)     beta     = Mathf.Max(SampleNormal(1.6f,   0.3f),  0f);
        if (alphaR)    alpha    = Mathf.Max(SampleNormal(0.15f,  0.07f), 0f);
        if (thetaR)    theta    = Mathf.Max(SampleNormal(0.02f,  0.02f), 0f);
        if (kappaR)    kappa    = Mathf.Max(SampleNormal(0.03f,  0.03f), 0f);
        if (muR)       mu       = Mathf.Max(SampleNormal(0.05f,  0.02f), 0f);
        if (gammaR)    gamma    = Mathf.Max(SampleNormal(0.001f, 0.01f), 0f);
        if (sigmaR)    sigma    = UnityEngine.Random.Range(0f, 1f);
        if (deltaRhoR) deltaRho = UnityEngine.Random.Range(0f, 0.00005f);
    }

    // Box-Muller法による正規分布サンプリング
    private float SampleNormal(float mean, float stddev)
    {
        float u1 = 1f - UnityEngine.Random.value;
        float u2 = 1f - UnityEngine.Random.value;
        float z  = Mathf.Sqrt(-2f * Mathf.Log(u1)) * Mathf.Cos(2f * Mathf.PI * u2);
        return mean + stddev * z;
    }
}