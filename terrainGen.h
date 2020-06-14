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

struct TerrainResult{
    BiomeResult*biomeResult;
    uint8_t *chunkCache;
    uint8_t *chunkHeights;
};

void delete_terrain_result(TerrainResult *terrainResult);
uint8_t *TerrainInternalWrapper(uint64_t worldSeed, int32_t chunkX, int32_t chunkZ, BiomeResult *biomeResult);
TerrainResult *TerrainWrapper(uint64_t worldSeed, int32_t chunkX, int32_t chunkZ);
#endif //TERRAINGENCPP_TERRAINGEN_H
