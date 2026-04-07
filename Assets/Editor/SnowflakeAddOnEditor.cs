using Cysharp.Threading.Tasks;
using Unity.VisualScripting;
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
            properties.SnowflakeGrowModal ??= new Operators.SnowflakeGrowModal(); //GameObjectごとにインスタンスを保持
            properties.SnowflakeGrowModal.Invoke(properties); //properties.growing = true;
        }
        if (GUILayout.Button("2000Steps"))
        {
            properties.SnowflakeGrow ??= new Operators.SnowflakeGrow(); //GameObjectごとにインスタンスを保持
            properties.ExecutionSteps = 2000;
            properties.CompletionSteps = 2000;
            
            // 完了を検知するためにEditorApplication.updateに登録
            var stopwatch = System.Diagnostics.Stopwatch.StartNew();
            
            void OnComplete()
            {
                if (properties.ProgressSteps >= properties.CompletionSteps)
                {
                    stopwatch.Stop();
                    UnityEngine.Debug.Log($"200Steps完了時間: {stopwatch.Elapsed.TotalSeconds}秒");
                    EditorApplication.update -= OnComplete;
                }
            }
            EditorApplication.update += OnComplete;
        }
    }
}

[CustomEditor(typeof(ClimateSettings))]
public class ClimateSettingsEditor : Editor
{
    public override void OnInspectorGUI()
    {
        DrawDefaultInspector();
        var settings = (ClimateSettings)target;
        if (GUILayout.Button("Generate New"))
        {
            settings.GenerateNewAsync().Forget(ex => Debug.LogException(ex));
        }
    }
}