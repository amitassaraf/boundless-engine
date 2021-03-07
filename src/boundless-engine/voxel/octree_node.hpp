#pragma once
#ifndef BD_OCTREE_NODE_HPP_
#define BD_OCTREE_NODE_HPP_

#include <cstdint>
#include <stdexcept>
#include <cstddef>
#include "voxel.hpp"

namespace Boundless {
    class OctreeNode {
        public:
            OctreeNode(uint32_t locationalCode, uint32_t nodeSize);

            std::size_t getDepth() const;
            std::uint32_t getSize() const;
            glm::vec3 getChunkOffset() const;
            bool isLeaf() const;
    
            // Voxel voxel;
            std::uint32_t m_locationalCode;
            std::uint8_t m_childrenMask;
            std::uint32_t m_nodeSize;
    };

}

#define BOTTOM_LEFT_FRONT 0
#define BOTTOM_RIGHT_FRONT 1
#define BOTTOM_LEFT_BACK 2
#define BOTTOM_RIGHT_BACK 3
#define TOP_LEFT_FRONT 4
#define TOP_RIGHT_FRONT 5
#define TOP_LEFT_BACK 6
#define TOP_RIGHT_BACK 7

#endif // !BD_OCTREE_NODE_HPP_
