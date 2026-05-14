// NdArrayOperatorOverloading.cs
using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace SnowflakeNative
{
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        // value == structを不可に、stringを有りにする。valueに参照型を渡すことはできない
        /// <summary> Scalar to NdArray<T> at Implicit Conversion </summary>
        public static implicit operator NdArray<T>(T value) //scalar
        {
            return Packing(new NdArray<T>(), CSAsArray(value));
        }
        public static implicit operator NdArray<T>(Array value) //array
        {
            
        }
    }
}