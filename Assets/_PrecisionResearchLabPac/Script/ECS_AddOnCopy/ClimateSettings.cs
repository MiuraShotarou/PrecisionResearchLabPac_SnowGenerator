using UnityEngine;
using UnityEditor;
using System;
using System.IO;
using System.Collections.Generic;
using System.Collections.Concurrent;
using Cysharp.Threading.Tasks;
using Unity.Burst;
[ExecuteAlways]
public class ClimateSettings : MonoBehaviour
{
    public static ClimateSettings Instance;
    /// <summary> EnableComponent or SceneLoaded </summary>
    private void OnEnable()
    {
        if (Instance == null){
            Instance = this;
            BurstCompiler.Options.EnableBurstCompilation = true;
            Application.runInBackground = true;
        }
        else{
            DestroyImmediate(gameObject);}
    }
    /// <summary> RemoveComponent or DestroyGameObject </summary>
    private void OnDestroy()
    {
        if (Instance == this){
            Instance = null;}
    }
    
    [Header("FieldParameters")]
    [Tooltip("CoverArea")]
    public Vector3 coverArea = new Vector3(100, 500, 100);
    
    [Tooltip("FallAmount")]
    [Range(0f, 100f)]
    public int fallAmount = 10;
    
    private void OnDrawGizmos()
    {
        Gizmos.color = Color.cyan;
        Gizmos.DrawWireCube(transform.position, coverArea);
    }
    
    [Header("IsOnEditor?")]
    public bool IsOnEditor = false;

    //==System==
    // public int CountLimit => (int)(fallAmount * (coverArea.x + coverArea.y + coverArea.z));
    public int CountLimit => fallAmount;
    public ConcurrentQueue<GameObject> SnowflakeQueue = new ConcurrentQueue<GameObject>();
    /// <summary> AddComponent or SceneLoaded to Once</summary>
    private void Awake()
    {
        // GenerateNewAsync().Forget();
    }
    private void Init()
    {
        // foreach (var obj in SnowflakeQueue){
        //     DestroyImmediate(obj);}
        for (int i = 0; i < SnowflakeQueue.Count; i++){
            if (SnowflakeQueue.TryDequeue(out GameObject obj)){
                DestroyImmediate(obj);
            }
        }
    }
    /// <summary> Authoring付きのGameObjectを予め生成する </summary>
    public async UniTask GenerateNewAsync()
    {
        Init();
        var total = System.Diagnostics.Stopwatch.StartNew();
        var tasks = new UniTask[CountLimit];
        for (int i = 0; i < CountLimit; i++)
        {
            // tasks[i] = GenerateSnowflakeAsync(i);
            await GenerateSnowflakeAsync(i);
        }
        // await UniTask.WhenAll(tasks);
        total.Stop();
        Debug.Log($"GenerateNewAsync完了: {total.Elapsed.TotalSeconds}秒");
        // ECSで移動処理
    }
    private async UniTask GenerateSnowflakeAsync(int i)
    {
        // Mesh生成
        var component = new SnowflakeManagedComponent(); //Snowflake毎のパラメータをランダム生成
        var data = new SystemData(component);
        var mesh = new Mesh();
        try
        {
            // 各メソッドの実行
            var steps = 2000; //
            var calculate = new Calculate(data);
            var simulateMesh = new SimulateMesh(mesh);
            await calculate.Execute(steps, component.deltaRho, i);
            data = calculate.GetData;
            await simulateMesh.Execute(data);
            mesh = simulateMesh.GetMesh;
            SaveMesh(mesh);
            component.rho = data.rho;
            component.steps += steps;
            // Entityにcomponentをアタッチ
            // ※dataは破棄して良い

            // Object生成
            var obj = Instantiate(new GameObject("Snowflake_"), gameObject.transform);
            obj.transform.localScale = new Vector3(0.01f, 0.01f, 0.01f);
            obj.AddComponent<MeshFilter>().mesh = mesh;
            obj.AddComponent<MeshRenderer>().sharedMaterial =
                AssetDatabase.LoadAssetAtPath<Material>(
                    "Packages/com.unity.render-pipelines.universal/Runtime/Materials/Lit.mat");
            obj.AddComponent<SnowflakeAuthoring>();
            SnowflakeQueue.Enqueue(obj);
        }
        catch (Exception e)
        {
            Debug.LogError("hoge");
            throw;
        }
        finally
        {
            data.Dispose();
        }
    }
    private void SaveMesh(Mesh mesh)
    {
        // string path = $"Assets/_PrecisionResearchLabPac/Mesh/Snowflake_{DateTime.Now.Ticks}.asset";
        string path = $"Assets/_PrecisionResearchLabPac/Mesh/Snowflake_0.asset";
        Directory.CreateDirectory("Assets/_PrecisionResearchLabPac/Mesh");
        AssetDatabase.CreateAsset(mesh, path);
        AssetDatabase.SaveAssets();
    }
}
