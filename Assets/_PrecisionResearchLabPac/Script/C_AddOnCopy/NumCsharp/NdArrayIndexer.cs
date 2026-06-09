// NdArrayIndexer.cs
using System;
using System.Runtime.InteropServices;

namespace SnowflakeNative
{
    // ----------------------------------------------------------------
    // AdvancedIndexing
    // ----------------------------------------------------------------
    public interface INdArray
    {
        internal IntPtr _pointer { get; }
        internal SDType _sdtype  { get; }
        internal bool   IsReference  { get; }
    }
    public abstract partial class CLanguageNative
    {
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr get_ndarray_advancedindexing(IntPtr src, IntPtr mask);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern void set_ndarray_advancedindexing(IntPtr src, IntPtr mask, IntPtr value);
    }
    // 設計：C#提供の配列はNdArrayに変換し、その後C言語側で処理をする
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        /// <summary> by NdArray </summary>
        public NdArray<T> this[INdArray mask]
        {
            get
            {
                IntPtr result = IntPtr.Zero;
                SDType maskType = mask._sdtype;
                switch (maskType)
                {
                    case SDType.Bool:
                        result = get_ndarray_advancedindexing(this._pointer, mask._pointer);
                        break;
                    case SDType.Int:
                        result = get_ndarray_advancedindexing(this._pointer, mask._pointer);
                        break;
                    case SDType.SByte:
                    case SDType.Byte:
                    case SDType.Short:
                    case SDType.UShort:
                    case SDType.UInt:
                    case SDType.Long:
                    case SDType.ULong:
                        IntPtr maskPtr = np_cast(mask._pointer, SDType.Int);
                        result = get_ndarray_advancedindexing(this._pointer, maskPtr);
                        ndarray_free(maskPtr);
                        break;
                    case SDType.Float:
                    case SDType.Double:
                        throw new NotSupportedException($"Floating point types cannot be used as indices: {maskType}"); //error:浮動小数点型はインデックスに使用できません
                    default:
                        throw new NotSupportedException($"Unsupported SDType for indexing: {maskType}");
                }
                return Packing(new NdArray<T>(), result);
            }
            set // NdArrayOperatorOverloading.csの実装により、スカラーや配列もvalueに指定可能
            {
                // maskの型をint, boolで固定できるようにする
                SDType maskType = mask._sdtype;
                switch (maskType)
                {
                    case SDType.Bool:
                        set_ndarray_advancedindexing(this._pointer, mask._pointer, value._pointer);
                        break;
                    case SDType.Int:
                        set_ndarray_advancedindexing(this._pointer, mask._pointer, value._pointer);
                        break;
                    case SDType.SByte:
                    case SDType.Byte:
                    case SDType.Short:
                    case SDType.UShort:
                    case SDType.UInt:
                    case SDType.Long:
                    case SDType.ULong:
                        IntPtr maskPtr = np_cast(mask._pointer, SDType.Int);
                        set_ndarray_advancedindexing(this._pointer, maskPtr, value._pointer);
                        ndarray_free(maskPtr);
                        break;
                    case SDType.Float:
                    case SDType.Double:
                        throw new NotSupportedException($"Floating point types cannot be used as indices: {maskType}");
                    default:
                        throw new NotSupportedException($"Unsupported SDType for indexing: {maskType}");
                }
            }
        }
        public NdArray<T> this[Array mask] //mask をNdArrayに変換する
        {
            get
            {
                IntPtr result = IntPtr.Zero;
                // NdArray の各要素を取得
                int nd = ArrayNd(mask);
                long[] dimensions = ArrayDimensions(mask, nd);
                SDType sdtype = ArraySDtype(mask);
                int itemsize = ItemSizeCastBySDtype(sdtype);
                IntPtr nd_mask = ndarray_convert(this._pointer, nd, dimensions, itemsize, sdtype); //値がコピーされていない → ndarray_convertを使用
                switch (sdtype)
                {
                    case SDType.Bool:
                        result = get_ndarray_advancedindexing(this._pointer, nd_mask);
                        break;
                    case SDType.Int:
                        result = get_ndarray_advancedindexing(this._pointer, nd_mask);
                        break;
                    case SDType.Long:
                    case SDType.UInt:
                    case SDType.ULong:
                    case SDType.UShort:
                    case SDType.Short:
                    case SDType.SByte:
                    case SDType.Byte:
                        nd_mask = np_cast(nd_mask, SDType.Int);
                        result = get_ndarray_advancedindexing(this._pointer, nd_mask);
                        break;
                    default:
                        throw new NotSupportedException($"Unsupported SDType for indexing: {sdtype}");
                }
                return Packing(new NdArray<T>(), nd_mask);
            }
            set
            {
                IntPtr mask_ndarray = ArrayToNdArray(mask);
                set_ndarray_advancedindexing(this._pointer, mask_ndarray, value._pointer);
            }
        }
    }

    public abstract partial class CSLanguageNative : CLanguageNative
    {
         
    }
    
    // ----------------------------------------------------------------
    // Slicing
    // ----------------------------------------------------------------
    [StructLayout(LayoutKind.Sequential)]
    public struct SliceStruct
    {
        public int Start, Stop, Step;
        public static implicit operator SliceStruct((int start, int stop, int step) t) => new SliceStruct { Start = t.start, Stop = t.stop, Step = t.step };
        public static implicit operator SliceStruct((int start, int stop) t) => new SliceStruct { Start = t.start, Stop = t.stop, Step = 1 };
        public static implicit operator SliceStruct(int start) => new SliceStruct { Start = start, Stop = start + 1, Step = 1 };
    }
    
    public abstract partial class CLanguageNative
    {
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr get_ndarray_slicing(IntPtr src, SliceStruct[] slices, int slice_nd);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern void set_ndarray_slicing(IntPtr src, SliceStruct[] slices, int slice_nd, IntPtr value);
    }
    
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        public NdArray<T> this[params SliceStruct[] slices]
        {
            get
            {
                IntPtr resultPtr = get_ndarray_slicing(this._pointer, slices, slices.Length);
                return Packing(new NdArray<T>(), resultPtr);
            }
            set
            {
                set_ndarray_slicing(this._pointer, slices, slices.Length, value._pointer);
            }
        }
    }
    
    public abstract partial class CSLanguageNative : CLanguageNative
    {
         
    }
}