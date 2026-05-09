機能： 引数の配列はbool[]として機能する（ブールインデックス）。または、インデックスの配列として機能する（ファンシーインデックス）。C#標準提供の配列も戻り値にできる予定
//形状は変化する。
//mask の形状が srcの形状より大きくなければ処理できる
//mask == bool ならboolIndex, unmanagedならfancyIndex, それ以外の型はエラーになる → C#側で実装s
//slice処理

NdArrayIndexer.csで定義されたインデクサーから呼び出す関数を実装する
戻り値がNdArray *以外のメソッドは実装しない
#include <stdbool.h>

// void * /= jag, 関数ポインタ //jagはキャストが必要（(void *)int *）。

static NdArray*
get_ndarray_advancedindexing(NdArray *src, NdArray *mask)
{
    // geter用 NdArrayを形状だけ一致させ新規生成
    NdArray *result = ndarray_create(src->nd, src->dimensions, src->itemsize);
    if (mask->sdtype == SDType.Bool)
    {
        assign_ndarray_boolindexing(src, mask, result);
    }
    else
    {
        assign_ndarray_fancyindexing(src, mask, result);
    }
    return get;
}

static NdArray* // mask == dimensions
get_ndarray_boolindex(NdArray *src, NdArray *mask, NdArray *out_res) //NdArray, Jag, 固定長
{
    // ①mask形状 >= src形状 のエラーチェック
    if (src->nd >= mask->nd || ) {
        //error
        return NULL;
    }
    // ②src == out_res の形状のチェック
    if (src->nd != out_res->nd || src->dimensions != out_res->dimensins || src->itemsize != out_res->itemsize) {
        return NULL;
    }
    
    //形状が変化するため、必要
    int res_nd;
    int64_t res_dimension;
    // stridesも？ → indexing専用のresult NdArray生成関数を書いても良いかも
    
    // ③srcの種類ごとに別の方法で要素にアクセスし、result用プロパティに値を代入する。
    int64_t *totalelements = get_totalelements(mask->nd, mask->dimensions, mask->itemsize);
    for (int f = 0; f < totalelements; f++) {
        // src, mask共に共通のindexを求める → strides
        
        bool value = mask->data + i * itemsize;
        
        // mask 配列にindexでアクセスし、trueであれば分岐処理
        // maskの要素をout_resへ代入する。out_resの形状が既に決まっているならば、順々にアクセスしていくだけで良い
        
    }
    
    // get 側で行うのでsrcは解放しない
    
    
    NdArray *result = ndarray_create(int nd, int64_t *dimensions, int itemsize);
    
    if (masktype == VoidPtrType.NdArray) {
        _boolIndex(src, mask_nd, mask);
    }
}

static NdArray*

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