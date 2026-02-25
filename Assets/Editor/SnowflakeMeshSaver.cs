// Assets/Editor/SnowflakeMeshSaver.cs
using UnityEngine;
using UnityEditor;

public class SnowflakeMeshSaver : EditorWindow
{
    [MenuItem("Tools/Save Snowflake Mesh")]
    public static void SaveMesh()
    {
        // 選択中のGameObjectからMeshFilterを取得
        GameObject selected = Selection.activeGameObject;
        if (selected == null)
        {
            EditorUtility.DisplayDialog("エラー", "GameObjectを選択してください", "OK");
            return;
        }

        MeshFilter meshFilter = selected.GetComponent<MeshFilter>();
        if (meshFilter == null || meshFilter.sharedMesh == null)
        {
            EditorUtility.DisplayDialog("エラー", "MeshFilterまたはメッシュが見つかりません", "OK");
            return;
        }

        // 保存先パスをダイアログで指定
        string path = EditorUtility.SaveFilePanelInProject(
            "Save Snowflake Mesh",
            selected.name + "_Mesh",
            "asset",
            "メッシュの保存先を選択してください",
            "Assets/Meshes"
        );

        if (string.IsNullOrEmpty(path)) return;

        // メッシュをAssetとして保存
        Mesh meshToSave = Object.Instantiate(meshFilter.sharedMesh);
        AssetDatabase.CreateAsset(meshToSave, path);
        AssetDatabase.SaveAssets();

        // 保存したメッシュをMeshFilterに再アサイン
        meshFilter.sharedMesh = meshToSave;

        // Prefab化
        string prefabPath = path.Replace(".asset", ".prefab");
        PrefabUtility.SaveAsPrefabAssetAndConnect(selected, prefabPath, InteractionMode.UserAction);

        EditorUtility.DisplayDialog("完了", $"保存しました\nMesh: {path}\nPrefab: {prefabPath}", "OK");

        // Projectウィンドウで保存したファイルをハイライト
        EditorGUIUtility.PingObject(AssetDatabase.LoadAssetAtPath<Mesh>(path));
    }
}