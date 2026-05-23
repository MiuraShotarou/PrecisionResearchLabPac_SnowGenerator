// #C long[] → C int64_t* → ndarray_create[dims]

// int itemsize = itemsize_cast_by_sdtype(sdtype)

// コンテナの0初期化処理
memset(初期化元, 0, sizeof(初期化元));
//memset(src_indices, 0, sizeof(src_indices));

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
    // memcpy(dest, &value, itemsize) //要素の代入 ((コピー先のアドレス), (コピー元のアドレス), (コピーするバイト数))
}

// ストライドを使った要素アクセス
int64_t distance = ndarray->strides[nd_i]; //nd_i(指定した次元)に該当する配列内で、要素同士が多次元配列全体で見たときに何バイト離れているか。→ [a, b, c, d, a, b, c, d] → a → a == 4
var value = ndarray->data + distance * f_i; //f_i(配列全体でのindex)

get_indices(int nd, int64_t *dimensions, int64_t flat, int64_t *out_indices) //indices とは、一次元配列にした際のindexを指定すると、多次元配列時のindexを返してくれる変数

// フラット → 多次元
for (int d = nd - 1; d > -1; d--) { //nd == 10 → for(9 ~ 0)
    indices[d] = flat % dimensions[d]; //flat == 最大index % 各次元ごとの要素数
                                        // {3, 4, 2} → 23 % {3, 4, 2} → {2, 3, 1}
    flat /= dimensions[d];              //メソッド内引数の更新 → 23 → 11 → 2
}

// 多次元 → フラット
int64_t flat = 0; //
int64_t stride = 1; //
for (int d = nd - 1; d > -1; d--) { //nd == 10 → 9 ~ 0
    flat += indices[d] * stride;    //stride == next Index byte → flat
    stride *= dimensions[d];        //
}

static int
get_flat(int64_t *indices, int64_t *dimensions, int nd)
{
    int64_t flat = 0;
    int64_t stride = 1;
    for (int d = nd - 1; d > -1; d--) {
        flat += indices[d] * stride;
        stride *= dimensions[d];
    }
}
    
// 多次元カウンタを1ずつ進める
int carry = 1; //dimensions == {3,4,3}
for (int d = nd - 1; d > -1 && carry; d--) { //nd == 3 → 2 ~ 0
    indices[d]++;                            //indices[] == {2, 3, 2} → {0, 0, 0}
    if (indices[d] < dimensions[d]) {
        carry = 0;
    } else {
        indices[d] = 0;  // 桁上がり
    }
}
if (carry) {
    assert()
};
    
// axis次元のオフセット管理
int64_t axis_idx = res_indices[axis];
int src_array = 0;
while (axis_idx >= arrays[src_array]->dimensions[axis]) {
    axis_idx -= arrays[src_array]->dimensions[axis];
    src_array++;
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