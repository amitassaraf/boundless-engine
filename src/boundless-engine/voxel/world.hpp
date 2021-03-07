#pragma once
#ifndef BD_WORLD_HPP_
#define BD_WORLD_HPP_

#include <cstdint>
#include <cstddef>
#include "voxel/octree.hpp"

namespace Boundless {

    class World {
        public:
            World();
            ~World();
            void generateWorld();
            int shouldDivide(const glm::vec3& chunkOffset, uint32_t nodeSize);
            Octree* m_octree;
            uint32_t m_size;
    };

}

#endif // !BD_WORLD_HPP_
