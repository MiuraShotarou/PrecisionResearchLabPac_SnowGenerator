using UnityEngine;

public class Memo : MonoBehaviour
{
    //アプローチ① 結晶の移動のみECS, Mesh生成はUniTaskで実行する
    // 期待する動作：Meshの生成 → GameObjectの生成 → すべてのGameObjectが生成し終えたら、Positionの初期化と移動処理を適用する
    
    //[IComponentData]
    // SnowflakeParams → IComponentData ○
    // ObjectProperties のフィールド群 → IComponentData (2次元配列は1次元配列に変換しなくてはならないらしい)
    // growing, steps, size などの状態フラグ
    
    //[ISystem]
    // Snowflake.Step
    // Operators.Create
    // Operators.Simulate
    
    //[Authoring]
    // ObjectProperties ○
    
    //コレクション系を NativeArrayに書き換える必要がある
}
