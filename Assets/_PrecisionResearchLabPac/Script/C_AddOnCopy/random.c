// random.c

typedef struct {
    uint64_t state;  // 初期値はseedで、indexがインクリメントされていく度に更新されていく
    uint64_t increment;    // インクリメント値（奇数である必要がある）。stateが同じでも、ここが違えば異なった乱数列になる。stepのようなもの。固定値である。
} PCG64Param;
typedef struct {
    PCG64Param param;
    uint64_t seed;
} Random;

//  ここから関数
#define PCG64_MULTIPLIER  6364136223846793005ULL
#define PCG64_INCREMENT   1442695040888963407ULL
#define PCG64_SHIFT_A     18u
#define PCG64_SHIFT_B     27u
#define PCG64_SHIFT_C     59u
#define PCG64_ROTATION    31u
// PCG64の乱数生成
static uint32_t
get_random(PCG64Param *state)
{
    uint64_t oldstate = state->state;
    state->state = oldstate * PCG64_MULTIPLIER + state->increment;
    uint32_t xorshifted = ((oldstate >> PCG64_SHIFT_A) ^ oldstate) >> PCG64_SHIFT_B;
    uint32_t rot = oldstate >> PCG64_SHIFT_C;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & PCG64_ROTATION));
}

// Randomの生成
Random*
random_create(uint64_t seed)
{
    Random *rng = (Random *)malloc(sizeof(Random));
    if (rng == NULL) {
        SET_ERROR_MESSAGE("ndrandom_create: malloc failed.");
        return NULL;
    }
    rng->seed = seed;
    rng->param.state = seed;
    rng->param.increment = PCG64_INCREMENT | 1; // 奇数を保証
    get_random(&rng->param); // 初期状態を1回進める
    return rng;
}

// NdRandomの解放
void
random_free(Random *rng)
{
    if (rng == NULL) return;
    free(rng);
}