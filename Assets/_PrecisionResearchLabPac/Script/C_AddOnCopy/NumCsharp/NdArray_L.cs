// NdArray_L.cs
using System;
using System.Runtime.InteropServices;

namespace SnowflakeNative
{
    /// <summary> Collection is NdArray </summary>
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        public L l => new L(this);
        public class L
        {
            private NdArray<T> _outer;
            public L(NdArray<T> outer)
            {
                _outer = outer;
            }
            // np_random_choice_argumentscalar(int max, int count, bool replace, SDType sdtype, Random *random)
            public NdArray<T> Choice(long max, long count, bool replace = true) {
                using (var random = new Random()) {
                    return Packing(new NdArray<T>(), CSLChoice<T>(random.Pointer, max, count, replace));
                }
            }
            public NdArray<T> Choice(Random random, long max, long count, bool replace = true)           => Packing(new NdArray<T>(), CSLChoice<T>(random.Pointer, max, count, replace));
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
