using Unity.Burst;
using Unity.Entities;
using Unity.Mathematics;
using Unity.Transforms;
using UnityEngine;

// 落下データ（通常のIComponentData）
public struct SnowFallData : IComponentData
{
    public float FallSpeed;
    public float SwaySpeed;
    public float SwayAmount;
    public float SwayOffset;
    public float RotSpeed;
    public float DespawnY;
}

/// <summary>
/// ECSのLocalTransformを毎フレーム更新して落下・揺れ・回転を処理する。
/// ManagedComponentのGameObjectにも反映する。
/// </summary>
public partial class SnowManagedFallSystem : SystemBase
{
    protected override void OnUpdate()
    {
        float dt   = SystemAPI.Time.DeltaTime;
        float time = (float)SystemAPI.Time.ElapsedTime;

        var ecb   = new EntityCommandBuffer(Unity.Collections.Allocator.Temp);

        // SnowFallDataを持つEntityを取得
        var query    = GetEntityQuery(
            ComponentType.ReadWrite<LocalTransform>(),
            ComponentType.ReadOnly<SnowFallData>(),
            ComponentType.ReadOnly<SnowMeshComponent>()
        );

        var entities   = query.ToEntityArray(Unity.Collections.Allocator.Temp);
        var transforms = query.ToComponentDataArray<LocalTransform>(Unity.Collections.Allocator.Temp);
        var falls      = query.ToComponentDataArray<SnowFallData>(Unity.Collections.Allocator.Temp);

        for (int i = 0; i < entities.Length; i++)
        {
            var transform = transforms[i];
            var fall      = falls[i];
            var snow      = EntityManager.GetComponentObject<SnowMeshComponent>(entities[i]);

            // 落下
            transform.Position.y -= fall.FallSpeed * dt;

            // 揺れ
            float sway = math.sin(time * fall.SwaySpeed + fall.SwayOffset) * fall.SwayAmount;
            transform.Position.x += sway * dt;
            transform.Position.z += sway * 0.5f * dt;

            // 回転
            transform.Rotation = math.mul(
                transform.Rotation,
                quaternion.AxisAngle(math.up(), math.radians(fall.RotSpeed * dt))
            );

            // GameObjectに同期
            if (snow?.GameObject != null)
            {
                snow.GameObject.transform.position = new Vector3(
                    transform.Position.x,
                    transform.Position.y,
                    transform.Position.z
                );
                snow.GameObject.transform.rotation = new Quaternion(
                    transform.Rotation.value.x,
                    transform.Rotation.value.y,
                    transform.Rotation.value.z,
                    transform.Rotation.value.w
                );
            }
            // ECS側に書き戻す
            EntityManager.SetComponentData(entities[i], transform);
            // 画面外で削除
            if (transform.Position.y < fall.DespawnY)
            {
                if (snow?.GameObject != null)
                    Object.Destroy(snow.GameObject);

                ecb.DestroyEntity(entities[i]);
            }
        }
        entities.Dispose();
        transforms.Dispose();
        falls.Dispose();
        ecb.Playback(EntityManager);
        ecb.Dispose();
    }
}