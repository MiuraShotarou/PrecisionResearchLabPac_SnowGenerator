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