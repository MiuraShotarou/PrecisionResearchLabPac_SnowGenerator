// static.h
#ifndef NUMPY_METHODS_H
#define NUMPY_METHODS_H
 
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "enum.h"

// SDType指定でキャスト用メソッドを呼び出す配列
typedef void (*DoubleScalarCast)(char *dest, double value);
extern DoubleScalarCast doublescalar_cast_by_sdtype[];

// SDTypeごとに double value を適性な型にキャストするメソッドの宣言
void double_to_bool   (char *dest, double value);
void double_to_sbyte  (char *dest, double value);
void double_to_byte   (char *dest, double value);
void double_to_short  (char *dest, double value);
void double_to_ushort (char *dest, double value);
void double_to_int    (char *dest, double value);
void double_to_uint   (char *dest, double value);
void double_to_long   (char *dest, double value);
void double_to_ulong  (char *dest, double value);
void double_to_float  (char *dest, double value);
void double_to_double (char *dest, double value);

// 各scalar値を持つアドレスをdouble型の値に変換するメソッドの宣言
double address_to_double(char *address, SDType sdtype);

// SDTypeを入れるとitemsizeがわかるメソッドの宣言
int itemsize_cast_by_sdtype(SDType sdtype);

// 配列の総要素数を取得するメソッド
int64_t get_totalelements(int size_nd, int64_t *size);

// 配列を一次元化した際の末尾インデクスを取得するメソッド
int64_t get_last_flat(int size_nd, int64_t *size);

// flatを引数に渡して、indicesを取得するメソッド
void get_indices(int nd, int64_t *dimensions, int64_t flat, int64_t *out_indices);

// indices & strides to address
char* get_address(char *pointer, int64_t *indices, int64_t *strides, int nd);

// indicesを引数に渡して、flatを取得するメソッド
int64_t get_flat(int nd, int64_t *dimensions, int64_t *indices);

// indicesを引数に渡して、index++ 時のindicesを取得するメソッド
void carry_indices(int64_t *dimensions, int nd, int64_t *out_indices);

// 負のindexの変換処理
int get_adjust_index(int axis, int nd);
int get_adjust_axis(int axis, int nd);

// ストライドの再計算
int64_t get_recalculatstride(int index, int src_nd, int64_t *src_dimensions, int itemsize);

// ストライドの計算
void assign_creaetstrides(int src_nd, int64_t *src_dimensions, int itemsize, int64_t *out_strides);

#endif /* NUMPY_METHODS_H */