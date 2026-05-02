#include "array_cast.h"

// SDType 指定で下記の(配列同士キャスト関数)を呼び出すことができるメソッド → 破壊的操作で変更する形に
NdArray* np_ndarray_cast (NdArray *src, SDType srctype, SDType restype) {
    /* check NULL */
    ArrayCast srcarray_to_resarray = srcarray_to_resarray_table[srctype][restype];
    if (srcarray_to_resarray == NULL) {
        return NULL; //pass C#
    }
    /* create array */
    int nd = src->nd;
    int64_t *dimensions = src->dimensions;
    int itemsize = itemsize_cast_by_sdtype(restype);
    if (itemsize == -1) {
        return NULL;
    }
    NdArray *result = ndarray_create(nd, dimensions, itemsize); //(int nd, int64_t *dimensions, int itemsize)
    if (result == NULL) {
        return NULL;
    }
    
    size_t n = 1;
    for (int i = 0; i < nd; i++) {
        n *= (size_t)dimensions[i];
    }
    
    char* srcdata = src->data;
    char* resdata = result->data;
    srcarray_to_resarray(srcdata, resdata, n);
    
    return result;
} //NdArray.dataに反映させれば良い