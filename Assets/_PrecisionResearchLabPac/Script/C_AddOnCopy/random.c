// random.c

/* 新しい試み
typedef struct {
    uint64_t state;  // 現在の内部状態
    uint64_t inc;    // インクリメント値（奇数である必要がある）
} PCG64State;

typedef struct {
    PCG64State pcg_state;
    uint64_t seed;
} NdRandom;
*/

typedef struct {
    uint64_t state;  // 初期値はseedで、indexがインクリメントされていく度に更新されていく
    uint64_t inc;    // インクリメント値（奇数である必要がある）。stateが同じでも、ここが違えば異なった乱数列になる。stepのようなもの。固定値である。
} PCG64State;

typedef struct {
    PCG64State pcg_state;
    uint64_t seed;
} NdRandom;

//  ここから関数
#define PCG64_MULTIPLIER  6364136223846793005ULL
#define PCG64_INCREMENT   1442695040888963407ULL
#define PCG64_SHIFT_A     18u
#define PCG64_SHIFT_B     27u
#define PCG64_SHIFT_C     59u
#define PCG64_ROTATION    31u
// PCG64の乱数生成
static uint32_t
pcg64_random(PCG64State *state)
{
    uint64_t oldstate = state->state;
    state->state = oldstate * PCG64_MULTIPLIER + state->inc;
    uint32_t xorshifted = ((oldstate >> PCG64_SHIFT_A) ^ oldstate) >> PCG64_SHIFT_B;
    uint32_t rot = oldstate >> PCG64_SHIFT_C;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & PCG64_ROTATION));
}

// NdRandomの生成
NdRandom*
ndrandom_create(uint64_t seed)
{
    NdRandom *rng = (NdRandom *)malloc(sizeof(NdRandom));
    if (rng == NULL) {
        SET_ERROR_MESSAGE("ndrandom_create: malloc failed.");
        return NULL;
    }
    rng->seed = seed;
    rng->pcg_state.state = seed;
    rng->pcg_state.inc = PCG64_INCREMENT | 1; // 奇数を保証
    pcg64_random(&rng->pcg_state); // 初期状態を1回進める
    return rng;
}

// NdRandomの解放
void
ndrandom_free(NdRandom *rng)
{
    if (rng == NULL) return;
    free(rng);
}

// random.Range
int32_t
np_random_range(NdRandom *rng, int32_t min, int32_t max)
{
    if (rng == NULL) {
        SET_ERROR_MESSAGE("np_random_range: rng is NULL.");
        return -1;
    }
    if (min >= max) {
        SET_ERROR_MESSAGE("np_random_range: min must be less than max.");
        return -1;
    }
    uint32_t value = pcg64_random(&rng->pcg_state);
    return min + (int32_t)(value % (uint32_t)(max - min));
}