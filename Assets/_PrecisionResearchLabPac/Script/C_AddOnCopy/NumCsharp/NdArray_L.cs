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
            /// <summary> コンストラクター </summary>
            public L(long[] dimensions, char order = 'C')
            {
                int nd = dimensions.Length;
                int itemsize = Marshal.SizeOf(typeof(T));
                SDType sdType = GenericsToSDType<T>();
                _outer._pointer = ndarray_create(nd, dimensions, itemsize, sdType);
                if (_outer._pointer == IntPtr.Zero)
                {
                    throw new InvalidOperationException("ndarray_create failed.");
                }
            }
            /// <summary> LArange </summary>
            public static NdArray<T> Arange(long end, char order) => Packing(new NdArray<T>(), CSLArange<T>(0, end, 1, order));
            public static NdArray<T> Arange(long start, long end, char order) => Packing(new NdArray<T>(), CSLArange<T>(start, end, 1, order));
            public static NdArray<T> Arange(long start, long end, long step, char order) => Packing(new NdArray<T>(), CSLArange<T>(start, end, step, order));
        }

    }

    public abstract partial class CSLanguageNative : CLanguageNative
    {
        protected static IntPtr CSLArange<T>(long start, long end, long step, char order) where T : unmanaged
        {
            SDType resType = GenericsToSDType<T>();
            return np_l_arange(start, end, step, resType, order);
        }
    }

    public abstract partial class CLanguageNative
    {
        /// <summary> Arange </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_l_arange(long start, long end, long step, SDType sdType, char order);
    }
}
