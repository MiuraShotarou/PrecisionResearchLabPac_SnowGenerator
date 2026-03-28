using System;
using System.Collections.Generic;

public static class PythonFunction
{
    public static T[] BoolIndex<T>(this T[] arr, bool[] mask)
    {
        var result = new T[arr.Length];
        for (int i = 0; i < arr.Length; i++)
            result[i] = mask[i] ? arr[i] : default;
        return result;
    }
    /// <summary> 引数に渡されたbool[,]ないでtrueに該当する部分にのみレシーバーの値を代入する。（※本来はtrueになった部分の要素のみだけで配列を作成すべきである） </summary>
    public static Array BoolIndex<T>(this T[,] arr, bool[,] mask)
    {
        int rows   = arr.GetLength(0);
        int cols   = arr.GetLength(1);
        var result = new T[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i, j] = mask[i, j] ? arr[i, j] : default;  // falseは0埋め
        return result;
    }
    
    // true の要素だけ抽出した1次元配列を返す（本来のブールインデックス）
    public static T[] BoolIndexExtract<T>(this T[,] arr, bool[,] mask)
    {
        var result = new List<T>();
        for (int i = 0; i < arr.GetLength(0); i++)
        for (int j = 0; j < arr.GetLength(1); j++)
            if (mask[i, j]) result.Add(arr[i, j]);
        return result.ToArray();
    }
    public static T[] BoolIndexExtract<T>(this T[] arr, bool[] mask)
    {
        var result = new List<T>();
        for (int i = 0; i < arr.Length; i++)
            if (mask[i]) result.Add(arr[i]);
        return result.ToArray();
    }

    // ----------------------------------------------------------------
    // ブールインデックス（書き込み）
    // arr[bool配列] = values に相当
    // ----------------------------------------------------------------
    public static void BoolIndexSet<T>(this T[,] arr, bool[,] mask, T[] values)
    {
        int idx = 0;
        for (int i = 0; i < arr.GetLength(0); i++)
        for (int j = 0; j < arr.GetLength(1); j++)
            if (mask[i, j]) arr[i, j] = values[idx++];
    }

    // ----------------------------------------------------------------
    // ブールインデックス（書き込み・スカラー）
    // arr[bool配列] = scalar に相当
    // ----------------------------------------------------------------
    public static void BoolIndexSet<T>(this T[,] arr, bool[,] mask, T value)
    {
        for (int i = 0; i < arr.GetLength(0); i++)
        for (int j = 0; j < arr.GetLength(1); j++)
            if (mask[i, j]) arr[i, j] = value;
    }

    // ----------------------------------------------------------------
    // ファンシーインデックス（読み取り）
    // arr[int配列] に相当
    // ----------------------------------------------------------------
    public static Array FancyIndex<T>(this T[] arr, int[] indices)
    {
        var result = new T[indices.Length];
        for (int i = 0; i < indices.Length; i++)
            result[i] = arr[indices[i]];
        return result;
    }

    // ----------------------------------------------------------------
    // ファンシーインデックス（書き込み）
    // arr[int配列] = values に相当
    // ----------------------------------------------------------------
    public static void FancyIndexSet<T>(this T[] arr, int[] indices, T[] values)
    {
        for (int i = 0; i < indices.Length; i++)
            arr[indices[i]] = values[i];
    }

    // ----------------------------------------------------------------
    // ファンシーインデックス（書き込み・スカラー）
    // arr[int配列] = scalar に相当
    // ----------------------------------------------------------------
    public static void FancyIndexSet<T>(this T[] arr, int[] indices, T value)
    {
        for (int i = 0; i < indices.Length; i++)
            arr[indices[i]] = value;
    }
    
    /// <summary> 加算 </summary>
    public static Array Add(this Array a, Array b, bool inplace = false)
    {
        int rows   = a.GetLength(0);
        int cols   = a.GetLength(1);
        var result = inplace ? a : new float[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.SetValue(Convert.ToSingle(a.GetValue(i, j)) + Convert.ToSingle(b.GetValue(i, j)), i, j); //ここ
        return result;
    }

    /// <summary> 減算 </summary>
    public static Array Sub(this Array a, Array b, bool inplace = false)
    {
        int rows   = a.GetLength(0);
        int cols   = a.GetLength(1);
        var result = inplace ? a : new float[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.SetValue(Convert.ToSingle(a.GetValue(i, j)) - Convert.ToSingle(b.GetValue(i, j)), i, j);
        return result;
    }

    /// <summary> 乗算 </summary>
    public static Array Mul(this Array a, Array b, bool inplace = false)
    {
        int rows   = a.GetLength(0);
        int cols   = a.GetLength(1);
        var result = inplace ? a : new float[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.SetValue(Convert.ToSingle(a.GetValue(i, j)) * Convert.ToSingle(b.GetValue(i, j)), i, j); //ここ
        return result;
    }

    /// <summary> 除算 </summary>
    public static Array Div(this Array a, Array b, bool inplace = false)
    {
        int rows   = a.GetLength(0);
        int cols   = a.GetLength(1);
        var result = inplace ? a : new float[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.SetValue(Convert.ToSingle(a.GetValue(i, j)) / Convert.ToSingle(b.GetValue(i, j)), i, j);
        return result;
    }

    /// <summary> 剰余 </summary>
    public static Array Mod(this Array a, Array b, bool inplace = false)
    {
        int rows   = a.GetLength(0);
        int cols   = a.GetLength(1);
        var result = inplace ? a : new float[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.SetValue(Convert.ToSingle(a.GetValue(i, j)) % Convert.ToSingle(b.GetValue(i, j)), i, j);
        return result;
    }
}