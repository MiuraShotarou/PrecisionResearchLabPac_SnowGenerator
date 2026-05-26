#include <string.h>
#include "shape.h"
#include "arrayobject.h"
#include "error.h"

static double
address_to_double(char *address, SDType sdtype)
{
    switch (sdtype) {
        case Bool:   { bool     v; memcpy(&v, address, sizeof(bool));     return (double)v; }
        case SByte:  { int8_t   v; memcpy(&v, address, sizeof(int8_t));   return (double)v; }
        case Byte:   { uint8_t  v; memcpy(&v, address, sizeof(uint8_t));  return (double)v; }
        case Short:  { int16_t  v; memcpy(&v, address, sizeof(int16_t));  return (double)v; }
        case UShort: { uint16_t v; memcpy(&v, address, sizeof(uint16_t)); return (double)v; }
        case Int:    { int32_t  v; memcpy(&v, address, sizeof(int32_t));  return (double)v; }
        case UInt:   { uint32_t v; memcpy(&v, address, sizeof(uint32_t)); return (double)v; }
        case Long:   { int64_t  v; memcpy(&v, address, sizeof(int64_t));  return (double)v; }
        case ULong:  { uint64_t v; memcpy(&v, address, sizeof(uint64_t)); return (double)v; }
        case Float:  { float    v; memcpy(&v, address, sizeof(float));    return (double)v; }
        case Double: { double   v; memcpy(&v, address, sizeof(double));   return v;         }
        default:     return 0.0;
    }
}

static NdArray* //(int axis) の軸に従って、((配列1), (配列2))で指定した複数の配列を結合する。新しい次元の追加はしない
np_concatenate(NdArray **arrays, int array_count, int axis) //shapeをふんだんに利用してコーディングされている
{
    for (int i = 1; i < array_count; i++) {
        if (arrays[i]->nd != arrays[0]->nd) //各配列のnd一致チェック
    
    
    /* axis以外のサイズが不一致だった場合 */
    for (int i = 0; i < arrays[0]->nd; i++) {
        if (i == axis) {
            continue;
        }
        else {
            for (int j = 1; j < array_count; j++) {
                if (arrays[0]->dimensions[i] != arrays[j]->dimensions[i]) {
                    SET_ERROR_MESSAGE_ARGUMENT("np_concatenate: shape mismatch at dimension %d.", i);
                    return NULL;
                }
            }
        }
    }
    
    if (axis < 0) {
        adjust_axis(axis);
    }
    
    if (array_count <= 0)
    if (axis < 0 || axis >= arrays[0]->nd)

    /*
     * a.dimensions == (2,4) b.dimensions == (1,4) axis == 0
     * [[a, a, a, a], [[b,b,b,b]]
     * [a, a, a, a]]
     * result.dimensions == (3,4)
     * [[a, a, a, a],
     *  [a, a, a, a], // 8 
     *  [b, b, b, b]]
     *  
     * a.dimensions == (2,2) b.dimensions == (2,4) axis == 1
     * [[a, a], [[b,b,b,b],
     *  [a, a]]  [b,b,b,b]]
     * result.dimensions == (2,6)
     * [[a, a, b, b, b, b],
     *  [a, a, b, b, b, b]]
     *  
     * a.dimensions == (2,3,4) b.dimensions == (2,3,4) axis == 0 //swich_point == 24
     * [[[a, a, a, a],
         [a, a, a, a],
         [a, a, a, a]], 12 = 3 * 4
         
        [[a, a, a, a],
         [a, a, a, a],
         [a, a, a, a]], 24 = 3 * 4 + 12
         
        [[b, b, b, b],
         [b, b, b, b],
         [b, b, b, b]], 
        
        [[b, b, b, b],
         [b, b, b, b],
         [b, b, b, b]]]
        
     * a.dimensions == (2,3,4) b.dimensions == (2,3,4) axis == 1 // swich_point == 12
     * 
     * [[[a, a, a, a],
         [a, a, a, a],
         [a, a, a, a], 12 = 3 * 4
         [b, b, b, b],
         [b, b, b, b],
         [b, b, b, b]],
        [[a, a, a, a],
         [a, a, a, a],
         [a, a, a, a],
         [b, b, b, b],
         [b, b, b, b],
         [b, b, b, b]]]
         
     * a.dimensions == (2,3,4) b.dimensions == (2,3,4) axis == 2 // swich_point == 36
    /*
     * [[[a, a, a, a, b, b, b, b], 4 = 4
         [a, a, a, a, b, b, b, b],
         [a, a, a, a, b, b, b, b]],
        [[a, a, a, a, b, b, b, b],
         [a, a, a, a, b, b, b, b],
         [a, a, a, a, b, b, b, b]]]
    */
    
    /* swich_point == (index == axis ~ maxIndex) dimensions[index] * dimensions[index]……/
    
    /* result配列の形状を計算 */
    int axis_size = 0;
    for (int i = 0; i < array_count; i++) {
        axis_size += arrays[i]->dimensions[axis];
    }
    
    int res_nd = arrays[0]->nd;
    int64_t res_dims[64];
    for (int i = 0; i < res_nd; i++) {
        res_dims[i] = i == axis? axis_size : arrays[0]->dimensions[i];
    }
    
    /* 型のメモリサイズを取得 */
    int itemsize = arrays[0]->itemsize;
    
    /* 形状を反映させたresult配列を作成 */
    NdArray* result = ndarray_create(res_nd, res_dims, itemsize);
    
    // 配列代入処理（resultの破壊的操作）
    merge_arrays_along_axis(arrays, array_count, axis, itemsize, result);
    
    return result;
}

/* array A B Concat */
static NdArray*
np_stack(NdArray **arrays, int array_count, int axis)
{
    /*
     *  {2, 3, 4}, {2, 3, 4}, axis = 0
        // result.shape(2, 2, 3, 4) → {2(0), 2, 3, 4}
        [ //0
         [ //1
          [ //2
           [a, a, a, a],
           [a, a, a, a],
           [a, a, a, a]
          ],
          [
           [a, a, a, a],
           [a, a, a, a],
           [a, a, a, a]
          ]
         ], 24 = 4 * 2 * 3
        [
         [
          [b, b, b, b], 
          [b, b, b, b],
          [b, b, b, b]
         ],
         [
          [b, b, b, b],
          [b, b, b, b],
          [b, b, b, b]
         ]
        ]
       ]
     *  {2, 3, 4}, {2, 3, 4}, axis = 1
        // result.shape(2, 2, 3, 4) → {2, 2(0), 3, 4}
        [//0
         [//1
          [//2
           [a, a, a, a],
           [a, a, a, a],
           [a, a, a, a]
          ], 12 = 3 * 4
          [
           [b, b, b, b],
           [b, b, b, b],
           [b, b, b, b]
          ]
         ],
         [
          [
           [a, a, a, a],
           [a, a, a, a],
           [a, a, a, a]
          ],
          [
           [b, b, b, b],
           [b, b, b, b],
           [b, b, b, b]
          ]
         ]
        ]
     *  {2, 3, 4}, {2, 3, 4}, axis = 2
        // result.shape(2, 3, 2(0), 4)
        [//'0'000
         [//0'0'00
          [//00'0'0
           [a, a, a, a], //000'0'
           [b, b, b, b]  //000'1'
          ],
          [//00'1'0
           [a, a, a, a], //
           [b, b, b, b]  //
          ],
          [
           [a, a, a, a],
           [b, b, b, b]
          ]
         ],
         [
          [
           [a, a, a, a],
           [b, b, b, b]
          ],
          [
           [a, a, a, a],
           [b, b, b, b]
          ],
          [
           [a, a, a, a],
           [b, b, b, b]
          ]
         ]
        ]
     *  {2, 3, 4}, {2, 3, 4}, axis = 3
        // result.shape(2, 3, 4, 2(0)) // 2(0) == array_count
        [[[[a, b],
           [a, b],
           [a, b],
           [a, b]],
          [[a, b],
           [a, b],
           [a, b],
           [a, b]],
          [[a, b],
           [a, b],
           [a, b],
           [a, b]]],
         [[[a, b],
           [a, b],
           [a, b],
           [a, b]],
          [[a, b],
           [a, b],
           [a, b],
           [a, b]],
          [[a, b],
           [a, b],
           [a, b],
           [a, b]]]]
    */

	if (arrays == NULL || array_count <= 0) {
    	SET_ERROR_MESSAGE("np_stack: arrays is NULL or array_count is invalid.");
    	return NULL;
    }
    
    int res_nd = arrays[0]->nd + 1;
    int64_t res_dims[64];
    for (int i = 0; i < res_nd; i++) { // res_nd == 4, axis == 2, result_shape(2, 3, 2(0), 4)
        if (i == axis) {
            res_dims[i] = array_count;
        }
        else {
            res_dims[i] = arrays[0]->dimensions[i < axis? i : i - 1];
        }
    }
    int itemsize = arrays[0]->itemsize;
    
    NdArray* result = ndarray_create(res_nd, res_dims, itemsize); //引数に既存のコレクションを指定
    
    // 配列代入処理（resultの破壊的操作）
    merge_arrays_along_axis(arrays, array_count, axis, itemsize, result);
    
    return result;
}

static NdArray*
np_vstack(NdArray **arrays, int array_count)
{
    if (arrays[0]->nd == 1) { //一次元配列だった場合
        return np_stack(arrays, array_count, 0);
    }
    else {
        return np_concatenate(arrays, array_count, 0);
    }
}
    
static NdArray*
np_hstack(NdArray **arrays, int array_count)
{
    if (arrays[0]->nd == 1) {
        return np_concatenate(arrays, array_count, 0);
    }
    else {
        return np_concatenate(arrays, array_count, 1);
    }
}

static void
merge_arrays_along_axis(NdArray **srcs, int array_count, int axis, int itemsize, NdArray *out_res)
{
    int64_t roop = 1;
    for (int i = 0; i < axis; i++) { //axis = 0 ~ 2
        roop *= srcs[0]->dimensions[i];
    }
    int64_t res_i = 0;
    for (int r = 0; r < roop; r++) //全配列のコピーを繰り返す回数
    {
        for (int i = 0; i < array_count; i++) { // 複数の配列ごとループ
            NdArray *src = srcs[i];
            int64_t swich_point = 1;
            for (int d = axis; d < src->nd; d++) { // {2,5,4} {2,3,4} axis = 1 → swich_point == 4 * 5 == 20, 4 * 3 == 12
                swich_point *= src->dimensions[d];
            }
            int64_t flat = r * swich_point; //r * 今回コピーするぶんの要素数 == 現在のindex（先頭） 0 ~ 19 → 20 ~ 39, 0 ~ 11 → 12 ~ 23
            for (int f = flat; f < flat + swich_point; f++) {
                memcpy(out_res->data + res_i++ * itemsize, src->data + f * itemsize, itemsize); //result->data * (要素を代入した回数 * itemsize)
            }
        }
    }
}
    
static NdArray*
np_ravel(NdArray *src)
{
    if (src == NULL) {
        return NULL;
    }
    
    int nd = 1;
    int64_t dimensions[64];
    int64_t total = get_totalelements(src->nd, src->dimensions);
    dimensions[0] = total;
    
    NdArray *result = ndarray_create(nd, dimensions, src->itemsize, src->sdtype);
    
    memcpy(result->data, src->data, total * src->itemsize);
        
    return result;
}

static NdArray*
np_reshape(NdArray *src, int64_t *size, int size_nd)
{
    int64_t src_total = get_totalelements(src->dimensions, src->nd);
    int64_t res_total = get_totalelements(size, size_nd);
    if (src_total != res_total) {
        SET_ERROR_MESSAGE_ARGUMENT("cannot reshape array of size %lld into shape with size %lld.", src_total, res_total);
        return NULL;
    }
    
    int itemsize = src->itemsize;
    NdArray *result = ndarray_create(size_nd, size, itemsize);
    //null check
    
    memcpy(result->data, src->data, res_total * src->itemsize);
    return result;
}
    
static NdArray*
np_resize(NdArray *src, int64_t *size, int size_nd)
{
    int64_t src_total = get_totalelements(src->dimensions, src->nd);
    int64_t res_total = get_totalelements(size, size_nd);
    if (src_total == res_total) { // conditions reshape
        return np_reshape(src, size, size_nd);
    }
        
    int itemsize = src->itemsize;
    NdArray *result = ndarray_create(size_nd, size, itemsize);
    
    int64_t roop = res_total / src_total; //64 / 66 == 0, 64 / 64 == 1, 64 / 32 == 2
    int i = 0;
    do {
        memcpy(result->data + (i++ * src_total * itemsize), src->data, src_total * itemsize);
    } while (--roop > 0);
        
    return result;
}
    
static NdArray*
np_squeeze(NdArray *src)
{
    /* result 配列の形状を確定 */
    int res_nd = 0;
    int64_t res_dims[64];
    for (int i = 0; i < src->nd; i++) {
        if (src->dimensions[i] > 1) {
            res_dims[res_nd++] = src->dimensions[i];
        }
    }
    
    if (res_nd == src->nd) {
        return src;
    }
    
    NdArray *result = ndarray_create(res_nd, res_dims, src->itemsize);
    
    int64_t src_total = get_totalelements(src->dimensions, src->nd);
    
    memcpy(result->data, src->data, src_total * src->itemsize);

    ndarray_free(src);

    return result;
}
    
static NdArray* //要素の入れ替え、反転
np_transpose(NdArray *src, int64_t *size) //size.default == (0, 1, 2, 3……)
{
    NdArray *result = ndarray_copy(src);

    for (int i = 0; i < src->nd; i++) {
        result->dimensions[i] = src->dimensions[size[i]];
        result->strides[i] = src->strides[size[i]];
    }

    ndarray_free(src);

    return result;
}