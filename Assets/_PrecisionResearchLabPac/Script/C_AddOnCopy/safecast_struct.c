// safecasttype_struct.c

SafeCastType* get_safecasttype(SDType sdtype)
{
    switch (sdtype) {
        case Bool:
        case SByte:
        case Short:
        case Int:
        case Long:   return &(SafeCastType){Long};
        case Float:
        case Double: return &(SafeCastType){Double};
        case Byte:
        case UShort:
        case UInt:
        case ULong:  return &(SafeCastType){ULong};
        default:     return NULL;
    }
}