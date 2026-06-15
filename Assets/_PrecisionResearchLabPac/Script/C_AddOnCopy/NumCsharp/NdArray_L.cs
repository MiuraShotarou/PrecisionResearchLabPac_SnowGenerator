// NdArray_L.cs
using System;
using System.Linq;
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
            public NdArray<T> Arange(long end, char order) => Packing(new NdArray<T>(), CSLArange<T>(0, end, 1, order));
            public NdArray<T> Arange(long start, long end, char order) => Packing(new NdArray<T>(), CSLArange<T>(start, end, 1, order));
            public NdArray<T> Arange(long start, long end, long step, char order) => Packing(new NdArray<T>(), CSLArange<T>(start, end, step, order));
            /// <summary> properties </summary>
            /// <summary> Shape </summary>
            public long[] Shape => CSLShape(_outer._pointer);
            /// <summary> Indices </summary>
            public NdArray<long>[] Indices(NdArray<T> src) => Packing(new NdArray<long>(), CSLIndices(src._pointer));
            /// <summary> Size </summary>
            public long Size => CSLSize(_outer._pointer);
        }

    }

    public abstract partial class CSLanguageNative : CLanguageNative
    {
        protected static IntPtr CSLArange<T>(long start, long end, long step, char order) where T : unmanaged
        {
            SDType resType = GenericsToSDType<T>();
            return np_l_arange(start, end, step, resType, order);
        }
        /// <summary> CSLShape </summary> //戻り値をint[]に変更したい
        protected static long[] CSLShape(IntPtr pointer)
        {
            var get = np_shape(pointer);
            int nd = np_nd(pointer);
            // int[] result = new int[nd];
            long[] result = new long[nd];
            Marshal.Copy(get, result, 0 ,nd);
            return result;
        }
        /// <summary> CSLIndices </summary>
        protected static IntPtr[] CSLIndices(IntPtr pointer)
        {
            long l_total = np_size(pointer);
            if (l_total > int.MaxValue || l_total < int.MinValue)
            {
                throw new OverflowException($"NdArray size {l_total} exceeds int range.");
            }
            int total = (int)l_total;
            IntPtr[] result = new IntPtr[total];
            np_indices(pointer, result, SDType.Long);
            return result;
        }
        protected static long CSLSize(IntPtr pointer)
        { 
            return np_size(pointer);
        }
    }

    public abstract partial class CLanguageNative
    {
        /// <summary> Arange </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_l_arange(long start, long end, long step, SDType sdType, char order);
    }
}
