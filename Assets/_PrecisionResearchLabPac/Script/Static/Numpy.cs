using System;
using System.Collections.Generic;

public static class Numpy
{
    // ----------------------------------------------------------------
    // ユーティリティ：Array から要素を取得・設定
    // ----------------------------------------------------------------
    private static object Get(Array arr, params int[] indices) => arr.GetValue(indices);
    private static void   Set(Array arr, object val, params int[] indices) => arr.SetValue(val, indices);

    // ----------------------------------------------------------------
    // np.arange(stop)
    // 0 から stop-1 までの連番配列を生成
    // ----------------------------------------------------------------
    public static Array Arange(int stop)
    {
        var result = new int[stop];
        for (int i = 0; i < stop; i++)
            result[i] = i;
        return result;
    }

    // ----------------------------------------------------------------
    // np.arange(start, stop)
    // ----------------------------------------------------------------
    public static Array Arange(int start, int stop)
    {
        var result = new int[stop - start];
        for (int i = 0; i < result.Length; i++)
            result[i] = start + i;
        return result;
    }

    // ----------------------------------------------------------------
    // np.reshape(arr, (rows, cols))
    // 1次元配列を2次元配列に変換
    // ----------------------------------------------------------------
    public static Array Reshape(Array arr, int rows, int cols)
    {
        if (arr.Length != rows * cols)
            throw new ArgumentException($"要素数が一致しません: {arr.Length} != {rows} * {cols}");

        var result = Array.CreateInstance(arr.GetType().GetElementType(), rows, cols);
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            Set(result, Get(arr, i * cols + j), i, j);
        return result;
    }

    // ----------------------------------------------------------------
    // np.ravel(arr)
    // 任意次元の配列を1次元配列に変換
    // ----------------------------------------------------------------
    public static T[] Ravel<T>(Array arr)
    {
        var result = new T[arr.Length];
        int index  = 0;
        foreach (var item in arr)
            result[index++] = (T)item;
        return result;
    }

    // ----------------------------------------------------------------
    // np.stack(arrays, axis)
    // 複数の配列を指定した軸方向に積み重ねる
    // ----------------------------------------------------------------
    public static Array Stack(List<Array> arrays, AxisType axis)
    {
        if (arrays.Count == 0)
            throw new ArgumentException("配列が空です");

        Type   elemType = arrays[0].GetType().GetElementType();
        int    rows     = arrays[0].GetLength(0);
        int    cols     = arrays[0].GetLength(1);
        int    depth    = arrays.Count;

        switch (axis)
        {
            case AxisType.Row:
            {
                // (size, size) × n → (n, size, size)
                var result = Array.CreateInstance(elemType, depth, rows, cols);
                for (int i = 0; i < depth; i++)
                for (int j = 0; j < rows;  j++)
                for (int k = 0; k < cols;  k++)
                    Set(result, Get(arrays[i], j, k), i, j, k);
                return result;
            }
            case AxisType.Col:
            {
                // (size, size) × n → (size, n, size)
                var result = Array.CreateInstance(elemType, rows, depth, cols);
                for (int i = 0; i < rows;  i++)
                for (int j = 0; j < depth; j++)
                for (int k = 0; k < cols;  k++)
                    Set(result, Get(arrays[j], i, k), i, j, k);
                return result;
            }
            case AxisType.None:
            {
                // (size, size) × n → (size, size, n)
                var result = Array.CreateInstance(elemType, rows, cols, depth);
                for (int i = 0; i < rows;  i++)
                for (int j = 0; j < cols;  j++)
                for (int k = 0; k < depth; k++)
                    Set(result, Get(arrays[k], i, j), i, j, k);
                return result;
            }
            default:
                throw new ArgumentException($"不正なaxisです: {axis}");
        }
    }

    // ----------------------------------------------------------------
    // np.column_stack([a, b, c])
    // 複数の1次元配列を列方向に結合して2次元配列にする
    // ----------------------------------------------------------------
    public static Array ColumnStack(List<Array> arrays)
    {
        if (arrays.Count == 0)
            throw new ArgumentException("配列が空です");

        Type elemType = arrays[0].GetType().GetElementType();
        int  rows     = arrays[0].Length;
        int  cols     = arrays.Count;
        var  result   = Array.CreateInstance(elemType, rows, cols);

        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            Set(result, Get(arrays[j], i), i, j);
        return result;
    }

    // ----------------------------------------------------------------
    // np.vstack([a, b])
    // 複数の2次元配列を行方向に結合する
    // ----------------------------------------------------------------
    public static Array VStack(List<Array> arrays)
    {
        if (arrays.Count == 0)
            throw new ArgumentException("配列が空です");

        Type elemType  = arrays[0].GetType().GetElementType();
        int  cols      = arrays[0].GetLength(1);
        int  totalRows = 0;

        foreach (var arr in arrays)
        {
            if (arr.GetLength(1) != cols)
                throw new ArgumentException("列数が一致しません");
            totalRows += arr.GetLength(0);
        }

        var result     = Array.CreateInstance(elemType, totalRows, cols);
        int currentRow = 0;

        foreach (var arr in arrays)
        {
            int rows = arr.GetLength(0);
            for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                Set(result, Get(arr, i, j), currentRow + i, j);
            currentRow += rows;
        }
        return result;
    }

    // ----------------------------------------------------------------
    // np.logical_and(a, b)
    // 2つのbool配列の各要素にANDを適用する
    // ----------------------------------------------------------------
    public static Array LogicalAnd(Array a, Array b)
    {
        if (a.Length != b.Length)
            throw new ArgumentException("配列のサイズが一致しません");

        int rank = a.Rank;

        if (rank == 1)
        {
            var result = new bool[a.Length];
            for (int i = 0; i < a.Length; i++)
                result[i] = (bool)Get(a, i) && (bool)Get(b, i);
            return result;
        }
        else
        {
            int rows   = a.GetLength(0);
            int cols   = a.GetLength(1);
            var result = new bool[rows, cols];
            for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                result[i, j] = (bool)Get(a, i, j) && (bool)Get(b, i, j);
            return result;
        }
    }

    // ----------------------------------------------------------------
    // np.ufunc.reduce(arr, axis)
    // 1次元配列版：全要素にfuncを適用して1つの値を返す
    // ----------------------------------------------------------------
    public static object Reduce(this Array arr, Func<object, object, object> func, object identity)
    {
        object acc = identity;
        foreach (var item in arr)
            acc = func(acc, item);
        return acc;
    }

    // ----------------------------------------------------------------
    // np.ufunc.reduce(arr, axis)
    // 2次元配列版：指定した軸方向にfuncを適用する
    // ----------------------------------------------------------------
    public static Array Reduce(this Array arr, Func<object, object, object> func, object identity, AxisType axis)
    {
        int rows = arr.GetLength(0);
        int cols = arr.GetLength(1);

        switch (axis)
        {
            case AxisType.Col:
            {
                var result = Array.CreateInstance(arr.GetType().GetElementType(), cols);
                for (int j = 0; j < cols; j++)
                {
                    object acc = identity;
                    for (int i = 0; i < rows; i++)
                        acc = func(acc, Get(arr, i, j));
                    result.SetValue(acc, j);
                }
                return result;
            }
            case AxisType.Row:
            {
                var result = Array.CreateInstance(arr.GetType().GetElementType(), rows);
                for (int i = 0; i < rows; i++)
                {
                    object acc = identity;
                    for (int j = 0; j < cols; j++)
                        acc = func(acc, Get(arr, i, j));
                    result.SetValue(acc, i);
                }
                return result;
            }
            case AxisType.None:
            {
                object acc = identity;
                for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                    acc = func(acc, Get(arr, i, j));
                var result = Array.CreateInstance(arr.GetType().GetElementType(), 1);
                result.SetValue(acc, 0);
                return result;
            }
            default:
                throw new ArgumentException($"不正なaxisです: {axis}");
        }
    }

    // ----------------------------------------------------------------
    // vertex_mask[triangles]
    // bool配列をマスクとして2次元int配列の各要素に適用する
    // ----------------------------------------------------------------
    public static Array IndexWithMask(Array mask, Array indices)
    {
        int rows   = indices.GetLength(0);
        int cols   = indices.GetLength(1);
        var result = new bool[rows, cols];

        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            int idx = (int)Get(indices, i, j);
            result[i, j] = (bool)mask.GetValue(idx);
        }
        return result;
    }

    // ----------------------------------------------------------------
    // np.cumsum(arr) - arr
    // 累積和を計算して元の配列を引く
    // ----------------------------------------------------------------
    public static Array CumSumMinusSelf(Array mask)
    {
        var result = new int[mask.Length];
        int sum    = 0;
        for (int i = 0; i < mask.Length; i++)
        {
            result[i] = sum;
            if ((bool)mask.GetValue(i)) sum++;
        }
        return result;
    }

    // ----------------------------------------------------------------
    // vertex_indices[triangles]
    // remapテーブルで2次元int配列の全要素を変換する
    // ----------------------------------------------------------------
    public static Array RemapIndices(Array remap, Array indices)
    {
        int rows   = indices.GetLength(0);
        int cols   = indices.GetLength(1);
        var result = new int[rows, cols];

        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            int idx    = (int)Get(indices, i, j);
            result[i, j] = (int)remap.GetValue(idx);
        }
        return result;
    }

    // ----------------------------------------------------------------
    // np.indices((rows, cols))
    // 各セルの行・列インデックスを値として持つ配列を生成
    // ----------------------------------------------------------------
    public static (Array x, Array y) Indices(int rows, int cols)
    {
        var x = new float[rows, cols];
        var y = new float[rows, cols];
        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            x[i, j] = i;
            y[i, j] = j;
        }
        return (x, y);
    }

    // ----------------------------------------------------------------
    // np.where(condition, ifTrue, ifFalse)
    // 条件に応じて2つの値を選択する
    // ----------------------------------------------------------------
    public static Array Where(Array condition, float ifTrue, Array ifFalse)
    {
        int rows   = condition.GetLength(0);
        int cols   = condition.GetLength(1);
        var result = new float[rows, cols];

        for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i, j] = (bool)Get(condition, i, j) ? ifTrue : (float)Get(ifFalse, i, j);
        return result;
    }

    // ----------------------------------------------------------------
    // np.pad(arr, padWidth, mode, padValue)
    // 配列の周囲にパディングを追加する
    // ----------------------------------------------------------------
    public static Array Pad(Array arr, int padWidth, ModeType modeType = ModeType.Constant, object padValue = null)
    {
        Type elemType = arr.GetType().GetElementType();
        int  rows     = arr.GetLength(0);
        int  cols     = arr.GetLength(1);
        int  newRows  = rows + padWidth * 2;
        int  newCols  = cols + padWidth * 2;
        var  result   = Array.CreateInstance(elemType, newRows, newCols);

        if (padValue == null)
            padValue = elemType.IsValueType ? Activator.CreateInstance(elemType) : null;

        switch (modeType)
        {
            case ModeType.Constant:
            {
                for (int i = 0; i < newRows; i++)
                for (int j = 0; j < newCols; j++)
                    Set(result, padValue, i, j);

                for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                    Set(result, Get(arr, i, j), i + padWidth, j + padWidth);

                return result;
            }
            case ModeType.Edge:
            {
                for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                    Set(result, Get(arr, i, j), i + padWidth, j + padWidth);

                for (int i = 0; i < padWidth; i++)
                for (int j = 0; j < cols; j++)
                {
                    Set(result, Get(arr, 0,        j), i,                  j + padWidth);
                    Set(result, Get(arr, rows - 1, j), rows + padWidth + i, j + padWidth);
                }

                for (int i = 0; i < newRows; i++)
                for (int j = 0; j < padWidth; j++)
                {
                    int srcI = Math.Clamp(i - padWidth, 0, rows - 1);
                    Set(result, Get(arr, srcI, 0),        i, j);
                    Set(result, Get(arr, srcI, cols - 1), i, cols + padWidth + j);
                }
                return result;
            }
            case ModeType.Reflect:
            {
                for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                    Set(result, Get(arr, i, j), i + padWidth, j + padWidth);

                for (int i = 0; i < padWidth; i++)
                for (int j = 0; j < cols; j++)
                {
                    Set(result, Get(arr, i,            j), padWidth - 1 - i,     j + padWidth);
                    Set(result, Get(arr, rows - 1 - i, j), rows + padWidth + i,  j + padWidth);
                }

                for (int i = 0; i < newRows; i++)
                for (int j = 0; j < padWidth; j++)
                {
                    int srcI = Math.Clamp(i - padWidth, 0, rows - 1);
                    Set(result, Get(arr, srcI, j),            i, padWidth - 1 - j);
                    Set(result, Get(arr, srcI, cols - 1 - j), i, cols + padWidth + j);
                }
                return result;
            }
            case ModeType.Wrap:
            {
                for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                    Set(result, Get(arr, i, j), i + padWidth, j + padWidth);

                for (int i = 0; i < padWidth; i++)
                for (int j = 0; j < cols; j++)
                {
                    Set(result, Get(arr, rows - padWidth + i, j), i,                  j + padWidth);
                    Set(result, Get(arr, i,                   j), rows + padWidth + i, j + padWidth);
                }

                for (int i = 0; i < newRows; i++)
                for (int j = 0; j < padWidth; j++)
                {
                    int srcI = Math.Clamp(i - padWidth, 0, rows - 1);
                    Set(result, Get(arr, srcI, cols - padWidth + j), i, j);
                    Set(result, Get(arr, srcI, j),                   i, cols + padWidth + j);
                }
                return result;
            }
            default:
                throw new ArgumentException($"不正なmodeです: {modeType}");
        }
    }

    // ----------------------------------------------------------------
    // arr[mask]
    // bool配列でフィルタリングして1次元配列を返す
    // ----------------------------------------------------------------
    public static Array MaskFilter(Array arr, Array mask)
    {
        Type        elemType = arr.GetType().GetElementType();
        var         result   = new List<object>();

        for (int i = 0; i < arr.Length; i++)
            if ((bool)mask.GetValue(i))
                result.Add(arr.GetValue(i));

        var output = Array.CreateInstance(elemType, result.Count);
        for (int i = 0; i < result.Count; i++)
            output.SetValue(result[i], i);
        return output;
    }
    /// <summary> レシーバーの型を型パラメータに指定した型のタプルへ変換する </summary>
    public static (T, T) Cast<T>(this (Array, Array) tuple)
    {
        return ((T)(object)tuple.Item1, (T)(object)tuple.Item2);
    }
    /// <summary> 引数の型を型パラメータに指定した型へ変換する </summary>
    public static (T1, T2) Cast<T1, T2>(this (Array, Array) tuple)
    {
        return ((T1)(object)tuple.Item1, (T2)(object)tuple.Item2);
    }
}

public enum AxisType
{
    Col,  // 列方向
    Row,  // 行方向
    None  // 全要素
}

public enum ModeType
{
    Constant,
    Edge,
    Reflect,
    Wrap
}
