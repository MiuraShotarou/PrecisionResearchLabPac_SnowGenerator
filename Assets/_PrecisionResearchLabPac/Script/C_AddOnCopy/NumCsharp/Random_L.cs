// Random_L.cs
using System;
using System.Runtime.InteropServices;
/*
default_rng は numpy.random.Generator インスタンスを生成する関数です。
    引数に seed を渡すと、その値を元に内部状態を初期化する
    seed=None（デフォルト）の場合はOSのエントロピーソースから内部状態を初期化する
    内部のアルゴリズムは PCG64 という乱数生成アルゴリズムを使用している
    旧来の np.random.seed() + np.random.choice() のグローバルAPIより再現性・統計的品質が高いとされている
*/

namespace SnowflakeNative
{
    public static class RandomLExtensions
    {
        /// <summary> Choice </summary>
        // np_random_choice_argumentndarray(NdArray *values, int count, bool replace, Random *random)
        
        // public static NdArray<T> Choice<T>(this Random random, NdArray<T> values, int count, bool replace = true) where T : unmanaged => values.Choice(random, count, replace);
    }
    
    public partial class Random : CSLanguageNative, IDisposable, IRandom //名前区間があるので他Randomクラスとの混同を防げる
    {
        public L l => new L(this);
        public class L
        {
            private Random _outer;
            public L(Random outer)
            {
                _outer = outer;
            }
            // // np_random_choice_argumentscalar(int max, int count, bool replace, SDType sdtype, Random *random)
            // public NdArray<T> Choice<T>(long max, long count, bool replace = true) where T : unmanaged {
            //     using (var random = new Random()) {
            //         ;
            //     }
            // }
            // public NdArray<T> Choice<T>(Random random, long max, long count, bool replace = true) where T : unmanaged => Packing(new NdArray<T>(), CSLChoice<T>(random.Pointer, max, count, replace));
            // // np_random_choice_argumentndarray(NdArray *values, long count, bool replace, Random *random)
            // public NdArray<T> Choice(long count, bool replace = true) { 
            //     using (var random = new Random()) {
            //         return Packing(new NdArray<T>(), CSLChoice(random.Pointer, _outer._pointer, count, replace));
            //     }
            // }
            // public NdArray<T> Choice(Random random, long count, bool replace = true) => Packing(new NdArray<T>(), CSLChoice(random.Pointer, _outer._pointer, count, replace));
        }
    }

    public abstract partial class CSLanguageNative : CLanguageNative
    {
        
    }

    public abstract partial class CLanguageNative
    {
        // [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        // protected static extern long np_l_random_range(IntPtr pointer, long min, long max);
        // /// <summary> RandomChoice </summary>
        // [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        // protected static extern IntPtr np_random_choice_argumentscalar(IntPtr random, int max, int count, bool replace, SDType sdtype);
        // [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        // protected static extern IntPtr np_random_choice_argumentndarray(IntPtr random, IntPtr values, int count, bool replace);
    }
}
