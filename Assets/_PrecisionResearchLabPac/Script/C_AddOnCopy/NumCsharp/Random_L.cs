// Random_L.cs
using System;
using System.Runtime.InteropServices;

namespace SnowflakeNative
{
    public static class RandomLExtensions
    {
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
            /// <summary> L_Choice </summary>
            public NdArray<T> Choice<T>(long max, long count, bool replace = true) where T : unmanaged => (NdArray<T>)CSLChoice<T>(_outer._pointer, max, count, replace);
            // 引数にNdArray<T>参照ありパターン
            public NdArray<T> Choice<T>(NdArray<T> values, long count, bool replace = true) where T : unmanaged => (NdArray<T>)CSLChoice(_outer._pointer, ((INdArray)values)._pointer, count, replace);
        }
    }

    public abstract partial class CSLanguageNative : CLanguageNative
    {
        protected static IntPtr CSLChoice<T>(IntPtr random, long max, long count, bool replace) where T : unmanaged
        {
            return np_random_l_choice_argumentscalar(random, max, count, replace, GenericsToSDType<T>());
        }
        protected static IntPtr CSLChoice(IntPtr random, IntPtr values, long count, bool replace)
        {
            return np_random_l_choice_argumentndarray(random, values, count, replace);
        }
    }

    public abstract partial class CLanguageNative
    {
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_l_choice_argumentscalar(IntPtr random, long max, long count, bool replace, SDType sdtype);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_l_choice_argumentndarray(IntPtr random, IntPtr values, long count, bool replace);
    }
}
