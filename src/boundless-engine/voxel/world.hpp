#pragma once
#ifndef BD_WORLD_HPP_
#define BD_WORLD_HPP_

#include <cstdint>
#include <cstddef>
#include "voxel/octree.hpp"
#include "core/core.hpp"
#include <SimplexNoise.h>

#define WORLD_SIZE 4096

namespace Boundless {

    class World {
        public:
            World();
            ~World();
            void generateWorld();
            void renderWorldAround(const glm::vec3& playerPosition);
            OctreeNode findIntersectingNode(const glm::vec3& position);
            int shouldDivide(const glm::vec3& chunkOffset, uint16_t nodeSize);
            bool collapseNode(OctreeNode& node);
            bool divideNode(OctreeNode& node, const glm::vec3& referenceOffset);
            inline Scope<Octree>& getOctree() { return m_octree; }
        private:
            Scope<Octree> m_octree;
            uint16_t m_size;
            SimplexNoise m_noise;
    };

}

#define COLLAPSE 0
#define DIVIDE 1

#endif // !BD_WORLD_HPP_
