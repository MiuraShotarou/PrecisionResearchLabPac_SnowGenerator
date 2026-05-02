/* array_assign.arrays_overlap */  //◎
NPY_NO_EXPORT int
arrays_overlap(PyArrayObject *arr1, PyArrayObject *arr2)
{
    mem_overlap_t result;

    result = solve_may_share_memory(arr1, arr2, NPY_MAY_SHARE_BOUNDS);
    if (result == MEM_OVERLAP_NO) {
        return 0;
    }
    else {
        return 1;
    }
}

/* mem_overlap.diophantine_precompute */
static int
diophantine_precompute(unsigned int n,
                       diophantine_term_t *E,
                       diophantine_term_t *Ep,
                       npy_int64 *Gamma, npy_int64 *Epsilon)
{
    npy_int64 a_gcd, gamma, epsilon, c1, c2;
    unsigned int j;
    char overflow = 0;

    assert(n >= 2);

    euclid(E[0].a, E[1].a, &a_gcd, &gamma, &epsilon);
    Ep[0].a = a_gcd;
    Gamma[0] = gamma;
    Epsilon[0] = epsilon;

    if (n > 2) {
        c1 = E[0].a / a_gcd;
        c2 = E[1].a / a_gcd;

        /* Ep[0].ub = E[0].ub * c1 + E[1].ub * c2; */
        Ep[0].ub = safe_add(safe_mul(E[0].ub, c1, &overflow),
                            safe_mul(E[1].ub, c2, &overflow), &overflow);
        if (overflow) {
            return 1;
        }
    }

    for (j = 2; j < n; ++j) {
        euclid(Ep[j-2].a, E[j].a, &a_gcd, &gamma, &epsilon);
        Ep[j-1].a = a_gcd;
        Gamma[j-1] = gamma;
        Epsilon[j-1] = epsilon;

        if (j < n - 1) {
            c1 = Ep[j-2].a / a_gcd;
            c2 = E[j].a / a_gcd;

            /* Ep[j-1].ub = c1 * Ep[j-2].ub + c2 * E[j].ub; */
            Ep[j-1].ub = safe_add(safe_mul(c1, Ep[j-2].ub, &overflow),
                                  safe_mul(c2, E[j].ub, &overflow), &overflow);

            if (overflow) {
                return 1;
            }
        }
    }

    return 0;
}

/* mem_overlap.diophantine_dfs */
static mem_overlap_t
diophantine_dfs(unsigned int n,
                unsigned int v,
                diophantine_term_t *E,
                diophantine_term_t *Ep,
                npy_int64 *Gamma, npy_int64 *Epsilon,
                npy_int64 b,
                Py_ssize_t max_work,
                int require_ub_nontrivial,
                npy_int64 *x,
                Py_ssize_t *count)
{
    npy_int64 a_gcd, gamma, epsilon, a1, u1, a2, u2, c, r, c1, c2, t, t_l, t_u, b2, x1, x2;
    npy_extint128_t x10, x20, t_l1, t_l2, t_u1, t_u2;
    mem_overlap_t res;
    char overflow = 0;

    if (max_work >= 0 && *count >= max_work) {
        return MEM_OVERLAP_TOO_HARD;
    }

    /* Fetch precomputed values for the reduced problem */
    if (v == 1) {
        a1 = E[0].a;
        u1 = E[0].ub;
    }
    else {
        a1 = Ep[v-2].a;
        u1 = Ep[v-2].ub;
    }

    a2 = E[v].a;
    u2 = E[v].ub;

    a_gcd = Ep[v-1].a;
    gamma = Gamma[v-1];
    epsilon = Epsilon[v-1];

    /* Generate set of allowed solutions */
    c = b / a_gcd;
    r = b % a_gcd;
    if (r != 0) {
        ++*count;
        return MEM_OVERLAP_NO;
    }

    c1 = a2 / a_gcd;
    c2 = a1 / a_gcd;

    /*
      The set to enumerate is:
      x1 = gamma*c + c1*t
      x2 = epsilon*c - c2*t
      t integer
      0 <= x1 <= u1
      0 <= x2 <= u2
      and we have c, c1, c2 >= 0
     */

    x10 = mul_64_64(gamma, c); //関数① overflow対策
    x20 = mul_64_64(epsilon, c);

	t_l1 = ceildiv_128_64(neg_128(x10), c1); //関数② overflow対策
    t_l2 = ceildiv_128_64(sub_128(x20, to_128(u2), &overflow), c2);

    t_u1 = floordiv_128_64(sub_128(to_128(u1), x10, &overflow), c1); //関数③ overflow対策
    t_u2 = floordiv_128_64(x20, c2);

    if (overflow) {
        return MEM_OVERLAP_OVERFLOW;
    }

    if (gt_128(t_l2, t_l1)) {
        t_l1 = t_l2;
    }

    if (gt_128(t_u1, t_u2)) {
        t_u1 = t_u2;
    }

    if (gt_128(t_l1, t_u1)) {
        ++*count;
        return MEM_OVERLAP_NO;
    }

    t_l = to_64(t_l1, &overflow); //関数④ overflow対策
    t_u = to_64(t_u1, &overflow);

    x10 = add_128(x10, mul_64_64(c1, t_l), &overflow); //関数⑤ overflow対策
    x20 = sub_128(x20, mul_64_64(c2, t_l), &overflow);

    t_u = safe_sub(t_u, t_l, &overflow); //関数⑥ overflow対策
    t_l = 0;
    x1 = to_64(x10, &overflow);
    x2 = to_64(x20, &overflow);

    if (overflow) {
        return MEM_OVERLAP_OVERFLOW;
    }

    /* The bounds t_l, t_u ensure the x computed below do not overflow */

    if (v == 1) {
        /* Base case */
        if (t_u >= t_l) {
            x[0] = x1 + c1*t_l;
            x[1] = x2 - c2*t_l;
            if (require_ub_nontrivial) {
                unsigned int j;
                int is_ub_trivial;

                is_ub_trivial = 1;
                for (j = 0; j < n; ++j) {
                    if (x[j] != E[j].ub/2) {
                        is_ub_trivial = 0;
                        break;
                    }
                }

                if (is_ub_trivial) {
                    /* Ignore 'trivial' solution */
                    ++*count;
                    return MEM_OVERLAP_NO;
                }
            }
            return MEM_OVERLAP_YES;
        }
        ++*count;
        return MEM_OVERLAP_NO;
    }
    else {
        /* Recurse to all candidates */
        for (t = t_l; t <= t_u; ++t) {
            x[v] = x2 - c2*t;

            /* b2 = b - a2*x[v]; */
            b2 = safe_sub(b, safe_mul(a2, x[v], &overflow), &overflow);
            if (overflow) {
                return MEM_OVERLAP_OVERFLOW;
            }

            res = diophantine_dfs(n, v-1, E, Ep, Gamma, Epsilon,
                                  b2, max_work, require_ub_nontrivial,
                                  x, count);
            if (res != MEM_OVERLAP_NO) {
                return res;
            }
        }
        ++*count;
        return MEM_OVERLAP_NO;
    }
}

/* mem_overlap.solve_may_share_memory */
NPY_VISIBILITY_HIDDEN mem_overlap_t
solve_may_share_memory(PyArrayObject *a, PyArrayObject *b,
                       Py_ssize_t max_work)
{
    npy_int64 rhs;
    diophantine_term_t terms[2*NPY_MAXDIMS + 2];
    npy_uintp start1 = 0, end1 = 0, size1 = 0;
    npy_uintp start2 = 0, end2 = 0, size2 = 0;
    npy_uintp uintp_rhs;
    npy_int64 x[2*NPY_MAXDIMS + 2];
    unsigned int nterms;

    get_array_memory_extents(a, &start1, &end1, &size1); //関数①
    get_array_memory_extents(b, &start2, &end2, &size2);

    if (!(start1 < end2 && start2 < end1 && start1 < end1 && start2 < end2)) {
        /* Memory extents don't overlap */
        return MEM_OVERLAP_NO;
    }

    if (max_work == 0) {
        /* Too much work required, give up */
        return MEM_OVERLAP_TOO_HARD;
    }

    /* Convert problem to Diophantine equation form with positive coefficients.
       The bounds computed by offset_bounds_from_strides correspond to
       all-positive strides.

       start1 + sum(abs(stride1)*x1)
       == start2 + sum(abs(stride2)*x2)
       == end1 - 1 - sum(abs(stride1)*x1')
       == end2 - 1 - sum(abs(stride2)*x2')

       <=>

       sum(abs(stride1)*x1) + sum(abs(stride2)*x2')
       == end2 - 1 - start1

       OR

       sum(abs(stride1)*x1') + sum(abs(stride2)*x2)
       == end1 - 1 - start2

       We pick the problem with the smaller RHS (they are non-negative due to
       the extent check above.)
    */

    uintp_rhs = MIN(end2 - 1 - start1, end1 - 1 - start2);
    if (uintp_rhs > NPY_MAX_INT64) {
        /* Integer overflow */
        return MEM_OVERLAP_OVERFLOW;
    }
    rhs = (npy_int64)uintp_rhs;

    nterms = 0;
    if (strides_to_terms(a, terms, &nterms, 1)) { //関数② ◎
        return MEM_OVERLAP_OVERFLOW;
    }
    if (strides_to_terms(b, terms, &nterms, 1)) {
        return MEM_OVERLAP_OVERFLOW;
    }
    if (PyArray_ITEMSIZE(a) > 1) { //itemsizeの取得
        terms[nterms].a = 1;
        terms[nterms].ub = PyArray_ITEMSIZE(a) - 1;
        ++nterms;
    }
    if (PyArray_ITEMSIZE(b) > 1) {
        terms[nterms].a = 1;
        terms[nterms].ub = PyArray_ITEMSIZE(b) - 1;
        ++nterms;
    }

    /* Simplify, if possible */
    if (diophantine_simplify(&nterms, terms, rhs)) { //関数③ ◎
        /* Integer overflow */
        return MEM_OVERLAP_OVERFLOW;
    }

    /* Solve */
    return solve_diophantine(nterms, terms, rhs, max_work, 0, x); //関数④ ◎
}

/* mem_overlap.strides_to_terms */
static int
strides_to_terms(PyArrayObject *arr, diophantine_term_t *terms,
                 unsigned int *nterms, int skip_empty)
{
    int i;
    for (i = 0; i < PyArray_NDIM(arr); ++i) {
        if (skip_empty) {
            if (PyArray_DIM(arr, i) <= 1 || PyArray_STRIDE(arr, i) == 0) {
                continue;
            }
        }

        terms[*nterms].a = PyArray_STRIDE(arr, i);

        if (terms[*nterms].a < 0) {
            terms[*nterms].a = -terms[*nterms].a;
        }

        if (terms[*nterms].a < 0) {
            /* integer overflow */
            return 1;
        }

        terms[*nterms].ub = PyArray_DIM(arr, i) - 1;
        ++*nterms;
    }

    return 0;
}

/* mem_overlap.solve_diophantine */
NPY_VISIBILITY_HIDDEN mem_overlap_t
solve_diophantine(unsigned int n, diophantine_term_t *E, npy_int64 b,
                  Py_ssize_t max_work, int require_ub_nontrivial, npy_int64 *x)
{
    mem_overlap_t res;
    unsigned int j;

    for (j = 0; j < n; ++j) {
        if (E[j].a <= 0) {
            return MEM_OVERLAP_ERROR;
        }
        else if (E[j].ub < 0) {
            return MEM_OVERLAP_NO;
        }
    }

    if (require_ub_nontrivial) {
        npy_int64 ub_sum = 0;
        char overflow = 0;
        for (j = 0; j < n; ++j) {
            if (E[j].ub % 2 != 0) {
                return MEM_OVERLAP_ERROR;
            }
            ub_sum = safe_add(ub_sum,
                              safe_mul(E[j].a, E[j].ub/2, &overflow),
                              &overflow);
        }
        if (overflow) {
            return MEM_OVERLAP_ERROR;
        }
        b = ub_sum;
    }

    if (b < 0) {
        return MEM_OVERLAP_NO;
    }

    if (n == 0) {
        if (require_ub_nontrivial) {
            /* Only trivial solution for 0-variable problem */
            return MEM_OVERLAP_NO;
        }
        if (b == 0) {
            return MEM_OVERLAP_YES;
        }
        return MEM_OVERLAP_NO;
    }
    else if (n == 1) {
        if (require_ub_nontrivial) {
            /* Only trivial solution for 1-variable problem */
            return MEM_OVERLAP_NO;
        }
        if (b % E[0].a == 0) {
            x[0] = b / E[0].a;
            if (x[0] >= 0 && x[0] <= E[0].ub) {
                return MEM_OVERLAP_YES;
            }
        }
        return MEM_OVERLAP_NO;
    }
    else {
        Py_ssize_t count = 0;
        diophantine_term_t *Ep = NULL;
        npy_int64 *Epsilon = NULL, *Gamma = NULL;

        Ep = malloc(n * sizeof(diophantine_term_t));
        Epsilon = malloc(n * sizeof(npy_int64));
        Gamma = malloc(n * sizeof(npy_int64));
        if (Ep == NULL || Epsilon == NULL || Gamma == NULL) {
            res = MEM_OVERLAP_ERROR;
        }
        else if (diophantine_precompute(n, E, Ep, Gamma, Epsilon)) {
            res = MEM_OVERLAP_OVERFLOW;
        }
        else {
            res = diophantine_dfs(n, n-1, E, Ep, Gamma, Epsilon, b, max_work,
                                  require_ub_nontrivial, x, &count);
        }
        free(Ep);
        free(Gamma);
        free(Epsilon);
        return res;
    }
}

/* Gets a half-open range [start, end) of offsets from the data pointer */
/* mem_overlap.offset_bounds_from_strides */
NPY_VISIBILITY_HIDDEN void
offset_bounds_from_strides(const int itemsize, const int nd,
                           const npy_intp *dims, const npy_intp *strides,
                           npy_intp *lower_offset, npy_intp *upper_offset)
{
    npy_intp max_axis_offset;
    npy_intp lower = 0;
    npy_intp upper = 0;
    int i;

    for (i = 0; i < nd; i++) {
        if (dims[i] == 0) {
            /* If the array size is zero, return an empty range */
            *lower_offset = 0;
            *upper_offset = 0;
            return;
        }
        /* Expand either upwards or downwards depending on stride */
        max_axis_offset = strides[i] * (dims[i] - 1);
        if (max_axis_offset > 0) {
            upper += max_axis_offset;
        }
        else {
            lower += max_axis_offset;
        }
    }
    /* Return a half-open range */
    upper += itemsize;
    *lower_offset = lower;
    *upper_offset = upper;
}


/* mem_overlap.get_array_memory_extents */ //◎
static void
get_array_memory_extents(PyArrayObject *arr,
                         npy_uintp *out_start, npy_uintp *out_end,
                         npy_uintp *num_bytes)
{
    npy_intp low, upper;
    int j;
    offset_bounds_from_strides(PyArray_ITEMSIZE(arr), PyArray_NDIM(arr), //関数① ◎
                               PyArray_DIMS(arr), PyArray_STRIDES(arr),
                               &low, &upper);
    *out_start = (npy_uintp)PyArray_DATA(arr) + (npy_uintp)low;
    *out_end = (npy_uintp)PyArray_DATA(arr) + (npy_uintp)upper;

    *num_bytes = PyArray_ITEMSIZE(arr);
    for (j = 0; j < PyArray_NDIM(arr); ++j) {
        *num_bytes *= PyArray_DIM(arr, j);
    }
}

/* mem_overlap.solve_diophantine */ //◎
NPY_VISIBILITY_HIDDEN mem_overlap_t
solve_diophantine(unsigned int n, diophantine_term_t *E, npy_int64 b,
                  Py_ssize_t max_work, int require_ub_nontrivial, npy_int64 *x)
{
    mem_overlap_t res;
    unsigned int j;

    for (j = 0; j < n; ++j) {
        if (E[j].a <= 0) {
            return MEM_OVERLAP_ERROR;
        }
        else if (E[j].ub < 0) {
            return MEM_OVERLAP_NO;
        }
    }

    if (require_ub_nontrivial) {
        npy_int64 ub_sum = 0;
        char overflow = 0;
        for (j = 0; j < n; ++j) {
            if (E[j].ub % 2 != 0) {
                return MEM_OVERLAP_ERROR;
            }
            ub_sum = safe_add(ub_sum, //関数① オーバーフローのチェック機能が追加されている
                              safe_mul(E[j].a, E[j].ub/2, &overflow), //関数②
                              &overflow);
        }
        if (overflow) { //0ではないなら == overflaw
            return MEM_OVERLAP_ERROR;
        }
        b = ub_sum;
    }

    if (b < 0) {
        return MEM_OVERLAP_NO;
    }

    if (n == 0) {
        if (require_ub_nontrivial) {
            /* Only trivial solution for 0-variable problem */
            return MEM_OVERLAP_NO;
        }
        if (b == 0) {
            return MEM_OVERLAP_YES;
        }
        return MEM_OVERLAP_NO;
    }
    else if (n == 1) {
        if (require_ub_nontrivial) {
            /* Only trivial solution for 1-variable problem */
            return MEM_OVERLAP_NO;
        }
        if (b % E[0].a == 0) {
            x[0] = b / E[0].a;
            if (x[0] >= 0 && x[0] <= E[0].ub) {
                return MEM_OVERLAP_YES;
            }
        }
        return MEM_OVERLAP_NO;
    }
    else {
        Py_ssize_t count = 0;
        diophantine_term_t *Ep = NULL;
        npy_int64 *Epsilon = NULL, *Gamma = NULL;

        Ep = malloc(n * sizeof(diophantine_term_t));
        Epsilon = malloc(n * sizeof(npy_int64));
        Gamma = malloc(n * sizeof(npy_int64));
        if (Ep == NULL || Epsilon == NULL || Gamma == NULL) {
            res = MEM_OVERLAP_ERROR;
        }
        else if (diophantine_precompute(n, E, Ep, Gamma, Epsilon)) { //関数①
            res = MEM_OVERLAP_OVERFLOW;
        }
        else {
            res = diophantine_dfs(n, n-1, E, Ep, Gamma, Epsilon, b, max_work, //関数②
                                  require_ub_nontrivial, x, &count);
        }
        free(Ep);
        free(Gamma);
        free(Epsilon);
        return res;
    }
}