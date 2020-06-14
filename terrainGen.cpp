#include <iostream>
#include <chrono>
#include "biomeGen.h"
#include "terrainGen.h"

static_assert(std::numeric_limits<double>::is_iec559, "This code requires IEEE-754 doubles");

static inline double lerp(double x, double a, double b) {
    return a + x * (b - a);
}

static inline double grad(uint32_t hash, double x, double y, double z) {
    switch (hash & 0xFu) {
        case 0x0:
            return x + y;
        case 0x1:
            return -x + y;
        case 0x2:
            return x - y;
        case 0x3:
            return -x - y;
        case 0x4:
            return x + z;
        case 0x5:
            return -x + z;
        case 0x6:
            return x - z;
        case 0x7:
            return -x - z;
        case 0x8:
            return y + z;
        case 0x9:
            return -y + z;
        case 0xA:
            return y - z;
        case 0xB:
            return -y - z;
        case 0xC:
            return y + x;
        case 0xD:
            return -y + z;
        case 0xE:
            return y - x;
        case 0xF:
            return -y - z;
        default:
            return 0; // never happens
    }
}

//we care only about 60-61, 77-78, 145-146, 162-163, 230-231, 247-248, 315-316, 332-333, 400-401, 417-418
static inline void generatePermutations(double **buffer, double x, double y, double z, int sizeX, int sizeY, int sizeZ, double noiseFactorX, double noiseFactorY, double noiseFactorZ, double octaveSize, PermutationTable permutationTable) {
    uint8_t *permutations = permutationTable.permutations;
    double octaveWidth = 1.0 / octaveSize;
    int32_t i2 = -1;
    double x1 = 0.0;
    double x2 = 0.0;
    double xx1 = 0.0;
    double xx2 = 0.0;
    double t;
    double w;
    int columnIndex = 51; // possibleX[0]*5*17+3*17
    int possibleX[10] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4};
    int possibleZ[10] = {3, 4, 3, 4, 3, 4, 3, 4, 3, 4};
    for (int index = 0; index < 10; index++) {
        double xCoord = (x + (double) possibleX[index]) * noiseFactorX + permutationTable.xo;
        auto clampedXcoord = (int32_t) xCoord;
        if (xCoord < (double) clampedXcoord) {
            clampedXcoord--;
        }
        auto xBottoms = (int32_t) ((uint32_t) clampedXcoord & 0xffu);
        xCoord -= clampedXcoord;
        t = xCoord * 6 - 15;
        w = (xCoord * t + 10);
        double fadeX = xCoord * xCoord * xCoord * w;
        double zCoord = (z + (double) possibleZ[index]) * noiseFactorZ + permutationTable.zo;
        auto clampedZCoord = (int32_t) zCoord;
        if (zCoord < (double) clampedZCoord) {
            clampedZCoord--;
        }
        auto zBottoms = (int32_t) ((uint32_t) clampedZCoord & 0xffu);
        zCoord -= clampedZCoord;
        t = zCoord * 6 - 15;
        w = (zCoord * t + 10);
        double fadeZ = zCoord * zCoord * zCoord * w;
        for (int Y = 0; Y < 11; Y++) { // we cannot limit on lower bound without some issues later
            // ZCoord
            double yCoords = (y + (double) Y) * noiseFactorY + permutationTable.yo;
            auto clampedYCoords = (int32_t) yCoords;
            if (yCoords < (double) clampedYCoords) {
                clampedYCoords--;
            }
            auto yBottoms = (int32_t) ((uint32_t) clampedYCoords & 0xffu);
            yCoords -= clampedYCoords;
            t = yCoords * 6 - 15;
            w = yCoords * t + 10;
            double fadeY = yCoords * yCoords * yCoords * w;
            // ZCoord

            if (Y == 0 || yBottoms != i2) { // this is wrong on so many levels, same ybottoms doesnt mean x and z were the same...
                i2 = yBottoms;
                uint8_t k2 = permutations[permutations[xBottoms] + yBottoms] + zBottoms;
                uint8_t l2 = permutations[permutations[xBottoms] + yBottoms + 1] + zBottoms;
                uint8_t k3 = permutations[permutations[xBottoms + 1] + yBottoms] + zBottoms;
                uint8_t l3 = permutations[permutations[xBottoms + 1] + yBottoms + 1] + zBottoms;
                x1 = lerp(fadeX, grad(permutations[k2], xCoord, yCoords, zCoord), grad(permutations[k3], xCoord - 1.0, yCoords, zCoord));
                x2 = lerp(fadeX, grad(permutations[l2], xCoord, yCoords - 1.0, zCoord), grad(permutations[l3], xCoord - 1.0, yCoords - 1.0, zCoord));
                xx1 = lerp(fadeX, grad(permutations[k2 + 1], xCoord, yCoords, zCoord - 1.0), grad(permutations[k3 + 1], xCoord - 1.0, yCoords, zCoord - 1.0));
                xx2 = lerp(fadeX, grad(permutations[l2 + 1], xCoord, yCoords - 1.0, zCoord - 1.0), grad(permutations[l3 + 1], xCoord - 1.0, yCoords - 1.0, zCoord - 1.0));
            }
            double y1 = lerp(fadeY, x1, x2);
            double y2 = lerp(fadeY, xx1, xx2);
            (*buffer)[columnIndex] = (*buffer)[columnIndex] + lerp(fadeZ, y1, y2) * octaveWidth;
            columnIndex++;
        }
        if (index % 2 == 0) {
            columnIndex += 6; // 6 to complete Y
        } else {
            columnIndex += possibleZ[0] * 17 + 6; // 3*17 on Z +6 complete Y
        }
    }
}

static inline void generateNoise(double *buffer, double x, double y, double z, int sizeX, int sizeY, int sizeZ, double offsetX, double offsetY, double offsetZ, PermutationTable *permutationTable, int nbOctaves) {

    double octavesFactor = 1.0;
    // we care only about 315 332 400 417 316 333 401 and 418
    for (int octave = 0; octave < nbOctaves; octave++) {
        generatePermutations(&buffer, x, y, z, sizeX, sizeY, sizeZ, offsetX * octavesFactor, offsetY * octavesFactor, offsetZ * octavesFactor, octavesFactor, permutationTable[octave]);
        octavesFactor /= 2.0;
    }
}

static inline void fillNoiseColumn(double **NoiseColumn, int x, int z, const double *temperature, const double *humidity, TerrainNoises terrainNoises) {
    // we only need
    // (60, 77, 145, 162, 61, 78, 146, 163)
    // (145, 162, 230, 247, 146, 163, 231, 248)
    // (230, 247, 315, 332, 231, 248, 316, 333)
    // (315, 332, 400, 417, 316, 333, 401, 418)
    // which is only 60-61, 77-78, 145-146, 162-163, 230-231, 247-248, 315-316, 332-333, 400-401, 417-418 // so 20
    // or as cellCounter 3,4,8,9,13,14,18,19,23,24
    // or as x indices 1 2 3 4 5 and fixed z to 3-4
    // 5 is the cellsize here and 17 the column size, they are inlined constants
    double d = 684.41200000000003;
    double d1 = 684.41200000000003;
    // this is super fast (but we only care about 3,4,8,9,13,14,18,19,23,24)
    auto *surfaceNoise = new double[5 * 5];
    auto *depthNoise = new double[5 * 5];
    surfaceNoise = generateFixedNoise(surfaceNoise, x, z, 5, 5, 1.121, 1.121, terrainNoises.scale);
    depthNoise = generateFixedNoise(depthNoise, x, z, 5, 5, 200, 200, terrainNoises.depth);

    auto *mainLimitPerlinNoise = new double[5 * 17 * 5];
    auto *minLimitPerlinNoise = new double[5 * 17 * 5];
    auto *maxLimitPerlinNoise = new double[5 * 17 * 5];
    generateNoise(mainLimitPerlinNoise, x, 0, z, 5, 17, 5, d / 80, d1 / 160, d / 80, terrainNoises.mainLimit,8);
    generateNoise(minLimitPerlinNoise, x, 0, z, 5, 17, 5, d, d1, d, terrainNoises.minLimit,16);
    generateNoise(maxLimitPerlinNoise, x, 0, z, 5, 17, 5, d, d1, d, terrainNoises.maxLimit,16);
    int possibleCellCounter[10] = {3, 4, 8, 9, 13, 14, 18, 19, 23, 24};
    for (int cellCounter : possibleCellCounter) {
        int X = (cellCounter / 5) * 3 + 1; // 1 4 7 10 13
        int Z = (cellCounter % 5) * 3 + 1; // 7 13
        double aridityXZ = 1.0 - humidity[X * 16 + Z] * temperature[X * 16 + Z];
        aridityXZ *= aridityXZ;
        aridityXZ *= aridityXZ;
        aridityXZ = 1.0 - aridityXZ;  // 1-(1-X)*(1-X)*(1-X)*(1-X) with X=humidity*Temp
        double surface = (surfaceNoise[cellCounter] / 512.0 + 256.0 / 512.0) * aridityXZ;
        if (surface > 1.0) {
            surface = 1.0; // clamp
        }
        double depth = depthNoise[cellCounter] / 8000.0;
        if (depth < 0.0) {
            depth = -depth * 0.29999999999999999;
        }
        depth = depth * 3 - 2;
        if (depth < 0.0) {
            depth /= 2.0;
            if (depth < -1) {
                depth = -1;
            }
            depth /= 1.3999999999999999;
            depth /= 2.0;
            surface = 0.0;
        } else {
            if (depth > 1.0) {
                depth = 1.0;
            }
            depth /= 8.0;
        }
        if (surface < 0.0) {
            surface = 0.0;
        }
        surface += 0.5;
        depth = (depth * (double) 17) / 16.0;
        double depthColumn = (double) 17 / 2.0 + depth * 4.0;

        for (int column = 9; column < 11; column++) { // we only care at pos 9 and 10 in the column so 2 times
            int columnCounter = cellCounter * 17 + column;
            double limit;
            double columnPerSurface = (((double) column - depthColumn) * 12.0) / surface;
            if (columnPerSurface < 0.0) {
                columnPerSurface *= 4.0;
            }
            double minLimit = minLimitPerlinNoise[columnCounter] / 512.0;
            double maxLimit = maxLimitPerlinNoise[columnCounter] / 512.0;
            double mainLimit = (mainLimitPerlinNoise[columnCounter] / 10.0 + 1.0) / 2.0;
            if (mainLimit < 0.0) {
                limit = minLimit;
            } else if (mainLimit > 1.0) {
                limit = maxLimit;
            } else {
                limit = minLimit + (maxLimit - minLimit) * mainLimit; // interpolation
            }
            limit -= columnPerSurface;
            (*NoiseColumn)[columnCounter] = limit;
        }

    }
}

static inline uint8_t *generateTerrain(int chunkX, int chunkZ, uint8_t *chunkCache, double *temperatures, double *humidity, TerrainNoises terrainNoises) {
    uint8_t quadrant = 4;
    uint8_t columnSize = 17;
    uint8_t cellsize = 5;
    double interpFirstOctave = 0.125;
    double interpSecondOctave = 0.25;
    double interpThirdOctave = 0.25;
    // we only need 315 332 400 417 316 333 401 and 418
    auto *NoiseColumn = new double[5 * 5 * 17];
    fillNoiseColumn(&NoiseColumn, chunkX * quadrant, chunkZ * quadrant, temperatures, humidity, terrainNoises);
    for (uint8_t x = 0; x < quadrant; x++) {
        uint8_t z = 3;
        for (int height = 9; height < 10; height++) {
            int off_0_0 = x * cellsize + z;
            int off_0_1 = x * cellsize + (z + 1);
            int off_1_0 = (x + 1) * cellsize + z;
            int off_1_1 = (x + 1) * cellsize + (z + 1);
            double firstNoise_0_0 = NoiseColumn[(off_0_0) * columnSize + (height)];
            double firstNoise_0_1 = NoiseColumn[(off_0_1) * columnSize + (height)];
            double firstNoise_1_0 = NoiseColumn[off_1_0 * columnSize + (height)];
            double firstNoise_1_1 = NoiseColumn[off_1_1 * columnSize + (height)];
            double stepFirstNoise_0_0 = (NoiseColumn[(off_0_0) * columnSize + (height + 1)] - firstNoise_0_0) * interpFirstOctave;
            double stepFirstNoise_0_1 = (NoiseColumn[(off_0_1) * columnSize + (height + 1)] - firstNoise_0_1) * interpFirstOctave;
            double stepFirstNoise_1_0 = (NoiseColumn[off_1_0 * columnSize + (height + 1)] - firstNoise_1_0) * interpFirstOctave;
            double stepFirstNoise_1_1 = (NoiseColumn[off_1_1 * columnSize + (height + 1)] - firstNoise_1_1) * interpFirstOctave;

            //double firstNoise_0_0 = NoiseColumn[(x * cellsize + 3) * 17 + 9]; // should only take care of (x*5+3)*17+9
            //double firstNoise_0_1 = NoiseColumn[(x * cellsize + 4) * 17 + 9]; // should only take care of (x*5+4)*17+9
            //double firstNoise_1_0 = NoiseColumn[((x + 1) * cellsize + 3) * 17 + 9]; // should only take care of ((x+1)*5+3)*17+9
            //double firstNoise_1_1 = NoiseColumn[((x + 1) * cellsize + 4) * 17 + 9]; // should only take care of ((x+1)*5+)*17+9
            //double stepFirstNoise_0_0 = (NoiseColumn[(x * cellsize + 3) * 17 + 10] - firstNoise_0_0) * interpFirstOctave;
            //double stepFirstNoise_0_1 = (NoiseColumn[(x * cellsize + 4) * 17 + 10] - firstNoise_0_1) * interpFirstOctave;
            //double stepFirstNoise_1_0 = (NoiseColumn[((x + 1) * cellsize + 3) * 17 + 10] - firstNoise_1_0) * interpFirstOctave;
            //double stepFirstNoise_1_1 = (NoiseColumn[((x + 1) * cellsize + 4) * 17 + 10] - firstNoise_1_1) * interpFirstOctave;
            for (uint8_t heightOffset = 0; heightOffset < 8; heightOffset++) {
                double secondNoise_0_0 = firstNoise_0_0;
                double secondNoise_0_1 = firstNoise_0_1;
                double stepSecondNoise_1_0 = (firstNoise_1_0 - firstNoise_0_0) * interpSecondOctave;
                double stepSecondNoise_1_1 = (firstNoise_1_1 - firstNoise_0_1) * interpSecondOctave;
                for (uint8_t xOffset = 0; xOffset < 4; xOffset++) {
                    uint8_t currentHeight = height * 8 + heightOffset; // max is 128
                    uint16_t index = (xOffset + x * 4u) << 11u | (z * 4u) << 7u | currentHeight;
                    double stoneLimit = secondNoise_0_0; // aka thirdNoise
                    double stepThirdNoise_0_1 = (secondNoise_0_1 - secondNoise_0_0) * interpThirdOctave;
                    for (int zOffset = 0; zOffset < 4; zOffset++) {
                        int block = 0;
                        if (stoneLimit > 0.0) { //3d perlin condition
                            block = 1;
                        }
                        chunkCache[index] = block;
                        index += 128;
                        stoneLimit += stepThirdNoise_0_1;
                    }

                    secondNoise_0_0 += stepSecondNoise_1_0;
                    secondNoise_0_1 += stepSecondNoise_1_1;
                }

                firstNoise_0_0 += stepFirstNoise_0_0;
                firstNoise_0_1 += stepFirstNoise_0_1;
                firstNoise_1_0 += stepFirstNoise_1_0;
                firstNoise_1_1 += stepFirstNoise_1_1;
            }
        }
    }
    return chunkCache;
}


static int replaceBlockForBiomes(int i, int j, uint8_t *chunkCache, const int *biomes, Random *worldRandom, TerrainNoises terrainNoises) {
    uint8_t oceanLevel = 64;
    uint8_t MIN = oceanLevel;
    double noiseFactor = 0.03125;
    auto *sandFields = new double[16 * 16];
    auto *gravelField = new double[16 * 16];
    auto *heightField = new double[16 * 16];
    generateNoise(sandFields, i * 16, j * 16, 0.0, 16, 16, 1, noiseFactor, noiseFactor, 1.0, terrainNoises.shoresBottomComposition, 4);
    generateNoise(gravelField, j * 16, 109.0134, i * 16, 16, 1, 16, noiseFactor, 1.0, noiseFactor, terrainNoises.shoresBottomComposition, 4);
    generateNoise(heightField, i * 16, j * 16, 0.0, 16, 16, 1, noiseFactor * 2.0, noiseFactor * 2.0, noiseFactor * 2.0, terrainNoises.surfaceElevation, 4);

    for (int x = 0; x < 16; x++) {
        for (int k = 0; k < 12; k++) {
            next_double(worldRandom);
            next_double(worldRandom);
            next_double(worldRandom);
            for (int w = 0; w < 128; w++) {
                random_next_int(worldRandom, 5);
            }
        }
        for (int z = 12; z < 16; z++) {
            int biome = biomes[x * 16 + z];
            bool sandy = sandFields[x + z * 16] + next_double(worldRandom) * 0.20000000000000001 > 0.0;
            bool gravelly = gravelField[x + z * 16] + next_double(worldRandom) * 0.20000000000000001 > 3;
            int elevation = (int) (heightField[x + z * 16] / 3.0 + 3.0 + next_double(worldRandom) * 0.25);
            int state = -1;
            uint8_t aboveOceanAkaLand = GRASS;
            uint8_t belowOceanAkaEarthCrust = DIRT;
            for (int y = 127; y >= MIN; y--) {
                int chunkCachePos = (x * 16 + z) * 128 + y;
                uint8_t previousBlock = chunkCache[chunkCachePos];

                if (previousBlock == 0) {
                    state = -1;
                    continue;
                }
                if (previousBlock != STONE) {
                    continue;
                }
                if (state == -1) { // AIR
                    if (elevation <= 0) { // if in a deep
                        aboveOceanAkaLand = 0;
                        belowOceanAkaEarthCrust = STONE;
                    } else if (y <= oceanLevel + 1) { // if at sea level do the shore and rivers
                        aboveOceanAkaLand = GRASS;
                        belowOceanAkaEarthCrust = DIRT;
                        if (gravelly) {
                            aboveOceanAkaLand = 0;
                        }
                        if (gravelly) {
                            belowOceanAkaEarthCrust = GRAVEL;
                        }
                        if (sandy) {
                            aboveOceanAkaLand = SAND;
                        }
                        if (sandy) {
                            belowOceanAkaEarthCrust = SAND;
                        }
                    }
                    state = elevation;
                    // above ocean level
                    chunkCache[chunkCachePos] = aboveOceanAkaLand;
                    continue;
                }
                if (state > 0) {
                    state--;
                    chunkCache[chunkCachePos] = belowOceanAkaEarthCrust;

                }
            }
            for (int k = 0; k < 128; k++) {
                random_next_int(worldRandom, 5);
            }

        }
    }
}


static inline TerrainNoises *initTerrain(uint64_t worldSeed) {
    auto *terrainNoises = new TerrainNoises;
    Random worldRandom = get_random(worldSeed);
    PermutationTable *octaves = terrainNoises->minLimit;
    initOctaves(octaves, &worldRandom, 16);
    octaves = terrainNoises->maxLimit;
    initOctaves(octaves, &worldRandom, 16);
    octaves = terrainNoises->mainLimit;
    initOctaves(octaves, &worldRandom, 8);
    octaves = terrainNoises->shoresBottomComposition;
    initOctaves(octaves, &worldRandom, 4);
    octaves = terrainNoises->surfaceElevation;
    initOctaves(octaves, &worldRandom, 4);
    octaves = terrainNoises->scale;
    initOctaves(octaves, &worldRandom, 10);
    octaves = terrainNoises->depth;
    initOctaves(octaves, &worldRandom, 16);
    octaves = terrainNoises->forest;
    initOctaves(octaves, &worldRandom, 8);
    return terrainNoises;
}

//static inline uint8_t *provideChunk(int chunkX, int chunkZ, BiomeGeneration biomeGenerationInstance, BiomesBase[] biomesForGeneration) {
//    worldRandom.setSeed((long) chunkX * 0x4f9939f508L + (long) chunkZ * 0x1ef1565bd5L);
//    byte[]
//    chunkCache = new byte[32768];
//    // this.biomesForGeneration=this.biomeGenerationInstance.loadBiomes(this.biomesForGeneration, chunkX * 16, chunkZ * 16, 16, 16);
//    this.biomeGenerationInstance = biomeGenerationInstance;
//    double[]
//    temperatures = this.biomeGenerationInstance.temperature;
//    generateTerrain(chunkX, chunkZ, chunkCache, temperatures);
//    replaceBlockForBiomes(chunkX, chunkZ, chunkCache, biomesForGeneration);
//    return chunkCache;
//}
int main() {
    initTerrain(10);
}