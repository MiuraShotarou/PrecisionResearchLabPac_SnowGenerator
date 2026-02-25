// Editor/PreviewSceneWindow.cs
using UnityEngine;
using UnityEditor;

public class PreviewSceneWindow : EditorWindow
{
    private PreviewRenderUtility previewRenderer; //
    private Mesh cubeMesh;
    private Material previewMaterial;

    private Vector2 dragDelta;
    private float rotationX = 20f;
    private float rotationY = -30f;
    private float zoom = 3f;

    [MenuItem("Tools/Preview Scene Window")]
    public static void Open()
    {
        GetWindow<PreviewSceneWindow>("Preview Scene");
    }

    private void OnEnable()
    {
        InitPreview();
    }

    private void OnDisable()
    {
        CleanUp();
    }

    // ---------------------------------------------------
    // 初期化
    // ---------------------------------------------------
    private void InitPreview()
    {
        // PreviewRenderUtility : Unityが提供するプレビュー用レンダラー
        previewRenderer = new PreviewRenderUtility();

        // カメラ設定
        previewRenderer.camera.backgroundColor = new Color(0.15f, 0.15f, 0.15f, 1f);
        previewRenderer.camera.clearFlags = CameraClearFlags.SolidColor; //カメラが描画を始める前に画面をどう初期化するか、を決める設定
        previewRenderer.camera.nearClipPlane = 0.01f;
        previewRenderer.camera.farClipPlane = 100f;
        previewRenderer.camera.transform.position = new Vector3(0, 0, -zoom);
        previewRenderer.camera.transform.LookAt(Vector3.zero);

        // ライト設定
        previewRenderer.lights[0].intensity = 1.2f;
        previewRenderer.lights[0].transform.rotation = Quaternion.Euler(50f, 50f, 0f);
        previewRenderer.lights[1].intensity = 0.4f;

        // Mesh・Material生成
        GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
        cube.transform.position = new Vector3(0, 0, 0);
        cube.transform.rotation = Quaternion.identity;
        cube.transform.localScale = new Vector3(1, 1, 1);
        cube.name = "ACube";
    }

    // ---------------------------------------------------
    // GUIレンダリング
    // ---------------------------------------------------
    private void OnGUI()
    {
        Rect previewRect = new Rect(0, 0, position.width, position.height);

        // マウス操作（ドラッグで回転、スクロールでズーム）
        HandleInput(previewRect);

        // プレビュー描画開始
        previewRenderer.BeginPreview(previewRect, GUIStyle.none);

        // カメラ位置更新
        UpdateCamera();

        // Meshを描画
        Matrix4x4 matrix = Matrix4x4.TRS(Vector3.zero, Quaternion.identity, Vector3.one);
        previewRenderer.DrawMesh(cubeMesh, matrix, previewMaterial, 0);

        // レンダリング実行・テクスチャとして取得
        previewRenderer.camera.Render();
        Texture resultTexture = previewRenderer.EndPreview();

        // EditorWindowに描画
        GUI.DrawTexture(previewRect, resultTexture, ScaleMode.StretchToFill, false);

        // 操作ガイド表示
        DrawGuide();

        // ドラッグ中は再描画
        if (Event.current.type == EventType.MouseDrag)
            Repaint();
    }

    // ---------------------------------------------------
    // マウス入力処理
    // ---------------------------------------------------
    private void HandleInput(Rect rect)
    {
        Event e = Event.current;

        // 左ドラッグ → 回転
        if (e.type == EventType.MouseDrag && e.button == 0)
        {
            rotationY += e.delta.x * 0.5f;
            rotationX += e.delta.y * 0.5f;
            rotationX = Mathf.Clamp(rotationX, -89f, 89f);
            Repaint();
        }

        // スクロール → ズーム
        if (e.type == EventType.ScrollWheel)
        {
            zoom += e.delta.y * 0.1f;
            zoom = Mathf.Clamp(zoom, 1f, 20f);
            Repaint();
        }
    }

    // ---------------------------------------------------
    // カメラ位置更新
    // ---------------------------------------------------
    private void UpdateCamera()
    {
        Quaternion rotation = Quaternion.Euler(rotationX, rotationY, 0f);
        Vector3 position = rotation * new Vector3(0, 0, -zoom);
        previewRenderer.camera.transform.position = position;
        previewRenderer.camera.transform.LookAt(Vector3.zero);
    }

    // ---------------------------------------------------
    // 操作ガイドのオーバーレイ表示
    // ---------------------------------------------------
    private void DrawGuide()
    {
        GUIStyle style = new GUIStyle(GUI.skin.label);
        style.normal.textColor = new Color(1f, 1f, 1f, 0.5f);
        style.fontSize = 11;

        GUI.Label(new Rect(8, 8, 300, 20), "左ドラッグ : 回転　|　スクロール : ズーム", style);
    }

    // ---------------------------------------------------
    // クリーンアップ
    // ---------------------------------------------------
    private void CleanUp()
    {
        if (previewRenderer != null)
        {
            previewRenderer.Cleanup();
            previewRenderer = null;
        }
        if (previewMaterial != null)
        {
            DestroyImmediate(previewMaterial);
            previewMaterial = null;
        }
        if (cubeMesh != null)
        {
            DestroyImmediate(cubeMesh);
            cubeMesh = null;
        }
    }

    // ---------------------------------------------------
    // Mesh生成（前回と同じ）
    // ---------------------------------------------------
    private Mesh CreateCubeMesh()
    {
        Mesh mesh = new Mesh();

        Vector3[] vertices = {
            new Vector3(-0.5f, -0.5f,  0.5f), new Vector3( 0.5f, -0.5f,  0.5f),
            new Vector3( 0.5f,  0.5f,  0.5f), new Vector3(-0.5f,  0.5f,  0.5f),
            new Vector3( 0.5f, -0.5f, -0.5f), new Vector3(-0.5f, -0.5f, -0.5f),
            new Vector3(-0.5f,  0.5f, -0.5f), new Vector3( 0.5f,  0.5f, -0.5f),
            new Vector3(-0.5f, -0.5f, -0.5f), new Vector3(-0.5f, -0.5f,  0.5f),
            new Vector3(-0.5f,  0.5f,  0.5f), new Vector3(-0.5f,  0.5f, -0.5f),
            new Vector3( 0.5f, -0.5f,  0.5f), new Vector3( 0.5f, -0.5f, -0.5f),
            new Vector3( 0.5f,  0.5f, -0.5f), new Vector3( 0.5f,  0.5f,  0.5f),
            new Vector3(-0.5f,  0.5f,  0.5f), new Vector3( 0.5f,  0.5f,  0.5f),
            new Vector3( 0.5f,  0.5f, -0.5f), new Vector3(-0.5f,  0.5f, -0.5f),
            new Vector3(-0.5f, -0.5f, -0.5f), new Vector3( 0.5f, -0.5f, -0.5f),
            new Vector3( 0.5f, -0.5f,  0.5f), new Vector3(-0.5f, -0.5f,  0.5f),
        };

        int[] triangles = new int[36];
        for (int i = 0; i < 6; i++)
        {
            int v = i * 4, t = i * 6;
            triangles[t]     = v;     triangles[t + 1] = v + 2; triangles[t + 2] = v + 1;
            triangles[t + 3] = v;     triangles[t + 4] = v + 3; triangles[t + 5] = v + 2;
        }

        mesh.vertices = vertices;
        mesh.triangles = triangles;
        mesh.RecalculateNormals();
        return mesh;
    }
}