using Unity.Entities;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEngine;
using UnityEditor;
using Object = UnityEngine.Object;

/// <summary> 雪の結晶を降らせる処理 </summary>
[WorldSystemFilter(WorldSystemFilterFlags.Editor | WorldSystemFilterFlags.Default)]
public partial struct SnowflakeMoveSystem : ISystem
{
    // public void OnUpdate(ref SystemState state)
    // {
    //     if (ClimateSettings.Instance)
    //     {
    //         if (EditorApplication.isPlaying || ClimateSettings.Instance.IsOnEditor)
    //         {
    //             int queryCount = state.EntityManager.CreateEntityQuery(typeof(SnowflakeManagedComponent))
    //                 .CalculateEntityCount();
    //             // 雪の結晶の内部データのみを生成する処理
    //             if (queryCount < ClimateSettings.Instance.CountLimit)
    //             {
    //                 for (int count = 0; count < ClimateSettings.Instance.CountLimit - queryCount; count++)
    //                 {
    //                     GetSnowflakeData(state);
    //                 }
    //             }
    //         }
    //     }
    // }
}

/// <summary> 格子計算 </summary>
public partial struct SnowflakeSystem : ISystem
{
}
