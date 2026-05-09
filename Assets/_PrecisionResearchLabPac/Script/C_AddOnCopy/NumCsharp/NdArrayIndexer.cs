using System;
using System.Runtime.InteropServices;

namespace SnowflakeNative
{
    public interface INdArray
    {
        IntPtr _pointer { get; }
        // SDType Type { get; } //不要？
        // long[] Shape { get; }//
    }
    public abstract partial class CLanguageNatives
    {
        
    }
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        public NdArray<T> this[NdArray<T> src, INdArray mask]
        {
            
        }
        public class NdArrayArgument
        {
            private IntPtr _pointer;
        }
    }
}