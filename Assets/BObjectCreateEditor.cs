// Editor/BObjectCreateEditor.cs
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(BObjectCreate))]
public class BObjectCreateEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        BObjectCreate script = (BObjectCreate)target;

        if (GUILayout.Button("Generate Cube"))
        {
            script.GenerateCube();
        }
    }
}