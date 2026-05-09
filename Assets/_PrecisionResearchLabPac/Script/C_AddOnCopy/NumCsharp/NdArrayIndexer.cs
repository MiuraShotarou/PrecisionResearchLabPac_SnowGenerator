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
    }
    // 設計：
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        /// <summary>  </summary>
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
                int nd = mask.Rank;
                long[] dimensions = new long[nd];
                for (int i = 0; i < nd; i++) {
                    dimensions[i] = mask.GetLength(i);  //各次元のサイズを取得
                }
                SDType sdtype= TypeToSDType(mask.GetType().GetElementType());
                int itemsize = ItemSizeCastBySDtype(sdtype);
                IntPtr result = ndarray_create(nd, dimensions, itemsize, 'C'); //値がコピーされていない → 要素を代入する処理だけC言語側で書けば良い → Array → NdArrayへの変換処理はC言語側へ
                
                return Packing(new NdArray<T>(), result);
            }
            set
            {
                IntPtr mask_ndarray = ArrayToNdArray(mask);
                this._pointer = set_ndarray_advancedindexing(this._pointer, mask_ndarray); //破壊的操作で良いかも
            }
        }
    }
}