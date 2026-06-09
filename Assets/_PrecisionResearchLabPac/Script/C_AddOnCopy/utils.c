// utils.c
// static.c → utils.cへリネーム
// DoubleScalarCast関連の実装を cast.cに移行した
#include <stdbool.h>
#include <assert.h>
#include "utils.h"

int64_t
get_totalelements(int size_nd, int64_t *size)
{
    int64_t result = 1;
    for (int i = 0; i < size_nd; i++) {
        result *= size[i];
    }
    return result;
}

int64_t
get_last_flat(int size_nd, int64_t *size)
{
    return get_totalelements(size, size_nd) - 1;
}

/* index アクセス系 */
void
assign_indices(int nd, int64_t *dimensions, int64_t flat, int64_t *out_indices)
{
    int64_t tmp = flat;
    for (int d = nd - 1; d > -1; d--) {
        out_indices[d] = tmp % dimensions[d];
        tmp /= dimensions[d];
    }
}

/* indices to flat */
int64_t
get_flat(int nd, int64_t *dimensions, int64_t *indices)
{
    int64_t flat = 0;
    int64_t stride = 1;
    for (int d = nd - 1; d > -1; d--) {
        flat += indices[d] * stride;
        stride *= dimensions[d];
    }
    return flat;
}

/* increment indices */
void
increment_indices(int64_t *indices, int64_t *dimensions, int nd, int64_t *out_indices)
{
    bool carry = true;
    for (int d = nd - 1; d > -1 && carry; d--) {
        indices[d]++;
        if (indices[d] < dimensions[d]) {
            carry = false;
        } else {
            indices[d] = 0;
        }
    }
    if (carry) {
        assert(true);
    }
}

/* indices & strides to adress */
char*
get_address(char *pointer, int64_t *indices, int64_t *strides, int nd)
{
    char *ptr = pointer;
    for (int d = 0; d < nd; d++) {
        ptr += indices[d] * strides[d];
    }
    return ptr;
}

/* get adjust axis (index)*/
int
get_adjust_axis(int axis, int nd)
{
    return get_adjust_index(axis, nd);
}

int
get_adjust_index(int index, int nd)
{
    return (index < 0) ? nd + index : index;
}

/* assign_creaetstrides */
void
assign_creaetstrides(int src_nd, int64_t *src_dimensions, int itemsize, int64_t *out_strides)
{
    int64_t stride = (int64_t)itemsize;
    for (int i = src_nd - 1; i > -1; i--) {
        out_strides[i] = stride;
        stride *= src_dimensions[i];
    }
}

int64_t
get_recalculatstride(int index, int src_nd, int64_t *src_dimensions, int itemsize)
{
    int64_t stride = (int64_t)itemsize;
    for (int i = index + 1; i < src_nd; i++) {
        stride *= src_dimensions[i];
    }
    return stride;
}