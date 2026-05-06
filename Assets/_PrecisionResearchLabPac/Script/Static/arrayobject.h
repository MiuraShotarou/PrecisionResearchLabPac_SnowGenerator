#include <stdlib.h>
#include <stdint.h>

// ----------------------------------------------------------------
// 型定義
// ----------------------------------------------------------------
typedef intptr_t int64_t;

typedef struct {
    char    *data;          // 実データへのポインタ（先頭アドレス）
    int      nd;            // 次元数
    int64_t *dimensions;   // 各次元のサイズ
    int64_t *strides;      // 各次元で1要素進む際のバイト数 → 転置ができる
    int   itemsize;      // 1要素のバイト数 → 実質データ型でありstridesでもある
} NdArray;

NdArray* ndarray_create(int nd, int64_t *dimensions, int itemsize); //引数に既存のコレクションを指定
NdArray* ndarray_empty(int row, int col, int itemsize); //未初期化での生成
NdArray* ndarray_copy(NdArray *arr);
void     ndarray_free(NdArray *arr);
void*    ndarray_get(NdArray *arr, int64_t *indices);

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

#endif