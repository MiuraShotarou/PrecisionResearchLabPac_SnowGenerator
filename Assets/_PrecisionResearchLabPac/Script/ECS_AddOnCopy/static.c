#include "numpy_methods.h"

DoubleScalarCast doublescalar_cast_by_sdtype[] = {
    double_to_bool,
    double_to_sbyte,
    double_to_byte,
    double_to_short,
    double_to_ushort,
    double_to_int,
    double_to_uint,
    double_to_long,
    double_to_ulong,
    double_to_float,
    double_to_double,
};
void double_to_bool(char *dest, double value) {
    bool v = (bool)value;
    memcpy(dest, &v, sizeof(bool));
}
void double_to_sbyte(char *dest, double value) {
    int8_t v = (int8_t)value;
    memcpy(dest, &v, sizeof(int8_t));
}
void double_to_byte(char *dest, double value) {
    uint8_t v = (uint8_t)value;
    memcpy(dest, &v, sizeof(uint8_t));
}
void double_to_short(char *dest, double value) {
    int16_t v = (int16_t)value;
    memcpy(dest, &v, sizeof(int16_t));
}
void double_to_ushort(char *dest, double value) {
    uint16_t v = (uint16_t)value;
    memcpy(dest, &v, sizeof(uint16_t));
}
void double_to_int(char *dest, double value) {
    int32_t v = (int32_t)value;
    memcpy(dest, &v, sizeof(int32_t));
}
void double_to_uint(char *dest, double value) {
    uint32_t v = (uint32_t)value;
    memcpy(dest, &v, sizeof(uint32_t));
}
void double_to_long(char *dest, double value) {
    int64_t v = (int64_t)value;
    memcpy(dest, &v, sizeof(int64_t));
}
void double_to_ulong(char *dest, double value) {
    uint64_t v = (uint64_t)value;
    memcpy(dest, &v, sizeof(uint64_t));
}
void double_to_float(char *dest, double value) {
    float v = (float)value;
    memcpy(dest, &v, sizeof(float));
}
void double_to_double(char *dest, double value) {
    memcpy(dest, &value, sizeof(double));
}

int itemsize_cast_by_sdtype(SDType sdtype)
{
    switch (sdtype) {
    case Bool:   return sizeof(bool);
    case SByte:  return sizeof(int8_t);
    case Byte:   return sizeof(uint8_t);
    case Short:  return sizeof(int16_t);
    case UShort: return sizeof(uint16_t);
    case Int:    return sizeof(int32_t);
    case UInt:   return sizeof(uint32_t);
    case Long:   return sizeof(int64_t);
    case ULong:  return sizeof(uint64_t);
    case Float:  return sizeof(float);
    case Double: return sizeof(double);
    default:     return -1;  // エラー
    }
}
int64_t get_totalelements(int64_t *size, int size_nd)
{
    int64_t result = 1;
    for (int i = 0; i < size_nd; i++) {
        result *= size[i];
    }
    return result;
}

static void
get_indices(int64_t flat, int64_t *dimensions, int nd, int64_t *out_indices)
{
    int64_t tmp = flat;
    for (int d = nd - 1; d > -1; d--) {
        out_indices[d] = tmp % dimensions[d];
        tmp /= dimensions[d];
    }
}