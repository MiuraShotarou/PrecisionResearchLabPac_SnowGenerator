using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(CreateNewPanel))]
public class CreateNewPanelEditor : Editor
{
    public override void OnInspectorGUI()
    {
        DrawDefaultInspector();
        var properties = ((CreateNewPanel)target).GetComponent<ObjectProperties>();
        if (GUILayout.Button("Create New"))
        {
            new Operators.SnowflakeCreate().Execute(properties); //インスタンスを即時破棄
        }
    }
}

[CustomEditor(typeof(SnowflakePanel))]
public class SnowflakePanelEditor : Editor
{
    public override void OnInspectorGUI()
    {
        DrawDefaultInspector();
        var properties = ((SnowflakePanel)target).GetComponent<ObjectProperties>();
        if (GUILayout.Button("Interactive"))
        {
            properties.SnowflakeGrowModal = new Operators.SnowflakeGrowModal(); //GameObjectごとにインスタンスを保持
            properties.SnowflakeGrowModal.Invoke(properties); //properties.growing = true;
        }
    }
}