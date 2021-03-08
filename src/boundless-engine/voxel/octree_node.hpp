#pragma once
#ifndef BD_OCTREE_NODE_HPP_
#define BD_OCTREE_NODE_HPP_

#include <cstdint>
#include <stdexcept>
#include <cstddef>
#include "voxel.hpp"
#include "core/core.hpp"
#include "logging/logger.hpp"

namespace Boundless {
    class OctreeNode {
        public:
            OctreeNode(uint32_t locationalCode, uint32_t nodeSize);
            ~OctreeNode() {}

            std::size_t getDepth() const;
            glm::vec3 getChunkOffset() const;
            std::uint32_t getSize() const;
            bool isLeaf() const;
            Voxel& getVoxelData();
            std::uint32_t getLocationalCode() const;
            std::uint8_t getChildrenMask() const;
            void setChildrenMask(std::uint8_t childMask);
            std::uint32_t getLOD() const;

        private:
            Voxel m_voxel;
            std::uint32_t m_locationalCode;
            std::uint8_t m_childrenMask = 0;
            std::uint32_t m_nodeSize;
            std::uint32_t m_lod = 1;
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
