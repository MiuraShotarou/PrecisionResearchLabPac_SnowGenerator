using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEngine;
using UnityEditor;
using pxr;
using UnityEngine.InputSystem;
using Object = UnityEngine.Object;

/// <summary>
/// snowflake.pyの結果をBlenderのメッシュに変換する。OK
/// </summary>
public class Operators
{
    // use Class
    private Icons__Init__ icons;
    private static readonly string USD_PRIM_PATH = "/Snowflake";
    public static SnowflakeParams MakeParams(ObjectProperties props)
    {
        return new SnowflakeParams()
        {
            rho = props.rho,
            beta = props.beta,
            alpha = props.alpha,
            theta = props.theta,
            kappa = props.kappa,
            mu = props.mu,
            gamma = props.gamma,
            sigma = props.sigma,
            // a =  props.a,
            // b =  props.b,
            // c =  props.c,
            // d =  props.d
        };
    }
    public static void RandomizeParams(
        SnowflakeParams @params,
        bool rho      = false,
        bool beta     = false,
        bool alpha    = false,
        bool theta    = false,
        bool kappa    = false,
        bool mu       = false,
        bool gamma    = false,
        bool sigma    = false,
        bool deltaRho = false
    )
    {
        if (rho)      @params.rho      = Mathf.Max(SampleNormal(0.6f,   0.2f),  0f);
        if (beta)     @params.beta     = Mathf.Max(SampleNormal(1.6f,   0.3f),  0f);
        if (alpha)    @params.alpha    = Mathf.Max(SampleNormal(0.15f,  0.07f), 0f);
        if (theta)    @params.theta    = Mathf.Max(SampleNormal(0.02f,  0.02f), 0f);
        if (kappa)    @params.kappa    = Mathf.Max(SampleNormal(0.03f,  0.03f), 0f);
        if (mu)       @params.mu       = Mathf.Max(SampleNormal(0.05f,  0.02f), 0f);
        if (gamma)    @params.gamma    = Mathf.Max(SampleNormal(0.001f, 0.01f), 0f);
        if (sigma)    @params.sigma    = UnityEngine.Random.Range(0f, 1f);
    }
    private (float[,] vertices, int[,] triangles) MakeMeshData(Snowflake snowflake)
    {
        var snowflakeSize = snowflake.size;
        int[,] vertex_indices_old = (int[,])NumpyClass.Reshape(NumpyClass.Arange((int)Math.Pow(snowflakeSize, 2)), snowflakeSize, snowflakeSize);
        var triangles_1 = (int[,])NumpyClass.ColumnStack(new List<Array>{
            NumpyClass.Ravel<int>(vertex_indices_old.Slice(rowStop: -1, colStop: -1)),
            NumpyClass.Ravel<int>(vertex_indices_old.Slice(rowStart: 1, colStop: -1)),
            NumpyClass.Ravel<int>(vertex_indices_old.Slice(rowStop: -1, colStart: 1))
        });
        var triangles_2 = (int[,])NumpyClass.ColumnStack(new List<Array>{
            NumpyClass.Ravel<int>(vertex_indices_old.Slice(rowStart: 1, colStop: -1)),
            NumpyClass.Ravel<int>(vertex_indices_old.Slice(rowStart: 1, colStart: 1)),
            NumpyClass.Ravel<int>(vertex_indices_old.Slice(rowStop: -1, colStart: 1))
        });
        int[,] triangles = (int[,])NumpyClass.VStack(new List<Array> { triangles_1, triangles_2 });
        bool[] vertex_mask = NumpyClass.Ravel<bool>(snowflake.a);
        bool[,] sortMask = (bool[,])UpdateVertexMask(vertex_mask, triangles);
        bool[] triangle_mask = (bool[])sortMask.Reduce((a, b) => (bool)a && (bool)b,
            true, AxisType.Row);//BoolIndexのオーバーライドが良い
        triangles = (int[,])UpdateTriangles(triangles, triangle_mask);
        int[] vertex_indices_new = (int[])NumpyClass.CumSum(vertex_mask);//メソッド名リネームしたいのと - vertex_mask の式を追加したい
        triangles = (int[,])NumpyClass.RemapIndices(vertex_indices_new, triangles); //BoolIndex
        var (x, y) = NumpyClass.Indices(snowflakeSize, snowflakeSize).Cast<float[,]>();
        x = (float[,])NumpyClass.Reshape(NumpyClass.Ravel<float>(x).Select(X => X - snowflakeSize / 2).ToArray(), x.GetLength(0), x.GetLength(1));
        y = (float[,])NumpyClass.Reshape(NumpyClass.Ravel<float>(y).Select(Y => Y - snowflakeSize / 2).ToArray(), y.GetLength(0), y.GetLength(1));
        x = (float[,])NumpyClass.Reshape(NumpyClass.Ravel<float>(x).Zip(NumpyClass.Ravel<float>(y), (X, Y) => X + Y / 2f).ToArray(), x.GetLength(0), x.GetLength(1));
        x = (float[,])NumpyClass.Reshape(NumpyClass.Ravel<float>(x).Select(X => X / Mathf.Cos(Mathf.Deg2Rad * 30f)).ToArray(), y.GetLength(0), y.GetLength(1));
        var z = (float[,])NumpyClass.Where(
            (bool[,])NumpyClass.LogicalAnd(snowflake.a, (bool[,])NumpyClass.Reshape(
                NumpyClass.Ravel<float>(snowflake.NbSum(
                        (int[,])NumpyClass.Reshape(
                            NumpyClass.Ravel<bool>(snowflake.a).Select(a => Convert.ToInt32(!a)).ToArray(),
                            snowflake.a.GetLength(0), snowflake.a.GetLength(1)),
                        0))
                    .Select(v => v > 0).ToArray(),
                snowflake.a.GetLength(0), snowflake.a.GetLength(1))),
            0f, snowflake.c);
        var vertices = (float[,])NumpyClass.ColumnStack(new List<Array>
        {
            NumpyClass.MaskFilter(NumpyClass.Ravel<float>(x), vertex_mask),
            NumpyClass.MaskFilter(NumpyClass.Ravel<float>(y), vertex_mask),
            NumpyClass.MaskFilter(NumpyClass.Ravel<float>(z), vertex_mask)
        });
        return (vertices, triangles);
    }
    /// <summary> ShadeAutoSmoothを内部で実行するようにした </summary>
    private Mesh MakeMesh(Snowflake snowflake)
    {
        var meshData = MakeMeshData(snowflake);
        meshData = ShadeAutoSmooth(meshData.vertices, meshData.triangles);
        var vertices  = meshData.vertices;
        var triangles = meshData.triangles;
        Mesh mesh = new Mesh();
        mesh.indexFormat = UnityEngine.Rendering.IndexFormat.UInt32;
        mesh.SetVertices(Enumerable.Range(0, vertices.GetLength(0))
            .Select(i => new Vector3(vertices[i, 0], vertices[i, 1], vertices[i, 2]))
            .ToList());
        mesh.SetTriangles(NumpyClass.Ravel<int>(triangles).ToList(), 0);
        mesh.RecalculateNormals(); //ポリゴンの頂点が共有されていればここでスムーズシェードの機能が使えるらしい
        mesh.RecalculateBounds();
        return mesh;
    }
    /// <summary> 事実上a,b,c,dの初期化 </summary>
    private Texture2D MakeImage(Snowflake snowflake)
    {
        var texture2D = new Texture2D(
            snowflake.size,
            snowflake.size,
            TextureFormat.RGBAFloat,
            true,
            true
        );
        texture2D.name = "Snowflake";
        texture2D.SetPixels(ToColors(
            NumpyClass.Ravel<object>(NumpyClass.Stack(
                new List<Array>{(float[,])NumpyClass.Reshape(NumpyClass.Ravel<bool>(snowflake.a).Select(v => v? 1f:0f).ToArray(), snowflake.a.GetLength(0), snowflake.a.GetLength(1)),
                    snowflake.b, snowflake.c, snowflake.d},
                AxisType.None))));
        texture2D.Apply();
        return texture2D;
    }

    private (float[,], int[,]) ShadeAutoSmooth(float[,] vertices, int[,] triangles)
    {//use_auto_smooth = True にするだ
        return ApplyMirror(vertices, triangles);
    }

    class AnimationExportError : IOException {public AnimationExportError(string message) : base(message) { } }

    public void SaveToUsd(Snowflake snowflake, string filepath, Matrix4x4 matrixWorld, int frame)
    {
        try
        {
            var assembly = System.Reflection.Assembly.Load("com.unity.formats.usd");
            if (string.IsNullOrEmpty(filepath)){
                throw new AnimationExportError("Invalid empty filepath");}
            filepath = Path.GetFullPath(filepath);
            if (Directory.Exists(filepath)){
                throw new AnimationExportError($"Path {filepath} is a directory");}
            if (!new[] { ".usd", ".usda", ".usdc" }.Contains(Path.GetExtension(filepath))){
                throw new AnimationExportError("File extension must be ‘.usd’, ‘.usda’, or ‘.usdc’");}
            // if filepath.exists() and not filepath.is_file raise AnimationExportError(f"Path ‘{filepath}’ is not a writable file")
            var usdStage = File.Exists(filepath)? 
                UsdStage.Open(filepath):
                UsdStage.CreateNew(filepath);
            usdStage.SetStartTimeCode(Math.Min(frame, usdStage.GetStartTimeCode()));
            usdStage.SetEndTimeCode(Math.Max(frame, usdStage.GetEndTimeCode()));
            usdStage.SetMetadata(new TfToken("comment"), "Generated by Snowflake add-on for Unity");
            UsdGeom.UsdGeomSetStageUpAxis(usdStage, UsdGeomTokens.y);
            
            if (frame == 0){
                usdStage.RemovePrim(USD_PRIM_PATH);} //USDファイル内からUSD_PRIM_PATHパスのオブジェクトを削除 USD_MESH へ代入するところで該当のオブジェクトが追加されている
            
            var usdMesh = new UsdGeomMesh();
            try{
                usdMesh = UsdGeomMesh.Define(usdStage, USD_PRIM_PATH);}
            catch (Exception){
                usdMesh = UsdGeomMesh.Get(usdStage, USD_PRIM_PATH);}
            usdMesh.GetSubdivisionSchemeAttr().Set("none");

            var usdMatrix = new GfMatrix4d(matrixWorld.m00, matrixWorld.m01, matrixWorld.m02, matrixWorld.m03,
                            matrixWorld.m10, matrixWorld.m11, matrixWorld.m12, matrixWorld.m13,
                            matrixWorld.m20, matrixWorld.m21, matrixWorld.m22, matrixWorld.m23,
                            matrixWorld.m30, matrixWorld.m31, matrixWorld.m32, matrixWorld.m33);
            var usdXfromOps = usdMesh.GetOrderedXformOps(out _);
            if (usdXfromOps != null && usdXfromOps.Count > 0){
                usdXfromOps[0].Set(usdMatrix);}
            else{
                usdMesh.AddTransformOp().Set(usdMatrix);}
            var usdPoints = usdMesh.CreatePointsAttr();
            var usdFaceVertexCounts = usdMesh.CreateFaceVertexCountsAttr();
            var usdFaceVertexIndices = usdMesh.CreateFaceVertexIndicesAttr();

            var (vertices, triangles) = MakeMeshData(snowflake);
            var faceVertexCounts = (int[])NumpyClass.Full(NumpyClass.Shape(triangles)[0], 3);
            var faceVertexIndices = NumpyClass.Ravel<int>(triangles);

            var vtVertices = new VtValue(ToVtVec3fArray(vertices));
            var vtCounts = new VtValue(ToVtIntArray(faceVertexCounts));
            var vtIndices = new VtValue(ToVtIntArray(faceVertexIndices));

            if (frame == 0){
                usdPoints.Set(vtVertices);
                usdFaceVertexCounts.Set(vtCounts);
                usdFaceVertexIndices.Set(vtIndices);}

            var usdFrame = new UsdTimeCode(frame);
            usdPoints.Set(vtVertices, usdFrame);
            usdFaceVertexCounts.Set(vtCounts, usdFrame);
            usdFaceVertexIndices.Set(vtIndices, usdFrame);
            
            usdStage.Save();
            usdStage.Unload();
        }
        catch (Exception e){
            throw new AnimationExportError(e.Message);}
    }
    
    class AnimationImportError : IOException {public AnimationImportError(string message) : base(message) { } }

    public static void ApplyUsdMeshCache(string filepath, ObjectProperties properties) //+ context
    {
        try
        {
            var modifierName = "Snowflake Anim";
            var obj = properties.gameObject;
            var modifier = obj.GetComponent<Modifier>();
            
            if (string.IsNullOrEmpty(filepath)){
                if (modifier != null){ //モディファイアがあるならば
                    Object.DestroyImmediate(modifier, true);}
                return; //filepathが空であれば返してしまう
            }
            if (modifier == null)//モディファイアが無いのなら
            {
                modifier = obj.AddComponent<Modifier>();
                //モディファイアを新規作成して貼り付ける
            }
            // すべてのモディファイア内で一番最初に処理が走るよう設定する（ScriptExecutionOrder）
            
            //Claudeの指示通りにコーディング
            AssetDatabase.ImportAsset(filepath);
            modifier.CachePath = filepath;
            modifier.ObjectPath = USD_PRIM_PATH;
        }
        catch (Exception e){
            throw new AnimationImportError(e.Message);}
    }

    public static void Simulate(Snowflake snowflake, int steps, float deltaRho, Action<int> callback) //function（関数）も引数に存在している → Action<int>で代用できる。
    {
        for (int step = 0; step < steps; step++)
        {
            Debug.Log(step);
            snowflake.Step();
            if (deltaRho != 0f){
                snowflake.Params.rho *= 1 + deltaRho;}
            //Actionの実行
            callback(step + 1); //ここ
        }
    }

    /// <summary> 利用しない </summary>
    public static object ObjectProperty(ObjectProperties properties, string name)
    {
        var annotation = properties.__annotations__[name];
        return annotation;
    }

    public class SnowflakeCreate //コンストラクタに bpy.types.Operator, object_utils.AddObjectHelper
    {
        private GameObject MakeObject()
        {
            var obj = new GameObject("Snowflake");
            if (obj.GetComponent<MeshFilter>() == null){
                obj.AddComponent<MeshFilter>().mesh = new Mesh(); }

            if (obj.GetComponent<MeshRenderer>() == null){
                obj.AddComponent<MeshRenderer>();}

            if (obj.GetComponent<MeshRenderer>().sharedMaterial == null){
                obj.GetComponent<MeshRenderer>().sharedMaterial = AssetDatabase.LoadAssetAtPath<Material>("Packages/com.unity.render-pipelines.universal/Runtime/Materials/Lit.mat");
            }

            if (obj.GetComponent<ObjectProperties>() == null){
                obj.AddComponent<ObjectProperties>();}

            if (obj.GetComponent<SnowflakePanel>() == null){
                obj.AddComponent<SnowflakePanel>();}
            
            return obj;
        }
        public void Execute(ObjectProperties properties)
        {
            var @params = MakeParams(properties);
            RandomizeParams(
                @params, //インスペクターから右辺のboolを設定できる必要がある。
            // rho:   properties.randomRho,
            rho:   properties.randomRho,
            beta:  properties.randomBeta,
            alpha: properties.randomAlpha,
            theta: properties.randomTheta,
            kappa: properties.randomKappa,
            mu:    properties.randomMu,
            gamma: properties.randomGamma,
            sigma: properties.randomSigma
            );
            RandomizeParams(
                @params,
                deltaRho: properties.randomDeltaRho
            );
            // var snowflake = new Snowflake(@params); //a,b,c,d の初期値を作成
            // SnowflakeParamsを代入しているだけなので改変
            // var mesh = new Operators().MakeMesh(snowflake); //不要
            // mesh = new Operators().ShadeAutoSmooth(mesh); //+ context
            var obj = MakeObject(); //+ context
            var objSnowflake = obj.GetComponent<ObjectProperties>();
            int size = 11;
            // objSnowflake.data              = new Operators().MakeImage(snowflake);
            objSnowflake.rho               = @params.rho;
            objSnowflake.beta              = @params.beta;
            objSnowflake.alpha             = @params.alpha;
            objSnowflake.theta             = @params.theta;
            objSnowflake.kappa             = @params.kappa;
            objSnowflake.mu                = @params.mu;
            objSnowflake.gamma             = @params.gamma;
            objSnowflake.sigma             = @params.sigma;
            objSnowflake.deltaRho          = properties.deltaRho;
            objSnowflake.exportAnimation   = properties.exportAnimation;
            objSnowflake.animationFilepath = properties.animationFilepath;
            objSnowflake.applyAnimation    = properties.applyAnimation;
            objSnowflake.stepsPerFrame     = properties.stepsPerFrame;
            objSnowflake.growing           = false;
            objSnowflake.steps             = 0;
            objSnowflake.a = new bool[size,size];
            objSnowflake.b = new float[size,size];
            objSnowflake.c = new float[size,size];
            objSnowflake.d = new float[size,size];
            for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                objSnowflake.d[i, j] = 1f;
            int mid = size / 2;
            objSnowflake.a[mid, mid] = true;
            objSnowflake.c[mid, mid] = 1f;
            objSnowflake.d[mid, mid] = 0f;
            new SnowflakeGrow().Execute(objSnowflake, properties.steps); // Grow();
            // return ["FINISHED"]
        }
    }

    public class SnowflakeGrow
    {
        private int steps = 100;
        /// <summary> OnUpdateFlameCall </summary>
        public void Execute(ObjectProperties properties, int steps = -1)
        {
            steps = steps == -1? this.steps : steps;
            int size = properties.a.GetLength(0);
            var snowflake = new Snowflake(properties, MakeParams(properties));
            Action<int> callback = step => //Simulateから呼ばれている
            {
                EditorUtility.DisplayProgressBar("Simulating", "Step 0", step);
            };

            try
            {
                Simulate(snowflake, steps, properties.deltaRho, callback);
            }
            catch (AnimationExportError e)
            {
                Debug.LogError($"Animation export failed: {e.Message}");
            }
            finally
            {
                EditorUtility.ClearProgressBar();
            }
            var mesh = new Operators().MakeMesh(snowflake);
            if (properties.gameObject && properties.gameObject.GetComponent<MeshFilter>() != null){
                properties.gameObject.GetComponent<MeshFilter>().mesh = mesh; }
            // MyCode
            properties.a = snowflake.a;
            properties.b = snowflake.b;
            properties.c = snowflake.c;
            properties.d = snowflake.d;
            properties.rho = snowflake.Params.rho;
            properties.steps += steps;
        }
    }
    /// <summary>
    /// ※Invokeを最初に呼ぶこと
    /// </summary>
    public class SnowflakeGrowModal
    {
        private int steps;
        public void Modal(ObjectProperties properties)
        {
            if (Keyboard.current.escapeKey.wasPressedThisFrame || Mouse.current.rightButton.wasPressedThisFrame){
                Cancel(properties);
                return;}

            if (!properties.growing){
                return;}

            var start = Time.realtimeSinceStartup;
            properties.growing = false;
            new SnowflakeGrow().Execute(properties, this.steps); //ObjectPropertiesを書き換える処理
            properties.growing = true;
            var stop = Time.realtimeSinceStartup;

            if (stop - start > 0.12f && this.steps > 1){
                this.steps--;}
            else if (stop - start < 0.08f){
                this.steps++;}
        }
        public void Invoke(ObjectProperties properties){
            properties.growing = true;
            steps = 10;
        }
        public void Cancel(ObjectProperties properties){
            properties.growing = false;
        }
    }

    class SnowflakeRandomize
    {
        void Execute(ObjectProperties properties)
        {
            RandomizeParams(
                new SnowflakeParams(),
                rho:      properties.randomRho,
                beta:     properties.randomBeta,
                alpha:    properties.randomAlpha,
                theta:    properties.randomTheta,
                kappa:    properties.randomKappa,
                mu:       properties.randomMu,
                gamma:    properties.randomGamma,
                sigma:    properties.randomSigma,
                deltaRho: properties.randomDeltaRho
            );
        }
    }

    //両方使わない
    void Register() {}
    void UnRegister() {}

    private static float SampleNormal(float mean, float stddev)
    {
        float u1 = 1f - UnityEngine.Random.value;
        float u2 = 1f - UnityEngine.Random.value;
        float z  = Mathf.Sqrt(-2f * Mathf.Log(u1))
                 * Mathf.Cos(2f * Mathf.PI * u2);
        return mean + stddev * z;
    }
    
    private (float[,], int[,]) ApplyMirror(float[,] vertices, int[,] triangles)
    {
        int vertRow      = vertices.GetLength(0);
        // vertCol == 3
        int triangleRow  = triangles.GetLength(0);
        int triangleCol  = triangles.GetLength(1);
        
        var expVertices  = new float[vertRow * 2, 3];
        var expTriangles = new int[triangleRow * 2, triangleCol];
        // 元の頂点をそのままコピー
        for (int i = 0; i < vertRow; i++){
            expVertices[i, 0] = vertices[i, 0];
            expVertices[i, 1] = vertices[i, 1];
            expVertices[i, 2] = vertices[i, 2];
        }
        // ミラーした頂点を追加
        for (int i = 0; i < vertRow; i++){
            expVertices[vertRow + i, 0] =  vertices[i, 0];
            expVertices[vertRow + i, 1] =  vertices[i, 1];
            expVertices[vertRow + i, 2] = -vertices[i, 2];
        }
        // 元のトライアングルをそのままコピー
        for (int i = 0; i < triangleRow; i++)
            for (int j = 0; j < triangleCol; j++){
            expTriangles[i, j] = triangles[i, j];
        }
        // ミラーしたトライアングルを追加（巻き順逆）
        for (int i = 0; i < triangleRow; i++){
            expTriangles[triangleRow + i, 0] = triangles[i, 0] + vertRow;
            expTriangles[triangleRow + i, 1] = triangles[i, 2] + vertRow;
            expTriangles[triangleRow + i, 2] = triangles[i, 1] + vertRow;
        }
        return (expVertices, expTriangles);
    }

    public static void SaveMesh(Mesh mesh, string name)
    {
        string path = $"Assets/_PrecisionResearchLabPac/Mesh/Snowflake_{DateTime.Now.Ticks}.asset";
        Directory.CreateDirectory("Assets/_PrecisionResearchLabPac/Mesh");
        AssetDatabase.CreateAsset(mesh, path);
        AssetDatabase.SaveAssets();
    }

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
        Type elemType  = arr.GetType().GetElementType();
        int  cols      = arr.GetLength(1);
        var  validRows = new List<int>();
        for (int i = 0; i < mask.Length; i++)
            if ((bool)mask.GetValue(i)) validRows.Add(i);
        var result = Array.CreateInstance(elemType, validRows.Count, cols);
        for (int i = 0; i < validRows.Count; i++)
        for (int j = 0; j < cols; j++)
            result.SetValue(arr.GetValue(validRows[i], j), i, j);
        return result;
    }

    Color[] ToColors(object[] arr)
    {
        var result = new Color[arr.Length / 4];
        for (int i = 0; i < result.Length; i++){
            result[i] = new Color(
                Convert.ToSingle(arr[i * 4 + 0]), // R → a
                Convert.ToSingle(arr[i * 4 + 1]), // G → b
                Convert.ToSingle(arr[i * 4 + 2]), // B → c
                Convert.ToSingle(arr[i * 4 + 3]) // A → d
            );
        }
        return result;
    }
    
    VtVec3fArray ToVtVec3fArray(float[,] arr)
    {
        var result = new VtVec3fArray((uint)arr.GetLength(0));
        for (int i = 0; i < arr.GetLength(0); i++)
            result[i] = new GfVec3f(arr[i, 0], arr[i, 1], arr[i, 2]);
        return result;
    }
    VtIntArray ToVtIntArray(int[] arr)
    {
        var result = new VtIntArray((uint)arr.Length);
        for (int i = 0; i < arr.Length; i++)
            result[i] = arr[i];
        return result;
    }
    public static float[,,] ToFloat3D(Color[] pixels)
    {
        int size   = (int)Mathf.Sqrt(pixels.Length);
        var result = new float[size, size, 4];
        for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            int idx = i * size + j;
            result[i, j, 0] = pixels[idx].r;
            result[i, j, 1] = pixels[idx].g;
            result[i, j, 2] = pixels[idx].b;
            result[i, j, 3] = pixels[idx].a;
        }
        return result;
    }
}
