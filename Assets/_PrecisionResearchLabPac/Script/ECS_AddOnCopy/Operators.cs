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
    public (float[,] vertices, int[,] triangles) MakeMeshData(Snowflake snowflake)
    {
        int[,] vertex_indices_old = (int[,])Numpy.Reshape(Numpy.Arange((int)Math.Pow(snowflake.size, 2)), snowflake.size, snowflake.size);
        var triangles_1 = (int[,])Numpy.ColumnStack(new List<Array>{
            Numpy.Ravel<int>(vertex_indices_old.Slice(rowStop: -1, colStop: -1)),
            Numpy.Ravel<int>(vertex_indices_old.Slice(rowStart: 1, colStop: -1)),
            Numpy.Ravel<int>(vertex_indices_old.Slice(rowStop: -1, colStart: 1))
        });
        var triangles_2 = (int[,])Numpy.ColumnStack(new List<Array>{
            Numpy.Ravel<int>(vertex_indices_old.Slice(rowStart: 1, colStop: -1)),
            Numpy.Ravel<int>(vertex_indices_old.Slice(rowStart: 1, colStart: 1)),
            Numpy.Ravel<int>(vertex_indices_old.Slice(rowStop: -1, colStart: 1))
        });
        int[,] triangles = (int[,])Numpy.VStack(new List<Array> { triangles_1, triangles_2 });
        bool[] vertex_mask = Numpy.Ravel<bool>(snowflake.a);
        bool[,] sortMask = (bool[,])UpdateVertexMask(vertex_mask, triangles);
        bool[] triangle_mask = (bool[])sortMask.Reduce((a, b) => (bool)a && (bool)b,
            true, AxisType.Row);//BoolIndexのオーバーライドが良い
        triangles = (int[,])UpdateTriangles(triangles, triangle_mask);
        int[] vertex_indices_new = (int[])Numpy.CumSum(vertex_mask);//メソッド名リネームしたいのと - vertex_mask の式を追加したい
        triangles = (int[,])Numpy.RemapIndices(vertex_indices_new, triangles); //BoolIndex
        var (x, y) = Numpy.Indices(snowflake.size, snowflake.size).Cast<float[,]>();
        x = (float[,])Numpy.Reshape(Numpy.Ravel<float>(x).Select(X => X - snowflake.size / 2).ToArray(), x.GetLength(0), x.GetLength(1));
        y = (float[,])Numpy.Reshape(Numpy.Ravel<float>(y).Select(Y => Y - snowflake.size / 2).ToArray(), y.GetLength(0), y.GetLength(1));
        x = (float[,])Numpy.Reshape(Numpy.Ravel<float>(x).Zip(Numpy.Ravel<float>(y), (X, Y) => X + Y / 2f).ToArray(), x.GetLength(0), x.GetLength(1));
        x = (float[,])Numpy.Reshape(Numpy.Ravel<float>(x).Select(X => X / Mathf.Cos(Mathf.Deg2Rad * 30f)).ToArray(), y.GetLength(0), y.GetLength(1));
        var z = (float[,])Numpy.Where(
            (bool[,])Numpy.LogicalAnd(snowflake.a, (bool[,])Numpy.Reshape(
                Numpy.Ravel<float>(snowflake.NbSum(
                        (int[,])Numpy.Reshape(
                            Numpy.Ravel<bool>(snowflake.a).Select(a => Convert.ToInt32(!a)).ToArray(),
                            snowflake.a.GetLength(0), snowflake.a.GetLength(1)),
                        0))
                    .Select(v => v > 0).ToArray(),
                snowflake.a.GetLength(0), snowflake.a.GetLength(1))),
            0f, snowflake.c);
        var vertices = (float[,])Numpy.ColumnStack(new List<Array>
        {
            Numpy.MaskFilter(Numpy.Ravel<float>(x), vertex_mask),
            Numpy.MaskFilter(Numpy.Ravel<float>(y), vertex_mask),
            Numpy.MaskFilter(Numpy.Ravel<float>(z), vertex_mask)
        });
        return (vertices, triangles);
    }
    public Mesh MakeMesh(Snowflake snowflake)
    {
        var meshData = MakeMeshData(snowflake);
        var vertices  = meshData.vertices;
        var triangles = meshData.triangles;
        Mesh mesh = new Mesh();
        mesh.indexFormat = UnityEngine.Rendering.IndexFormat.UInt32;
        mesh.SetVertices(Enumerable.Range(0, vertices.GetLength(0))
            .Select(i => new Vector3(vertices[i, 0], vertices[i, 1], vertices[i, 2]))
            .ToList());
        mesh.SetTriangles(Numpy.Ravel<int>(triangles).ToList(), 0);
        mesh.RecalculateNormals(); //ポリゴンの頂点が共有されていればここでスムーズシェードの機能が使えるらしい
        mesh.RecalculateBounds();
        return mesh;
    }
    public Texture2D MakeImage(Snowflake snowflake)
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
            Numpy.Ravel<object>(Numpy.Stack(
                new List<Array>{(float[,])Numpy.Reshape(Numpy.Ravel<bool>(snowflake.a).Select(v => v? 1f:0f).ToArray(), snowflake.a.GetLength(0), snowflake.a.GetLength(1)),
                    snowflake.b, snowflake.c, snowflake.d},
                AxisType.None))));
        texture2D.Apply();
        return texture2D;
    }

    public Mesh ShadeAutoSmooth(Mesh mesh)
    {//use_auto_smooth = True にするだ
        return ApplyMirror(mesh);
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
            var faceVertexCounts = (int[])Numpy.Full(Numpy.Shape(triangles)[0], 3);
            var faceVertexIndices = Numpy.Ravel<int>(triangles);

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
            snowflake.Step();
            if (deltaRho != 0f){
                snowflake.Params.rho *= 1 + deltaRho;}
            //Actionの実行
            callback(step + 1); //ここ
        }
    }
    public static object ObjectProperty(ObjectProperties properties, string name)
    {
        var annotation = properties.__annotations__[name];
        return annotation;
    }

    public class SnowflakeCreate //コンストラクタに bpy.types.Operator, object_utils.AddObjectHelper
    {
        //UIと内部プロパティの紐づけ処理は省略
        //poll メソッドも省略
        //drawも省略
        public GameObject MakeObject(Mesh mesh)
        {
            var obj = new GameObject("Snowflake");
            if (obj.GetComponent<MeshFilter>() == null){
                obj.AddComponent<MeshFilter>().mesh = mesh; }

            if (obj.GetComponent<MeshRenderer>() == null){
                obj.AddComponent<MeshRenderer>();}

            if (obj.GetComponent<MeshRenderer>().sharedMaterial == null){
                obj.GetComponent<MeshRenderer>().sharedMaterial = AssetDatabase.LoadAssetAtPath<Material>("Packages/com.unity.render-pipelines.universal/Runtime/Materials/Lit.mat");
            }

            if (obj.GetComponent<ObjectProperties>() == null){
                obj.AddComponent<ObjectProperties>();}

            if (obj.GetComponent<SnowflakePanel>() == null){
                obj.AddComponent<SnowflakePanel>();}
            
            //scaleの設定は無視
            //※そのうちコーディング
            //modifiers.new(Mirror)
            //modifiers.useAxis = [false, false, true] //どの軸にミラーを適用させるのか
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
            var snowflake = new Snowflake(@params);
            var mesh = new Operators().MakeMesh(snowflake);
            mesh = new Operators().ShadeAutoSmooth(mesh); //+ context
            var obj = MakeObject(mesh); //+ context
            var objSnowflake = obj.GetComponent<ObjectProperties>(); //pyの方はsnowflakeという名前だが、型はこれで大丈
            objSnowflake.data              = new Operators().MakeImage(snowflake);
            objSnowflake.rho               = snowflake.Params.rho;
            objSnowflake.beta              = snowflake.Params.beta;
            objSnowflake.alpha             = snowflake.Params.alpha;
            objSnowflake.theta             = snowflake.Params.theta;
            objSnowflake.kappa             = snowflake.Params.kappa;
            objSnowflake.mu                = snowflake.Params.mu;
            objSnowflake.gamma             = snowflake.Params.gamma;
            objSnowflake.sigma             = snowflake.Params.sigma;
            objSnowflake.deltaRho          = properties.deltaRho;
            objSnowflake.exportAnimation   = properties.exportAnimation;
            objSnowflake.animationFilepath = properties.animationFilepath;
            objSnowflake.applyAnimation    = properties.applyAnimation;
            objSnowflake.stepsPerFrame     = properties.stepsPerFrame;
            objSnowflake.growing           = false;
            objSnowflake.steps             = 0;
            new SnowflakeGrow().Execute(objSnowflake, properties.steps); // Grow();
            // return ["FINISHED"]
        }
    }

    public class SnowflakeGrow
    {
        public int steps = 100;
        public void Execute(ObjectProperties properties, int steps = -1)
        {
            steps = steps == -1? this.steps : steps;
            var snowflakeProps = properties;
            var (size, _) = (snowflakeProps.data.width, snowflakeProps.data.height);
            var snowflake = new Snowflake(MakeParams(snowflakeProps), size: size);
            var data = ToFloat3D(snowflakeProps.data.GetPixels());
            snowflake.a = (bool[,])Numpy.Reshape(Numpy.Ravel<float>(data.Slice(channel: 0)).Select(v => v != 0f).ToArray(),
                                                                                    data.GetLength(0), data.GetLength(1));
            snowflake.b = (float[,])data.Slice(channel: 1);
            snowflake.c = (float[,])data.Slice(channel: 2);
            snowflake.d = (float[,])data.Slice(channel: 3);
            var startSteps = snowflakeProps.steps;
            var stepsPerFrame = snowflakeProps.stepsPerFrame;
            var frame = startSteps / stepsPerFrame;
            var usdFilePath = snowflakeProps.exportAnimation? snowflakeProps.animationFilepath : null;
            Action<int> callback = step => //Simulateから呼ばれている
            {
                EditorUtility.DisplayProgressBar("Simulating", "Step 0", step);
                if (!string.IsNullOrEmpty(usdFilePath) && (startSteps + step) % stepsPerFrame == 0)
                {
                    frame = (startSteps + step) / stepsPerFrame;
                    new Operators().SaveToUsd(snowflake,
                        usdFilePath,
                        properties.gameObject.transform.localToWorldMatrix,
                        frame);}
            };

            try
            {
                if (!string.IsNullOrEmpty(usdFilePath) && frame == 0)
                {
                    new Operators().SaveToUsd(snowflake, usdFilePath,
                        properties.gameObject.transform.localToWorldMatrix, frame);
                }
                
                Simulate(snowflake, steps, snowflakeProps.deltaRho, callback
                );
            }
            catch (AnimationExportError e)
            {
                Debug.LogError($"Animation export failed: {e.Message}");
            }
            finally
            {
                EditorUtility.ClearProgressBar(); //追加
            }
            EditorUtility.ClearProgressBar();
            var mesh = new Operators().MakeMesh(snowflake);
            mesh = new Operators().ShadeAutoSmooth(mesh);
            if (properties.gameObject && properties.gameObject.GetComponent<MeshFilter>() != null){
                properties.gameObject.GetComponent<MeshFilter>().mesh = mesh; }
            if (snowflake.size == size){
                data.SetChannel(0, snowflake.a);
                data.SetChannel(1, snowflake.b);
                data.SetChannel(2, snowflake.c);
                data.SetChannel(3, snowflake.d);
                
                // properties.data.SetPixelData(Numpy.Ravel<float>(data), 0); //失敗例
                properties.data.SetPixels(new Operators().ToColors(Numpy.Ravel<object>(data)));
                properties.data.Apply();}
            else{
                #if UNITY_EDITOR
                Object.DestroyImmediate(snowflakeProps.data);
                #endif
                snowflakeProps.data = null;
                snowflakeProps.data = new Operators().MakeImage(snowflake);}
            snowflakeProps.rho = snowflake.Params.rho;
            snowflakeProps.steps += steps;

            if (!snowflakeProps.applyAnimation){
                usdFilePath = null;}
            try
            {
                ApplyUsdMeshCache(usdFilePath, properties);
            }
            catch (AnimationImportError e)
            {
                Debug.LogWarning($"Animation import failed: {e.Message}");
            }
            if (!string.IsNullOrEmpty(usdFilePath)){
                EditorApplication.delayCall += () => { EditorWindow.GetWindow<AnimationWindow>().time = frame;} ;}
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
    void UnRegister() { }

    private static float SampleNormal(float mean, float stddev)
    {
        float u1 = 1f - UnityEngine.Random.value;
        float u2 = 1f - UnityEngine.Random.value;
        float z  = Mathf.Sqrt(-2f * Mathf.Log(u1))
                 * Mathf.Cos(2f * Mathf.PI * u2);
        return mean + stddev * z;
    }
    
    private Mesh ApplyMirror(Mesh mesh)
    {
        if (mesh == null) return mesh;
        var srcVertices  = mesh.vertices;
        var srcTriangles = mesh.triangles;
        var srcNormals   = mesh.normals;
        var newVertices  = new Vector3[srcVertices.Length * 2];
        var newNormals   = new Vector3[srcNormals.Length * 2];
        var newTriangles = new int[srcTriangles.Length * 2];
        // 元の頂点をそのままコピー
        for (int i = 0; i < srcVertices.Length; i++)
        {
            newVertices[i] = srcVertices[i];
            newNormals[i]  = srcNormals[i];
        }
        // ミラーした頂点を追加
        for (int i = 0; i < srcVertices.Length; i++)
        {
            var v = srcVertices[i];
            newVertices[srcVertices.Length + i] = new Vector3(
                v.x,
                v.y,
                -v.z
            );
            var n = srcNormals[i];
            newNormals[srcNormals.Length + i] = new Vector3(
                n.x,
                n.y,
                -n.z
            );
        }
        // 元のトライアングルをそのままコピー
        for (int i = 0; i < srcTriangles.Length; i++)
            newTriangles[i] = srcTriangles[i];
        // ミラーしたトライアングル（Index）を追加（巻き順を逆にする）
        for (int i = 0; i < srcTriangles.Length; i += 3)
        {
            int offset = srcTriangles.Length; //36
            int vOffset = srcVertices.Length; //24
            //srcTriangles頂点Index
            newTriangles[offset + i + 0] = srcTriangles[i + 0] + vOffset;
            newTriangles[offset + i + 1] = srcTriangles[i + 2] + vOffset; // 巻き順を逆に
            newTriangles[offset + i + 2] = srcTriangles[i + 1] + vOffset;
        }
        var mirroredMesh = new Mesh();
        mirroredMesh.vertices  = newVertices;
        mirroredMesh.triangles = newTriangles;
        mirroredMesh.normals   = newNormals;
        mirroredMesh.RecalculateBounds();
        return mirroredMesh;
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
        if (arr.Length % 4 != 0)
            throw new ArgumentException($"要素数が4の倍数ではありません: {arr.Length}");
        var result = new Color[arr.Length / 4];
        for (int i = 0; i < result.Length; i++)
            result[i] = new Color(
                Convert.ToSingle(arr[i * 4 + 0]),  // R → a
                Convert.ToSingle(arr[i * 4 + 1]),  // G → b
                Convert.ToSingle(arr[i * 4 + 2]),  // B → c
                Convert.ToSingle(arr[i * 4 + 3])   // A → d
            );
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
        int size   = (int)Mathf.Sqrt(pixels.Length);  // √(length) = 一辺のサイズ
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
