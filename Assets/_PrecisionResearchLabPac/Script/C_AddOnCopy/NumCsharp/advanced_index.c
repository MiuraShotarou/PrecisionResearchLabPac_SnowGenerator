機能： 引数の配列はbool[]として機能する（ブールインデックス）。または、インデックスの配列として機能する（ファンシーインデックス）。C#標準提供の配列も戻り値にできる予定
//形状は変化する。
//mask の次元数が srcの次元数より大きくなければ処理できる。dimensions の各要素数は一致していなければならない
// mask と src の形状が同じなら、結果は必ず一次元配列になる。src の次元数 > mask の次元数 だった場合、maskに指定されている範囲内でindexing操作が行われる。
//mask == bool ならboolIndex, unmanagedならfancyIndex, それ以外の型はエラーになる → C#側で実装
//slice処理

NdArrayIndexer.csで定義されたインデクサーから呼び出す関数を実装する
戻り値がNdArray *以外のメソッドは実装しない
#include <stdbool.h>

// void * /= jag, 関数ポインタ //jagはキャストが必要（(void *)int *）。

static NdArray*
get_ndarray_advancedindexing(NdArray *src, NdArray *mask)
{
    // ①src->nd >= mask->nd のエラーチェック
    if (mask->nd > src->nd) {
        //error
        return NULL;
    }
    
    // ②dimensionsの各要素数が一致しているかのエラーチェック
    for (int d = 0; d < mask->nd; d++) {
        if (src->dimensions[d] != mask->dimensions[d]) {
            return NULL;
        }
    }
    
    // geter用 NdArrayを形状だけ一致させ新規生成
    // NdArray *result = ndarray_create(src->nd, src->dimensions, src->itemsize);
    NdArray *result = ndarray_create(src->nd, src->dimensions, src->itemsize, src->sdtype); //空で良い

    if (mask->sdtype == Bool) // == SDType.Bool
    {
        // boolindexing用のresult配列作成関数を実装
        result = boolindexingndarray_create(src, mask);
        assign_ndarray_boolindexing(src, mask, result);
    }
    else
    {
        assign_ndarray_fancyindexing(src, mask, result);
    }
    return result;
}
static NdArray*
boolindexingndarray_create(NdArray *src, NdArray *mask)
{
    // src, maskのnullチェック
    
    // ①ndの確定
    int nd = 0;
    // src->nd == 1 && mask->nd > 1
    if (src->nd == mask->nd) {
        // result は必ず一次元配列
        nd = 1; //macro化
        //dimensions[0] = n;
    }
    else //if (src->nd > mask->nd)
    {
        nd = src->nd;
    }
    
    // ②dimensionsの確定
    int64_t dimensions[64];
    memset(dimensions, 0, sizeof(dimensions));
    for (int nd_i = 0; nd_i < src->nd; nd_i++) {
        // 現状、src->nd ぶん繰り返すのでmask->dimensionsが範囲外アクセスになってしまう
        // mask->dimensionsにアクセスできる範囲なら
        if (nd_i < mask->nd) {
            // maskの要素を取り出し、条件チェック
            for (int dim_i = 0; dim_i < mask->dimensions[nd_i]; dim_i++) {
                bool value;
                memcpy(&value, mask->data + mask->strides[nd_i] * dim_i, sizeof(bool));
                if (value) {
                    dimensions[nd_i]++;
                }
            }
        }
        // mask->dimensions にアクセスできなくなった場合、
        else {
            // result->dimensions[nd_i] に直接 src->dimensions[nd_i]を代入する
            dimensions[nd_i] = src->dimensions[nd_i];
        }
    }

    // ③itemsize, sdtypeの設定
    NdArray *result = ndarray_create(nd, dimensions, src->itemsize, src->sdtype);
    
    if (result == NULL) {
        SET_ERROR_MESSAGE("get_ndarray_boolIndex: ndarray_create failed.");
        return NULL;
    }
    
    return result;
}

static NdArray*
assign_ndarray_boolindexing(NdArray *src, NdArray *mask, NdArray *out_res) //NdArray, Jag, 固定長
{
    // ①mask形状 >= src形状 のエラーチェック
    if (mask->nd > src->nd) {
        //error
        return NULL;
    }
    // ②src == out_res の形状のチェック
    if (src->nd != out_res->nd || src->dimensions != out_res->dimensins || src->itemsize != out_res->itemsize || src->sdtype != out_res->sdtype) {
        return NULL;
    }
    // ① は呼び出し元のメソッドでも実装している
    
    // indexing専用のresult NdArray生成関数を書いても良いかも → 書いた
    
    // ③srcの種類ごとに別の方法で要素にアクセスし、result用プロパティに値を代入する。
    int64_t mask_total = get_totalelements(mask->nd, mask->dimensions);
    int64_t res_i = 0;
    for (int f = 0; f < mask_total; f++) {
        // maskの要素をコピーする
        bool value;
        memcpy(&value, mask->data + f * mask->itemsize, sizeof(bool));
        // bool is true であればout_res に src の値を代入。out_resの形状が既に決まっているならば、順々にアクセスしていくだけで良い
        if(value) {
            memcpy(out_res->data + (res_i++ * out_res->itemsize), src->data + (f * src->itemsize), out_res->itemsize);
        }
    }
    
    // get 側でsrcの解放は行わない
    
    // 終了
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