// macro.c

void
assign_flags_c_init(int *out_flags)
{
    *out_flags = NDARRAY_FLAG_C_CONTIGUOUS | NDARRAY_FLAG_OWNDATA;
}

void assign_flags_c_contiguous_on(int *out_flags)
{
    *out_flags |= NDARRAY_FLAG_C_CONTIGUOUS;
}
void assign_flags_c_contiguous_off(int *out_flags)
{
    *out_flags &= ~NDARRAY_FLAG_C_CONTIGUOUS;
}