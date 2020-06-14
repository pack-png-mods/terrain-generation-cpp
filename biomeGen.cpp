#include <iostream>
#include <chrono>
#include "biomeGen.h"

static_assert(std::numeric_limits<double>::is_iec559, "This code requires IEEE-754 doubles");

static inline void simplexNoise(double **buffer, double chunkX, double chunkZ, int x, int z, double offsetX, double offsetZ, double octaveFactor, PermutationTable permutationTable) {
    int k = 0;

    uint8_t *permutations = permutationTable.permutations;
    for (int X = 0; X < x; X++) {
        double Xcoords = (chunkX + (double) X) * offsetX + permutationTable.xo;
        for (int Z = 0; Z < z; Z++) {
            double Zcoords = (chunkZ + (double) Z) * offsetZ + permutationTable.yo;
            // Skew the input space to determine which simplex cell we're in
            double hairyFactor = (Xcoords + Zcoords) * F2;
            auto tempX = static_cast<int32_t>(Xcoords + hairyFactor);
            auto tempZ = static_cast<int32_t>(Zcoords + hairyFactor);
            uint32_t xHairy = (Xcoords + hairyFactor < tempX) ? (tempX - 1) : (tempX);
            uint32_t zHairy = (Zcoords + hairyFactor < tempZ) ? (tempZ - 1) : (tempZ);
            double d11 = (double) (xHairy + zHairy) * G2;
            double X0 = (double) xHairy - d11; // Unskew the cell origin back to (x,y) space
            double Y0 = (double) zHairy - d11;
            double x0 = Xcoords - X0; // The x,y distances from the cell origin
            double y0 = Zcoords - Y0;
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
            uint32_t ii = xHairy & 0xffu;
            uint32_t jj = zHairy & 0xffu;
            uint8_t gi0 = permutations[ii + permutations[jj]] % 12;
            uint8_t gi1 = permutations[ii + offsetSecondCornerX + permutations[jj + offsetSecondCornerZ]] % 12;
            uint8_t gi2 = permutations[ii + 1 + permutations[jj + 1]] % 12;

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
            (*buffer)[k] = (*buffer)[k] + 70.0 * (n0 + n1 + n2) * octaveFactor;
            k++;

        }

    }
}


static inline void getFixedNoise(double *buffer, double chunkX, double chunkZ, int x, int z, double offsetX, double offsetZ, double ampFactor, PermutationTable *permutationTable, uint8_t octaves) {
    offsetX /= 1.5;
    offsetZ /= 1.5;
    // cache should be handled by the caller
    for (int i = 0; i < x * z; ++i) {
        buffer[i] = 0.0;
    }
    double octaveDiminution = 1.0;
    double octaveAmplification = 1.0;
    for (uint8_t j = 0; j < octaves; ++j) {
        simplexNoise(&buffer, chunkX, chunkZ, x, z, offsetX * octaveAmplification, offsetZ * octaveAmplification, 0.55000000000000004 / octaveDiminution, permutationTable[j]);
        octaveAmplification *= ampFactor;
        octaveDiminution *= 0.5;
    }

}


static inline BiomeNoises *initBiomeGen(uint64_t worldSeed) {
    auto *pBiomeNoises = new BiomeNoises;
    Random worldRandom;
    PermutationTable *octaves;
    worldRandom = get_random(worldSeed * 9871L);
    octaves = pBiomeNoises->temperatureOctaves;
    initOctaves(octaves, &worldRandom, 4);
    worldRandom = get_random(worldSeed * 39811L);
    octaves = pBiomeNoises->humidityOctaves;
    initOctaves(octaves, &worldRandom, 4);
    worldRandom = get_random(worldSeed * 0x84a59L);
    octaves = pBiomeNoises->precipitationOctaves;
    initOctaves(octaves, &worldRandom, 2);
    return pBiomeNoises;
}


static inline Biomes *getBiomes(Biomes *biomes, int posX, int posZ, int sizeX, int sizeZ, BiomeNoises *biomesOctaves) {
    auto *temperature = new double[sizeX * sizeZ];
    auto *humidity = new double[sizeX * sizeZ];
    auto *precipitation = new double[sizeX * sizeZ];
    getFixedNoise(temperature, posX, posZ, sizeX, sizeZ, 0.02500000037252903, 0.02500000037252903, 0.25, (*biomesOctaves).temperatureOctaves, 4);
    getFixedNoise(humidity, posX, posZ, sizeX, sizeZ, 0.05000000074505806, 0.05000000074505806, 0.33333333333333331, (*biomesOctaves).humidityOctaves, 4);
    getFixedNoise(precipitation, posX, posZ, sizeX, sizeZ, 0.25, 0.25, 0.58823529411764708, (*biomesOctaves).precipitationOctaves, 2);
    int index = 0;
    for (int X = 0; X < sizeX; X++) {
        for (int Z = 0; Z < sizeZ; Z++) {
            double preci = precipitation[index] * 1.1000000000000001 + 0.5;
            double temp = (temperature[index] * 0.14999999999999999 + 0.69999999999999996) * (1.0 - 0.01) + preci * 0.01;
            temp = 1.0 - (1.0 - temp) * (1.0 - temp);
            if (temp < 0.0) {
                temp = 0.0;
            }
            if (temp > 1.0) {
                temp = 1.0;
            }
            double humi = (humidity[index] * 0.14999999999999999 + 0.5) * (1.0 - 0.002) + preci * 0.002;
            if (humi < 0.0) {
                humi = 0.0;
            }
            if (humi > 1.0) {
                humi = 1.0;
            }
            temperature[index] = temp;
            humidity[index] = humi;;;
            biomes[index] = biomesTable[(int) (temp * 63) + (int) (humi * 63) * 64];
            index++;
        }
    }
    return biomes;
}

static Biomes *BiomeWrapper(uint64_t worldSeed, int32_t chunkX, int32_t chunkZ) {
    BiomeNoises *biomesOctaves = initBiomeGen(worldSeed);
    return getBiomes(new Biomes[16 * 16], chunkX * 16, chunkZ * 16, 16, 16, biomesOctaves);
}

static inline void printBiomes(int64_t worldSeed, int32_t chunkX, int32_t chunkZ) {
    Biomes *biomes = BiomeWrapper(worldSeed, chunkX, chunkZ);
    for (int i = 0; i < 16 * 16; ++i) {
        std::cout << biomesNames[biomes[i]] << " ";
    }
    std::cout << std::endl;
}

int main() {
    Random random = get_random(123456u);
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
        long seed = random_next_long(&random);
        //std::cout<<seed<<std::endl;
        printBiomes(seed, 15, 16);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1e9 << " s\n";

}
