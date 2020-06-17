#include <iostream>
#include <chrono>
#include <cstring>

static_assert(std::numeric_limits<double>::is_iec559, "This code requires IEEE-754 doubles");

#define Random uint64_t
#define RANDOM_MULTIPLIER 0x5DEECE66DULL
#define RANDOM_ADDEND 0xBULL
#define RANDOM_MASK ((1ULL << 48u) - 1)
#define RANDOM_SCALE 0x1.0p-53
#define get_random(seed) ((Random)((seed ^ RANDOM_MULTIPLIER) & RANDOM_MASK))


static inline int32_t random_next(Random *random, int bits) {
    *random = (*random * RANDOM_MULTIPLIER + RANDOM_ADDEND) & RANDOM_MASK;
    return (int32_t) (*random >> (48u - bits));
}

static inline int32_t random_next_int(Random *random, const uint16_t bound) {
    int32_t r = random_next(random, 31);
    const uint16_t m = bound - 1u;
    if ((bound & m) == 0) {
        r = (int32_t) ((bound * (uint64_t) r) >> 31u);
    } else {
        for (int32_t u = r;
             u - (r = u % bound) + m < 0;
             u = random_next(random, 31));
    }
    return r;
}

static inline double next_double(Random *random) {
    return (double) ((((uint64_t) ((uint32_t) random_next(random, 26)) << 27u)) + random_next(random, 27)) * RANDOM_SCALE;
}

static inline uint64_t random_next_long(Random *random) {
    return (((uint64_t) random_next(random, 32)) << 32u) + (int32_t) random_next(random, 32);
}

static inline void advance2(Random *random) {
    *random = (*random * 0xBB20B4600A69LLU + 0x40942DE6BALLU) & RANDOM_MASK;
}

/* Display stuff */
enum Biomes {
    Rainforest,
    Swampland,
    Seasonal_forest,
    Forest,
    Savanna,
    Shrubland,
    Taiga,
    Desert,
    Plains,
    IceDesert,
    Tundra,
};

const char *biomesNames[] = {"Rainforest", "Swampland", "Seasonal Forest", "Forest", "Savanna", "Shrubland", "Taiga", "Desert", "Plains", "IceDesert", "Tundra"};
// @formatter:off
const Biomes biomesTable[4096]={Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Desert, Desert, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Desert, Desert, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Savanna, Savanna, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Plains, Plains, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Shrubland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Seasonal_forest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Seasonal_forest, Rainforest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Rainforest, Rainforest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Rainforest, Rainforest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Rainforest, Rainforest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Rainforest, Rainforest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Forest, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Rainforest, Rainforest, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Tundra, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Taiga, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Swampland, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Forest, Rainforest, Rainforest,};
// @formatter:on

struct BiomeResult {
    double *temperature;
    double *humidity;
    Biomes *biomes;
};
/* End of Display stuff */

/* constant for simplex noise*/
#define F2 0.3660254037844386
#define G2 0.21132486540518713

int grad2[12][2] = {{1,  1,},
                    {-1, 1,},
                    {1,  -1,},
                    {-1, -1,},
                    {1,  0,},
                    {-1, 0,},
                    {1,  0,},
                    {-1, 0,},
                    {0,  1,},
                    {0,  -1,},
                    {0,  1,},
                    {0,  -1,}};

/* End of constant for simplex noise*/



/* simplex noise result is in buffer */
static inline double getSimplexNoise(double chunkX, double chunkZ, double offsetX, double offsetZ, double ampFactor, uint8_t nbOctaves, Random *random) {
    offsetX /= 1.5;
    offsetZ /= 1.5;
    double res = 0.0;
    double octaveDiminution = 1.0;
    double octaveAmplification = 1.0;
    double xo;
    double yo;
    uint8_t permutations[256];
    for (uint8_t j = 0; j < nbOctaves; ++j) {
        xo = next_double(random) * 256.0;
        yo = next_double(random) * 256.0;
        advance2(random);
        uint8_t w = 0;
        do {
            permutations[w] = w;
        } while (w++ != 255);
        uint8_t index = 0;
        do {
            uint32_t randomIndex = random_next_int(random, 256u - index) + index;
            if (randomIndex != index) {
                // swap
                permutations[index] ^= permutations[randomIndex];
                permutations[randomIndex] ^= permutations[index];
                permutations[index] ^= permutations[randomIndex];
            }
        } while (index++ != 255);
        double XCoords = (double) chunkX * offsetX * octaveAmplification + xo;
        double ZCoords = (double) chunkZ * offsetZ * octaveAmplification + yo;
        // Skew the input space to determine which simplex cell we're in
        double hairyFactor = (XCoords + ZCoords) * F2;
        auto tempX = static_cast<int32_t>(XCoords + hairyFactor);
        auto tempZ = static_cast<int32_t>(ZCoords + hairyFactor);
        int32_t xHairy = (XCoords + hairyFactor < tempX) ? (tempX - 1) : (tempX);
        int32_t zHairy = (ZCoords + hairyFactor < tempZ) ? (tempZ - 1) : (tempZ);
        double d11 = (double) (xHairy + zHairy) * G2;
        double X0 = (double) xHairy - d11; // Unskew the cell origin back to (x,y) space
        double Y0 = (double) zHairy - d11;
        double x0 = XCoords - X0; // The x,y distances from the cell origin
        double y0 = ZCoords - Y0;
        // For the 2D case, the simplex shape is an equilateral triangle.
        // Determine which simplex we are in.
        int offsetSecondCornerX, offsetSecondCornerZ; // Offsets for second (middle) corner of simplex in (i,j) coords

        if (x0 > y0) {  // lower triangle, XY order: (0,0)->(1,0)->(1,1)
            offsetSecondCornerX = 1;
            offsetSecondCornerZ = 0;
        } else { // upper triangle, YX order: (0,0)->(0,1)->(1,1)
            offsetSecondCornerX = 0;
            offsetSecondCornerZ = 1;
        }

        double x1 = (x0 - (double) offsetSecondCornerX) + G2; // Offsets for middle corner in (x,y) unskewed coords
        double y1 = (y0 - (double) offsetSecondCornerZ) + G2;
        double x2 = (x0 - 1.0) + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
        double y2 = (y0 - 1.0) + 2.0 * G2;

        // Work out the hashed gradient indices of the three simplex corners
        uint8_t ii = (uint32_t) xHairy & 0xffu;
        uint8_t jj = (uint32_t) zHairy & 0xffu;
        uint8_t gi0 = permutations[(uint16_t) (ii + permutations[jj]) & 0xffu] % 12u;
        uint8_t gi1 = permutations[(uint16_t)(ii + offsetSecondCornerX + permutations[(uint16_t) (jj + offsetSecondCornerZ) & 0xffu])& 0xffu] % 12u;
        uint8_t gi2 = permutations[(uint16_t)(ii + 1 + permutations[(uint16_t)(jj + 1)& 0xffu])& 0xffu] % 12u;

        // Calculate the contribution from the three corners
        double t0 = 0.5 - x0 * x0 - y0 * y0;
        double n0;
        if (t0 < 0.0) {
            n0 = 0.0;
        } else {
            t0 *= t0;
            n0 = t0 * t0 * ((double) grad2[gi0][0] * x0 + (double) grad2[gi0][1] * y0);  // (x,y) of grad2 used for 2D gradient
        }
        double t1 = 0.5 - x1 * x1 - y1 * y1;
        double n1;
        if (t1 < 0.0) {
            n1 = 0.0;
        } else {
            t1 *= t1;
            n1 = t1 * t1 * ((double) grad2[gi1][0] * x1 + (double) grad2[gi1][1] * y1);
        }
        double t2 = 0.5 - x2 * x2 - y2 * y2;
        double n2;
        if (t2 < 0.0) {
            n2 = 0.0;
        } else {
            t2 *= t2;
            n2 = t2 * t2 * ((double) grad2[gi2][0] * x2 + (double) grad2[gi2][1] * y2);
        }
        // Add contributions from each corner to get the final noise value.
        // The result is scaled to return values in the interval [-1,1].
        res = res + 70.0 * (n0 + n1 + n2) * 0.55000000000000004 / octaveDiminution;
        octaveAmplification *= ampFactor;
        octaveDiminution *= 0.5;
    }
    return res;

}

/* End of simplex noise result is in buffer */

static inline Biomes getBiomeForPos(uint64_t worldSeed, int posX, int posZ) {
    Random worldRandom;
    worldRandom = get_random(worldSeed * 9871L);
    double temperature = getSimplexNoise(posX, posZ, 0.02500000037252903, 0.02500000037252903, 0.25, 4, &worldRandom);
    worldRandom = get_random(worldSeed * 39811L);
    double humidity = getSimplexNoise(posX, posZ, 0.05000000074505806, 0.05000000074505806, 0.33333333333333331, 4, &worldRandom);
    worldRandom = get_random(worldSeed * 0x84a59L);
    double precipitation = getSimplexNoise(posX, posZ, 0.25, 0.25, 0.58823529411764708, 2, &worldRandom);

    double preci = precipitation * 1.1000000000000001 + 0.5;
    double temp = (temperature * 0.14999999999999999 + 0.69999999999999996) * (1.0 - 0.01) + preci * 0.01;
    temp = 1.0 - (1.0 - temp) * (1.0 - temp);
    if (temp < 0.0) {
        temp = 0.0;
    }
    if (temp > 1.0) {
        temp = 1.0;
    }
    double humi = (humidity * 0.14999999999999999 + 0.5) * (1.0 - 0.002) + preci * 0.002;
    if (humi < 0.0) {
        humi = 0.0;
    }
    if (humi > 1.0) {
        humi = 1.0;
    }
    return biomesTable[(int) (temp * 63) + (int) (humi * 63) * 64]; // ,temperature,humidity
}

static inline void printBiome(uint64_t worldSeed, int32_t posX, int32_t posZ) {
    Biomes biome = getBiomeForPos(worldSeed, posX, posZ);
    std::cout << biomesNames[biome] << " ";
}

static inline void printBiomes(uint64_t worldSeed, int32_t posX, int32_t posZ, int sizeX, int sizeZ) {
    for (int x = 0; x < sizeX; ++x) {
        for (int z = 0; z < sizeZ; ++z) {
            printBiome(worldSeed, posX + x, posZ + z);
        }
    }
    std::cout << std::endl;
}

int main() {
    Random random = get_random(123456u);
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
        long seed = random_next_long(&random);
        //std::cout<<seed<<std::endl;
        printBiomes(seed, 15*16, 16*16,16,16);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1e9 << " s\n";
}
