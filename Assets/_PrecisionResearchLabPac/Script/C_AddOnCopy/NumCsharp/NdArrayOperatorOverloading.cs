// NdArrayOperatorOverloading.cs
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using NUnit.Framework;
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
            return Packing(new NdArray<T>(), ArrayToNdArray(value));
        }
        public static implicit operator NdArray<T>(List<T> value) //List
        {
            // TODO
        }
        public static implicit operator NdArray<T>(Stack<T> value) //Stack
        {
            // TODO
        }
        // ----------------------------------------------------------------
        // 算術演算子
        // ----------------------------------------------------------------
        public static NdArray<T> operator +(NdArray<T> left, NdArray<T> right)   => Packing(new NdArray<T>(), np_add(left._pointer, right._pointer));
        public static NdArray<T> operator +(NdArray<T> left, T right)            => Packing(new NdArray<T>(), np_add_scalar(left._pointer, right));
        public static NdArray<T> operator +(T left, NdArray<T> right)            => Packing(new NdArray<T>(), np_add_scalar(right._pointer, left));

        public static NdArray<T> operator -(NdArray<T> left, NdArray<T> right)   => Packing(new NdArray<T>(), np_subtract(left._pointer, right._pointer));
        public static NdArray<T> operator -(NdArray<T> left, T right)            => Packing(new NdArray<T>(), np_subtract_scalar(left._pointer, right));
        public static NdArray<T> operator -(T left, NdArray<T> right)            => Packing(new NdArray<T>(), np_subtract_scalar_left(left, right._pointer));

        public static NdArray<T> operator *(NdArray<T> left, NdArray<T> right)   => Packing(new NdArray<T>(), np_multiply(left._pointer, right._pointer));
        public static NdArray<T> operator *(NdArray<T> left, T right)            => Packing(new NdArray<T>(), np_multiply_scalar(left._pointer, right));
        public static NdArray<T> operator *(T left, NdArray<T> right)            => Packing(new NdArray<T>(), np_multiply_scalar(right._pointer, left));

        public static NdArray<T> operator /(NdArray<T> left, NdArray<T> right)   => Packing(new NdArray<T>(), np_divide(left._pointer, right._pointer));
        public static NdArray<T> operator /(NdArray<T> left, T right)            => Packing(new NdArray<T>(), np_divide_scalar(left._pointer, right));
        public static NdArray<T> operator /(T left, NdArray<T> right)            => Packing(new NdArray<T>(), np_divide_scalar_left(left, right._pointer));

        public static NdArray<T> operator %(NdArray<T> left, NdArray<T> right)   => Packing(new NdArray<T>(), np_modulo(left._pointer, right._pointer));
        public static NdArray<T> operator %(NdArray<T> left, T right)            => Packing(new NdArray<T>(), np_modulo_scalar(left._pointer, right));
        public static NdArray<T> operator %(T left, NdArray<T> right)            => Packing(new NdArray<T>(), np_modulo_scalar_left(left, right._pointer));

        public static NdArray<T> operator -(NdArray<T> src)                      => Packing(new NdArray<T>(), np_negative(src._pointer));

        // ----------------------------------------------------------------
        // 比較演算子 → 戻り値は NdArray<bool>
        // ----------------------------------------------------------------
        public static NdArray<bool> operator ==(NdArray<T> left, NdArray<T> right)  => Packing(new NdArray<bool>(), np_equal(left._pointer, right._pointer));
        public static NdArray<bool> operator ==(NdArray<T> left, T right)           => Packing(new NdArray<bool>(), np_equal_scalar(left._pointer, right));

        public static NdArray<bool> operator !=(NdArray<T> left, NdArray<T> right)  => Packing(new NdArray<bool>(), np_not_equal(left._pointer, right._pointer));
        public static NdArray<bool> operator !=(NdArray<T> left, T right)           => Packing(new NdArray<bool>(), np_not_equal_scalar(left._pointer, right));

        public static NdArray<bool> operator <(NdArray<T> left, NdArray<T> right)   => Packing(new NdArray<bool>(), np_less(left._pointer, right._pointer));
        public static NdArray<bool> operator <(NdArray<T> left, T right)            => Packing(new NdArray<bool>(), np_less_scalar(left._pointer, right));

        public static NdArray<bool> operator >(NdArray<T> left, NdArray<T> right)   => Packing(new NdArray<bool>(), np_greater(left._pointer, right._pointer));
        public static NdArray<bool> operator >(NdArray<T> left, T right)            => Packing(new NdArray<bool>(), np_greater_scalar(left._pointer, right));

        public static NdArray<bool> operator <=(NdArray<T> left, NdArray<T> right)  => Packing(new NdArray<bool>(), np_less_equal(left._pointer, right._pointer));
        public static NdArray<bool> operator <=(NdArray<T> left, T right)           => Packing(new NdArray<bool>(), np_less_equal_scalar(left._pointer, right));

        public static NdArray<bool> operator >=(NdArray<T> left, NdArray<T> right)  => Packing(new NdArray<bool>(), np_greater_equal(left._pointer, right._pointer));
        public static NdArray<bool> operator >=(NdArray<T> left, T right)           => Packing(new NdArray<bool>(), np_greater_equal_scalar(left._pointer, right));

        // ----------------------------------------------------------------
        // 論理演算子 → 戻り値は NdArray<bool>
        // ----------------------------------------------------------------
        public static NdArray<bool> operator !(NdArray<T> src)                      => Packing(new NdArray<bool>(), np_logical_not(src._pointer));

        // ----------------------------------------------------------------
        // == と != をオーバーロードした場合、以下も必須
        // ----------------------------------------------------------------
        public override bool Equals(object obj)    => obj is NdArray<T> other && this._pointer == other._pointer;
        public override int GetHashCode()          => _pointer.GetHashCode();
    }
}