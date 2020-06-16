#ifndef TERRAINGENCPP_JAVARND_H
#define TERRAINGENCPP_JAVARND_H
#define Random uint64_t
#define RANDOM_MULTIPLIER 0x5DEECE66DULL
#define RANDOM_ADDEND 0xBULL
#define RANDOM_MASK ((1ULL << 48u) - 1)
#define RANDOM_SCALE 0x1.0p-53
#define get_random(seed) ((Random)((seed ^ RANDOM_MULTIPLIER) & RANDOM_MASK))


static inline uint32_t random_next(Random *random, int bits) {
    *random = (*random * RANDOM_MULTIPLIER + RANDOM_ADDEND) & RANDOM_MASK;
    return (uint32_t) (*random >> (48u - bits));
}

static inline uint32_t random_next_int(Random *random, const uint16_t bound) {
    uint32_t r = random_next(random, 31);
    const uint16_t m = bound - 1u;
    if ((bound & m) == 0) {
        // Could probably use __mul64hi here
        r = (uint32_t) ((bound * (uint64_t) r) >> 31u);
    } else {
        for (int u = r;
             u - (r = u % bound) + m < 0;
             u = random_next(random, 31));
    }
    return r;
}

static inline double next_double(Random *random) {
    return (double) ((((uint64_t) ((uint32_t) random_next(random, 26)) << 27u)) + random_next(random, 27)) * RANDOM_SCALE;
}
inline uint64_t random_next_long (Random *random) {
    return (((uint64_t)random_next(random, 32)) << 32u) + (int32_t)random_next(random, 32);
}
#endif //TERRAINGENCPP_JAVARND_H
