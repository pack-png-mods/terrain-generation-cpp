//
// Created by r on 14/06/2020.
//

#ifndef TERRAINGENCPP_PERLINCOMMON_H
#define TERRAINGENCPP_PERLINCOMMON_H
#include "javaRnd.h"
struct PermutationTable {
    double xo;
    double yo;
    double zo; // this actually never used in fixed noise aka 2d noise;)
    uint8_t permutations[512];
};

static inline void initOctaves(PermutationTable octaves[], Random *random, int nbOctaves) {
    for (int i = 0; i < nbOctaves; ++i) {
        octaves[i].xo = next_double(random) * 256.0;
        octaves[i].yo = next_double(random) * 256.0;
        octaves[i].zo = next_double(random) * 256.0;
        uint8_t *permutations = octaves[i].permutations;
        for (int j = 0; j < 256; ++j) {
            permutations[j] = j;
        }
        for (int index = 0; index < 256; ++index) {
            uint32_t randomIndex = random_next_int(random, 256 - index) + index;
            if (randomIndex != index) {
                // swap
                permutations[index] ^= permutations[randomIndex];
                permutations[randomIndex] ^= permutations[index];
                permutations[index] ^= permutations[randomIndex];
            }
            permutations[index + 256] = permutations[index];
        }
    }
}

#endif //TERRAINGENCPP_PERLINCOMMON_H
