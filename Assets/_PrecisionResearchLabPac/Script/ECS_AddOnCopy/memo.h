// #C long[] → C int64_t* → ndarray_create[dims]

// int itemsize = itemsize_cast_by_sdtype(sdtype)

// C# long[]で行列指定した配列の総要素数計算 → {
    int64_t total = 1;
    for (int i = 0; i < size_nd; i++) {
        total *= dimenstions[i];
    }
}
// C# NdArray呼び出しと整合性のあるindexアクセス手段 → {
DScalarCast cast = dscalar_cast_by_sdtype[sdtype];
for (int i = 0; i < 総要素数; i++) {
    cast(array->data + i * itemsize, value); //char*, double → char* == Index * 型サイズ + 固有アドレスの先頭
    // memcpy(dest, &value, itemsize) //要素の代入
}

//<copy concatenate>
_nx.concatenate
from . import fromnumeric as _from_nx, numeric as _nx, overrides → (.)同パッケージ内の、fromnumeric を _from_nx に(as)リネームする。numeric を (as)_nx にリネームする。
//(overrides)はNumpyの独自モジュールでC#実装するうえでは特に気にする必要はない

_nx == numeric == numeric.py

from .multiarray import ( → multiarray
concatenate

multiarray == multiarray.py

@array_function_from_c_func_and_dispatcher(_multiarray_umath.concatenate) → ()内のC言語関数を()前の名前でラップする
def concatenate(arrays, axis=0, out=None, *, dtype=None, casting="same_kind"):
    """
	"""
    if out is not None: // out 配列が引数にあるなら
        // # optimize for the typical case where only arrays is provided
        arrays = list(arrays) //list() → Pythonの組み込み関数で、イテラブルなオブジェクトをlistに変換する（キャスト処理）
        arrays.append(out) //
    return arrays

array_function_from_c_func_and_dispatcher(_multiarray_umath.concatenate) == _multiarray_umath.py
numpy.numpy.core._multiarray_umath.py == from numpy._core import _multiarray_umath == _core.src._multiarray(フォルダ) + _core.src.umath(フォルダ) + _core.src.common(フォルダ)

_core.src._multiarray.multiarraymodule.c → PyArray_ConcatenateArrays.PyArray_AssignArray() → NPY_cast_info 

//<確実に削除して良いコード群>
NPY_BEGIN_THREADS_THRESHOLDED()
NPY_BEGIN_THREADS_DEF
NPY_BEGIN_THREADS_THRESHOLDED(n)
NPY_END_THREADS
npy_clear_floatstatus_barrier
npy_get_floatstatus_barrier
PyUFunc_GiveFloatingpointErrors