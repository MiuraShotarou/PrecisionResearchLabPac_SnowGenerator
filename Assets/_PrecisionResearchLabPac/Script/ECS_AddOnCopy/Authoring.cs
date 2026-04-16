using System;
using UnityEngine;
using System.Collections.Generic;
using SnowflakeNative;
using Unity.Entities;
using Unity.VisualScripting;
using UnityEditor;
using UnityEditor.UI;

/// <summary> Authoring </summary>
public class SnowflakeAuthoring : MonoBehaviour
{
    // private NdArray<int> hoge = new NdArray();
    public class Baker : Baker<SnowflakeAuthoring>
    {
        public override void Bake(SnowflakeAuthoring authoring)
        {
            var entity = GetEntity(TransformUsageFlags.Dynamic);
            AddComponent(entity, new MoveComponent());
        }
    }

    private void Awake()
    {
        
    }
}