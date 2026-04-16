#define NPY_NO_DEPRECATED_API NPY_API_VERSION
#define _MULTIARRAYMODULE

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "numpy/arrayobject.h"
#include "numpy/arrayscalars.h"

#include "arrayobject.h"
#include "arrayfunction_override.h"
#include "npy_argparse.h"
#include "npy_config.h"
#include "npy_pycompat.h"
#include "npy_import.h"
#include "ufunc_override.h"
#include "array_coercion.h"
#include "common.h"
#include "templ_common.h" /* for npy_mul_sizes_with_overflow */
#include "ctors.h"
#include "calculation.h"
#include "convert_datatype.h"
#include "descriptor.h"
#include "dtypemeta.h"
#include "item_selection.h"
#include "conversion_utils.h"
#include "shape.h"
#include "strfuncs.h"
#include "array_assign.h"
#include "npy_dlpack.h"
#include "npy_static_data.h"
#include "multiarraymodule.h"

#include "methods.h"
#include "alloc.h"
#include "array_api_standard.h"

#include <stdarg.h>

arange
choice
np.full
copy
//typedef struct {
//    char    *data;          // 実データへのポインタ
//    int      nd;            // 次元数
//    int64_t *dimensions;   // 各次元のサイズ
//    int64_t *strides;      // 各次元でステップする際ののバイト数 → 転置ができる
//    int   itemsize;      // 1要素のバイト数 → 実質データ型でありstridesでもある
//} NdArray;

 /*  */
static NdArray *
np_zeros(int nd, int64_t *dims, size_t itemsize, char order)//order='C'C言語, 'F'Fotran
{
    NdArray *array = ndarray_create(nd, dims, itemsize);
    if (array == NULL) {
		return NULL;
	}
    return array;
}
 /*  */
static NdArray *
np_ones(int nd, int64_t *dims, size_t itemsize, char order)//order='C'C言語, 'F'Fotran
{
    NdArray *array = ndarray_create(nd, dims, itemsize);
    if (array == NULL) {
		return NULL;
	}

	int64_t total = 1;
    for (int i = 0; i < nd; i++) {
        total *= dims[i];
    }
    if (itemsize == sizeof(double)) {
        double *p = (double *)array->data;
        double *end = p + total;
        while (p < end) {
            *p++ = 1.0;
        }
    }
    else if (itemsize == sizeof(float)) {
        float *p = (float *)array->data;
        float *end = p + total;
        while (p < end) {
            *p++ = 1.0f;
        }
    }
    else if (itemsize == sizeof(int32_t)) {
        int32_t *p = (int32_t *)array->data;
        int32_t *end = p + total;
        while (p < end) {
            *p++ = 1;
        }
    }
    return array;
}
// エラー条件を検出する必要
/*  */
static NdArray *
np_arange(int start, int end, int step, SDType sdtype, char order){
	int nd = 1;
	int64_t dimensions[1] = {(end - start) / step};
	int64_t *dims = dimensions;

	NdArray *array = ndarray_create(nd, dims, itemsize);
    if (array == NULL) {
		return NULL;
	}

	for (int i = 0; i < dimensions[0]; i++) {
		double value = start + i * step; //型をitemsizeに合わせる必要り
        memcpy(array->data + i * itemsize, &value, itemsize);
	}
}

// 使い終わったら必ずこれで解放する
static void
np_free(NdArray *arr)
{
    if (arr != NULL) {
        free(arr->data);
        free(arr);
    }
}

PyArray_Zeros(int nd, npy_intp const *dims, PyArray_Descr *type, int is_f_order)//次元数, 各次元配列へのポインタ, データ型, 行優先か列優先か
{
    npy_dtype_info dt_info = {NULL, NULL};

    int res = PyArray_ExtractDTypeAndDescriptor(
        type, &dt_info.descr, &dt_info.dtype);

    // steal reference
    Py_XDECREF(type);

    if (res < 0) {
        Py_XDECREF(dt_info.descr);
        Py_XDECREF(dt_info.dtype);
        return NULL;
    }

    PyObject *ret = PyArray_Zeros_int(nd, dims, dt_info.descr, dt_info.dtype,
                                      is_f_order);

    Py_XDECREF(dt_info.descr);
    Py_XDECREF(dt_info.dtype);

    return ret;
}

NPY_NO_EXPORT PyObject *
PyArray_Zeros_int(int nd, npy_intp const *dims, PyArray_Descr *descr,
                  PyArray_DTypeMeta *dtype, int is_f_order)
{
    PyObject *ret = NULL;

    if (descr == NULL) {
        descr = _infer_descr_from_dtype(dtype);
        if (descr == NULL) {
            return NULL;
        }
    }
    /*
     * PyArray_NewFromDescr_int steals a ref to descr,
     * incref so caller of this function can clean up descr
     */
    Py_INCREF(descr);
    ret = PyArray_NewFromDescr_int(
            &PyArray_Type, descr,
            nd, dims, NULL, NULL,
            is_f_order, NULL, NULL,
            _NPY_ARRAY_ZEROED);

    return ret;
}



/* NpyArg_ParseKeywords
 *
 * Utility function that provides the keyword parsing functionality of
 * PyArg_ParseTupleAndKeywords without having to have an args argument.
 *
 */

PyArray_View() //戻り値PyObject*
PyArray_NewCopy()

//要素数をチェック
//メモリ連続性チェック
//strides計算
//return

// 戻り値: PyObject*（ndarray）。reshapeした配列、またはビューを返す
// array: reshape対象のndarray
// newdims: 新しい形状情報（ptr=各次元サイズの配列、len=次元数）
// order: メモリレイアウト順序（C順/Fortran順）
// copy: コピーの挙動（常にコピー/必要時のみ/コピー禁止）
NPY_NO_EXPORT PyObject *
_reshape_with_copy_arg(PyArrayObject *array, PyArray_Dims *newdims,
                       NPY_ORDER order, NPY_COPYMODE copy)
{
    int64_t i;
    int64_t *dimensions = newdims->ptr; // 新しい形状の各次元サイズへのポインタ
    PyArrayObject *ret;                 // 戻り値となるndarray
    int ndim = newdims->len;            // 新しい次元数
    bool same;                          // 形状が同じかどうかのフラグ
    int64_t *strides = NULL;            // ストライド（NULL=自動計算）
    int64_t newstrides[NPY_MAXDIMS];    // コピーなしreshape時の新ストライド格納用
    int flags;                          // 配列フラグ（C連続/Fortran連続など）

    // ANYORDERの場合、元配列がFortran順ならFortran順、そうでなければC順に決定
    if (order == NPY_ANYORDER) {
        order = PyArray_ISFORTRAN(array) ? NPY_FORTRANORDER : NPY_CORDER;
    }
    // KEEPORDER（元のレイアウトを維持）はreshapeでは使用不可
    else if (order == NPY_KEEPORDER) {
        PyErr_SetString(PyExc_ValueError,
                "order 'K' is not permitted for reshaping");
        return NULL;
    }

    // 常にコピーが不要で、かつ次元数が同じ場合は形状の一致を確認する
    if (ndim == PyArray_NDIM(array) && copy != NPY_COPY_ALWAYS) {
        same = NPY_TRUE;
        i = 0;
        // 各次元のサイズを比較し、一つでも違えばsame=falseにする
        while (same && i < ndim) {
            if (PyArray_DIM(array, i) != dimensions[i]) {
                same = NPY_FALSE;
            }
            i++;
        }
        // 形状が完全に同じならコピー不要のビューをそのまま返す
        if (same) {
            return PyArray_View(array, NULL, NULL); //View → 配列の一部分を指定可能な参照型。C#だとSpan<T>で実現可能
        }
    }

    // -1で指定された次元（自動計算）を実際のサイズに解決し、
    // 総要素数が一致するかチェックする
    if (_fix_unknown_dimension(newdims, array) < 0) {
        return NULL;
    }

    // NPY_COPY_ALWAYSの場合は指定のorder順で必ず新しいコピーを作成する
    if (copy == NPY_COPY_ALWAYS) {
        PyObject *newcopy = PyArray_NewCopy(array, order);
        if (newcopy == NULL) {
            return NULL;
        }
        array = (PyArrayObject *)newcopy; // 以降はコピーを操作対象とする
    }
    else {
        // 参照カウントを増やして配列を保持する（後でDECREFする）
        Py_INCREF(array);

        // 要求されたorder順と元配列のメモリレイアウトが一致しない場合
        if (((order == NPY_CORDER && !PyArray_IS_C_CONTIGUOUS(array)) ||
                (order == NPY_FORTRANORDER && !PyArray_IS_F_CONTIGUOUS(array)))) {

            int success = 0;
            // コピーなしでreshapeできるか試みる（ストライドの調整で対応可能か）
            success = _attempt_nocopy_reshape(array, ndim, dimensions,
                                              newstrides, order); //bool を返す
            if (success) {
                // コピー不要でreshapeできた場合は新ストライドを使用する
                strides = newstrides;
            }
            else if (copy == NPY_COPY_NEVER) {
                // コピー禁止なのにコピーが必要な状況 → エラー
                PyErr_SetString(PyExc_ValueError,
                                "Unable to avoid creating a copy while reshaping.");
                Py_DECREF(array);
                return NULL;
            }
            else {
                // コピーが必要な場合は指定order順でコピーを作成する
                PyObject *newcopy = PyArray_NewCopy(array, order);
                Py_DECREF(array); // 元の参照を解放
                if (newcopy == NULL) {
                    return NULL;
                }
                array = (PyArrayObject *)newcopy;
            }
        }
    }

    // 配列フラグを取得し、次元数と順序に応じてC連続/Fortran連続フラグを更新する
    flags = PyArray_FLAGS(array);
    if (ndim > 1) {
        if (order == NPY_FORTRANORDER) {
            flags &= ~NPY_ARRAY_C_CONTIGUOUS;   // C連続フラグを落とす
            flags |= NPY_ARRAY_F_CONTIGUOUS;    // Fortran連続フラグを立てる
        }
        else {
            flags &= ~NPY_ARRAY_F_CONTIGUOUS;   // Fortran連続フラグを落とす
            flags |= NPY_ARRAY_C_CONTIGUOUS;    // C連続フラグを立てる
        }
    }

    // dtypeの参照カウントを増やしてから新しいndarrayを生成する
    // 既存のデータバッファ（PyArray_DATA(array)）をそのまま共有する形で作成
    Py_INCREF(PyArray_DESCR(array));
    ret = (PyArrayObject *)PyArray_NewFromDescr_int(
            Py_TYPE(array),         // 元と同じ型（サブクラス対応）
            PyArray_DESCR(array),   // 元と同じdtype
            ndim,                   // 新しい次元数
            dimensions,             // 新しい各次元サイズ
            strides,                // ストライド（NULLなら自動計算）
            PyArray_DATA(array),    // 元配列のデータバッファを共有
            flags,                  // 更新済みフラグ
            (PyObject *)array,      // 元のPythonオブジェクト
            (PyObject *)array,      // baseオブジェクト（メモリ所有者）
            _NPY_ARRAY_ENSURE_DTYPE_IDENTITY); // dtype同一性を保証するフラグ

    Py_DECREF(array); // 参照カウントを戻す
    return (PyObject *)ret;
}
//
static int
_attempt_nocopy_reshape(PyArrayObject *self, int newnd, const npy_intp *newdims,
                        npy_intp *newstrides, int is_f_order)
{
    int oldnd;
    npy_intp olddims[NPY_MAXDIMS];
    npy_intp oldstrides[NPY_MAXDIMS];
    npy_intp last_stride;
    int oi, oj, ok, ni, nj, nk;

    oldnd = 0;
    /*
     * Remove axes with dimension 1 from the old array. They have no effect
     * but would need special cases since their strides do not matter.
     */
    for (oi = 0; oi < PyArray_NDIM(self); oi++) {
        if (PyArray_DIMS(self)[oi]!= 1) {
            olddims[oldnd] = PyArray_DIMS(self)[oi];
            oldstrides[oldnd] = PyArray_STRIDES(self)[oi];
            oldnd++;
        }
    }

    /* oi to oj and ni to nj give the axis ranges currently worked with */
    oi = 0;
    oj = 1;
    ni = 0;
    nj = 1;
    while (ni < newnd && oi < oldnd) {
        npy_intp np = newdims[ni];
        npy_intp op = olddims[oi];

        while (np != op) {
            if (np < op) {
                /* Misses trailing 1s, these are handled later */
                np *= newdims[nj++];
            } else {
                op *= olddims[oj++];
            }
        }

        /* Check whether the original axes can be combined */
        for (ok = oi; ok < oj - 1; ok++) {
            if (is_f_order) {
                if (oldstrides[ok+1] != olddims[ok]*oldstrides[ok]) {
                     /* not contiguous enough */
                    return 0;
                }
            }
            else {
                /* C order */
                if (oldstrides[ok] != olddims[ok+1]*oldstrides[ok+1]) {
                    /* not contiguous enough */
                    return 0;
                }
            }
        }

        /* Calculate new strides for all axes currently worked with */
        if (is_f_order) {
            newstrides[ni] = oldstrides[oi];
            for (nk = ni + 1; nk < nj; nk++) {
                newstrides[nk] = newstrides[nk - 1]*newdims[nk - 1];
            }
        }
        else {
            /* C order */
            newstrides[nj - 1] = oldstrides[oj - 1];
            for (nk = nj - 1; nk > ni; nk--) {
                newstrides[nk - 1] = newstrides[nk]*newdims[nk];
            }
        }
        ni = nj++;
        oi = oj++;
    }

    /*
     * Set strides corresponding to trailing 1s of the new shape.
     */
    if (ni >= 1) {
        last_stride = newstrides[ni - 1];
    }
    else {
        last_stride = PyArray_ITEMSIZE(self);
    }
    if (is_f_order) {
        last_stride *= newdims[ni - 1];
    }
    for (nk = ni; nk < newnd; nk++) {
        newstrides[nk] = last_stride;
    }

    return 1;
}