using System;
using System.Runtime.InteropServices;

namespace SnowflakeNative
{
    public static class Error 
    {
        /// <summary> エラー </summary>
        public static string GetErrorMessage() => Marshal.PtrToStringAnsi(get_error_message()) ?? "unknown error.";
        
        private const string DLL_Name = "CLanguageNative";
        /// <summary> GetErrorMessage </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr get_error_message();
    }
    

}