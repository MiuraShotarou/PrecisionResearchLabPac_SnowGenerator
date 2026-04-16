#include <stdlib.h>
#include <stdint.h>

// ----------------------------------------------------------------
// 型定義
// ----------------------------------------------------------------
typedef intptr_t int64_t;

typedef struct {
    char    *data;          // 実データへのポインタ
    int      nd;            // 次元数
    int64_t *dimensions;   // 各次元のサイズ
    int64_t *strides;      // 各次元でステップする際ののバイト数 → 転置ができる
    int   itemsize;      // 1要素のバイト数 → 実質データ型でありstridesでもある
} NdArray;

NdArray* ndarray_create(int nd, int64_t *dimensions, int itemsize); //引数に既存のコレクションを指定
NdArray* ndarray_empty(int row, int col, int itemsize); //未初期化での生成
void     ndarray_free(NdArray *arr);
void*    ndarray_get(NdArray *arr, int64_t *indices);

#endif