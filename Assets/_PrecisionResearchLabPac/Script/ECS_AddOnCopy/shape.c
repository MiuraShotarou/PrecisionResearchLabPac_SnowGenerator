#include <string.h>
#include "shape.h"
#include "arrayobject.h"
#include "error.h"

concatenate //(int axis) の軸に従って、((配列1), (配列2))で指定した複数の配列を結合する。新しい次元の追加はしない。
stack       //(int axis) の軸に従って、((配列1), (配列2))で指定した複数の配列を結合する。新しく次元を追加する
reshape     //(配列)を、行数、列数に対応した多次元配列に変換する
shape       //(配列)の各次元のサイズをタプルで返すプロパティ
resize      //(配列)を、行数、列数に対応した多次元配列に変換する。resultの要素数が足りなくてもそれを補うようにまた先頭の要素からコピーしていく
squeeze     //(配列)からサイズが1の次元を除去する関数
transpose   //(配列)の軸の順序を入れ替える

static NdArray* //(int axis) の軸に従って、((配列1), (配列2))で指定した複数の配列を結合する。新しい次元の追加はしない
np_concatenate(NdArray **arrays, int array_count, int axis, SDType restype) //shapeをふんだんに利用してコーディングされている
{
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
    
    
    /*
     * a.dimensions == (2,4) b.dimensions == (1,4) axis == 0
     * [[a, a, a, a], [[b,b,b,b]]
     * [a, a, a, a]]
     * result.dimensions == (3,4)
     * [[a, a, a, a],
     *  [a, a, a, a],
     *  [b, b, b, b]]
     *  
     * a.dimensions == (2,2) b.dimensions == (2,4) axis == 1
     * [[a, a], [[b,b,b,b],
     *  [a, a]]  [b,b,b,b]]
     * result.dimensions == (2,6)
     * [[a, a, b, b, b, b],
     *  [a, a, b, b, b, b]]
     *  
     * a.dimensions == (2,3,4) b.dimensions == (2,3,4) axis == 0
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
        
     * a.dimensions == (2,3,4) b.dimensions == (2,3,4) axis == 1
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
         
     * a.dimensions == (2,3,4) b.dimensions == (2,3,4) axis == 2
    /*
    [[[a, a, a, a, a, a],
      [a, a, a, a, a, a],
      [a, a, a, a, a, a]],
     [[a, a, a, a, a, a],
      [a, a, a, a, a, a],
      [a, a, a, a, a, a]]
    ]
    [[[b, b, b, b],
      [b, b, b, b],
      [b, b, b, b]],
     [[b, b, b, b],
      [b, b, b, b],
      [b, b, b, b]]
    ]
    */
    
    /* result配列の形状を計算 */
    int axis_size = 0;
    for (int i = 0; i < array_count; i++) {
        axis_size += arrays[i]->dimensions[axis];
    }
    
    int res_nd = arrays[0]->nd;
    int64_t* res_dims[64];
    for (int i = 0; i < res_nd; i++) {
        res_dims[i] = i == axis? axis_size : arrays[0]->dimensions[i];
    }
    
    /* 型のメモリサイズを計算 */
    int itemsize = itemsize_cast_by_sdtype(restype);
    
    /* 形状を反映させたresult配列を作成 */
    NdArray* result = ndarray_create(res_nd, res_dims, itemsize);
    
	// axis に従って結合処理を分岐させる
    // 先に、結果として出力される配列の形状を計算しておく
    
    // フラット → 多次元
    int64_t tmp = flat;
    for (int d = nd - 1; d >= 0; d--) {
        indices[d] = tmp % dimensions[d];
        tmp /= dimensions[d];
    }
    
    // 多次元 → フラット
    int64_t flat = 0; //
    int64_t stride = 1; //
    for (int d = nd - 1; d > -1; d--) { //nd == 10 → 9 ~ 0
        flat += indices[d] * stride; //
        stride *= dimensions[d];
    }
    
    // 多次元カウンタを1ずつ進める
    int carry = 1;
    for (int d = nd - 1; d >= 0 && carry; d--) {
        indices[d]++;
        if (indices[d] < dimensions[d]) {
            carry = 0;
        } else {
            indices[d] = 0;  // 桁上がり
        }
    }
    
    // ストライドを使った要素アクセス
    char *ptr = data;
    for (int d = 0; d < nd; d++) {
        ptr += indices[d] * strides[d];
    }
    
    // axis次元のオフセット管理
    int64_t axis_idx = res_indices[axis];
    int src_array = 0;
    while (axis_idx >= arrays[src_array]->dimensions[axis]) {
        axis_idx -= arrays[src_array]->dimensions[axis];
        src_array++;
    }
    
    int64_t total = 1;
    for (int i = 0; i < result->nd; i++) {
        total *= result->dimensions[i];
    }
    
    
    memcpy(result->data + i * itemsize, &value, itemsize);
    
    // その配列に、for 文を使って要素を入れていく
    // 終了、で良いんじゃないだろうか
}

/* array A B Concat */
static NdArray*
np_stack(NdArray **arrays, int32_t array_count, uint32_t axis, SDType restype)
{
	if (arrays == NULL || array_count <= 0) {
    	SET_ERROR_MESSAGE("np_stack: arrays is NULL or array_count is invalid.");
    	return NULL;
    }

    /* 全配列の形状が一致するか確認 */
    int src_nd = arrays[0]->nd;
    int itemsize = arrays[0]->itemsize;
    for (int i = 1; i < array_count; i++) {
        if (arrays[i]->nd != src_nd) {
            SET_ERROR_MESSAGE_ARGUMENT("np_stack: arrays[%d] has different nd.", i);
            return NULL;
        }
        for (int d = 0; d < src_nd; d++) {
            if (arrays[i]->dimensions[d] != arrays[0]->dimensions[d]) {
                SET_ERROR_MESSAGE_ARGUMENT("np_stack: arrays[%d] has different shape.", i);
                return NULL;
            }
        }
    }

    /* axisの範囲チェック */
    int res_nd = src_nd + 1; //+1 次元
    if (axis > src_nd) { //C#から防ぎたいかも
        SET_ERROR_MESSAGE_ARGUMENT("np_stack: axis %d is out of range.", axis);
        return NULL;
    }

    /* 出力配列の形状を計算 */
    int64_t res_dims[64];
    for (int res_d = 0; res_d < res_nd; res_d++) {
        if (res_d == axis) {
            res_dims[res_d] = (int64_t)array_count; //追加した次元の要素数を代入している
        } else {
            int src_d = res_d < axis ? res_d : res_d - 1;
            res_dims[res_d] = arrays[0]->dimensions[src_d]; //それ以外はsroceの要素数と同じ要素数で登録
        }
    }
	
    /* 出力配列を生成 */
    NdArray *result = ndarray_create(res_nd, res_dims, itemsize);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_stack: ndarray_create failed.");
        return NULL;
    }

    /* res_stridesを計算 */
    int64_t res_strides[64];
    res_strides[res_nd - 1] = 1;
    for (int d = res_nd - 2; d >= 0; d--) {
        res_strides[d] = res_strides[d + 1] * res_dims[d + 1];
    }

    /* 各配列のデータをresultにコピー */
    int64_t src_total = get_totalelements(arrays[0]->dimensions, src_nd);

    for (int i = 0; i < array_count; i++) {
        for (int64_t idx = 0; idx < src_total; idx++) {
            /* srcのフラットインデックスから各次元のインデックスを計算 */
            int64_t src_indices[64];
            int64_t tmp = idx; // 0 ~ src_total
            for (int d = src_nd - 1; d >= 0; d--) { // 次元数 -1 → indexアクセスに使用するため
                src_indices[d] = tmp % arrays[i]->dimensions[d]; //
                tmp /= arrays[i]->dimensions[d];
            }

            /* res上のフラットインデックスを計算 */
            int64_t res_idx_flat = 0;
            int src_d = 0;
            for (int d = 0; d < res_nd; d++) {
                if (d == axis) {
                    res_idx_flat += (int64_t)i * res_strides[d];
                } else {
                    res_idx_flat += src_indices[src_d++] * res_strides[d];
                }
            }

            memcpy(
                result->data + res_idx_flat * itemsize,
                arrays[i]->data + idx * itemsize,
                itemsize
            );
        }
    }

    return result;
}


static NdArray*
np_reshape(NdArray *array, int64_t *size, int size_nd)
{
    int64_t src_total = get_totalelements(array, array->nd);
    int64_t res_total = get_totalelements(size, size_nd);
    if (src_total != res_total) {
        SET_ERROR_MESSAGE_ARGUMENT("cannot reshape array of size %lld into shape with size %lld.", src_total, res_total);
        return NULL;
    }
    
    int itemsize = array->itemsize;
    NdArray *result = ndarray_create(size_nd, size, itemsize);
    //null check
    
    memcpy(result->data, array->data, res_total * array->itemsize);
    return result;
}