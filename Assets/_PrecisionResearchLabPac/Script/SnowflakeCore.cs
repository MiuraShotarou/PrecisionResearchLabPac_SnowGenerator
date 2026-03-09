using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEngine;

/// <summary>
/// MonoBehaviourに依存しない純粋なシミュレーションコア
/// 雪の結晶を生成するアルゴリズムからメッシュ作成までを行う
/// </summary>
public class SnowflakeCore
{
    // パラメータ
    private float _rho, _beta, _alpha, _theta;
    private float _kappa, _mu, _gamma, _sigma, _deltaRho;

    // 格子
    private bool[,]  _a; //そのセルが結晶化しているかどうか
    private float[,] _b, _c, _d; //境界質量, 結晶質量, 拡散質量
    private int[,]   _neighbors; //隣接している結晶数[ , ]
    private int      _size; //セルの情報を保持している配列の最大要素数を指定する値

    private readonly int   _maxSteps;
    private readonly float _scale;

    public SnowflakeCore(int maxSteps, float scale)
    {
        _maxSteps = maxSteps;
        _scale    = scale;
        InitGrid(11);

        // デフォルトパラメータ（SnowflakeParams）
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
    /// <summary> パラメータをランダムに設定する </summary>
    public void RandomizeParams()
    {
        _rho      = Mathf.Max(SampleNormal(0.6f,   0.2f),  0.01f); //0.6, 0.2
        _beta     = Mathf.Max(SampleNormal(1.6f,   0.3f),  0.01f); //1.6, 0.3
        _alpha    = Mathf.Max(SampleNormal(0.15f,  0.07f), 0.01f);
        _theta    = Mathf.Max(SampleNormal(0.02f,  0.02f), 0.001f);
        _kappa    = Mathf.Max(SampleNormal(0.03f,  0.03f), 0.001f);
        _mu       = Mathf.Max(SampleNormal(0.05f,  0.02f), 0.001f);
        _gamma    = Mathf.Max(SampleNormal(0.001f, 0.01f), 0.0001f);
        _sigma    = UnityEngine.Random.Range(0.001f, 0.05f); // 必ずランダム性を持たせる
        _deltaRho = UnityEngine.Random.Range(0f,     0.00005f);
    }

    /// <summary> 全ステップを一括実行する </summary>
    public void RunAll() //コルーチン化してみる
    {
        for (int s = 0; s < _maxSteps; s++)
            Step();
    }

    /// <summary> 完了判定 </summary>
    public bool IsFinished(int currentStep) => currentStep >= _maxSteps;
    
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
            float wy = _c[i, j] * _scale * 1f; //0.5
    
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
        // pyと同じ処理になっている
        var mesh = new Mesh();
        mesh.indexFormat = UnityEngine.Rendering.IndexFormat.UInt32;
        if (vertices.Count > 0) //nullチェックみたいなもの
        {
            mesh.SetVertices(vertices); //頂点
            mesh.SetTriangles(triangles, 0); //面
            mesh.RecalculateNormals(); //メッシュのノルムを再計算
            mesh.RecalculateBounds();  //バウンディングボックスを再計算
        }
        return mesh;
    }

    /// <summary>
    /// ①operators.py make_meshを模倣 → 取り敢えずはOK
    /// </summary>
    private Mesh Make_Mesh()
    {
        var meshData = Make_MeshData();
        var vertices  = meshData.vertices;
        var triangles = meshData.triangles;
        Mesh mesh = new Mesh(); //ここのクラスが違う説
        mesh.indexFormat = UnityEngine.Rendering.IndexFormat.UInt32;
        mesh.SetVertices(Enumerable.Range(0, vertices.GetLength(0))
            .Select(i => new Vector3(vertices[i, 0], vertices[i, 1], vertices[i, 2]))
            .ToList());   // 頂点座標を登録
        mesh.SetTriangles(Numpy.Ravel<int>(triangles).ToList(), 0); // インデックス配列を登録
        mesh.RecalculateNormals();
        mesh.RecalculateBounds();
        return mesh;
    }

    /// <summary>
    /// ②operators.py make_meshdataを模倣
    /// </summary>
    private (float[,] vertices, int[,] triangles) Make_MeshData()
    {
        int[ , ] vi = (int[,])Numpy.Reshape(Numpy.Arange(_size * _size), _size, _size);
        var triangles_1 = (int[,])Numpy.ColumnStack(new List<Array>{
            Numpy.Ravel<int>(vi.Slice(rowStop: -1, colStop: -1)),
            Numpy.Ravel<int>(vi.Slice(rowStart: 1, colStop: -1)),
            Numpy.Ravel<int>(vi.Slice(rowStop: -1, colStart: 1))
        });
        var triangles_2 = (int[,])Numpy.ColumnStack(new List<Array>{
            Numpy.Ravel<int>(vi.Slice(rowStart: 1, colStop: -1)),
            Numpy.Ravel<int>(vi.Slice(rowStart: 1, colStart: 1)),
            Numpy.Ravel<int>(vi.Slice(rowStop: -1, colStart: 1))
        });
        int[,] triangles = (int[,])Numpy.VStack(new List<Array> { triangles_1, triangles_2 });
        //del vertex_indices, triangles_1, triangles_2 #このコードはC#で書く意味がない
        bool[] vertex_mask = Numpy.Ravel<bool>(_a); //_a 2次元配列を1次元化
        bool[,] sortMask = (bool[,])UpdateVertexMask(vertex_mask, triangles); //bool[] と int[,] を bool[,] に
        bool[] triangle_mask = (bool[])sortMask.Reduce((a, b) => (bool)a && (bool)b, // logical_and
         true, AxisType.Row);
        triangles = (int[,])UpdateTriangles(triangles, triangle_mask); //int[,] bool[]
        var (x, y) = Numpy.Indices(_size, _size).Cast<float[,]>();
        x = (float[,])Numpy.Reshape(Numpy.Ravel<float>(x).Select(X => X - _size / 2).ToArray(), x.GetLength(0), x.GetLength(1));
        y = (float[,])Numpy.Reshape(Numpy.Ravel<float>(y).Select(Y => Y - _size / 2).ToArray(), y.GetLength(0), y.GetLength(1));
        x = (float[,])Numpy.Reshape(Numpy.Ravel<float>(x).Zip(Numpy.Ravel<float>(y), (X, Y) => X + Y / 2f).ToArray(), x.GetLength(0), x.GetLength(1));
        x = (float[,])Numpy.Reshape(Numpy.Ravel<float>(x).Select(X => X / Mathf.Cos(Mathf.Deg2Rad * 30f)).ToArray(), y.GetLength(0), y.GetLength(1));
        var z = (float[,])Numpy.Where(
            (bool[,])Numpy.LogicalAnd(_a, (bool[,])Numpy.Reshape(
                Numpy.Ravel<int>(NbSum(
                        (int[,])Numpy.Reshape(
                            Numpy.Ravel<bool>(_a).Select(a => Convert.ToInt32(!a)).ToArray(),
                            _a.GetLength(0), _a.GetLength(1)),
                        0))
                    .Select(v => v > 0).ToArray(),
                _a.GetLength(0), _a.GetLength(1))),
            0f, _c);
        var vertices = (float[,])Numpy.ColumnStack(new List<Array>
        {
            Numpy.MaskFilter(Numpy.Ravel<float>(x), vertex_mask),
            Numpy.MaskFilter(Numpy.Ravel<float>(y), vertex_mask),
            Numpy.MaskFilter(Numpy.Ravel<float>(z), vertex_mask)
        });
        return (vertices, triangles);
    }

    Texture2D MakeImage()
    {
        //Blenderで言うところのImage生成{
        var texture2D = new Texture2D(
                 //width の定義
            _size,
                 //heightの定義
            _size,
            //alphaを描画に含める
            //pixelの色値をfloatで保持する
            TextureFormat.RGBAFloat,
            true,
            //画像を色情報ではなく、データとして扱う、という設定を有効にする
            true
        );
        //name プロパティの代入
        texture2D.name = "Snowflake";
        texture2D.SetPixels(ToColors(
            Numpy.Ravel<object>(Numpy.Stack(
                new List<Array>{
                    _a,
                    _b,
                    _c,
                    _d}
                , AxisType.None)))); //Color[] を渡すところだけがPythonと違う。
        texture2D.Apply();
        return texture2D;
    }
    // モデル形状とあまり関係がないので省略
    class AnimationExportError : IOException{AnimationExportError(string message) : base(message){}} //baseは親のコンストラクタを召喚している
    void SaveToUsd()
    {
        
    }
    class AnimationImportError : IOException{AnimationImportError(string message) : base(message){}}

    void ApplyUSDMeshCache()
    {
        
    }
    // 必要かも
    void Simulate()
    {
    }
    void ObjectProperty()
    {
    }

    class SnowflakeCreate
    {
        
    }

    class SnowflakeGrow
    {
        
    }

    class SnowflakeGrowModal
    {
        
    }

    class SnowflakeRandomize
    {
        
    }
    
    // CLASSESがあり

    void Register()
    {
        
    }

    void UnRegister()
    {
        
    }
    // -------------------------------------------------------
    // シミュレーション処理（SnowflakeSimulator.cs から移植）
    // -------------------------------------------------------
    private void InitGrid(int size) //sizeの更新
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
    /// <summary> 空気中の水蒸気が結晶に取り込まれていくプロセスを再現する </summary>
    public void Step() //publicに変更
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
    /// <summary>  </summary>
    private void UpdateNeighbors()
    {
        for (int i = 0; i < _size; i++) //_size * _size ぶん繰り返している → 11 * 11
        for (int j = 0; j < _size; j++)
        {
            // _aがtrueであれば隣接する結晶はない、と判断される
            if (_a[i, j]) { _neighbors[i, j] = 0; continue; }
            _neighbors[i, j] = NbCrystal(i, j); //
        }
    }
    /// <summary>隣接しているセルのなかで結晶化しているセルの数を返す</summary>
    /// <param name="i"></param>
    /// <param name="j"></param>
    /// <returns></returns>
    private int NbCrystal(int i, int j)
    {
        int sum = 0;
        foreach (var (ni, nj) in HexNeighbors(i, j))
            if (InBounds(ni, nj) && _a[ni, nj]) sum++; //_a && 
        return sum;
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
            _d[i, j] *= (UnityEngine.Random.value < 0.5f)
                ? (1f - _sigma)
                : (1f + _sigma);
        }
    }
    /// <summary>
    /// 新
    /// </summary>
    private void Diffusion()
    {
        float[,] dNew = new float[_size, _size];

        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            // 結晶セルはd=0を維持
            if (_a[i, j])
            {
                dNew[i, j] = 0f;
                continue;
            }

            int   nbs   = NbCrystal(i, j);
            float nbSum = NbSumD(i, j);
            dNew[i, j]  = (_d[i, j] * (1 + nbs) + nbSum) / 7f;
        }

        // dNewをそのまま_dに反映（結晶セルも含めて上書き）
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
            _d[i, j] = dNew[i, j];
    }
    //
    // private void Diffusion()
    // {
    //     float[,] dNew = new float[_size, _size];
    //     for (int i = 0; i < _size; i++)
    //     for (int j = 0; j < _size; j++)
    //     {
    //         if (_a[i, j]) continue;
    //         float nbSum = NbSumD(i, j);
    //         dNew[i, j]  = (_d[i, j] * (1 + _neighbors[i, j]) + nbSum) / 7f;
    //     }
    //     for (int i = 0; i < _size; i++)
    //     for (int j = 0; j < _size; j++)
    //         if (!_a[i, j]) _d[i, j] = dNew[i, j];
    // }

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
    /// <summary> 隣接する結晶数 nbs と境界質量 _b が条件を満たした場合にセルを結晶化する</summary>
    private void Attachment()
    {
        for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
        {
            if (_neighbors[i, j] <= 0) continue; //隣接している結晶数が0だったら次の処理へ移る
            int   nbs  = _neighbors[i, j];       //隣接している結晶数
            float b    = _b[i, j];               //境界質量 (雪の結晶の表面に付着している準液体層の質量のこと)
            float near = NbSumD(i, j) * _rho;    //

            bool attach =
                ((nbs == 1 || nbs == 2) && b >= _beta)
                || (nbs == 3 && (b >= 1f || (b >= _alpha && near < _theta)))
                || (nbs >= 4);

            if (!attach) continue;
            _a[i, j]  = true;
            _c[i, j] += _b[i, j];
            _b[i, j]  = 0f; //結晶化に必要な境界質量の閾値 // 結晶の表面に付着した水蒸気の量
            _d[i, j]  = 0f;
        }
    }
    /// <summary>_sizeの更新がある</summary>
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
        int newSize = _size + pad * 2; //配列の拡張

        bool[,]  newA = new bool [newSize, newSize];
        float[,] newB = new float[newSize, newSize];
        float[,] newC = new float[newSize, newSize];
        float[,] newD = new float[newSize, newSize];

        for (int i = 0; i < newSize; i++)
        for (int j = 0; j < newSize; j++)
            newD[i, j] = 1f; //newDはすべて 1

        for (int i = 0; i < _size; i++) //i == 0 ~10
        for (int j = 0; j < _size; j++) //j == 0 ~10
        {
            // 格子の中心 == 配列の要素数 / 2
            newA[i + pad, j + pad] = _a[i, j];
            newB[i + pad, j + pad] = _b[i, j];
            newC[i + pad, j + pad] = _c[i, j];
            newD[i + pad, j + pad] = _d[i, j];
        }

        _size      = newSize; //_size の更新
        _a = newA; _b = newB; _c = newC; _d = newD;
        _neighbors = new int[newSize, newSize]; //_neighborsの初期化
    }

    /// <summary></summary>
    /// <param name="i"></param>
    /// <param name="j"></param>
    /// <returns></returns>
    private float NbSumD(int i, int j)
    {
        float sum  = 0f;
        float cval = _d[i, j]; //代入値は１固定でも良いらしい
        foreach (var (ni, nj) in HexNeighbors(i, j))
            sum += (!InBounds(ni, nj) || _a[ni, nj]) ? cval : _d[ni, nj];
        return sum;
    }
    /// <summary> Pythonのものを移植 </summary>
    private int[,] NbSum(int[,] field, int cval = 0)
    {
        return (int[,])Numpy.Pad(
            Numpy.Reshape(
                Numpy.Ravel<int>(field.Slice(0, -2, 1, -1))
                    .Zip(Numpy.Ravel<int>(field.Slice(0, -2, 2,  0)), (a, b) => a + b)
                    .Zip(Numpy.Ravel<int>(field.Slice(1, -1, 0, -2)), (c, d) => c + d)
                    .Zip(Numpy.Ravel<int>(field.Slice(1, -1, 2,  0)), (e, f) => e + f)
                    .Zip(Numpy.Ravel<int>(field.Slice(2,  0, 0, -2)), (g, h) => g + h)
                    .Zip(Numpy.Ravel<int>(field.Slice(2,  0, 1, -1)), (i, j) => i + j)
                    .ToArray(),
                field.GetLength(0) - 2,
                field.GetLength(1) - 2),
            1, ModeType.Constant, cval * 6);
    }
    /// <summary>引数を中点とした六方向を返す</summary>
    /// <param name="i"></param>
    /// <param name="j"></param>
    /// <returns></returns>
    private System.Collections.Generic.IEnumerable<(int, int)> HexNeighbors(int i, int j)
    {
        yield return (i - 1, j    );
        yield return (i - 1, j + 1);
        yield return (i,     j - 1);
        yield return (i,     j + 1);
        yield return (i + 1, j - 1);
        yield return (i + 1, j    );
    }
    /// <summary>引数に受け取った頂点が格子の範囲内であるかを判定する</summary>
    /// <param name="i"></param>
    /// <param name="j"></param>
    /// <returns></returns>
    private bool InBounds(int i, int j)
        => i >= 0 && i < _size && j >= 0 && j < _size;

    private float SampleNormal(float mean, float stddev)
    {
        float u1 = 1f - UnityEngine.Random.value;
        float u2 = 1f - UnityEngine.Random.value;
        float z  = Mathf.Sqrt(-2f * Mathf.Log(u1))
                 * Mathf.Cos(2f * Mathf.PI * u2);
        return mean + stddev * z;
    }
    //仕方なし関数群
    /// <summary> vertex_mask[triangles] </summary>
    Array UpdateVertexMask(Array mask, Array indices)
    {
        int rows   = indices.GetLength(0);
        int cols   = indices.GetLength(1);
        var result = new bool[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i, j] = (bool)mask.GetValue((int)indices.GetValue(i, j));
        return result;
    }
    /// <summary> triangles[triangle_mask] </summary>
    Array UpdateTriangles(Array arr, Array mask)
    {
        Type elemType = arr.GetType().GetElementType();
        int  cols     = arr.GetLength(1);
        var  validRows = new List<int>();

        for (int i = 0; i < mask.Length; i++)
            if ((bool)mask.GetValue(i)) validRows.Add(i);

        var result = Array.CreateInstance(elemType, validRows.Count, cols);
        for (int i = 0; i < validRows.Count; i++)
        for (int j = 0; j < cols; j++)
            result.SetValue(arr.GetValue(validRows[i], j), i, j);
        return result;
    }
    /// <summary> arr[mask] </summary>
    Array UpdateXYZ(Array arr, Array mask)
    {
        Type elemType = arr.GetType().GetElementType();
        var  result   = new List<object>();

        for (int i = 0; i < arr.Length; i++)
            if ((bool)mask.GetValue(i)) result.Add(arr.GetValue(i));

        var output = Array.CreateInstance(elemType, result.Count);
        for (int i = 0; i < result.Count; i++)
            output.SetValue(result[i], i);
        return output;
    }
    Color[] ToColors(object[] arr)
    {
        if (arr.Length % 4 != 0)
            throw new ArgumentException($"要素数が4の倍数ではありません: {arr.Length}");
        var result = new Color[arr.Length / 4];
        for (int i = 0; i < result.Length; i++)
            result[i] = new Color(
                Convert.ToSingle(arr[i * 4 + 0]),  // R → a
                Convert.ToSingle(arr[i * 4 + 1]),  // G → b
                Convert.ToSingle(arr[i * 4 + 2]),  // B → c
                Convert.ToSingle(arr[i * 4 + 3])   // A → ds
            );
        return result;
    }
}