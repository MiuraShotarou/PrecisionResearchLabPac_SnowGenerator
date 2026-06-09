// NdArray_L.cs
using System;
using System.Runtime.InteropServices;

namespace SnowflakeNative
{
    /// <summary> Collection is NdArray </summary>
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        public D d => new D(this);
        public class D
        {
            private NdArray<T> _outer;
            public D(NdArray<T> outer)
            {
                _outer = outer;
            }
        }
        public static NdArray<T> Arange(double end, char order) => Packing(new NdArray<T>(), CSDArange<T>(0, end, 1, order));
        public static NdArray<T> Arange(double start, double end, char order) => Packing(new NdArray<T>(), CSDArange<T>(start, end, 1, order));
        public static NdArray<T> Arange(double start, double end, double step, char order) => Packing(new NdArray<T>(), CSDArange<T>(start, end, step, order));
    }

    public abstract partial class CSLanguageNative : CLanguageNative
    {
        protected static IntPtr CSDArange<T>(double start, double end, double step, char order) where T : unmanaged
        {
            SDType resType = GenericsToSDType<T>();
            return np_arange(start, end, step, resType, order); //orderはC言語で処理するかFortranで処するかを指定する
        }
    }

    public abstract partial class CLanguageNative
    {

    }
}