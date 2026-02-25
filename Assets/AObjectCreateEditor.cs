using UnityEngine;
using UnityEditor;
[CustomEditor(typeof(AObjectCreate))]
public class AObjectCreateEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        AObjectCreate script = (AObjectCreate)target;

        if (GUILayout.Button("Generate Cube"))
        {
            script.GenerateCube();
        }
    }
}