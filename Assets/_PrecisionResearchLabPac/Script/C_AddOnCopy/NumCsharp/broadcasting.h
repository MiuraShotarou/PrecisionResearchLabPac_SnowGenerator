// broadcasting.h
#ifndef BROADCASTING_H
#define BROADCASTING_H

#include <stdint.h>
#include <stdbool.h>
#include "arrayobject.h"
#include "static.h"
#include "error.h"
#include "macro.h"

// 形状一致型のブロードキャストが可能かどうかを判断する
static bool check_broadcastable(NdArray *a, int b_nd, int64_t *b_dimensions);

// 形状拡張型のブロードキャストが可能かどうかを判断する
static bool check_broadcastexpand(NdArray *src, int dest_nd, int64_t *dest_dimensions);

// 形状一致型のブロードキャストを行い、結果として確定される形状をnd, dimensionsパラメータで取得する
static void assign_broadcastingshape(NdArray *a, NdArray *b, int *out_nd, int64_t *out_dimensions);

// 既存ndarrayのstridesをブロードキャスト用に更新する
static void assign_broadcastingstrides(NdArray *src, NdArray *dest, int64_t *out_strides);

#endif /* BROADCASTING_H */