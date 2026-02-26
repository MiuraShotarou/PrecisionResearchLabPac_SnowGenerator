using System.Collections;
using System.Collections.Generic;
using Unity.Entities;
using Unity.Mathematics;
using Unity.Transforms;
using UnityEngine;

/// <summary>
/// MonoBehaviourとECSの橋渡し。
/// 結晶を事前生成してECS Entityとして登録する。
/// </summary>
public class SnowManagedBootstrapper : MonoBehaviour
{
    [Header("参照")]
    [SerializeField] private Material _snowMaterial;

    [Header("結晶設定")]
    [SerializeField] private int   _snowCount   = 200;
    [SerializeField] private int   _maxSteps    = 1000;
    [SerializeField] private float _scale       = 0.01f;

    [Header("降雪設定")]
    [SerializeField] private float _spawnHeight = 13f;
    [SerializeField] private float _spawnRange  = 20f;
    [SerializeField] private float _fallSpeed   = 1f;
    [SerializeField] private float _despawnY    = -5f;
    [SerializeField] private float _spawnRate   = 0.1f;  // 生成間隔（秒）

    private void Start()
    {
        StartCoroutine(SpawnRoutine());
    }

    private IEnumerator SpawnRoutine()
    {
        var world = World.DefaultGameObjectInjectionWorld;
        var em    = world.EntityManager;

        for (int i = 0; i < _snowCount; i++)
        {
            // 1結晶ごとにユニークなメッシュを生成
            var core = new SnowflakeCore(_maxSteps, _scale);
            core.RandomizeParams();
            core.RunAll();
            Mesh mesh = core.BuildMesh();

            // GameObjectを生成してメッシュを割り当てる
            var go = new GameObject($"Snowflake_{i}");
            var mf = go.AddComponent<MeshFilter>();
            var mr = go.AddComponent<MeshRenderer>();
            mf.mesh     = mesh;
            mr.material = _snowMaterial;

            // ランダムな初期位置・回転
            go.transform.position = new Vector3(
                UnityEngine.Random.Range(-_spawnRange, _spawnRange),
                _spawnHeight,
                UnityEngine.Random.Range(-_spawnRange, _spawnRange)
            );
            go.transform.rotation = UnityEngine.Random.rotation;

            // ECS Entityを生成してコンポーネントを付与
            Entity entity = em.CreateEntity();

            em.AddComponentData(entity, new LocalTransform
            {
                Position = new float3(
                    go.transform.position.x,
                    go.transform.position.y,
                    go.transform.position.z),
                Rotation = new quaternion(
                    go.transform.rotation.x,
                    go.transform.rotation.y,
                    go.transform.rotation.z,
                    go.transform.rotation.w),
                Scale = 1f,
            });

            em.AddComponentData(entity, new SnowFallData
            {
                FallSpeed  = _fallSpeed  * UnityEngine.Random.Range(0.7f, 1.3f),
                SwaySpeed  = UnityEngine.Random.Range(0.3f,  1.0f),
                SwayAmount = UnityEngine.Random.Range(0.01f, 0.05f),
                SwayOffset = UnityEngine.Random.Range(0f, math.PI * 2f),
                RotSpeed   = UnityEngine.Random.Range(-30f, 30f),
                DespawnY   = _despawnY,
            });

            // ManagedComponentでGameObjectの参照を持たせる
            em.AddComponentObject(entity, new SnowMeshComponent
            {
                Mesh         = mesh,
                MeshFilter   = mf,
                MeshRenderer = mr,
                GameObject   = go,
            });

            // 生成間隔を空けてフリーズを防ぐ
            yield return new WaitForSeconds(_spawnRate);
        }
    }
}