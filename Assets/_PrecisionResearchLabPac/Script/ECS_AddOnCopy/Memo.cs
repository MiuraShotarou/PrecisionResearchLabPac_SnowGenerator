using UnityEngine;

public class Memo : MonoBehaviour
{
    //アプローチ① 非同期処理（UniTask）でシミュレーション計算を行い、Mesh生成のみメインスレッドで行う
    // Snowflakeごとに異なるパラメータ（ObjectProperties）を持たせ、そこからSnowflakeGrow.Executeを呼ぶ必要がある
    
    //[IComponentData]
    // SnowflakeParams → IComponentData ○
    // ObjectProperties のフィールド群 → IComponentData (2次元配列は1次元配列に変換しなくてはならないらしい)
    // growing, steps, size などの状態フラグ
    
    //[ISystem]
    // Snowflake.Step
    // Operators.MakeMesh
    // Operators.Simulate
    
    //[Authoring]
    // ObjectProperties ○
    
    //コレクション系を NativeArrayに書き換える必要がある
}
