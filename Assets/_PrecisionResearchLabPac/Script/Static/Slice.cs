using System;
/// <summary>
/// Pythonのスライス操作を C# で再現するユーティリティ
/// arr[1:]  arr[:-1]  arr[1:, :-1] などに相当
/// </summary>
public static class SliceUtils
{
    /// <summary>
    /// 1次元配列用
    /// </summary>
    public static T[] Slice<T>(this T[] arr, int start = 0, int stop = int.MinValue){
        int len       = arr.Length;
        int actualStop = stop == int.MinValue ? len : (stop < 0 ? len + stop : stop);
        int actualStart = start < 0 ? len + start : start;
        int size   = actualStop - actualStart;
        var result = new T[size];
        for (int i = 0; i < size; i++)
            result[i] = arr[actualStart + i];
        return result;}
    /// <summary>
    /// 2次元配列用
    /// </summary>
    public static T[,] Slice<T>(this T[,] arr,int rowStart = 0,int rowStop = int.MinValue,int colStart = 0,int colStop = int.MinValue){
        int rows = arr.GetLength(0);
        int cols = arr.GetLength(1);
        int rStart = rowStart < 0 ? rows + rowStart : rowStart;
        int rStop  = rowStop  == int.MinValue ? rows : (rowStop  < 0 ? rows + rowStop  : rowStop);
        int cStart = colStart < 0 ? cols + colStart : colStart;
        int cStop  = colStop  == int.MinValue ? cols : (colStop  < 0 ? cols + colStop  : colStop);
        int rSize  = rStop - rStart;
        int cSize  = cStop - cStart;
        var result = new T[rSize, cSize];
        for (int i = 0; i < rSize; i++)
        for (int j = 0; j < cSize; j++)
            result[i, j] = arr[rStart + i, cStart + j];
        return result;}
}