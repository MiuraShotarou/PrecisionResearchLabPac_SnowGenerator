using Unity.Entities;

// 雪のエンティティが持つデータ
public struct SnowComponent : IComponentData
{
    public float FallSpeed;
}

// スポナーが持つ設定データ
public struct SnowSpawnerComponent : IComponentData
{
    public Entity  Prefab;
    public int     SnowCount; //
    public float   SpawnHeight;
    public float   SpawnRange;
    public float   FallSpeed;
}