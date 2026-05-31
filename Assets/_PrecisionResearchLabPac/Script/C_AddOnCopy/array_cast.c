// array_cast.c

// sdtype修正を行った
// ブロードキャスト後のNdArrayが引数に渡された時、連続したメモリのコピー処理を分岐するように実装する

#include "array_cast.h"

// SDType 指定で下記の(配列同士キャスト関数)を呼び出すことができるメソッド → 破壊的操作で変更する形に
NdArray* np_ndarray_cast (NdArray *src, SDType restype) {
	NdArray *result = NULL;
    /* check NULL */
	if (src == NULL) {
		SET_ERROR_MESSAGE("np_ndarray_cast: src is NULL.");
		goto fail;
	}
    /* create array */
    int nd = src->nd;
    int64_t *dimensions = src->dimensions;
    int itemsize = itemsize_cast_by_sdtype(restype);
    if (itemsize == -1) {
		SET_ERROR_MESSAGE("np_ndarray_cast: unsupported sdtype.");
        goto fail;
    }
    result = ndarray_create(nd, dimensions, itemsize, restype);
    if (result == NULL) {
        return NULL;
    }
    int64_t total = get_totalelements(result->nd, result->dimensions);
	if (src->flags & NDARRAY_FLAG_C_CONTIGUOUS) {
    	ArrayCast arraycast = srcarray_to_resarray_table[src->sdtype][restype];
	    if (arraycast == NULL) {
			SET_ERROR_MESSAGE("np_ndarray_cast: arraycast is NULL.");
        	goto fail; //pass C#
    	}
    	char* src_ptr = src->data;
    	char* res_ptr = result->data;
    	arraycast(src_ptr, res_ptr, total);
	}
	else {
		DoubleScalarCast cast = doublescalar_cast_by_sdtype[restype];
		if (cast == NULL) {
			SET_ERROR_MESSAGE("np_ndarray_cast: cast is NULL.");
        	goto fail;
    	}
		for (int64_t f = 0; f < total; f++) {
	        int64_t indices[NDARRAY_MAX_DIMENSIONS];
    	    assign_indices(src->nd, src->dimensions, f, indices);
        	char *address = get_address(src->data, indices, src->strides, src->nd);
			double value = address_to_double(address, src->sdtype);
        	cast(result->data + f * result->itemsize, value);
    	}
	}
    return result;
	fail:
		ndarray_free(result);
		return NULL;
}