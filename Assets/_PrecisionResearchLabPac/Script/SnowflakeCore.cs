using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// MonoBehaviourに依存しない純粋なシミュレーションコア
/// SnowflakeSimulator.cs のロジックをそのまま移植
/// </summary>
public class SnowflakeCore
{
    // パラメータ
    private float _rho, _beta, _alpha, _theta;
    private float _kappa, _mu, _gamma, _sigma, _deltaRho;

    // 格子
    private bool[,]  _a;
    private float[,] _b, _c, _d;
    private int[,]   _neighbors;
    private int      _size;

    private readonly int   _maxSteps;
    private readonly float _scale;

    public SnowflakeCore(int maxSteps, float scale)
    {
        _maxSteps = maxSteps;
        _scale    = scale;
        InitGrid(11);

        // デフォルトパラメータ
        _rho      = 0.635f;
        _beta     = 1.6f;
        _alpha    = 0.4f;
        _theta    = 0.025f;
        _kappa    = 0.005f;
        _mu       = 0.015f;
        _gamma    = 0.0005f;
        _sigma    = 0.0f;
        _deltaRho = 0.0f;
    }

    public void RandomizeParams()
    {
        _rho      = Mathf.Max(SampleNormal(0.6f,   0.2f),  0.01f);
        _beta     = Mathf.Max(SampleNormal(1.6f,   0.3f),  0.01f);
        _alpha    = Mathf.Max(SampleNormal(0.15f,  0.07f), 0.01f);
        _theta    = Mathf.Max(SampleNormal(0.02f,  0.02f), 0.001f);
        _kappa    = Mathf.Max(SampleNormal(0.03f,  0.03f), 0.001f);
        _mu       = Mathf.Max(SampleNormal(0.05f,  0.02f), 0.001f);
        _gamma    = Mathf.Max(SampleNormal(0.001f, 0.01f), 0.0001f);
        _sigma    = Random.Range(0.001f, 0.05f); // 必ずランダム性を持たせる
        _deltaRho = Random.Range(0f,     0.00005f);
    }

    /// <summary> 全ステップを一括実行する </summary>
    public void RunAll()
    {
        for (int s = 0; s < _maxSteps; s++)
            Step();
    }

    /// <summary> シミュレーション結果からMeshを生成する </summary>
    public Mesh BuildMesh()
    {
        var vertices  = new List<Vector3>();
        var triangles = new List<int>();

        float cosR = Mathf.Cos(Mathf.Deg2Rad * 30f);
        int   half = _size / 2;

        int[,] vi = new int[_size, _size];
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
            vi[i, j] = -1;

        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (!_a[i, j]) continue;

            float fi = i - half;
            float fj = j - half;
            float wx = (fi + fj / 2f) / cosR * _scale;
            float wz = fj * _scale;
            float wy = _c[i, j] * _scale * 0.5f;

            vi[i, j] = vertices.Count;
            vertices.Add(new Vector3(wx, wy, wz));
        }

        for (int i = 0; i < _size - 1; i++)
        for (int j = 0; j < _size - 1; j++)
        {
            int v00 = vi[i,     j    ];
            int v10 = vi[i + 1, j    ];
            int v01 = vi[i,     j + 1];
            int v11 = vi[i + 1, j + 1];

            if (v00 >= 0 && v10 >= 0 && v01 >= 0)
            {
                // 表面
                triangles.Add(v00); triangles.Add(v10); triangles.Add(v01);
                // 裏面
                triangles.Add(v00); triangles.Add(v01); triangles.Add(v10);
            }
            if (v10 >= 0 && v11 >= 0 && v01 >= 0)
            {
                triangles.Add(v10); triangles.Add(v11); triangles.Add(v01);
                triangles.Add(v10); triangles.Add(v01); triangles.Add(v11);
            }
        }

        var mesh = new Mesh();
        mesh.indexFormat = UnityEngine.Rendering.IndexFormat.UInt32;
        if (vertices.Count > 0)
        {
            mesh.SetVertices(vertices);
            mesh.SetTriangles(triangles, 0);
            mesh.RecalculateNormals();
            mesh.RecalculateBounds();
        }
        return mesh;
    }

    // -------------------------------------------------------
    // シミュレーション処理（SnowflakeSimulator.cs から移植）
    // -------------------------------------------------------
    private void InitGrid(int size)
    {
        _size      = size;
        _a         = new bool [size, size];
        _b         = new float[size, size];
        _c         = new float[size, size];
        _d         = new float[size, size];
        _neighbors = new int  [size, size];

        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            _d[i, j] = 1f;

        int mid      = size / 2;
        _a[mid, mid] = true;
        _c[mid, mid] = 1f;
        _d[mid, mid] = 0f;
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

        if (_deltaRho != 0f)
            _rho *= 1f + _deltaRho;
    }

    private void UpdateNeighbors()
    {
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_a[i, j]) { _neighbors[i, j] = 0; continue; }
            _neighbors[i, j] = NbCrystal(i, j);
        }
    }

    private void Melting()
    {
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_neighbors[i, j] <= 0) continue;
            _d[i, j] += (_b[i, j] * _mu + _c[i, j] * _gamma) / _rho;
            _b[i, j] -= _b[i, j] * _mu;
            _c[i, j] -= _c[i, j] * _gamma;
        }
    }

    private void Noise()
    {
        if (_sigma <= 0f) return;
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_neighbors[i, j] <= 0) continue;
            _d[i, j] *= (Random.value < 0.5f)
                ? (1f - _sigma)
                : (1f + _sigma);
        }
    }

    private void Diffusion()
    {
        float[,] dNew = new float[_size, _size];
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_a[i, j]) continue;
            float nbSum = NbSumD(i, j);
            dNew[i, j]  = (_d[i, j] * (1 + _neighbors[i, j]) + nbSum) / 7f;
        }
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
            if (!_a[i, j]) _d[i, j] = dNew[i, j];
    }

    private void Symmetrize()
    {
        int half = _size / 2;
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
            if (i + j <= half || i + j >= _size + half - 1)
                _d[i, j] = 1f;
    }

    private void Freezing()
    {
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_neighbors[i, j] <= 0) continue;
            float dRho  = _d[i, j] * _rho;
            _b[i, j]   += dRho * (1f - _kappa);
            _c[i, j]   += dRho * _kappa;
            _d[i, j]    = 0f;
        }
    }

    private void Attachment()
    {
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_neighbors[i, j] <= 0) continue;
            int   nbs  = _neighbors[i, j];
            float b    = _b[i, j];
            float near = NbSumD(i, j) * _rho;

            bool attach =
                ((nbs == 1 || nbs == 2) && b >= _beta)
                || (nbs == 3 && (b >= 1f || (b >= _alpha && near < _theta)))
                || (nbs >= 4);

            if (!attach) continue;
            _a[i, j]  = true;
            _c[i, j] += _b[i, j];
            _b[i, j]  = 0f;
            _d[i, j]  = 0f;
        }
    }

    private void Grow()
    {
        float dmax = 0f;
        for (int j = 0; j < _size; j++)
        {
            dmax = Mathf.Max(dmax, Mathf.Abs(_d[1,          j] - 1f));
            dmax = Mathf.Max(dmax, Mathf.Abs(_d[_size - 2,  j] - 1f));
            dmax = Mathf.Max(dmax, Mathf.Abs(_d[j,          1] - 1f));
            dmax = Mathf.Max(dmax, Mathf.Abs(_d[j, _size - 2] - 1f));
        }
        if (dmax <= 1e-2f) return;

        int pad     = 10;
        int newSize = _size + pad * 2;

        bool[,]  newA = new bool [newSize, newSize];
        float[,] newB = new float[newSize, newSize];
        float[,] newC = new float[newSize, newSize];
        float[,] newD = new float[newSize, newSize];

        for (int i = 0; i < newSize; i++)
        for (int j = 0; j < newSize; j++)
            newD[i, j] = 1f;

        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            newA[i + pad, j + pad] = _a[i, j];
            newB[i + pad, j + pad] = _b[i, j];
            newC[i + pad, j + pad] = _c[i, j];
            newD[i + pad, j + pad] = _d[i, j];
        }

        _size      = newSize;
        _a = newA; _b = newB; _c = newC; _d = newD;
        _neighbors = new int[newSize, newSize];
    }

    private int NbCrystal(int i, int j)
    {
        int sum = 0;
        foreach (var (ni, nj) in HexNeighbors(i, j))
            if (InBounds(ni, nj) && _a[ni, nj]) sum++;
        return sum;
    }

    private float NbSumD(int i, int j)
    {
        float sum  = 0f;
        float cval = _d[i, j];
        foreach (var (ni, nj) in HexNeighbors(i, j))
            sum += (!InBounds(ni, nj) || _a[ni, nj]) ? cval : _d[ni, nj];
        return sum;
    }

    private System.Collections.Generic.IEnumerable<(int, int)> HexNeighbors(int i, int j)
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

    private float SampleNormal(float mean, float stddev)
    {
        float u1 = 1f - Random.value;
        float u2 = 1f - Random.value;
        float z  = Mathf.Sqrt(-2f * Mathf.Log(u1))
                 * Mathf.Cos(2f * Mathf.PI * u2);
        return mean + stddev * z;
    }
}