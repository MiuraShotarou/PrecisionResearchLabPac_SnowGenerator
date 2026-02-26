// SnowflakeSimulator.cs
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// Gravner & Griffeath (2007) モデルによる雪の結晶シミュレーター
/// snowflake.py の Snowflake クラスを C# に移植したもの
/// ECSを利用するためSnowflakeParamsComponentに同じ処理のものを実装した
/// </summary>
[RequireComponent(typeof(MeshFilter), typeof(MeshRenderer))]
public class SnowflakeSimulator : MonoBehaviour
{
    // -------------------------------------------------------
    // Inspector
    // -------------------------------------------------------
    [Header("シミュレーションパラメータ")]
    public SnowflakeParams Params = new SnowflakeParams();

    [Header("ランダム生成（Start時に適用）")]
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
    [SerializeField] private int _stepsPerFrame = 1;
    [SerializeField] private int _maxSteps      = 1000; // メッシュの更新回数
    [SerializeField] private int _autoGrow      = 10;   // 格子が足りなくなったときの拡張量
    [SerializeField] private float _scale       = 0.01f;

    // -------------------------------------------------------
    // 格子データ（snowflake.py の a / b / c / d に対応）
    // -------------------------------------------------------
    private bool[,]  _a; // 結晶フラグ
    private float[,] _b; // 境界質量（準液体）
    private float[,] _c; // 結晶質量（氷）
    private float[,] _d; // 相対拡散質量（水蒸気）

    private int[,]  _neighbors; // 各セルの結晶隣接数

    private int  _size;
    private int  _totalSteps;
    private bool _running = true;

    private MeshFilter   _meshFilter;
    private MeshRenderer _meshRenderer;
    private Mesh         _mesh;

    // -------------------------------------------------------
    // 初期化
    // -------------------------------------------------------
    private void Start()
    {
        _meshFilter            = GetComponent<MeshFilter>();
        _meshRenderer          = GetComponent<MeshRenderer>();
        _mesh                  = new Mesh();
        _mesh.indexFormat      = UnityEngine.Rendering.IndexFormat.UInt32;
        _meshFilter.mesh       = _mesh;
        _meshRenderer.material = new Material(Shader.Find("_PrecisionResearchLabPac/Snow"));
        _meshRenderer.sharedMaterial.SetFloat("_Cull", 0f);

        // ランダムパラメータ適用
        Params.Randomize(
            RandomRho, RandomBeta, RandomAlpha, RandomTheta,
            RandomKappa, RandomMu, RandomGamma, RandomSigma, RandomDeltaRho
        );

        InitGrid(11);
        UpdateMesh();
    }

    private void InitGrid(int size)
    {
        _size       = size;
        _a          = new bool [size, size];
        _b          = new float[size, size];
        _c          = new float[size, size];
        _d          = new float[size, size];
        _neighbors  = new int  [size, size];

        // 全セルを蒸気で満たす（d_rel = 1）
        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            _d[i, j] = 1f;

        // 中心セルを初期結晶として配置
        int mid    = size / 2;
        _a[mid, mid] = true;
        _c[mid, mid] = 1f;
        _d[mid, mid] = 0f;
    }

    // -------------------------------------------------------
    // メインループ
    // -------------------------------------------------------
    private void Update()
    {
        if (!_running) return;

        for (int i = 0; i < _stepsPerFrame; i++)
        {
            if (_totalSteps >= _maxSteps) { _running = false; break; }
            Step();
            _totalSteps++;
        }

        UpdateMesh();
    }

    // -------------------------------------------------------
    // 1ステップ（snowflake.py の step() に対応）
    // -------------------------------------------------------
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

        // deltaRho による蒸気密度の更新
        if (Params.deltaRho != 0f)
            Params.rho *= 1f + Params.deltaRho;
    }

    // -------------------------------------------------------
    // 隣接結晶数を更新（六角格子の6方向）
    // snowflake.py の update_neighbors() に対応
    // -------------------------------------------------------
    private void UpdateNeighbors()
    {
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_a[i, j]) { _neighbors[i, j] = 0; continue; }
            _neighbors[i, j] = NbSum_Crystal(i, j);
        }
    }

    // -------------------------------------------------------
    // 融解（snowflake.py の melting() に対応）
    // -------------------------------------------------------
    private void Melting()
    {
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_neighbors[i, j] <= 0) continue;

            _d[i, j] += (_b[i, j] * Params.mu + _c[i, j] * Params.gamma) / Params.rho;
            _b[i, j] -= _b[i, j] * Params.mu;
            _c[i, j] -= _c[i, j] * Params.gamma;
        }
    }

    // -------------------------------------------------------
    // ノイズ（snowflake.py の noise() に対応）
    // -------------------------------------------------------
    private void Noise()
    {
        if (Params.sigma <= 0f) return;

        float factorLow  = 1f - Params.sigma;
        float factorHigh = 1f + Params.sigma;

        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_neighbors[i, j] <= 0) continue;
            _d[i, j] *= (Random.value < 0.5f) ? factorLow : factorHigh;
        }
    }

    // -------------------------------------------------------
    // 拡散（snowflake.py の diffusion() に対応）
    // -------------------------------------------------------
    private void Diffusion()
    {
        float[,] dNew = new float[_size, _size];

        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_a[i, j]) continue;

            // 隣接セルの d の合計（結晶セルは反射境界として自分自身の値を使う）
            float nbSum = NbSum_D(i, j, useReflect: true);

            // d * (1 + neighbors) + nbSum(d) を 7 で割る
            dNew[i, j] = (_d[i, j] * (1 + _neighbors[i, j]) + nbSum) / 7f;
        }

        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (!_a[i, j]) _d[i, j] = dNew[i, j];
        }
    }

    // -------------------------------------------------------
    // 対称化（snowflake.py の symmetrize() に対応）
    // 六角形の外縁を蒸気密度1に保つことで六回対称を維持する
    // -------------------------------------------------------
    private void Symmetrize()
    {
        int half = _size / 2;
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (i + j <= half || i + j >= _size + half - 1)
                _d[i, j] = 1f;
        }
    }

    // -------------------------------------------------------
    // 凍結（snowflake.py の freezing() に対応）
    // -------------------------------------------------------
    private void Freezing()
    {
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_neighbors[i, j] <= 0) continue;

            float dRho = _d[i, j] * Params.rho;
            _b[i, j] += dRho * (1f - Params.kappa);
            _c[i, j] += dRho * Params.kappa;
            _d[i, j]  = 0f;
        }
    }

    // -------------------------------------------------------
    // 付着（snowflake.py の attachement() に対応）
    // -------------------------------------------------------
    private void Attachment()
    {
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_neighbors[i, j] <= 0) continue;

            int   nbs                  = _neighbors[i, j];
            float b                    = _b[i, j];
            float nearbyDiffusiveMass  = NbSum_D(i, j, useReflect: true) * Params.rho;

            bool attach =
                ((nbs == 1 || nbs == 2) && b >= Params.beta)
                || (nbs == 3 && (b >= 1f || (b >= Params.alpha && nearbyDiffusiveMass < Params.theta)))
                || (nbs >= 4);

            if (!attach) continue;

            _a[i, j]  = true;
            _c[i, j] += _b[i, j];
            _b[i, j]  = 0f;
            _d[i, j]  = 0f;
        }
    }

    // -------------------------------------------------------
    // 格子の自動拡張（snowflake.py の grow() に対応）
    // 結晶が端に近づいたら格子を広げる
    // -------------------------------------------------------
    private void Grow()
    {
        if (_autoGrow <= 0) return;

        float dmax = 0f;
        for (int j = 0; j < _size; j++)
        {
            dmax = Mathf.Max(dmax, Mathf.Abs(_d[1, j]       - 1f));
            dmax = Mathf.Max(dmax, Mathf.Abs(_d[_size - 2, j] - 1f));
            dmax = Mathf.Max(dmax, Mathf.Abs(_d[j, 1]       - 1f));
            dmax = Mathf.Max(dmax, Mathf.Abs(_d[j, _size - 2] - 1f));
        }

        if (dmax <= 1e-2f) return;

        int pad     = _autoGrow;
        int newSize = _size + pad * 2;

        bool[,]  newA = new bool [newSize, newSize];
        float[,] newB = new float[newSize, newSize];
        float[,] newC = new float[newSize, newSize];
        float[,] newD = new float[newSize, newSize];

        // 新しい領域は蒸気密度1で初期化
        for (int i = 0; i < newSize; i++)
        for (int j = 0; j < newSize; j++)
            newD[i, j] = 1f;

        // 既存データをコピー
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            newA[i + pad, j + pad] = _a[i, j];
            newB[i + pad, j + pad] = _b[i, j];
            newC[i + pad, j + pad] = _c[i, j];
            newD[i + pad, j + pad] = _d[i, j];
        }

        _size      = newSize;
        _a         = newA;
        _b         = newB;
        _c         = newC;
        _d         = newD;
        _neighbors = new int[newSize, newSize];
    }

    // -------------------------------------------------------
    // 隣接セルの合計計算ユーティリティ
    // 六角格子の6方向（snowflake.py の nbsum() に対応）
    // -------------------------------------------------------

    // 結晶隣接数
    private int NbSum_Crystal(int i, int j)
    {
        int sum = 0;
        foreach (var (ni, nj) in HexNeighbors(i, j))
        {
            if (InBounds(ni, nj) && _a[ni, nj]) sum++;
        }
        return sum;
    }

    // 拡散質量の隣接合計（結晶セルはcvalで補完）
    private float NbSum_D(int i, int j, bool useReflect = false)
    {
        float sum  = 0f;
        float cval = useReflect ? _d[i, j] : 1f;
        foreach (var (ni, nj) in HexNeighbors(i, j))
        {
            if (!InBounds(ni, nj) || _a[ni, nj])
                sum += cval;
            else
                sum += _d[ni, nj];
        }
        return sum;
    }

    // 六角格子の6方向（offset座標）
    private IEnumerable<(int, int)> HexNeighbors(int i, int j)
    {
        yield return (i - 1, j    );
        yield return (i - 1, j + 1);
        yield return (i,     j - 1);
        yield return (i,     j + 1);
        yield return (i + 1, j - 1);
        yield return (i + 1, j    );
    }

    private bool InBounds(int i, int j)
        => i >= 0 && i < _size && j >= 0 && j < _size;

    // -------------------------------------------------------
    // メッシュ生成（operators.py の make_meshdata() に対応）
    // -------------------------------------------------------
    private void UpdateMesh()
    {
        var vertices  = new List<Vector3>();
        var triangles = new List<int>();

        // 六角格子のオフセット座標 → ワールド座標変換
        // x += y / 2、さらに cos(30°)で正規化（論文の格子定義に基づく）
        float cosR = Mathf.Cos(Mathf.Deg2Rad * 30f);
        float scale = _scale;
        int   half  = _size / 2;

        int[,] vertexIndex = new int[_size, _size];
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
            vertexIndex[i, j] = -1;

        // 付着セルのみ頂点として登録
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (!_a[i, j]) continue;

            float fi = i - half;
            float fj = j - half;
            float wx = (fi + fj / 2f) / cosR * scale;
            float wz = fj * scale;
            float wy = _c[i, j] * scale * 0.5f; // 結晶質量をY高さに反映

            vertexIndex[i, j] = vertices.Count;
            vertices.Add(new Vector3(wx, wy, wz));
        }

        // 隣接する付着セル同士で三角形を生成
        for (int i = 0; i < _size - 1; i++)
        for (int j = 0; j < _size - 1; j++)
        {
            int v00 = vertexIndex[i,     j    ];
            int v10 = vertexIndex[i + 1, j    ];
            int v01 = vertexIndex[i,     j + 1];
            int v11 = vertexIndex[i + 1, j + 1];

            if (v00 >= 0 && v10 >= 0 && v01 >= 0)
            {
                triangles.Add(v00); triangles.Add(v10); triangles.Add(v01);
            }
            if (v10 >= 0 && v11 >= 0 && v01 >= 0)
            {
                triangles.Add(v10); triangles.Add(v11); triangles.Add(v01);
            }
        }

        _mesh.Clear();
        if (vertices.Count > 0)
        {
            _mesh.SetVertices(vertices);
            _mesh.SetTriangles(triangles, 0);
            _mesh.RecalculateNormals();
            _mesh.RecalculateBounds();
        }
    }
}