using Unity.Entities;
using UnityEngine;

// InspectorからECSに変換するためのAuthoring
public class SnowSpawnerAuthoring : MonoBehaviour
{
    public GameObject SnowPrefab;
    public int        SnowCount   = 100000;
    public float      SpawnHeight = 13f;
    public float      SpawnRange  = 20f;
    public float      FallSpeed   = 2f;

    class Baker : Baker<SnowSpawnerAuthoring>
    {
        // Entityデータに変換
        public override void Bake(SnowSpawnerAuthoring authoring)
        {
            Entity entity = GetEntity(TransformUsageFlags.None);
            AddComponent(entity, new SnowSpawnerComponent
            {
                Prefab      = GetEntity(authoring.SnowPrefab, TransformUsageFlags.Dynamic), //SnowPrefabをEntityに変換する → が、MonoBehaviourのみが
                SnowCount   = authoring.SnowCount, //
                SpawnHeight = authoring.SpawnHeight,
                SpawnRange  = authoring.SpawnRange,
                FallSpeed   = authoring.FallSpeed,
            });
        }
    }
}