using UnityEngine;
using System.Collections.Generic;
using Unity.Entities;
using UnityEditor;

/// <summary> Authoring </summary>
public class Properties_ECS : MonoBehaviour
{
    [Header("FieldParameters")]
    [Tooltip("CoverArea")]
    public Vector3 coverArea = new Vector3(100, 500, 100);
    
    [Tooltip("FallAmount")]
    [Range(0f, 100f)]
    public int fallAmount = 100;
}
