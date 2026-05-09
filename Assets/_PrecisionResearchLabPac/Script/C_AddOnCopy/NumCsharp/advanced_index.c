機能： 引数の配列はbool[]として機能する（ブールインデックス）。または、インデックスの配列として機能する（ファンシーインデックス）。C#標準提供の配列も戻り値にできる予定
//形状は変化する。
//mask の形状が srcの形状より大きくなければ処理できる
//mask == bool ならboolIndex, unmanagedならfancyIndex, それ以外の型はエラーになる → C#側で実装
//slice処理

NdArrayIndexer.csで定義されたインデクサーから呼び出す関数を実装する
戻り値がNdArray *以外のメソッドは実装しない
#include <stdbool.h>

// void * /= jag, 関数ポインタ //jagはキャストが必要（(void *)int *）。

static NdArray*
get_ndarray_advancedindexing(NdArray *src, NdArray *mask)
{
    if (mask->sdtype == SDType.Bool)
    {
        
    }
}

static NdArray* // mask == dimensions
get_ndarray_boolIndex(NdArray *src, NdArray *mask) //NdArray, Jag, 固定長
{
    
    
    // mask形状 == src形状 のエラーチェック
    // ①総要素数のチェック
    if (get_totalelements(src->dimensions, src->nd) != get_totalelements(mask, mask_nd))
    {
        //error
        return NULL;
    }
    // ②srcの種類ごとに別の方法で要素にアクセスし、result用プロパティに値を代入する。
    int res_nd = 0;
    int64_t res_dims[64];
    int itemsize = src->itemsize; //変更しない
    
    // result の形状を設定する
    
    for (int i = 0; i < mask_nd; i++) { //各次元にアクセス
        for (int j = 0; j < mask[i]; j++) { //各要素にアクセス
            // 現時点では、各次元の各要素にアクセスするループになっている → 実際は(要素index * cols(dimensions[i]) + 行index)
            // 考えられるパターン → 
            if ()
            
        }
    }
    
    // get 側で行うのでsrcは解放しない
    
    
    NdArray *result = ndarray_create(int nd, int64_t *dimensions, int itemsize);
    
    if (masktype == VoidPtrType.NdArray) {
        _boolIndex(src, mask_nd, mask);
    }
}

/* src == array && mask == NdArray || array */
static void*
get_array_boolIndex(void *src, void *mask, int mask_nd, VoidPtrType masktype)
{
    
}


static void
assign_dims_byboolindex(NdArray *src, int64_t *mask, int mask_nd, int64_t *out_dimension) //破壊的操作で本実装は対応 out_res
{
    // src → void* に変更予定
    
    // mask をbool[]として扱う繰り返し文
    for (int i = 0; i < mask_nd; i++) {
        // true扱いの場合、src から要素をコピーし、サイズをインクリメントし、形状を確定させる。
        
    }
    
    
    // 一致する要素だけ抽出する処理を書けば良い
}

assign_nd_byboolindex()