//
// Created by Amit Assaraf on 17/07/2021.
//

#include "world.hpp"
#include "FastNoise/FastNoise.h"

float normalize(float min, float max, float input) {
    float average = (min + max) / 2.0f;
    float range = (max - min) / 2.0f;
    float normalized_x = (input - average) / range;
    return (normalized_x + 1.0f) / 2.0f;
}

float noise[WORLD_SIZE * TILE_SIZE][WORLD_SIZE * TILE_SIZE];

FastNoise::SmartNode<> fnGenerator = FastNoise::NewFromEncodedNodeTree( "FwAAAIC/AACAPwAAAL8AAEA/EwBcj8I+IgCPwpVASOF6PxcAAACAvwAAgD8AAIC/AACAPxUAAAAAAArXozwAAMA/FwAAAIC/AACAPwAAgL8AAIA/EQACAAAAAAAgQBAAAAAAQBkAEwDD9Sg/DQAEAAAAAAAgQAkAAGZmJj8AAAAAPwEEAAAAAAAAAEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAM3MTD4AMzMzPwAAAAA/" );

namespace Boundless {

    World::World() {
        // Create an array of floats to store the noise output in
        int size = TILE_SIZE * WORLD_SIZE;
        std::vector<float> noiseOutput(size * size);

        FastNoise::OutputMinMax minMax = fnGenerator->GenUniformGrid2D(noiseOutput.data(), 0, 0, size,  size, 0.003f, 1337);

        int index = 0;
        for (int z = 0; z < size; z++) {
            for (int x = 0; x < size; x++) {
                noise[x][z] = floor(normalize(minMax.min, minMax.max, noiseOutput[index++]) * (TILE_SIZE * WORLD_HEIGHT));
            }
        }

        for (int x = 0; x < WORLD_SIZE; x++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                for (int y = 0; y < WORLD_HEIGHT; y++) {
                    Ref<Tile> tile = std::make_shared<Tile>(x, y, z);

                    tile->initialize(Boundless::World::shouldDivide);
                    m_tiles.push_back(tile);
                }
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
                float normalized = noise[x][z];
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
