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


struct PermutationTable {
    double xo;
    double yo;
    double zo;
    uint8_t permutations[512];
};

static inline void initOctaves(PermutationTable octaves[], Random *random, int nbOctaves) {
    for (int i = 0; i < nbOctaves; ++i) {
        octaves[i].xo = next_double(random) * 256.0;
        octaves[i].yo = next_double(random) * 256.0;
        octaves[i].zo = next_double(random) * 256.0;
        uint8_t *permutations = octaves[i].permutations;
        uint8_t j = 0;
        do {
            permutations[j] = j;
        } while (j++ != 255);
        uint8_t index = 0;
        do {
            uint32_t randomIndex = random_next_int(random, 256u - index) + index;
            if (randomIndex != index) {
                // swap
                permutations[index] ^= permutations[randomIndex];
                permutations[randomIndex] ^= permutations[index];
                permutations[index] ^= permutations[randomIndex];
            }
            permutations[index + 256] = permutations[index];
        } while (index++ != 255);
    }
}

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
enum blocks {
    AIR,
    STONE,
    GRASS,
    DIRT,
    BEDROCK [[maybe_unused]],
    MOVING_WATER [[maybe_unused]],
    SAND,
    GRAVEL,
    ICE [[maybe_unused]],
};

static inline double lerp(double x, double a, double b) {
    return a + x * (b - a);
}

static inline double grad(uint8_t hash, double x, double y, double z) {
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


static inline void generateNormalPermutations(double** buffer, double x, double y, int posXinChunk, int posZinChunk, double noiseFactorX, double noiseFactorY, double octaveSize, PermutationTable permutationTable) {
    uint8_t *permutations = permutationTable.permutations;
    double octaveWidth = 1.0 / octaveSize;
    int32_t i2 = -1;
    double x1 = 0.0;
    double x2 = 0.0;
    double xx1 = 0.0;
    double xx2 = 0.0;
    double t;
    double w;
    int columnIndex = 0;
    double xCoord = (x + (double) posXinChunk) * noiseFactorX + permutationTable.xo;
    auto clampedXcoord = (int32_t) xCoord;
    if (xCoord < (double) clampedXcoord) {
        clampedXcoord--;
    }
    auto xBottoms = (uint8_t) ((uint32_t) clampedXcoord & 0xffu);
    xCoord -= clampedXcoord;
    t = xCoord * 6 - 15;
    w = (xCoord * t + 10);
    double fadeX = xCoord * xCoord * xCoord * w;

    double zCoord = permutationTable.zo;
    auto clampedZCoord = (int32_t) zCoord;
    if (zCoord < (double) clampedZCoord) {
        clampedZCoord--;
    }
    auto zBottoms = (uint8_t) ((uint32_t) clampedZCoord & 0xffu);
    zCoord -= clampedZCoord;
    t = zCoord * 6 - 15;
    w = (zCoord * t + 10);
    double fadeZ = zCoord * zCoord * zCoord * w;
    for (int Y = 0; Y <= posZinChunk; Y++) {
        double yCoords = (y + (double) Y) * noiseFactorY + permutationTable.yo;
        auto clampedYCoords = (int32_t) yCoords;
        if (yCoords < (double) clampedYCoords) {
            clampedYCoords--;
        }
        auto yBottoms = (uint8_t) ((uint32_t) clampedYCoords & 0xffu);
        yCoords -= clampedYCoords;
        t = yCoords * 6 - 15;
        w = yCoords * t + 10;
        double fadeY = yCoords * yCoords * yCoords * w;
        // ZCoord

        if (Y == 0 || yBottoms != i2) { // this is wrong on so many levels, same ybottoms doesnt mean x and z were the same...
            i2 = yBottoms;
            uint16_t k2 = permutations[permutations[xBottoms] + yBottoms] + zBottoms;
            uint16_t l2 = permutations[permutations[xBottoms] + yBottoms + 1] + zBottoms;
            uint16_t k3 = permutations[permutations[xBottoms + 1] + yBottoms] + zBottoms;
            uint16_t l3 = permutations[permutations[xBottoms + 1] + yBottoms + 1] + zBottoms;
            x1 = lerp(fadeX, grad(permutations[k2], xCoord, yCoords, zCoord), grad(permutations[k3], xCoord - 1.0, yCoords, zCoord));
            x2 = lerp(fadeX, grad(permutations[l2], xCoord, yCoords - 1.0, zCoord), grad(permutations[l3], xCoord - 1.0, yCoords - 1.0, zCoord));
            xx1 = lerp(fadeX, grad(permutations[k2 + 1], xCoord, yCoords, zCoord - 1.0), grad(permutations[k3 + 1], xCoord - 1.0, yCoords, zCoord - 1.0));
            xx2 = lerp(fadeX, grad(permutations[l2 + 1], xCoord, yCoords - 1.0, zCoord - 1.0), grad(permutations[l3 + 1], xCoord - 1.0, yCoords - 1.0, zCoord - 1.0));
        }
        double y1 = lerp(fadeY, x1, x2);
        double y2 = lerp(fadeY, xx1, xx2);
        (*buffer)[columnIndex] = (*buffer)[columnIndex]  + lerp(fadeZ, y1, y2) * octaveWidth;
        columnIndex++;
    }
}


static inline void replaceBlockForBiomes(int chunkX, int chunkZ, TerrainNoises terrainNoises, int posX, int posZ) {
    double noiseFactor = 0.03125;
    auto *sandFields = new double[posZ+1];
    auto *heightField = new double[posZ+1];
    memset(sandFields, 0, sizeof(double) * (posZ+1));
    memset(heightField, 0, sizeof(double) * (posZ+1));

    double octavesFactor = 1.0;
    for (const auto &octave : terrainNoises.shoresBottomComposition) {
        generateNormalPermutations(&sandFields, chunkX * 16, chunkZ * 16, posX, posZ, noiseFactor * octavesFactor, noiseFactor * octavesFactor, octavesFactor, octave);
        octavesFactor /= 2.0;
    }

    octavesFactor = 1.0;
    for (const auto &octave : terrainNoises.surfaceElevation) {
        generateNormalPermutations(&heightField, chunkX * 16, chunkZ * 16, posX, posZ, noiseFactor * 2.0 * octavesFactor, noiseFactor * 2.0 * octavesFactor, octavesFactor, octave);
        octavesFactor /= 2.0;
    }
    printf("%f \n", sandFields[posZ]);
    printf("%f \n", heightField[posZ]);

    delete[] sandFields;
    delete[] heightField;
}


void TerrainInternalWrapper(uint64_t worldSeed, int32_t chunkX, int32_t chunkZ, int posX, int posZ) {
    auto *terrainNoises = new TerrainNoises;
    Random worldRandom = get_random(worldSeed);
    // can be optimized out
    PermutationTable *octaves = terrainNoises->minLimit;
    initOctaves(octaves, &worldRandom, 16);
    octaves = terrainNoises->maxLimit;
    initOctaves(octaves, &worldRandom, 16);
    octaves = terrainNoises->mainLimit;
    initOctaves(octaves, &worldRandom, 8);

    //crucial
    octaves = terrainNoises->shoresBottomComposition;
    initOctaves(octaves, &worldRandom, 4);
    octaves = terrainNoises->surfaceElevation;
    initOctaves(octaves, &worldRandom, 4);


    replaceBlockForBiomes(chunkX, chunkZ, *terrainNoises, posX, posZ);
    delete terrainNoises;
}

int main() {
    // 0 0 in the chunk -3 6 should be 1.297115 0.305733
    // with 16 16:
    //1.297115 1.277326 1.258765 1.241899 1.227131 1.214788 1.205103 1.198203 1.194097 1.192655 1.193602 1.196496 1.200721 1.205466 1.485083 1.487643 1.344847 1.323743 1.303790 1.285452 1.269133 1.255161 1.243772 1.235099 1.229156 1.225823 1.224835 1.225767 1.228017 1.230795 1.493258 1.493929 1.391145 1.369417 1.348724 1.329522 1.312205 1.297095 1.284427 1.274334 1.266835 1.261822 1.259046 1.258100 1.258413 1.259228 1.500391 1.499158 1.435645 1.414016 1.393270 1.373842 1.356108 1.340377 1.326876 1.315736 1.306980 1.300512 1.296102 1.293374 1.291793 1.290653 1.506374 1.503227 1.477945 1.457152 1.437053 1.418048 1.400490 1.384667 1.370789 1.358984 1.349278 1.341587 1.335706 1.331295 1.327870 1.324786 1.511069 1.506001 1.517626 1.498400 1.479639 1.461704 1.444910 1.429518 1.415719 1.403630 1.393280 1.384600 1.377413 1.371424 1.366206 1.361197 1.514313 1.507322 1.554269 1.537314 1.520559 1.504315 1.488854 1.474400 1.461121 1.449119 1.438425 1.428986 1.420656 1.413192 1.406239 1.399327 1.515920 1.507011 1.587467 1.573440 1.559318 1.545350 1.531756 1.518718 1.506373 1.494810 1.484057 1.474078 1.464764 1.455930 1.447301 1.438512 1.515693 1.504875 1.616839 1.606340 1.595419 1.584258 1.573017 1.561830 1.550801 1.539998 1.529449 1.519135 1.508988 1.498884 1.488640 1.478007 1.513425 1.500716 1.642044 1.635594 1.628372 1.620482 1.612021 1.603069 1.593692 1.583936 1.573825 1.563362 1.552519 1.541243 1.529445 1.517005 1.508909 1.494330 1.662791 1.660823 1.657711 1.653480 1.648154 1.641759 1.634316 1.625850 1.616381 1.605930 1.594516 1.582156 1.568866 1.554659 1.501941 1.485523 1.678847 1.681693 1.683009 1.682735 1.680822 1.677234 1.671950 1.664968 1.656305 1.646000 1.634121 1.620759 1.606037 1.590110 1.492330 1.474109 1.690044 1.697927 1.703885 1.707773 1.709463 1.708858 1.705891 1.700534 1.692798 1.682745 1.670488 1.656195 1.640100 1.622502 1.479902 1.459921 1.696287 1.709314 1.720021 1.728173 1.733566 1.736038 1.735478 1.731829 1.725100 1.715370 1.702802 1.687640 1.670229 1.651012 1.464507 1.442815 1.697556 1.715716 1.731166 1.743581 1.752682 1.758244 1.760109 1.758194 1.752503 1.743137 1.730301 1.714321 1.695646 1.674867 1.446028 1.422682 1.693910 1.717075 1.737150 1.753724 1.766444 1.775024 1.779261 1.779048 1.774382 1.765383 1.752303 1.735542 1.715655 1.693373 1.424389 1.399450
    //0.305733 0.221421 0.150732 0.093559 -0.055063 -0.089703 -0.102984 -0.088360 0.445624 0.520316 0.620626 0.740555 0.872809 1.009471 1.142684 1.265323 0.394940 0.314910 0.247225 0.192008 0.015900 -0.017480 -0.029710 -0.014334 0.520060 0.594823 0.694914 0.814405 0.946089 1.082147 1.214811 1.337040 0.511059 0.434624 0.369593 0.316260 0.113058 0.080867 0.069043 0.083690 0.616085 0.687626 0.783469 0.897992 1.024359 1.155146 1.282977 1.401154 0.647532 0.573803 0.510963 0.459395 0.231241 0.200190 0.188245 0.200944 0.729346 0.794931 0.883182 0.988975 1.106078 1.227721 1.347164 1.458266 0.795928 0.723923 0.662753 0.612812 0.363392 0.333471 0.321076 0.331026 0.854112 0.911921 0.990347 1.084895 1.190099 1.300009 1.408691 1.510707 0.946751 0.875504 0.815495 0.767037 0.501217 0.472465 0.459529 0.466453 0.983736 1.033079 1.100825 1.183161 1.275449 1.372633 1.469647 1.561819 1.090902 1.019568 0.960273 0.913176 0.636460 0.608962 0.595641 0.599794 1.111640 1.152989 1.210613 1.281355 1.361374 1.446470 1.532412 1.615265 1.223259 1.151196 1.092281 1.046460 0.764284 0.738171 0.724827 0.726907 1.234383 1.269157 1.318375 1.379441 1.449197 1.524183 1.600886 1.676000 1.343590 1.270411 1.211683 1.167102 0.883959 0.859378 0.846462 0.847356 1.351836 1.381873 1.424902 1.478774 1.540871 1.608310 1.678142 1.747560 1.457845 1.383415 1.324823 1.281499 0.999890 0.976961 0.964809 0.965165 1.467643 1.494265 1.532704 1.581169 1.637476 1.699210 1.763878 1.829072 1.577207 1.501521 1.443084 1.401055 1.122231 1.100973 1.089496 1.089070 1.589126 1.611748 1.644874 1.687114 1.736772 1.791957 1.850686 1.910999 1.707989 1.631027 1.572756 1.532059 1.259397 1.239717 1.228352 1.225899 1.721533 1.737432 1.761923 1.794206 1.833300 1.878081 1.927319 1.979711 1.850612 1.772452 1.714412 1.675097 1.413614 1.395379 1.383401 1.377343 1.866026 1.871769 1.883438 1.901059 1.924654 1.954177 1.989446 2.030083 2.001781 1.922678 1.865030 1.827185 1.582525 1.565622 1.552401 1.541359 2.020879 2.013458 2.008640 2.007482 2.011259 2.021271 2.038652 2.064178 2.156124 2.076530 2.019546 1.983297 1.760899 1.745272 1.730430 1.713559 2.182554 2.160110 2.136536 2.114049 2.095332 2.083201 2.080269 2.088611 2.307550 2.228089 2.172137 2.137642 1.942081 1.927748 1.911239 1.888398 2.346621 2.308806 2.266046 2.221740 2.180015 2.145236 2.121529 2.112296
    TerrainInternalWrapper(18420882071630, -3, 6, 0,6);
}