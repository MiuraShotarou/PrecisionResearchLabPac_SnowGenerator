// cast.c

// sdtype修正を行った
// ブロードキャスト後のNdArrayが引数に渡された時、連続したメモリのコピー処理を分岐するように実装する
// np_ndarray_cast を method.cに移行した

#include "cast.h"

ULongScalarCast ulongscalar_cast_by_sdtype[] = {
    uint64_to_bool,
    uint64_to_sbyte,
    uint64_to_byte,
    uint64_to_short,
    uint64_to_ushort,
    uint64_to_int,
    uint64_to_uint,
    uint64_to_long,
    uint64_to_ulong,
    uint64_to_float,
    uint64_to_double,
};
void uint64_to_bool   (char *dest, uint64_t value) { bool     v = (bool)    value; memcpy(dest, &v, sizeof(bool));     }
void uint64_to_sbyte  (char *dest, uint64_t value) { int8_t   v = (int8_t)  value; memcpy(dest, &v, sizeof(int8_t));   }
void uint64_to_byte   (char *dest, uint64_t value) { uint8_t  v = (uint8_t) value; memcpy(dest, &v, sizeof(uint8_t));  }
void uint64_to_short  (char *dest, uint64_t value) { int16_t  v = (int16_t) value; memcpy(dest, &v, sizeof(int16_t));  }
void uint64_to_ushort (char *dest, uint64_t value) { uint16_t v = (uint16_t)value; memcpy(dest, &v, sizeof(uint16_t)); }
void uint64_to_int    (char *dest, uint64_t value) { int32_t  v = (int32_t) value; memcpy(dest, &v, sizeof(int32_t));  }
void uint64_to_uint   (char *dest, uint64_t value) { uint32_t v = (uint32_t)value; memcpy(dest, &v, sizeof(uint32_t)); }
void uint64_to_long   (char *dest, uint64_t value) { int64_t  v = (int64_t) value; memcpy(dest, &v, sizeof(int64_t));  }
void uint64_to_ulong  (char *dest, uint64_t value) { uint64_t v =           value; memcpy(dest, &v, sizeof(uint64_t)); }
void uint64_to_float  (char *dest, uint64_t value) { float    v = (float)   value; memcpy(dest, &v, sizeof(float));    }
void uint64_to_double (char *dest, uint64_t value) { double   v = (double)  value; memcpy(dest, &v, sizeof(double));   }

LongScalarCast longscalar_cast_by_sdtype[] = {
    int64_to_bool,
    int64_to_sbyte,
    int64_to_byte,
    int64_to_short,
    int64_to_ushort,
    int64_to_int,
    int64_to_uint,
    int64_to_long,
    int64_to_ulong,
    int64_to_float,
    int64_to_double,
};
void int64_to_bool   (char *dest, int64_t value) { bool     v = (bool)    value; memcpy(dest, &v, sizeof(bool));     }
void int64_to_sbyte  (char *dest, int64_t value) { int8_t   v = (int8_t)  value; memcpy(dest, &v, sizeof(int8_t));   }
void int64_to_byte   (char *dest, int64_t value) { uint8_t  v = (uint8_t) value; memcpy(dest, &v, sizeof(uint8_t));  }
void int64_to_short  (char *dest, int64_t value) { int16_t  v = (int16_t) value; memcpy(dest, &v, sizeof(int16_t));  }
void int64_to_ushort (char *dest, int64_t value) { uint16_t v = (uint16_t)value; memcpy(dest, &v, sizeof(uint16_t)); }
void int64_to_int    (char *dest, int64_t value) { int32_t  v = (int32_t) value; memcpy(dest, &v, sizeof(int32_t));  }
void int64_to_uint   (char *dest, int64_t value) { uint32_t v = (uint32_t)value; memcpy(dest, &v, sizeof(uint32_t)); }
void int64_to_long   (char *dest, int64_t value) { int64_t  v =           value; memcpy(dest, &v, sizeof(int64_t));  }
void int64_to_ulong  (char *dest, int64_t value) { uint64_t v = (uint64_t)value; memcpy(dest, &v, sizeof(uint64_t)); }
void int64_to_float  (char *dest, int64_t value) { float    v = (float)   value; memcpy(dest, &v, sizeof(float));    }
void int64_to_double (char *dest, int64_t value) { double   v = (double)  value; memcpy(dest, &v, sizeof(double));   }

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
void double_to_bool   (char *dest, double value) { bool     v = (bool)    value; memcpy(dest, &v, sizeof(bool));     }
void double_to_sbyte  (char *dest, double value) { int8_t   v = (int8_t)  value; memcpy(dest, &v, sizeof(int8_t));   }
void double_to_byte   (char *dest, double value) { uint8_t  v = (uint8_t) value; memcpy(dest, &v, sizeof(uint8_t));  }
void double_to_short  (char *dest, double value) { int16_t  v = (int16_t) value; memcpy(dest, &v, sizeof(int16_t));  }
void double_to_ushort (char *dest, double value) { uint16_t v = (uint16_t)value; memcpy(dest, &v, sizeof(uint16_t)); }
void double_to_int    (char *dest, double value) { int32_t  v = (int32_t) value; memcpy(dest, &v, sizeof(int32_t));  }
void double_to_uint   (char *dest, double value) { uint32_t v = (uint32_t)value; memcpy(dest, &v, sizeof(uint32_t)); }
void double_to_long   (char *dest, double value) { int64_t  v = (int64_t) value; memcpy(dest, &v, sizeof(int64_t));  }
void double_to_ulong  (char *dest, double value) { uint64_t v = (uint64_t)value; memcpy(dest, &v, sizeof(uint64_t)); }
void double_to_float  (char *dest, double value) { float    v = (float)   value; memcpy(dest, &v, sizeof(float));    }
void double_to_double (char *dest, double value) { double   v =           value; memcpy(dest, &v, sizeof(double));   }

uint64_t
address_to_ulong(char *address, SDType sdtype)
{
    switch (sdtype) {
    case Bool:   { bool     v; memcpy(&v, address, sizeof(bool));     return (uint64_t)v; }
    case SByte:  { int8_t   v; memcpy(&v, address, sizeof(int8_t));   return (uint64_t)v; }
    case Byte:   { uint8_t  v; memcpy(&v, address, sizeof(uint8_t));  return (uint64_t)v; }
    case Short:  { int16_t  v; memcpy(&v, address, sizeof(int16_t));  return (uint64_t)v; }
    case UShort: { uint16_t v; memcpy(&v, address, sizeof(uint16_t)); return (uint64_t)v; }
    case Int:    { int32_t  v; memcpy(&v, address, sizeof(int32_t));  return (uint64_t)v; }
    case UInt:   { uint32_t v; memcpy(&v, address, sizeof(uint32_t)); return (uint64_t)v; }
    case Long:   { int64_t  v; memcpy(&v, address, sizeof(int64_t));  return (uint64_t)v; }
    case ULong:  { uint64_t v; memcpy(&v, address, sizeof(uint64_t)); return v;           }
    case Float:  { float    v; memcpy(&v, address, sizeof(float));    return (uint64_t)v; }
    case Double: { double   v; memcpy(&v, address, sizeof(double));   return (uint64_t)v; }
    default:     return 0;
    }
}
int64_t
address_to_long(char *address, SDType sdtype)
{
    switch (sdtype) {
    case Bool:   { bool     v; memcpy(&v, address, sizeof(bool));     return (int64_t)v; }
    case SByte:  { int8_t   v; memcpy(&v, address, sizeof(int8_t));   return (int64_t)v; }
    case Byte:   { uint8_t  v; memcpy(&v, address, sizeof(uint8_t));  return (int64_t)v; }
    case Short:  { int16_t  v; memcpy(&v, address, sizeof(int16_t));  return (int64_t)v; }
    case UShort: { uint16_t v; memcpy(&v, address, sizeof(uint16_t)); return (int64_t)v; }
    case Int:    { int32_t  v; memcpy(&v, address, sizeof(int32_t));  return (int64_t)v; }
    case UInt:   { uint32_t v; memcpy(&v, address, sizeof(uint32_t)); return (int64_t)v; }
    case Long:   { int64_t  v; memcpy(&v, address, sizeof(int64_t));  return v;          }
    case ULong:  { uint64_t v; memcpy(&v, address, sizeof(uint64_t)); return (int64_t)v; }
    case Float:  { float    v; memcpy(&v, address, sizeof(float));    return (int64_t)v; }
    case Double: { double   v; memcpy(&v, address, sizeof(double));   return (int64_t)v; }
    default:     return 0;
    }
}
double
address_to_double(char *address, SDType sdtype)
{
    switch (sdtype) {
        case Bool:   { bool     v; memcpy(&v, address, sizeof(bool));     return (double)v; }
        case SByte:  { int8_t   v; memcpy(&v, address, sizeof(int8_t));   return (double)v; }
        case Byte:   { uint8_t  v; memcpy(&v, address, sizeof(uint8_t));  return (double)v; }
        case Short:  { int16_t  v; memcpy(&v, address, sizeof(int16_t));  return (double)v; }
        case UShort: { uint16_t v; memcpy(&v, address, sizeof(uint16_t)); return (double)v; }
        case Int:    { int32_t  v; memcpy(&v, address, sizeof(int32_t));  return (double)v; }
        case UInt:   { uint32_t v; memcpy(&v, address, sizeof(uint32_t)); return (double)v; }
        case Long:   { int64_t  v; memcpy(&v, address, sizeof(int64_t));  return (double)v; }
        case ULong:  { uint64_t v; memcpy(&v, address, sizeof(uint64_t)); return (double)v; }
        case Float:  { float    v; memcpy(&v, address, sizeof(float));    return (double)v; }
        case Double: { double   v; memcpy(&v, address, sizeof(double));   return v;         }
        default:     return 0.0;
    }
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
        default:     return -1;
    }
}