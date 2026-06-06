// random_l.c
typedef struct {
    uint64_t s[4]; // 256ビットの内部状態
} Xoshiro256State;

static inline uint64_t
xoshiro256_rotl(uint64_t x, int k)
{
    return (x << k) | (x >> (64 - k));
}
// xoshiro256の乱数生成(最大値約1844京)
static uint64_t
get_random_l(Xoshiro256State *state)
{
    uint64_t result = xoshiro256_rotl(state->s[1] * 5, 7) * 9;
    uint64_t t = state->s[1] << 17;

    state->s[2] ^= state->s[0];
    state->s[3] ^= state->s[1];
    state->s[1] ^= state->s[2];
    state->s[0] ^= state->s[3];
    state->s[2] ^= t;
    state->s[3] = xoshiro256_rotl(state->s[3], 45);

    return result;
}

// シード値から内部状態を初期化
static void
xoshiro256_init(Xoshiro256State *state, uint64_t seed)
{
    // SplitMix64でシードを展開して4つの内部状態を生成
    for (int i = 0; i < 4; i++) {
        seed += 0x9e3779b97f4a7c15ULL;
        uint64_t z = seed;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        state->s[i] = z ^ (z >> 31);
    }
}