#ifndef NUMPY_METHODS_H
#define NUMPY_METHODS_H
 
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "enum.h"

// SDType指定でキャスト用メソッドを呼び出す配列 ○
typedef void (*DoubleScalarCast)(char *dest, double value);
extern DoubleScalarCast doublescalar_cast_by_sdtype[];
// SDTypeごとに double value を適性な型にキャストするメソッドの宣言 ○
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

// SDTypeを入れるとitemsizeがわかるメソッドの宣言 ○
int itemsize_cast_by_sdtype (SDType sdtype);

// 配列の総要素数を取得するメソッド
int64_t get_totalelements(int64_t *size, int size_nd);

// NdArrayのindicesを取得するメソッド
static void get_indices(int64_t flat, int64_t *dimensions, int nd, int64_t *out_indices); //out_indices == int64_t *[64]