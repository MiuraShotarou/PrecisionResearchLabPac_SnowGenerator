// NdArray_L.cs
using System;
using System.Runtime.InteropServices;

namespace SnowflakeNative
{
    /// <summary> Collection is NdArray </summary>
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        public UL ul => new UL(this);
        public class UL
        {
            private NdArray<T> _outer;
            public UL(NdArray<T> outer)
            {
                _outer = outer;
            }
        }
    }

    public abstract partial class CSLanguageNative : CLanguageNative
    {
    }

    public abstract partial class CLanguageNative
    {

    }
}