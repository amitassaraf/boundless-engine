//
// Created by Amit Assaraf on 17/07/2021.
//

#include "world.hpp"
#include "FastNoise/FastNoise.h"
#include<iostream>


namespace Boundless {

    World::World() {
        // Create an array of floats to store the noise output in
        for (int x = 0; x < WORLD_SIZE; x++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                for (int y = 0; y < WORLD_HEIGHT; y++) {
                    Ref<Tile> tile = std::make_shared<Tile>(x, y, z,  "GQAlAM3MzD7NzMw+AACAPwAAgD8XAAAAgL8AAIA/AACAvwAAgD8TAAAAgD8iAJqZeUDXo3A/FwAAAIC/AACAPwAAgL8AAIA/FQAAAAAACtcjvbgexT8XAAAAgL8AAIA/AACAvwAAgD8RAAMAAACamRk/EADNzEw/GQATADMzMz8NAAUAAAC4HiVACQAAKVxPPwAfhWs/AQQAAAAAAAAAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAuB6FPgDsUTg+AK5HYT4Aj8J1vg==" );

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

    int World::shouldDivide(const glm::vec3 &chunkOffset, uint16_t nodeSize, std::vector<std::vector<float> >& noise) {
        int step = 1;

        if (nodeSize > 64) {
            step = round(nodeSize / 10);
        }

        float normalized = noise[0][0];
        for (int x = chunkOffset.x; x < chunkOffset.x + nodeSize; x += step) {
            for (int z = chunkOffset.z; z < chunkOffset.z + nodeSize; z += step) {
                normalized = noise[x][z];
                if (nodeSize > 1.0f && normalized >= chunkOffset.y && normalized < chunkOffset.y + nodeSize) {
                    return 0;
                }
            }
        }

        if (normalized < chunkOffset.y) {
            return -1;
        }

        return 1;
    }

    World::~World() = default;

}
