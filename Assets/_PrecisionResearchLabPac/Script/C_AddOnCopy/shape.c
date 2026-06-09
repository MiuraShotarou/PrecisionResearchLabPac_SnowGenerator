// shape.c
// ndarray_createの引数sdtypeを確認した
// staticの除外を行った
// get_totalelementsの引数順を修正した

#include <string.h>
#include "shape.h"
#include "arrayobject.h"
#include "error.h"

NdArray* //(int axis) の軸に従って、((配列1), (配列2))で指定した複数の配列を結合する。新しい次元の追加はしない
np_concatenate(NdArray **arrays, int array_count, int axis) //shapeをふんだんに利用してコーディングされている
{
	NdArray* result = NULL;
    if (arrays == NULL || array_count < 1) {
        SET_ERROR_MESSAGE("np_concatenate: arrays is NULL or array_count is invalid.");
        goto fail;
    }
    for (int i = 1; i < array_count; i++) {
        if (arrays[i]->nd != arrays[0]->nd) { //各配列のnd一致チェック
            SET_ERROR_MESSAGE_ARGUMENT("np_concatenate: shape mismatch at dimension %d.", i);
            goto fail;
		}
    }
    /* axis以外のサイズが不一致だった場合 */
    for (int d = 0; d < arrays[0]->nd; d++) {
        if (d == axis) {
            continue;
        }
        else {
            for (int i = 1; i < array_count; i++) {
                if (arrays[0]->dimensions[d] != arrays[i]->dimensions[d]) {
                    SET_ERROR_MESSAGE_ARGUMENT("np_concatenate: shape mismatch at dimension %d.", d);
                    goto fail;
                }
            }
        }
    }
    axis = get_adjust_axis(axis, arrays[0]->nd);
    if (axis >= arrays[0]->nd) {
        SET_ERROR_MESSAGE_ARGUMENT("np_concatenate: axis %d is out of range for array with %d dimensions.", axis, arrays[0]->nd);
        goto fail;;
    }
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
    int nd = arrays[0]->nd;
    int64_t dimensions[NDARRAY_MAX_DIMENSIONS];
    for (int i = 0; i < nd; i++) {
        dimensions[i] = i == axis? axis_size : arrays[0]->dimensions[i];
    }
    /* 形状を反映させたresult配列を作成 */
    result = ndarray_create(nd, dimensions, arrays[0]->itemsize, arrays[0]->sdtype);
    // 配列代入処理（resultの破壊的操作）
    merge_arrays_along_axis(arrays, array_count, axis, result->itemsize, result);
    return result;
    fail: 
		ndarray_free(result);
        return NULL;
}

/* array A B Concat */
NdArray*
np_stack(NdArray **arrays, int array_count, int axis)
{
	NdArray* result = NULL;
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
	if (arrays == NULL || array_count < 1) {
    	SET_ERROR_MESSAGE("np_stack: arrays is NULL or array_count is invalid.");
    	goto fail;
    }
    for (int i = 1; i < array_count; i++) {
        if (arrays[i]->nd != arrays[0]->nd) { //各配列のnd一致チェック
            SET_ERROR_MESSAGE_ARGUMENT("np_concatenate: shape mismatch at dimension %d.", i);
            goto fail;
        }
    }
    /* axis以外のサイズが不一致だった場合 */
    for (int d = 0; d < arrays[0]->nd; d++) {
        if (d == axis) {
            continue;
        }
        else {
            for (int i = 1; i < array_count; i++) {
                if (arrays[0]->dimensions[d] != arrays[i]->dimensions[d]) {
                    SET_ERROR_MESSAGE_ARGUMENT("np_concatenate: shape mismatch at dimension %d.", d);
                    goto fail;
                }
            }
        }
    }
    axis = get_adjust_axis(axis, arrays[0]->nd);
    if (axis >= arrays[0]->nd) {
        SET_ERROR_MESSAGE_ARGUMENT("np_concatenate: axis %d is out of range for array with %d dimensions.", axis, arrays[0]->nd);
        goto fail;;
    }
    int nd = arrays[0]->nd + 1;
    int64_t dimensions[NDARRAY_MAX_DIMENSIONS];
    for (int i = 0; i < nd; i++) { // nd == 4, axis == 2, result_shape(2, 3, 2(0), 4)
        if (i == axis) {
            dimensions[i] = array_count;
        }
        else {
            dimensions[i] = arrays[0]->dimensions[i < axis? i : i - 1];
        }
    }
    result = ndarray_create(nd, dimensions, arrays[0]->itemsize, arrays[0]->sdtype); //引数に既存のコレクションを指定
    // 配列代入処理（resultの破壊的操作）
    merge_arrays_along_axis(arrays, array_count, axis, result->itemsize, result);
    return result;
    fail:
		ndarray_free(result);
        return NULL;
}

NdArray*
np_vstack(NdArray **arrays, int array_count)
{
    if (arrays[0]->nd == 1) { //一次元配列だった場合
        return np_stack(arrays, array_count, 0);
    }
    else {
        return np_concatenate(arrays, array_count, 0);
    }
}
    
NdArray*
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
    int64_t loop = 1;
    for (int d = 0; d < axis; d++) { //axis = 0 ~ 2
        loop *= srcs[0]->dimensions[d];
    }
    int64_t res_i = 0;
    for (int l = 0; l < loop; l++) //全配列のコピーを繰り返す回数
    {
        for (int i = 0; i < array_count; i++) { // 複数の配列ごとループ
            NdArray *src = srcs[i];
            int64_t swich_point = 1;
            for (int d = axis; d < src->nd; d++) { // {2,5,4} {2,3,4} axis = 1 → swich_point == 4 * 5 == 20, 4 * 3 == 12
                swich_point *= src->dimensions[d];
            }
            int64_t flat = l * swich_point; //l * 今回コピーするぶんの要素数 == 現在のindex（先頭） 0 ~ 19 → 20 ~ 39, 0 ~ 11 → 12 ~ 23
            for (int f = flat; f < flat + swich_point; f++) {
                memcpy(out_res->data + res_i++ * itemsize, src->data + f * itemsize, itemsize); //result->data * (要素を代入した回数 * itemsize)
            }
        }
    }
}
NdArray*
np_ravel(NdArray *src)
{
	NdArray *result = NULL; 
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_ravel: src is NULL.");
        goto fail;
    }
    int nd = NDARRAY_MIN_ND;
    int64_t dimensions[NDARRAY_MAX_DIMENSIONS];
    int64_t total = get_totalelements(src->nd, src->dimensions);
    dimensions[0] = total;
    result = ndarray_create(nd, dimensions, src->itemsize, src->sdtype);
	if (result == NULL) {
		SET_ERROR_MESSAGE("np_ravel: result is NULL.");
		goto fail;
	}
	if (src->flags & NDARRAY_FLAG_C_CONTIGUOUS) { //メモリが連続していれば
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
    fail:
		ndarray_free(result);
        return NULL;
}

NdArray*
np_reshape(NdArray *src, int64_t *size, int size_nd)
{
	NdArray *result = NULL;
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_reshape: src is NULL.");
        goto fail;
    }
    int64_t src_total = get_totalelements(src->nd, src->dimensions);
    int64_t res_total = get_totalelements(size_nd, size);
    if (src_total != res_total) {
        SET_ERROR_MESSAGE_ARGUMENT("cannot reshape array of size %lld into shape with size %lld.", src_total, res_total);
        goto fail;
    }
    result = ndarray_create(size_nd, size, src->itemsize, src->sdtype);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_reshape: result is NULL.");
        goto fail;
    }
    if (src->flags & NDARRAY_FLAG_C_CONTIGUOUS) {
    	memcpy(result->data, src->data, src_total * src->itemsize);
	}
	else {
		for (int64_t f = 0; f < src_total; f++) {
    		int64_t indices[NDARRAY_MAX_DIMENSIONS];
    		assign_indices(src->nd, src->dimensions, f, indices);
    		char *src_address = get_address(src->data, indices, src->strides, src->nd);
    		memcpy(result->data + f * src->itemsize, src_address, src->itemsize);
		}
	}
    return result;
    fail:
		ndarray_free(result);
        return NULL;
}
    
NdArray*
np_resize(NdArray *src, int64_t *size, int size_nd)
{
	NdArray *result = NULL;
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_resize: src is NULL.");
        goto fail;
    }
    int64_t src_total = get_totalelements(src->nd, src->dimensions);
    int64_t res_total = get_totalelements(size_nd, size);
    if (src_total == res_total) { // conditions reshape
        return np_reshape(src, size, size_nd);
    }
    result = ndarray_create(size_nd, size, src->itemsize, src->sdtype);
    int64_t loop = res_total / src_total; //64 / 66 == 0, 64 / 64 == 1, 64 / 32 == 2, 66 / 64 = 1;
    for (int64_t l = 0; l < loop; l++) {
        memcpy(result->data + (l * src_total * src->itemsize), src->data, src_total * src->itemsize);
    }
    int64_t remainder = res_total % src_total; // 66 % 64 = 2
    for (int64_t r = 0; r < remainder; r++) {
        memcpy(result->data + (loop * src_total + r) * src->itemsize, src->data + (r * src->itemsize), src->itemsize);
    }
    return result;
    fail:
		ndarray_free(result);
        return NULL;
}
    
NdArray*
np_squeeze(NdArray *src)
{
	NdArray *result = NULL;
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_squeeze: src is NULL.");
        goto fail;
    }
    /* result 配列の形状を確定 */
    int nd = 0;
    int64_t dimensions[NDARRAY_MAX_DIMENSIONS];
    for (int d = 0; d < src->nd; d++) {
        if (src->dimensions[d] > 1) {
            dimensions[nd] = src->dimensions[d];
            nd++;
        }
    }
    result = ndarray_create(nd, dimensions, src->itemsize, src->sdtype);
	//形状を変え、reference(生データを持たない)にするだけ
	ndarray_asreference(src, result);
    return result;
    fail:
		ndarray_free(result);
        return NULL;
}
    
NdArray* //アクセス順序を変更する
np_transpose(NdArray *src, int64_t *size) //size.default == (0, 1, 2, 3……)
{
	NdArray *result = NULL;
    if (src == NULL || size == NULL) {
        SET_ERROR_MESSAGE("np_transpose: src or size is NULL.");
        goto fail;
    }
    result = ndarray_create(src->nd, src->dimensions, src->itemsize, src->sdtype); //Numpyではsrcのviewを返す
    for (int d = 0; d < src->nd; d++) {
        result->dimensions[d] = src->dimensions[size[d]];
        result->strides[d] = src->strides[size[d]];
    }
	ndarray_asreference(src, result);
	// &= ~C_CONTIGUOUS, |= F_CONTIGUOUS
	if (checkndarray_flag_c_contiguous(result)) { //メモリが連続していた場合
		assign_flags_c_contiguous_on(&result->flags);
	}
	else {
		assign_flags_c_contiguous_off(&result->flags);
	}
    return result;
    fail:
		ndarray_free(result);
        return NULL;
}