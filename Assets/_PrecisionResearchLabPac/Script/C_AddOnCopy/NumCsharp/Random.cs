using UnityEngine;
/*
default_rng は numpy.random.Generator インスタンスを生成する関数です。
    引数に seed を渡すと、その値を元に内部状態を初期化する
    seed=None（デフォルト）の場合はOSのエントロピーソースから内部状態を初期化する
    内部のアルゴリズムは PCG64 という乱数生成アルゴリズムを使用している
    旧来の np.random.seed() + np.random.choice() のグローバルAPIより再現性・統計的品質が高いとされている
*/

namespace SnowflakeNative
{
    public class Random : CSLanguageNative //名前区間があるので他Randomクラスとの混同を防げる
    {
        
    }

}
