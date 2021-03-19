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
            void renderWorldAround(glm::vec3 playerPosition);
            int shouldDivide(const glm::vec3& chunkOffset, uint16_t nodeSize);
            bool changeLOD(Ref<OctreeNode>& node, uint8_t command);
            inline Scope<Octree>& getOctree() { return m_octree; }
        private:
            Scope<Octree> m_octree;
            uint16_t m_size;
            SimplexNoise m_noise;
            std::unordered_map<uint32_t, uint16_t> m_samples;
    };

}

#define COLLAPSE 0
#define DIVIDE 1

#endif // !BD_WORLD_HPP_
