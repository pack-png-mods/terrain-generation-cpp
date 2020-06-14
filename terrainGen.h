#ifndef TERRAINGENCPP_TERRAINGEN_H
#define TERRAINGENCPP_TERRAINGEN_H
#include "perlinCommon.h"
struct TerrainNoises {
    PermutationTable minLimit[16];
    PermutationTable maxLimit[16];
    PermutationTable mainLimit[8];
    PermutationTable shoresBottomComposition[4];
    PermutationTable surfaceElevation[4];
    PermutationTable scale[10];
    PermutationTable depth[16];
    PermutationTable forest[8];
};
enum blocks{
    AIR,
    STONE,
    GRASS,
    DIRT,
    BEDROCK,
    MOVING_WATER,
    SAND,
    GRAVEL,
    ICE,
};
#endif //TERRAINGENCPP_TERRAINGEN_H
