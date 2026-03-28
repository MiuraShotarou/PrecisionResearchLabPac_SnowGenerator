using Unity.Entities;
using UnityEngine;

/// <summary>
/// ManagedComponentはclassで定義する（ECSのIComponentDataはstructだが
/// Mesh等のマネージドオブジェクトを持つ場合はIComponentDataではなくこちらを使う）
/// </summary>
public class SnowMeshComponent : IComponentData
{
    public Mesh           Mesh;
    public MeshFilter     MeshFilter;
    public MeshRenderer   MeshRenderer;
    public GameObject     GameObject;
}