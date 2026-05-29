// slicing.c
// ndarray_createの引数sdtypeを確認した
// staticの除外を行った
// get_totalelementsの引数順を修正した

機能: (配列)[start:end:step] → 引数に従って配列の要素を抽出する

// 基本は、普通のindexアクセスと同じ機能
// start, end, の範囲外チェックが必要
// start == minIndex, end == maxIndex-1 という仕様で実装する
// stepに負の値を指定すると、末尾からstartし、先頭に向かってendする処理になる
// レシーバーに複数次元の配列が指定されている場合、slicingは次元数ぶん渡さなければならなくなる

#include "slicing.h"
typedef struct {
    int start;
    int stop;
    int step;
    int sign;
} SliceStruct;

// ①渡されたSliceStructを、計算処理に渡せる状態に改変(解決)する。
// ②start, end, step を基準に、ゲッター側とセッター側のスライス処理を実装する。
// ③ゲッター処理を考慮し、値をコピーする前にリザルト用のNdArrayを生成するcreate関数を実装する

NdArray*
get_ndarray_slicing(NdArray *src, SliceStruct **slice, int slice_nd)
{
    if (src == NULL || slice == NULL) {
        SET_ERROR_MESSAGE("get_ndarray_slicing: src or slice is NULL.");
        goto fail;
    }
    
    if (slice_nd > src->nd) {
        SET_ERROR_MESSAGE("np_slice: slices_nd exceeds the number of dimensions of src.");
        goto fail;
    }
    
    bool conditions = check_sliceconditions_and_assignstepsign(slice, slice_nd);
    if (!conditions) {
        SET_ERROR_MESSAGE("np_slice: invalid slice parameter detected. Step cannot be zero.");
        goto fail;
    }
    NdArray *result = slicingndarray_create(slice, slice_nd, src);
    
    assign_ndarray_slicing(src, slice, slice_nd, result);
	
	return result;
    fail:
       return NULL; 
}

static void
assign_ndarray_slicing(NdArray *src, SliceStruct **slices, int slice_nd, NdArray *out_res)
{
    if (src == NULL || slices == NULL || out_res == NULL) {
        SET_ERROR_MESSAGE("assign_ndarray_slicing: src, slices or out_res is NULL.");
        return;
    }
/*
    src の形状 (4, 5)
    src =
    [[0,  1,  2,  3,  4],   // index 0
     [5,  6,  7,  8,  9],   // index 1
    [10, 11, 12, 13, 14],  // index 2
    [15, 16, 17, 18, 19]]  // index 3

    slices = [(1, 3, 1), (2, 4, 1)]
    result =
    [[7,  8], // *slicesのmax次元側からindexをインクリメントしないといけない
    [12, 13]]

    / 負のstep /
    slices = [(2, 0, -1), (3, 1, -1)]
    result =
    [[13, 12],
     [8,  7]]

	/ slicing指定されていない次元が存在する場合 /
	src の形状 (4, 5, 3)
	src =
	[[[0,  1,  2],  [3,  4,  5],  [6,  7,  8],  [9,  10, 11], [12, 13, 14]],  // index 0
 	[[15, 16, 17], [18, 19, 20], [21, 22, 23], [24, 25, 26], [27, 28, 29]],  // index 1
 	[[30, 31, 32], [33, 34, 35], [36, 37, 38], [39, 40, 41], [42, 43, 44]],  // index 2
 	[[45, 46, 47], [48, 49, 50], [51, 52, 53], [54, 55, 56], [57, 58, 59]]]  // index 3

	slices = [(1, 3, 1), (2, 4, 1)] //(1, 2, 0) ~ (2, 3, 2)

	result =
	[[[21, 22, 23], [24, 25, 26]],
 	[[36, 37, 38], [39, 40, 41]]]
*/
    /* init indices */
    int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
    memset(src_indices, 0, sizeof(src_indices));
    for (int d = 0; d < slice_nd; d++) {
       src_indices[d] = slices[d]->start;
    }

	/* count remaining elements */
	int64_t remaining = 0;
    if (src->nd > slice_nd) {
        remaining = get_totalelements(src->nd - slice_nd, src->dimensions + slice_nd);
    }
    else {
		remaining = 1;
	}

    /* slicing copy */
    int64_t copied_elements = 0;
    char *src_address = NULL, *res_address = NULL;
    while (true) {
    	src_address = check_scalar(src)? src->data : get_address(src->data, src_indices, src->strides, src->nd);
    	res_address = out_res->data + copied_elements * out_res->itemsize;
    	memcpy(res_address, src_address, src->itemsize * remaining);
		copied_elements += remaining;

    	for (int d = slice_nd - 1; d > -1; d--) {
            src_indices[d] += slices[d]->step;
            if (slices[d]->sign == 1 ? src_indices[d] < slices[d]->stop : src_indices[d] > slices[d]->stop) {
                break;
            }
            else {
                if (d == 0) {
                    goto done;
                } else {
                	src_indices[d] = slices[d]->start;
				}
            }
        }
    }
    done:;
}

static NdArray*
slicingndarray_create(SliceStruct **slice, int slice_nd, NdArray *src)
{
    if (slice == NULL || src == NULL) {
        SET_ERROR_MESSAGE("slicingndarray_create: slice or src is NULL.");
        goto fail;
    }
    int nd = src->nd;
    int64_t dimensions[NDARRAY_MAX_DIMENSIONS]; 
    for (int d = 0; d < src->nd; d++) {
        if (d < slice_nd)  {
			if (slice[d]->sign == 1) {
            	dimensions[d] = (slice[d]->stop - slice[d]->start + slice[d]->step - 1) / slice[d]->step;
			} else if (slice[d]->sign == -1) {
				dimensions[d] = (slice[d]->start - slice[d]->stop - slice[d]->step - 1) / (-slice[d]->step);
			} else {
    			SET_ERROR_MESSAGE("slicingndarray_create: sign is not set.");
    			goto fail;
			}
        }
        else {
            dimensions[d] = src->dimensions[d];
        }
    }
    NdArray *result = ndarray_create(nd, dimensions, src->itemsize, src->sdtype);
    return result;
    fail:
        return NULL;
}

static bool
check_sliceconditions_and_assignstepsign(SliceStruct **out_slice, int slice_nd)
{
    for (int i = 0; i < slice_nd; i++) {
        if (out_slice[i]->step == 0) { //0
            return false;
        }
        if (out_slice[i]->step > 0) { //正
            if (out_slice[i]->start > out_slice[i]->stop) { //&& 始点 > 終点
                return false; //error
            }
            else {
                out_slice[i]->sign = 1;
            }
        }
        if (out_slice[i]->step < 0) { //負
            if (out_slice[i]->stop > out_slice[i]->start) { //&& 終点 > 始点
                return false;
            }
            else {
                out_slice[i]->sign = -1;
            }
        }
    }
    return true;
}

static void
set_ndarray_slicing(NdArray *out_src, SliceStruct **slice, int slice_nd, NdArray *value)
{
    if (out_src == NULL || slice == NULL || value == NULL) {
        SET_ERROR_MESSAGE("set_ndarray_slicing: out_src, slice or value is NULL.");
        goto fail;
    }
    
	if (slice_nd > out_src->nd) {
        SET_ERROR_MESSAGE("np_slice: slices_nd exceeds the number of dimensions of src.");
        goto fail;
    }
    
    bool conditions = check_sliceconditions_and_assignstepsign(slice, slice_nd);
    if (!conditions) {
        SET_ERROR_MESSAGE("np_slice: invalid slice parameter detected. Step cannot be zero.");
        goto fail;
    }
    
    assign_ndarray_slicing(value, slice, slice_nd, out_src);
    fail:
        return;
}