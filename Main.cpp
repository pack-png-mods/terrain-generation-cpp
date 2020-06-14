#include <iostream>
#include <fstream>
#include <chrono>
#include "biomeGen.h"
#include "terrainGen.h"

# define OFFSET 12

void filterDownSeeds(const int64_t *worldSeeds, int32_t posX, int64_t nbSeeds) {
    int mapWat[] = {77, 78, 77, 75}; // from z 12 to z15 in chunk
    int chunkX = (int32_t) ((posX + 16u) >> 4u) - 1;
    int chunkZ = -3;
    for (int i = 0; i < nbSeeds; ++i) {
        int64_t seed = worldSeeds[i];
        BiomeResult *biomeResult = BiomeWrapper(seed, chunkX, chunkZ);
        bool skipIt = false;
        for (int j = 0; j < 16 * 16; ++j) {
            Biomes biome = biomeResult->biomes[j];
            if (biome == Rainforest || biome == Swampland || biome == Savanna || biome == Taiga || biome == Desert || biome == IceDesert || biome == Tundra) {
                skipIt = true;
                break;
            }
        }
        if (skipIt) {
            delete_biome_result(biomeResult);
            continue;
        }
        uint8_t *chunkCache = TerrainInternalWrapper(seed, chunkX, chunkZ, biomeResult);
        for (int x = 0; x < 16; x++) {
            bool flag = true;
            for (int z = 0; z < (16 - OFFSET); z++) {
                uint32_t pos = 128 * x * 16 + 128 * (z + OFFSET);
                uint32_t y;

                for (y = 80; y >= 70 && chunkCache[pos + y] == 0; y--);
                if ((y + 1) != mapWat[z]) {
                    flag = false;
                    break;
                }

            }
            if (flag) {
                std::cout << "Found seed: " << seed << " at x: " << posX << " and z:-30" << std::endl;
            }
        }
        delete[] chunkCache;
        delete_biome_result(biomeResult);
    }

}

int main(int argc, char *argv[]) {


    FILE *fp = fopen("test.txt", "r");
    if (fp == nullptr) {
        std::cout << "file was not loaded" << std::endl;
        throw std::runtime_error("file was not loaded");
    }
    int64_t length=0;
    for (int c = getc(fp); c != EOF; c = getc(fp))if (c == '\n')length = length + 1;
    rewind(fp);
    auto *worldSeeds = new int64_t[length];
    char *line = nullptr;
    size_t len = 0;
    char *endPtr;
    int64_t seed;
    int64_t index=0;
    while ((getline(&line, &len, fp)) != -1) {
        errno = 0;
        seed = strtoll(line, &endPtr, 10);
        if ((errno == ERANGE && (seed == INT64_MAX || seed == INT64_MIN))
            || (errno != 0 && seed == 0)) {
            std::cout << "Error conversion" << std::endl;
            perror("strtol");
            exit(EXIT_FAILURE);
        }
        if (endPtr == line) {
            fprintf(stderr, "No digits were found\n");
            exit(EXIT_FAILURE);
        }
        worldSeeds[index++]=seed;
    }
    fclose(fp);
    if (line)
        free(line);
    auto start = std::chrono::high_resolution_clock::now();
    filterDownSeeds(worldSeeds, 99, length);
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1e9 << " s\n";
    delete[] worldSeeds;
}