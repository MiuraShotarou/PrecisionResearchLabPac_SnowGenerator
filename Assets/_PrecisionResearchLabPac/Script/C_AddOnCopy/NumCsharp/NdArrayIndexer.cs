using System;
using System.Runtime.InteropServices;
using pxr;
using UnityEngine.Rendering;

namespace SnowflakeNative
{
    public interface INdArray
    {
        IntPtr _pointer { get; }
    }
    public abstract partial class CLanguageNative
    {
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr get_ndarray_advancedindexing(IntPtr src, IntPtr mask);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr set_ndarray_advancedindexing(IntPtr src, IntPtr mask);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr ndarray_convert(IntPtr src, int nd, long[] dimensions, int itemsize, SDType sdtype);
    }
    // 設計：C#提供の配列はNdArrayに変換し、その後C言語側で処理をする
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        /// <summary> by NdArray </summary>
        public NdArray<T> this[INdArray mask]
        {
            get
            {
                return Packing(new NdArray<T>(), get_ndarray_advancedindexing(this._pointer, mask._pointer));
            }
            set
            {
                this._pointer = set_ndarray_advancedindexing(this._pointer, mask._pointer); //破壊的操作で良いかも
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
                        nd_mask = np_ndarray_cast(nd_mask, SDType.Int);
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
                this._pointer = set_ndarray_advancedindexing(this._pointer, mask_ndarray); //破壊的操作で良いかも
            }
        }
    }

    public abstract partial class CSLanguageNative : CLanguageNative
    {
         
    }
}