using System;
using System.Runtime.InteropServices;

namespace SnowflakeNative
{
    /// <summary> Collection is NdArray </summary>
    public class NdArray<T> : IntPtrNative where T : unmanaged
    {
        private IntPtr _pointer; //DisPoseを実装すべき
        public NdArray(long[] dimensions, char order = 'C')
        {
            int nd = dimensions.Length;
            int itemsize = Marshal.SizeOf(typeof(T));
            _pointer = NdArrayCreate(nd, dimensions, itemsize, order);

            if (_pointer == IntPtr.Zero)
            {
                throw new InvalidOperationException("ndarray_create failed.");
            }
        }
        /// <summary> NdArrayCreate※コンストラクタと同じ動き </summary>
        private static IntPtr NdArrayCreate(int nd, long[] dimensions, int itemsize, char order) => ndarray_create(nd, dimensions, itemsize, order);
        private static NdArray<T> Packing<T>(NdArray<T> src, IntPtr pointer) where T : unmanaged {
            src._pointer = pointer;
            return src;
        }
        /// <summary> NdArrayCopy </summary>
        public static NdArray<T> NdArrayCopy(NdArray<T> src) => Packing(src, ndarray_copy(src._pointer));
        /// <summary> Arange </summary>
        public static NdArray<T> Arange(int end, char order) => np_arange(0, end, 1, order); //C言語内で型を決める必要がある。機能：①TResultで戻り値Tを指定可能。②TResultの指定が無ければ、戻り値の型をC言語が判断する
        public static NdArray<T> Arange(int start, int end, char order) => np_arange(start, end, 1, order);
        public static NdArray<T> Arange(int start, int end, int step, char order) => np_arange(start, end, step, order); //arangeのTResult宣言から
        /// <summary> DArange </summary>
        public static NdArray<T> DArange(double start, double end, double step, char order) => np_d_arange(start, end, step, order);
        /// <summary> Cast </summary>
        public static NdArray<TResult> Cast<TSource, TResult>(NdArray<TSource> source) where TSource : unmanaged where TResult : unmanaged {
            NdArray<TResult> result = default;
            var pointer = ndarray_copy(source._pointer);
            result._pointer = pointer;
            result._pointer = NdArrayCast<TSource, TResult>(source._pointer);
            return result;
        }
    }

    /// <summary> Native Code for NdArray </summary>
    public abstract class IntPtrNative
    {
        private const string DLL_Name = "IntPtrNative";

        /// <summary> NdArrayCreate </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr ndarray_create(int nd, long[] dimensions, int itemsize, char order);
        /// <summary> NdArrayCopy </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr ndarray_copy(IntPtr src);
        /// <summary> Arange </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_arange(int start, int end, int step, char order); //C#でオーバーロード
        /// <summary> DArange </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_d_arange(double start, double end, double step, char order);
        // []
        protected static IntPtr NdArrayCast<TSource, TResult>(IntPtr pointer) where TSource : unmanaged where TResult : unmanaged 
        {
            SDType srcType, resType;
            switch (typeof(TSource)){ //引数の型
                case Type t when t == typeof(bool): srcType = SDType.Bool; break;
                case Type t when t == typeof(sbyte): srcType = SDType.SByte; break;
                case Type t when t == typeof(byte): srcType = SDType.Byte; break;
                case Type t when t == typeof(short): srcType = SDType.Short; break;
                case Type t when t == typeof(ushort): srcType = SDType.UShort; break;
                case Type t when t == typeof(int): srcType = SDType.Int; break;
                case Type t when t == typeof(uint): srcType = SDType.UInt; break;
                case Type t when t == typeof(long): srcType = SDType.Long; break;
                case Type t when t == typeof(ulong): srcType = SDType.ULong; break;
                case Type t when t == typeof(float): srcType = SDType.Float; break;
                case Type t when t == typeof(double): srcType = SDType.Double; break;
                default: throw new NotSupportedException($"Unsupported type: {typeof(TSource)}");
            }
            switch (typeof(TResult)) { //戻り値の型
                case Type t when t == typeof(bool): resType = SDType.Bool; break;
                case Type t when t == typeof(sbyte): resType = SDType.SByte; break;
                case Type t when t == typeof(byte): resType = SDType.Byte; break;
                case Type t when t == typeof(short): resType = SDType.Short; break;
                case Type t when t == typeof(ushort): resType = SDType.UShort; break;
                case Type t when t == typeof(int): resType = SDType.Int; break;
                case Type t when t == typeof(uint): resType = SDType.UInt; break;
                case Type t when t == typeof(long): resType = SDType.Long; break;
                case Type t when t == typeof(ulong): resType = SDType.ULong; break;
                case Type t when t == typeof(float): resType = SDType.Float; break;
                case Type t when t == typeof(double): resType = SDType.Double; break;
                default: throw new NotSupportedException($"Unsupported type: {typeof(TResult)}");
            }
            // pointerをC言語側へ渡す
            
            return result;
        }
    }
}