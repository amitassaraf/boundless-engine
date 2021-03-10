#pragma once
#ifndef BD_WORLD_HPP_
#define BD_WORLD_HPP_

#include <cstdint>
#include <cstddef>
#include "voxel/octree.hpp"
#include "core/core.hpp"
#include <SimplexNoise.h>

namespace Boundless {

    class World {
        public:
            World();
            ~World();
            void generateWorld();
            int shouldDivide(const glm::vec3& chunkOffset, uint32_t nodeSize, uint32_t lod);
            void changeLOD(Ref<OctreeNode>& node, uint32_t lod);
            inline Octree* getOctree() { return m_octree; }
        private:
            Octree* m_octree;
            uint32_t m_size;
            SimplexNoise m_noise;
    };

}

#endif // !BD_WORLD_HPP_
