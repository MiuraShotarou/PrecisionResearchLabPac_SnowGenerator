#include <stdlib.h>
#include <stdint.h>

// ----------------------------------------------------------------
// 生成
// ----------------------------------------------------------------
NdArray* ndarray_create(int nd, int64_t *dimensions, int itemsize) {
    NdArray *arr = (NdArray *)malloc(sizeof(NdArray));
    if (!arr) return NULL;

    arr->nd       = nd;
    arr->itemsize = itemsize;

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
    arr->data = (char *)calloc(stride, 1);
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
    free(arr->data);
    free(arr->dimensions);
    free(arr->strides);
    free(arr);
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
/* ndarrayポインタから新規ndarrayをコピーして作成 */
NdArray* ndarray_copy(NdArray *arr) {
    if (!arr) return NULL;
    NdArray *copy = ndarray_create(arr->nd, arr->dimensions, arr->itemsize);
    if (!copy) return NULL;
    int n = 1;
    for (int i = 0; i < arr->nd; i++) {
        n *= (int)arr->dimensions[i];
    }
    memcpy(copy->data, arr->data, n * arr->itemsize);
    return copy;
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

static int64_t*
ndarray_shape(NdArray *src)
{
    int64_t *shape = (int64_t *)malloc(sizeof(int64_t) * src->nd);
    if (shape == NULL) return NULL;
    memcpy(shape, src->dimensions, sizeof(int64_t) * src->nd);
    return shape;
}