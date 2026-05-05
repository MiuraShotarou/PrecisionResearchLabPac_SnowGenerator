#include <string.h>
#include "shape.h"
#include "arrayobject.h"
#include "error.h"

concatenate //(int axis) の軸に従って、((配列1), (配列2))で指定した複数の配列を結合する。新しい次元の追加はしない。○
stack       //(int axis) の軸に従って、((配列1), (配列2))で指定した複数の配列を結合する。新しく次元を追加する ○
reshape     //(配列)を、行数、列数に対応した多次元配列に変換する ○
shape       //(配列)の各次元のサイズをタプルで返すプロパティ → arrayobject.cに移動 ○
resize      //(配列)を、行数、列数に対応した多次元配列に変換する。resultの要素数が足りなくてもそれを補うようにまた先頭の要素からコピーしていく
squeeze     //(配列)からサイズが1の次元を除去する関数
transpose   //(配列)の軸の順序を入れ替える

static NdArray* //(int axis) の軸に従って、((配列1), (配列2))で指定した複数の配列を結合する。新しい次元の追加はしない
np_concatenate(NdArray **arrays, int array_count, int axis, SDType restype) //shapeをふんだんに利用してコーディングされている
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
    
    /* 型のメモリサイズを計算 */
    int itemsize = itemsize_cast_by_sdtype(restype);
    
    /* 形状を反映させたresult配列を作成 */
    NdArray* result = ndarray_create(res_nd, res_dims, itemsize);
    
    // 配列代入処理（resultの破壊的操作）
    merge_arrays_along_axis(arrays, array_count, axis, itemsize, result);
    
    return result;
}

/* array A B Concat */
static NdArray*
np_stack(NdArray **arrays, int array_count, int axis, SDType restype)
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
    int itemsize = itemsize_cast_by_sdtype(restype);
    
    NdArray* result = ndarray_create(res_nd, res_dims, itemsize); //引数に既存のコレクションを指定
    
    // 配列代入処理（resultの破壊的操作）
    merge_arrays_along_axis(arrays, array_count, axis, itemsize, result);
    
    return result;
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