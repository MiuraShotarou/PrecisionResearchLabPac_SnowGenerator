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

static NdArray*
get_ndarray_slicing(NdArray *src, SliceStruct *slice, int slice_nd)
{
    if (slice_nd > src->nd) {
        SET_ERROR_MESSAGE("np_slice: slices_nd exceeds the number of dimensions of src.");
        return NULL;
    }
    
    bool conditions = check_sliceconditions_and_assignstepsign(slice, slice_nd);
    if (!conditions) {
        SET_ERROR_MESSAGE("np_slice: invalid slice parameter detected. Step cannot be zero.");
        return NULL;
    }
    assign_resolve_slice(slice, sign_nd);
    
    NdArray result = slicingndarray_create(slice, sign, src);
};

static NdArray*
slicingndarray_create(SliceStruct *slice, int sign_nd, NdArray *src)
{
    int nd = src->nd;
    // NdArray* ndarray_create(int nd, int64_t *dimensions, int itemsize, SDType sdtype);
    for (int i = 0; i < slice_nd; i++) {
        // TODO
    }
}

static void
assign_resolve_slice(SliceStruct *out_slice, int sign_nd)
{
    // ⓪NULLチェック
    
    // step < 0 → start > stop でなければならない
    // ①sliceパラメータの条件分岐 → sliceのパラメータ操作
    for (int i = 0; i < slice_nd; i++) {
        if (slice[i]->sign == 1) {
            
        }
        else {
            
        }
    }
    
}

static bool
check_sliceconditions_and_assignstepsign(SliceStruct *out_slice, int slice_nd)
{
    for (int i = 0; i < slice_nd; i++) {
        if (out_slice[i]->step == 0) { //0
            out_slice[i]->sign = 0; //なくて良い
            return false;
        }
        if (out_slice[i]->step > 0) { //正
            if (out_slice->start > out_slice->stop) { //&& 始点 > 終点
                out_slice[i]->sign = 0;
                return false; //error
            }
            else {
                out_slice[i]->sign = 1;
            }
        }
        if (out_slice[i]->step < 0) { //負
            if (out_slice->stop > out_slice->start) { //&& 終点 > 始点
                out_slice[i]->sign = 0;
                return false;
            }
            else {
                out_slice[i]->sign = -1;
            }
        }
    }
    return true;
}