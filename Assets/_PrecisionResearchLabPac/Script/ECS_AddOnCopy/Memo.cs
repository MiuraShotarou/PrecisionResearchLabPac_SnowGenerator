using UnityEngine;

public class Memo : MonoBehaviour
{
    //アプローチ① 結晶の移動のみECS, Mesh生成はUniTaskで実行する
    // 期待する動作：Meshの生成 → GameObjectの生成 → すべてのGameObjectが生成し終えたら、Positionの初期化と移動処理を適用する
    // trianglesとverticesを１次元配列で管理するのはあり
    
    //C# and PythonCopy
    //totalTime = 1960.3354558秒 (32.67分) → リファクタ 2124.1140521秒（35分）
    //①meshへ a,b,c,dを格納する処理を削除してみる → 1847.2653162秒（30分）
    
    // C# and Claude{
    // step: 1 ~ 1.5 second
    // UpdateNeighbors △
    // Melting         OK!
    // Noise           OK!
    // Diffusion       OK!
    // Symmetrize      OK!
    // Freezing        OK!
    // Attachment      △
    // Grow            Heavy!
    // }
}
