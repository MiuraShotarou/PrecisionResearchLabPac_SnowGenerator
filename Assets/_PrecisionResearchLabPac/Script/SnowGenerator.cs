using UnityEngine;

public class SnowGenerator : MonoBehaviour
{
    GameObject _snowPrefab;
    int _snowCount;

    [SerializeField] private float _spawnHeight    = 13f;  // 生成する高さ
    [SerializeField] private float _spawnRange     = 20f;  // XZ方向の生成範囲
    [SerializeField] private float _fallSpeed      = 2f;   // 落下速度

    void Awake()
    {
        _snowPrefab = Resources.Load("==Snow==") as GameObject;
        _snowCount  = 100000;
    }

    void Start()
    {
        for (int i = 0; i < _snowCount; i++)
        {
            // XZ方向にランダムな位置、Y方向は固定の高さから生成
            Vector3 spawnPos = new Vector3(
                Random.Range(-_spawnRange, _spawnRange),
                _spawnHeight,
                Random.Range(-_spawnRange, _spawnRange)
            );

            GameObject snow = Instantiate(_snowPrefab, spawnPos, Quaternion.identity);

            // 落下用のRigidbodyを追加して重力で落とす
            Rigidbody rb       = snow.GetComponent<Rigidbody>();
            rb.useGravity      = false; // 物理エンジンの重力は使わず速度で制御
            rb.linearVelocity  = new Vector3(0f, -_fallSpeed, 0f);
        }
    }
}