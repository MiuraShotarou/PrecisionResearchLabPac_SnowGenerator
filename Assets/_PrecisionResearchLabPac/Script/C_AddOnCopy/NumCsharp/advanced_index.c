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

/* get advancedindexing */
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
    NdArray *result = NULL; //空で良い
    
    switch (mask->sdtype) {
        case Bool:
            result = boolindexingndarray_create(src, mask);
            assign_ndarray_boolindexing(src, mask, result);
            break;
        case Int:
            result = fancyindexingndarray_create(src, mask);
            assign_ndarray_fancyindexing(src, mask, result);
            break;
        case UInt:
        case Short:
        case UShort:
        case Long:
        case ULong:
        case SByte:
        case Byte:
            SET_ERROR_MESSAGE("Casting to int type is recommended in the calling code.");
            result = fancyindexingndarray_create(src, mask);
            assign_ndarray_fancyindexing(src, mask, result);
            break;
        default:
            SET_ERROR_MESSAGE("get_ndarray_advancedindexing: unsupported mask sdtype.");
            return NULL;
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
    int64_t n = 0; //mask内のtrue数
    int64_t mask_total = get_totalelements(mask->nd, mask->dimensions);
    for (int64_t f = 0; f < mask_total; f++) {
        bool value;
        memcpy(&value, mask->data + f * mask->itemsize, sizeof(bool));
        if (value) {
            n++;
        }
    }
    dimensions[0] = n;
    for (int d = mask->nd; d < src->nd; d++) {
        dimensions[d - mask->nd + 1] = src->dimensions[d]; //dimensions[1]……
    }

    // ③itemsize, sdtypeの設定
    NdArray *result = ndarray_create(nd, dimensions, src->itemsize, src->sdtype);
    
    if (result == NULL) {
        SET_ERROR_MESSAGE("get_ndarray_boolIndex: ndarray_create failed.");
        return NULL;
    }
    
    return result;
}

static void
assign_ndarray_boolindexing(NdArray *src, NdArray *mask, NdArray *out_res)
{
    // ①mask形状 >= src形状 のエラーチェック
    if (mask->nd > src->nd) {
        //error
        return;
    }
    // ① は呼び出し元のメソッドでも実装している
    
    // indexing専用のresult NdArray生成関数を書いても良いかも → 書いた
    
    // ②srcの種類ごとに別の方法で要素にアクセスし、result用プロパティに値を代入する。
    // maskの次元数がsrcの次元数を超えていれば、スカラー代入ではなく子のdimensionに含まれるすべての要素を代入する
    int64_t remaining = 1;
    if (src->nd > mask->nd) {
        for (int d = mask->nd; d < src->nd; d++) {
            remaining *= src->dimensions[d];
        }
    }
    // mask内のtrueをすべてコピーする
    int64_t mask_total = get_totalelements(mask->nd, mask->dimensions);
    int64_t res_i = 0;
    for (int f = 0; f < mask_total; f++) {
        // maskの要素をコピーする
        bool value;
        memcpy(&value, mask->data + f * mask->itemsize, sizeof(bool));
        // bool is true であればout_res に src の値を代入。out_resの形状が既に決まっているならば、順々にアクセスしていくだけで良い
        if(value) {
            memcpy(out_res->data + (res_i++ * out_res->itemsize * remaining), src->data + (f * src->itemsize * remaining), out_res->itemsize * remaining);
        }
    }
    // get 側でsrcの解放は行わない
    // 終了
}

static NdArray*
fancyindexingndarray_create(NdArray *src, NdArray *mask)
{
    // src, maskのnullチェック
    
    // ①ndの確定
    int nd = 0;
    if (src->nd == mask->nd) {
        nd = 1; //macro化
    }
    else //if (src->nd > mask->nd)
    {
        nd = src->nd;
    }
    // ②dimensionsの確定
    int64_t dimensions[64];
    memset(dimensions, 0, sizeof(dimensions));
    int64_t mask_total = get_totalelements(mask->nd, mask->dimensions);
    dimensions[0] = mask_total;  // maskの総要素数
    for (int d = mask->nd; d < src->nd; d++) {
        dimensions[d - mask->nd + 1] = src->dimensions[d];  // 残りの次元
    }

    // ③itemsize, sdtypeの設定
    NdArray *result = ndarray_create(nd, dimensions, src->itemsize, src->sdtype);
    
    if (result == NULL) {
        SET_ERROR_MESSAGE("fancyindexingndarray_create: ndarray_create failed.");
        return NULL;
    }
    
    return result;
}

static void
assign_ndarray_fancyindexing(NdArray *src, NdArray *mask, NdArray *out_res)
{
    // ①mask形状 >= src形状 のエラーチェック
    if (mask->nd > src->nd) {
        //error
        return;
    }
    // ②maskの型チェック
    if (mask->itemsize != sizeof(int32_t) || mask->sdtype != Int) {
        return;
    }
    
    // ③srcの種類ごとに別の方法で要素にアクセスし、result用プロパティに値を代入する。
    // maskの次元数がsrcの次元数を超えていれば、スカラー代入ではなく子のdimensionに含まれるすべての要素を代入する
    int64_t remaining = 1;
    if (src->nd > mask->nd) {
        for (int d = mask->nd; d < src->nd; d++) {
            remaining *= src->dimensions[d];
        }
    }
    
    // mask内のtrueをすべてコピーする
    int64_t mask_total = get_totalelements(mask->nd, mask->dimensions);
    int64_t res_i = 0;
    int d_i = 0;
    int dim_i = 0;
    for (int f = 0; f < mask_total; f++) {
        // maskの要素をコピーする
        int value;
        memcpy(&value, mask->data + f * mask->itemsize, sizeof(int32_t));
        value = adjust_axis(value, src->dimensions[d_i]);
        // indexの範囲チェック
        if (value < 0 || value >= src->dimensions[d_i]) {
            SET_ERROR_MESSAGE("assign_ndarray_fancyindexing: index out of range.");
            return;
        }
        // bool is true であればout_res に src の値を代入。out_resの形状が既に決まっているならば、順々にアクセスしていくだけで良い
        memcpy(out_res->data + (res_i++ * out_res->itemsize * remaining), src->data + (value * src->itemsize * remaining), out_res->itemsize * remaining);
        
        // d_i & dim_i のインクリメント
        if (++dim_i == mask->dimensions[d_i]) {
            d_i++;
            dim_i = 0;
        }
    }
    // get 側でsrcの解放は行わない
    // 終了
}

/* set advancedindexing */
static void
set_ndarray_advancedindexing(NdArray *out_src, NdArray *mask, NdArray *value)
{
    // ①src->nd >= mask->nd のエラーチェック
    if (mask->nd > out_src->nd) {
        //error
        return;
    }
    // ②dimensionsの各要素数が一致しているかのエラーチェック
    for (int d = 0; d < mask->nd; d++) {
        if (out_src->dimensions[d] != mask->dimensions[d]) {
            return;
        }
    }
    // geter用 NdArrayを形状だけ一致させ新規生成
    // NdArray *result = ndarray_create(src->nd, src->dimensions, src->itemsize);
    NdArray *src = NULL;
    if (check_scalar(out_src))
    {
        NdArray *src = ndarray_copy(out_src);
    }
    
    switch (mask->sdtype) {
        case Bool:
            assign_ndarray_boolindexing(src, mask, out_src);//※
            break;
        case Int:
            assign_ndarray_fancyindexing(src, mask, out_src);
            break;
        case UInt:
        case Short:
        case UShort:
        case Long:
        case ULong:
        case SByte:
        case Byte:
            SET_ERROR_MESSAGE("Casting to int type is recommended in the calling code.");
            assign_ndarray_fancyindexing(src, mask, out_src);
            break;
        default:
            SET_ERROR_MESSAGE("get_ndarray_advancedindexing: unsupported mask sdtype.");
            break;
    }
    //破壊的操作のため戻り値は無し
}