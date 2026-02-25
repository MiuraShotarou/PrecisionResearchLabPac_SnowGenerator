using Unity.Burst;
using Unity.Collections;
using Unity.Entities;
using Unity.Mathematics;
using Unity.Transforms;

public partial struct SnowSpawnSystem : ISystem
{
    [BurstCompile]
    public void OnCreate(ref SystemState state) //System生成時に一回呼ばれる
    {
        state.RequireForUpdate<SnowSpawnerComponent>();
    }

    [BurstCompile]
    public void OnUpdate(ref SystemState state)
    {
        // 1回だけ実行してスポナーを削除する
        EntityCommandBuffer ecb = new EntityCommandBuffer(Allocator.Temp);

        // RefRO<SnowSpawnerComponent>, Entity に RefR0,Entity を代入する
        foreach (var (spawner, entity) in
            SystemAPI.Query<RefRO<SnowSpawnerComponent>>().WithEntityAccess()) //配列：<T>, Entity
        {
            var rng = new Unity.Mathematics.Random(12345); //乱数生成器をインスタンス化

            for (int i = 0; i < spawner.ValueRO.SnowCount; i++) //生成されたSnowオブジェクトぶん
            {
                Entity snow = ecb.Instantiate(spawner.ValueRO.Prefab);

                // ランダムなXZ位置・固定のY高さ
                float3 pos = new float3(
                    rng.NextFloat(-spawner.ValueRO.SpawnRange, spawner.ValueRO.SpawnRange),
                    spawner.ValueRO.SpawnHeight,
                    rng.NextFloat(-spawner.ValueRO.SpawnRange, spawner.ValueRO.SpawnRange)
                );

                ecb.SetComponent(snow, LocalTransform.FromPosition(pos));
                ecb.AddComponent(snow, new SnowComponent
                {
                    FallSpeed = spawner.ValueRO.FallSpeed,
                });
            }

            // スポナーは生成後に削除
            ecb.DestroyEntity(entity);
        }

        ecb.Playback(state.EntityManager);
        ecb.Dispose();
    }
}

public partial struct SnowFallSystem : ISystem
{
    [BurstCompile]
    public void OnUpdate(ref SystemState state)
    {
        float dt = SystemAPI.Time.DeltaTime;

        // 全雪エンティティを並列で落下させる
        new SnowFallJob { DeltaTime = dt }.ScheduleParallel();
    }

    [BurstCompile]
    partial struct SnowFallJob : IJobEntity
    {
        public float DeltaTime;

        void Execute(ref LocalTransform transform, in SnowComponent snow)
        {
            transform.Position.y -= snow.FallSpeed * DeltaTime;
        }
    }
}