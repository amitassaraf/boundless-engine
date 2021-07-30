//
// Created by Amit Assaraf on 17/07/2021.
//

#include "world.hpp"
#include "FastNoise/FastNoise.h"

float scale = 100.f;
float lacunarity = 0.8f;
float persistance = 0.8f;

const int octaves = static_cast<int>(3 + std::log(scale)); // Estimate number of octaves needed for the current scale

const float min = -1.0f;
const float max = 1.0f;

float normalize(float input) {
    float average = (min + max) / 2.0f;
    float range = (max - min) / 2.0f;
    float normalized_x = (input - average) / range;
    return (normalized_x + 1.0f) / 2.0f;
}

float noise[WORLD_SIZE * TILE_SIZE][WORLD_SIZE * TILE_SIZE];

FastNoise::SmartNode<> fnGenerator = FastNoise::NewFromEncodedNodeTree( "EwDNzMw9CQA=" );

namespace Boundless {

    World::World() : m_noise(SimplexNoise(0.1f / scale, 0.1f, lacunarity, persistance)) {
        // Create an array of floats to store the noise output in
        int size = TILE_SIZE * WORLD_SIZE;
        std::vector<float> noiseOutput(size * size * size);

        fnGenerator->GenUniformGrid3D(noiseOutput.data(), 0, 0, 0, size,  size, size, 0.02f, 1337);
        int index = 0;
        for (int z = 0; z < size; z++) {
            for (int y = 0; y < size; y++) {
                for (int x = 0; x < size; x++) {

                    float noiseValue = noiseOutput[index++];
                    if (noise[x][z] < noiseValue) {
                        noise[x][z] = noiseValue;
                    }
                }
            }
        }

        for (int x = 0; x < WORLD_SIZE; x++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                Ref<Tile> tile = std::make_shared<Tile>(x, z);

                tile->initialize(Boundless::World::shouldDivide);
                m_tiles.push_back(tile);

            }
        }
    }

    void World::update(const glm::vec3 &lodCenter) {
        for (const Ref<Tile>& tile : m_tiles) {
            tile->updateLOD(lodCenter, Boundless::World::shouldDivide);
        }
    }

    int World::shouldDivide(const glm::vec3 &chunkOffset, uint16_t nodeSize) {
        int above = 1;

        int step = 1;

        if (nodeSize > 64) {
            step = round(nodeSize / 10);
        }

        for (int x = chunkOffset.x; x < chunkOffset.x + nodeSize; x += step) {
            for (int z = chunkOffset.z; z < chunkOffset.z + nodeSize; z += step) {
                float normalized = floor(noise[x][z] * TILE_SIZE);
                if (nodeSize > 1.0f && normalized >= chunkOffset.y && normalized < chunkOffset.y + nodeSize) {
                    return 0;
                } else if (normalized >= chunkOffset.y + nodeSize) {
                    above = 1;
                } else if (normalized < chunkOffset.y) {
                    above = -1;
                }
            }
        }
        return above;
    }

    World::~World() = default;

}
