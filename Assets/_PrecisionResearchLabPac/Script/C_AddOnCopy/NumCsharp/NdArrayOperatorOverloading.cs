// NdArrayOperatorOverloading.cs
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using NUnit.Framework;
using Unity.VisualScripting;
using UnityEngine;

namespace SnowflakeNative
{
    public abstract partial class CSLanguageNative : CLanguageNative
    {
         
    }
    
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
            return Packing(new NdArray<T>(), ArrayToNdArray(value));
        }
        // public static implicit operator NdArray<T>(List<T> value) //List
        // {
        //     // TODO
        // }
        // public static implicit operator NdArray<T>(Stack<T> value) //Stack
        // {
        //     // TODO
        // }
        // ----------------------------------------------------------------
        // 算術演算子
        // ----------------------------------------------------------------
        public static NdArray<T> operator +(NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError("+") : Packing(new NdArray<T>(), np_add(left._pointer, right._pointer, GenericsToSDType<T>()));
        public static NdArray<T> operator -(NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError("-") : Packing(new NdArray<T>(), np_subtract(left._pointer, right._pointer, GenericsToSDType<T>()));
        public static NdArray<T> operator *(NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError("*") : Packing(new NdArray<T>(), np_multiply(left._pointer, right._pointer, GenericsToSDType<T>()));
        public static NdArray<T> operator /(NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError("/") : Packing(new NdArray<T>(), np_divide(left._pointer, right._pointer, GenericsToSDType<T>()));
        public static NdArray<T> operator %(NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError("%") : Packing(new NdArray<T>(), np_modulo(left._pointer, right._pointer, GenericsToSDType<T>()));

        // ----------------------------------------------------------------
        // 比較演算子 → 戻り値は NdArray<bool>
        // ----------------------------------------------------------------
        public static NdArray<bool> operator ==(NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError("==") : Packing(new NdArray<bool>(), np_equal(left._pointer, right._pointer));
        public static NdArray<bool> operator !=(NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError("!=") : Packing(new NdArray<bool>(), np_not_equal(left._pointer, right._pointer));
        public static NdArray<bool> operator < (NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError("<")  : Packing(new NdArray<bool>(), np_less(left._pointer, right._pointer));
        public static NdArray<bool> operator > (NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError(">")  : Packing(new NdArray<bool>(), np_greater(left._pointer, right._pointer));
        public static NdArray<bool> operator <=(NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError("<=") : Packing(new NdArray<bool>(), np_less_equal(left._pointer, right._pointer));
        public static NdArray<bool> operator >=(NdArray<T> left, NdArray<T> right) => typeof(T) == typeof(bool) ? throw OpError(">=") : Packing(new NdArray<bool>(), np_greater_equal(left._pointer, right._pointer));

        public static NdArray<bool> operator &(NdArray<T> left, NdArray<T> right) => typeof(T) != typeof(bool) ? throw OpError("&") : Packing(new NdArray<bool>(), np_logical_and(left._pointer, right._pointer));
        public static NdArray<bool> operator |(NdArray<T> left, NdArray<T> right) => typeof(T) != typeof(bool) ? throw OpError("|") : Packing(new NdArray<bool>(), np_logical_or(left._pointer, right._pointer));
        public static NdArray<bool> operator ^(NdArray<T> left, NdArray<T> right) => typeof(T) != typeof(bool) ? throw OpError("^") : Packing(new NdArray<bool>(), np_logical_xor(left._pointer, right._pointer));

        // ----------------------------------------------------------------
        // 単項演算子
        // ----------------------------------------------------------------
        public static NdArray<bool> operator !(NdArray<T> src) => typeof(T) != typeof(bool) ? throw OpError("!") : Packing(new NdArray<bool>(), np_logical_not(src._pointer));
        public static NdArray<T>    operator -(NdArray<T> src) => typeof(T) == typeof(bool) ? throw OpError("-") : Packing(new NdArray<T>(), np_negative(src._pointer));

        // ----------------------------------------------------------------
        // == と != をオーバーロードした場合、以下も必須
        // ----------------------------------------------------------------
        public override bool Equals(object obj) => obj is NdArray<T> other && this._pointer == other._pointer;
        public override int GetHashCode()       => _pointer.GetHashCode();
            
        // Error
        private static NotSupportedException OpError(string op) => new ($"Operator '{op}' is not supported for NdArray<{typeof(T).Name}>.");
    }
    
    public abstract partial class CLanguageNative
    {
        // ----------------------------------------------------------------
        // 算術演算系
        // ----------------------------------------------------------------
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_add     (IntPtr a, IntPtr b, SDType restype);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_subtract(IntPtr a, IntPtr b, SDType restype);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_multiply(IntPtr a, IntPtr b, SDType restype);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_divide  (IntPtr a, IntPtr b, SDType restype);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_modulo  (IntPtr a, IntPtr b, SDType restype);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_negative(IntPtr src);

        // ----------------------------------------------------------------
        // 比較演算系
        // ----------------------------------------------------------------
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_less         (IntPtr a, IntPtr b);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_less_equal   (IntPtr a, IntPtr b);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_greater      (IntPtr a, IntPtr b);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_greater_equal(IntPtr a, IntPtr b);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_equal        (IntPtr a, IntPtr b);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_not_equal    (IntPtr a, IntPtr b);

        // ----------------------------------------------------------------
        // 論理演算系
        // ----------------------------------------------------------------
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_logical_and(IntPtr a, IntPtr b);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_logical_or (IntPtr a, IntPtr b);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_logical_xor(IntPtr a, IntPtr b);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_logical_not(IntPtr a);
    }
}