using System;
using System.Runtime.InteropServices;
using UnityEngine.UIElements;

namespace SnowflakeNative
{
    public abstract partial class CSLanguageNative
    {

    }
    public abstract partial class CLanguageNative
    {
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr get_ndarray_boolIndex(IntPtr src, long[] mask, int mask_nd, SDType sdtype); //long[] は C言語でbool[]に変換される
        protected static extern IntPtr get_ndarray_oolIndex(IntPtr src, IntPtr mask, int mask_nd, SDType sdtype); //long[] は C言語でbool[]に変換される
    }
    public partial class NdArray<T> where T : unmanaged
    {
        /// <summary> AdvancedIndex - NdArray T </summary>
        public NdArray<T> this[NdArray<T> mask]
        {
            get
            {
                var hoge = get_ndarray_oolIndex(this._pointer, mask._pointer, mask.Length, GenericsToSDType<T>());
                return ;
            }
            set
            {
                
            }
        }
        /// <summary> AdvancedIndex - bool[] </summary>
        public NdArray<T> this[bool[] mask]
        {
            get
            {
                return Packing(new NdArray<T>(), get_ndarray_boolIndex(this._pointer, mask, mask.Length, GenericsToSDType<T>()));
            }
            set
            {
            }
        }
        /// <summary> AdvancedIndex - NdArray bool </summary>
        public NdArray<T> this[NdArray<bool> mask]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - NdArray sbyte </summary>
        public NdArray<T> this[NdArray<sbyte> indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - NdArray byte </summary>
        public NdArray<T> this[NdArray<byte> indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - NdArray short </summary>
        public NdArray<T> this[NdArray<short> indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - NdArray ushort </summary>
        public NdArray<T> this[NdArray<ushort> indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - NdArray int </summary>
        public NdArray<T> this[NdArray<int> indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - NdArray uint </summary>
        public NdArray<T> this[NdArray<uint> indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - NdArray long </summary>
        public NdArray<T> this[NdArray<long> indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - NdArray ulong </summary>
        public NdArray<T> this[NdArray<ulong> indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - NdArray float </summary>
        public NdArray<T> this[NdArray<float> indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - NdArray double </summary>
        public NdArray<T> this[NdArray<double> indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - sbyte[] </summary>
        public NdArray<T> this[sbyte[] indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - byte[] </summary>
        public NdArray<T> this[byte[] indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - short[] </summary>
        public NdArray<T> this[short[] indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - ushort[] </summary>
        public NdArray<T> this[ushort[] indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - int[] </summary>
        public NdArray<T> this[int[] indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - uint[] </summary>
        public NdArray<T> this[uint[] indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - long[] </summary>
        public NdArray<T> this[long[] indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }

        /// <summary> AdvancedIndex - ulong[] </summary>
        public NdArray<T> this[ulong[] indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }
        /// <summary> AdvancedIndex - float[] </summary>
        public NdArray<T> this[float[] indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }
        /// <summary> AdvancedIndex - double[] </summary>
        public NdArray<T> this[double[] indices]
        {
            get
            {
                return null;
            }
            set
            {
            }
        }
    }
}