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
    // public static class RandomLExtensions
    // {
    //     /// <summary> Choice </summary>
    //     // np_random_choice_argumentndarray(NdArray *values, int count, bool replace, Random *random)
    //     public static NdArray<T> Choice<T>(this Random random, NdArray<T> values, int count, bool replace = true) where T : unmanaged => values.Choice(random, count, replace);
    // }
    
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
            // np_random_choice_argumentscalar(int max, int count, bool replace, SDType sdtype, Random *random)
            public NdArray<T> Choice<T>(long max, long count, bool replace = true) where T : unmanaged {
                using (var random = new Random()) {
                    return Packing(new NdArray<T>(), CSLChoice<T>(random.Pointer, max, count, replace));
                }
            }
            public NdArray<T> Choice<T>(Random random, long max, long count, bool replace = true) where T : unmanaged => Packing(new NdArray<T>(), CSLChoice<T>(random.Pointer, max, count, replace));
            // np_random_choice_argumentndarray(NdArray *values, long count, bool replace, Random *random)
            public NdArray<T> Choice(long count, bool replace = true) { 
                using (var random = new Random()) {
                    return Packing(new NdArray<T>(), CSLChoice(random.Pointer, _outer._pointer, count, replace));
                }
            }
            public NdArray<T> Choice(Random random, long count, bool replace = true) => Packing(new NdArray<T>(), CSLChoice(random.Pointer, _outer._pointer, count, replace));
        }
    }

    public abstract partial class CSLanguageNative : CLanguageNative
    {
        protected static void CSRandomDispose(ref IntPtr random)
        {
            random_free(random);
            random = IntPtr.Zero;
        }
        protected static IntPtr CSRandomCreate(ulong seed)
        {
            return random_create(seed);
        }
        protected static double CSRandomRandom(IntPtr random)
        {
            return np_random_random(random);
        }
        protected static int CSRandomRange(IntPtr random, int min, int max)
        {
            return np_random_range(random, min, max);
        }
        protected static long CSRandomLRange(IntPtr random, long min, long max)
        {
            return np_l_random_range(random, min, max);
        }

        protected static IntPtr CSChoice<T>(IntPtr random, int max, int count, bool replace) where T : unmanaged
        {
            return np_random_choice_argumentscalar(random, max, count, replace, GenericsToSDType<T>());
        }
        protected static IntPtr CSChoice(IntPtr random, IntPtr values, int count, bool replace)
        {
            return np_random_choice_argumentndarray(random, values, count, replace);
        }

        protected static void CSRandomShuffle(IntPtr random, IntPtr src)
        {
            np_random_shuffle(random, src);
        }

        protected static IntPtr CSRandomPermutation(IntPtr random, IntPtr src)
        {
            return np_random_permutation(random, src);
        }

        protected static IntPtr CSRandomUniform(IntPtr random, double low, double high, long[] dimensions, int nd)
        {
            return np_random_uniform(random, low, high, dimensions, nd);
        }

        protected static IntPtr CSRandomNormal(IntPtr random, double loc, double scale, long[] dimensions, int nd)
        {
            return np_random_normal(random, loc, scale, dimensions, nd);
        }

        protected static IntPtr CSRandomRandint(IntPtr random, int low, int high, long[] dimensions, int nd)
        {
            return np_random_randint(random, low, high, dimensions, nd);
        }

        protected static IntPtr CSRandomBinomial(IntPtr random, int n, double p, long[] dimensions, int nd)
        {
            return np_random_binomial(random, n, p, dimensions, nd);
        }

        protected static IntPtr CSRandomPoisson(IntPtr random, double lam, long[] dimensions, int nd)
        {
            return np_random_poisson(random, lam, dimensions, nd);
        }

        protected static IntPtr CSRandomExponential(IntPtr random, double scale, long[] dimensions, int nd)
        {
            return np_random_exponential(random, scale, dimensions, nd);
        }
    }

    public abstract partial class CLanguageNative
    {
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr random_create(ulong seed);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern void random_free(IntPtr pointer);

        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern double np_random_random(IntPtr pointer);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern int np_random_range(IntPtr pointer, int min, int max);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern long np_l_random_range(IntPtr pointer, long min, long max);
        /// <summary> RandomChoice </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_choice_argumentscalar(IntPtr random, int max, int count, bool replace, SDType sdtype);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_choice_argumentndarray(IntPtr random, IntPtr values, int count, bool replace);

        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern void np_random_shuffle(IntPtr pointer, IntPtr src);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_permutation(IntPtr pointer, IntPtr src);

        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_uniform(IntPtr pointer, double low, double high, long[] dimensions, int nd);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_normal(IntPtr pointer, double loc, double scale, long[] dimensions, int nd);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_randint(IntPtr pointer, int low, int high, long[] dimensions, int nd);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_binomial(IntPtr pointer, int n, double p, long[] dimensions, int nd);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_poisson(IntPtr pointer, double lam, long[] dimensions, int nd);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_exponential(IntPtr pointer, double scale, long[] dimensions, int nd);
    }
}
