#include <stdlib.h>
#include <stdint.h>

// ----------------------------------------------------------------
// 生成（未初期化）
// ----------------------------------------------------------------
NdArray* ndarray_create(int nd, int64_t *dimensions, int itemsize, SDType sdtype) {
    NdArray *arr = (NdArray *)malloc(sizeof(NdArray));
    if (!arr) return NULL;
    arr->nd       = nd;
    arr->itemsize = itemsize;
	arr->sdtype   = sdtype;
	assign_flags_c_init(&arr->flags);
    // dimensions をコピー
    arr->dimensions = (int64_t *)malloc(sizeof(int64_t) * nd);
    arr->strides    = (int64_t *)malloc(sizeof(int64_t) * nd);
    if (!arr->dimensions || !arr->strides) {
        free(arr);
        return NULL;
    }
    // strides を計算（C順：末尾次元から計算）
    int64_t stride = (int64_t)itemsize;
    for (int i = nd - 1; i >= 0; i--) {
        arr->dimensions[i] = dimensions[i];
        arr->strides[i]    = stride;
        stride            *= dimensions[i];
    }
    // データ領域を確保
    arr->data = (char *)malloc(stride);
    if (!arr->data) {
        free(arr->dimensions);
        free(arr->strides);
        free(arr);
        return NULL;
    }
    return arr;
}

// ----------------------------------------------------------------
// 解放
// ----------------------------------------------------------------
void ndarray_free(NdArray *arr) {
    if (!arr) return;
	if (arr->flags & NDARRAY_FLAG_OWNDATA) {
    	free(arr->data);
	}
    free(arr->dimensions);
    free(arr->strides);
    free(arr);
}
// reference is NOT_OWNDATA
void ndarray_asreference(NdArray *arr, NdArray *out_reference) {
    if (!arr || !out_reference) {
		SET_ERROR_MESSAGE("ndarray_asreference: argument is NULL.");
		goto fail;
	}
    if (!(out_reference->flags & NDARRAY_FLAG_OWNDATA)) {
		SET_ERROR_MESSAGE("ndarray_asreference: out_reference is NULL.");
		goto fail; //既に参照型ならリターンする
	}
    free(out_reference->data);
    out_reference->data = arr->data;
    out_reference->flags &= ~NDARRAY_FLAG_OWNDATA;
	fail:
		out_reference = NULL;
		return;
}

void reference_asndarray(NdArray *reference, NdArray *out_arr) {
    if (!reference || !out_arr) {
		SET_ERROR_MESSAGE("reference_asndarray: argument is NULL.");
		goto fail;
	}
    if (reference->flags & NDARRAY_FLAG_OWNDATA) {
		SET_ERROR_MESSAGE("reference_asndarray: reference is OWNDATA"); //参照型として渡された変数に生のデータが入っていた場合、リターンする
		goto fail;
	}
    int64_t total = get_totalelements(reference->nd, reference->dimensions);
    char *data = (char *)malloc(reference->itemsize * total);
    if (data == NULL) {
        SET_ERROR_MESSAGE("reference_asndarray: malloc failed.");
		goto fail;
    }
    memcpy(data, reference->data, reference->itemsize * total);
    free(out_arr->data);
    out_arr->data = data;
    out_arr->flags |= NDARRAY_FLAG_OWNDATA;
	fail:
		out_arr = NULL;
        return;
}

bool checkndarray_flag_c_contiguous(NdArray *arr)
{
	if (arr == NULL) {
		SET_ERROR_MESSAGE("checkndarray_flag_c_contiguous: arr is NULL.");
		return false;
	}
	int64_t stride = arr->itemsize;
    for (int i = arr->nd - 1; i > -1; i--) {
        if (arr->strides[i] != stride) { 
			return false; 
		}
        stride *= arr->dimensions[i];
    }
    return true;
}

bool get_ndarray_flag_owndata(NdArray *arr)
{
	if (arr == NULL) {
		SET_ERROR_MESSAGE("check_ndarray_flag_owndata: arr is NULL.");
		return false;
	}
	return (arr->flags & NDARRAY_FLAG_OWNDATA) != 0;
}

// ----------------------------------------------------------------
// 要素アクセス（任意次元のインデックスからポインタを返す）
// ----------------------------------------------------------------
void* ndarray_get(NdArray *arr, int64_t *indices) {
    char *ptr = arr->data;
    for (int i = 0; i < arr->nd; i++) {
        ptr += indices[i] * arr->strides[i];
    }
    return (void *)ptr;
}
/* NdArray* copy to NdArray */
NdArray* ndarray_copy(NdArray *src) {
	// ※注意※　stringなども処理が通ってしまうため、C#側で防いでおくこと
	NdArray *result = ndarray_create(src->nd, src->dimensions, src->itemsize, src->sdtype);
	if (result == NULL) return NULL;
	int64_t total = get_totalelements(src->nd, src->dimensions);
	if (src->flags & NDARRAY_FLAG_C_CONTIGUOUS) {
		memcpy(result->data, src->data, total * src->itemsize);
	}
	else {
		for (int64_t f = 0; f < total; f++) {
			int64_t indices[NDARRAY_MAX_DIMENSIONS];
			assign_indices(src->nd, src->dimensions, f, indices);
			char *src_address = get_address(src->data, indices, src->strides, src->nd);
			memcpy(result->data + f * src->itemsize, src_address, src->itemsize);
		}
	}
	return result;
}
/* void* convert NdArray */
NdArray*
ndarray_convert(void *src, int nd, int64_t *dimensions, int itemsize, SDType sdtype)
{
	NdArray *result = ndarray_create(nd, dimensions, itemsize, sdtype);
	if (result == NULL) return NULL;
	int64_t total = get_totalelements(nd, dimensions);
	memcpy(result->data, src, total * itemsize);
	return result;
}
/* get prop */

/* ver.int32 
static int32_t*
ndarray_shape(NdArray *src)
{
    int32_t *shape = (int32_t *)malloc(sizeof(int32_t) * src->nd);
    if (shape == NULL) return NULL;
    for (int i = 0; i < src->nd; i++) {
        shape[i] = (int32_t)src->dimensions[i];
    }
    return shape;
}
*/

/* properties */

/* convenience */
bool
check_scalar(NdArray *arr)
{
    if (arr == NULL || arr->nd != 1 || arr->dimensions[0] != 1) {
		return false;
	}
	return true;
}