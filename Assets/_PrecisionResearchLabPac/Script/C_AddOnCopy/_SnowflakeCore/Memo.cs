using System;
using System.Linq;
using UnityEngine;
using System.Collections.Generic;
using SnowflakeNative;

namespace SnowflakeNative
{

}

public class Memo : MonoBehaviour
{
    private void Awake()
    {
        var hoge = new NdArray<int>(new long[]{3, 4});
        var list = new List<float>();
        INdArray Ihoge = new NdArray<int>(new long[]{3, 4});
        CSLanguageNative kk = new NdArray<int>(new long[]{3, 4});
        var Ihoge_p = Ihoge._pointer; //internalで阻止する予定
        
        

        var zeros_f = NdArray<float>.Zeros(new long[] { 3, 4 });
        var fuga = zeros_f.Cast<int>();
        hoge += fuga;
        zeros_f = new long[]{3, 4};
        
        NdArray<int> foo = NdArray<int>.Concatenate(new []{fuga} , 0);
        var zeros_i = NdArray<int>.Zeros(new long[] { 3, 4 });
        zeros_i = zeros_i.Sum();
        var zeros_scalar_f = zeros_i.Sum();

        // list = new List<int>();
        hoge.Ravel();
        hoge = hoge.Copy();
        NdArray<int> a = new SnowflakeNative.Random().Choice<int>(5, 1, true);
        
        
    }

    private void Start()
    {
        NdArray<float> a = new NdArray<float>(new long[]{3, 4});
        NdArray<float> b = new NdArray<float>(new long[]{3, 4});
        NdArray<bool> conditions = new NdArray<bool>(new long[]{3, 4});
        
        var result = NdArray<float>.Where(conditions, a, b);

        
        // var arange_i = NdArray.Arange(1,1,1,'d');
    }

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

class NumPyDevMemo
{
    //<fromnumeric.py>
    // np.reshape
    // np.ravel → == reshape(-1)
    // np.cumsum
    //<numeric.py>
    // np.full
    // np.indices
    // np.logical_and
    // np.logical_or
    // np.where
    //<shape_base.py>
    // np.stack
    // np.vstack
    // np.column_stack
    //<multiarray.py>
    // np.arange
    // np.pad
    // arr[bool_array]（ブールインデックス）
    // arr[bool_array] = value（ブールインデックス書き込み）
    // arr[int_array]（ファンシーインデックス）
    // arr[int_array] = value（ファンシーインデックス書き込み）
    // np.add / +
    // np.subtract / -
    // np.multiply / *
    // np.divide / /
    // np.mod / %
    //<random>
    // np.random.Generator.normal
}
    
