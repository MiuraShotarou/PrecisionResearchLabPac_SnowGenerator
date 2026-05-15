#include <stdlib.h>
#include <stdint.h>

// ----------------------------------------------------------------
// 型定義
// ----------------------------------------------------------------
/* struct NdArray */
typedef struct {
    char    *data;          // 実データへのポインタ（先頭アドレス）
    int      nd;            // 次元数
    int64_t *dimensions;   // 各次元のサイズ
    int64_t *strides;      // 各次元の配列内において、多次元配列全体で見たときに要素同士が何バイト（距離）離れているか → 転置ができる
    int   itemsize;      // 1要素のバイト数 → 実質データ型でありstridesでもある
	SDType sdtype;		// new! NdArrayから取得出来るように変更予定
} NdArray;

NdArray* ndarray_create(int nd, int64_t *dimensions, int itemsize, SDType sdtype); //引数に既存のコレクションを指定
NdArray* ndarray_empty(int row, int col, int itemsize); //未初期化での生成
NdArray* ndarray_copy(NdArray *arr);
NdArray* ndarray_convert(void *src, int nd, int64_t *dimensions, int itemsize, SDType sdtype);
void     ndarray_free(NdArray *arr);
void*    ndarray_get(NdArray *arr, int64_t *indices);

/* 便利系 */
// NdArrayの要素数が１であることをboolで取得するメソッド
static bool check_scalar(NdArray *arr);

//Numpy copy
typedef struct {
    int64_t stride;
    int perm;
} npy_stride_sort_item;

typedef struct {
    PyArrayMethod_StridedLoop *func;  // 実際のキャスト処理関数ポインタ
    NpyAuxData *auxdata;              // キャスト処理に必要な補助データ
    PyArrayMethod_Context context;    // キャストのコンテキスト情報
    PyArray_Descr *descriptors[2];    // [0]=src dtype, [1]=dst dtype
} NPY_cast_info;

/* struct  */

#endif