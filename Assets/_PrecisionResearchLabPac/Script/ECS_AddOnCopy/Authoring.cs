using UnityEngine;
using System.Collections.Generic;
using Unity.Entities;
using Unity.VisualScripting;
using UnityEditor;

/// <summary> Authoring </summary>
public class SnowflakeAuthoring : MonoBehaviour
{
    public class Baker : Baker<SnowflakeAuthoring>
    {
        public override void Bake(SnowflakeAuthoring authoring)
        {
            var entity = GetEntity(TransformUsageFlags.Dynamic);
            AddComponent(entity, new MoveComponent());
        }
    }
}