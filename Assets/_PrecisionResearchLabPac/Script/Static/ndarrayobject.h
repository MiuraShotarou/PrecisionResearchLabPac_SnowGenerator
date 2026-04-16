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
    int64_t *strides;      // 各次元のバイトステップ → 転置ができる
    int   itemsize;      // 1要素のバイト数
} NdArray;
