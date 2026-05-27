// array_cast.c

// sdtype修正を行った

#include "array_cast.h"

// SDType 指定で下記の(配列同士キャスト関数)を呼び出すことができるメソッド → 破壊的操作で変更する形に
NdArray* np_ndarray_cast (NdArray *src, SDType restype) {
    /* check NULL */
	if (src == NULL) {
		return NULL;
	}

	SDType srctype = src->sdtype;
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
    NdArray *result = ndarray_create(nd, dimensions, itemsize, restype);
    if (result == NULL) {
        return NULL;
    }
    
    int64_t total = get_totalelements(result->nd, result->dimensions);
    
    char* src_ptr = src->data;
    char* res_ptr = result->data;
    srcarray_to_resarray(src_ptr, res_ptr, total);
    
    return result;
} //NdArray.dataに反映させれば良い