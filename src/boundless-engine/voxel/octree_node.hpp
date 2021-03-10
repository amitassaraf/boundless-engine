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
            std::uint8_t getFaceMask() const;
            void setFaceMask(std::uint8_t faceMask);
            void setLOD(std::uint32_t lod);

        private:
            Voxel m_voxel;
            std::uint32_t m_locationalCode;
            std::uint32_t m_nodeSize;
            std::uint8_t m_childrenMask = 0;
            std::uint8_t m_faceMask = 0;
            std::uint32_t m_lod = 1;
    };

}

#define BOTTOM_LEFT_FRONT 0u // 000
#define BOTTOM_RIGHT_FRONT 1u // 001
#define BOTTOM_LEFT_BACK 2u // 010
#define BOTTOM_RIGHT_BACK 3u // 011
#define TOP_LEFT_FRONT 4u  // 100
#define TOP_RIGHT_FRONT 5u // 101
#define TOP_LEFT_BACK 6u // 110
#define TOP_RIGHT_BACK 7u // 111

#define LEFT_RIGHT_FACE_BITS_TEST 1u  // 0b001001001001001001001001001001
#define FRONT_BACK_FACE_BITS_TEST 2u // 0b010010010010010010010010010010
#define TOP_BOTTOM_FACE_BITS_TEST 4u // 0b100100100100100100100100100100

#define FACE_TOP 1u // 000
#define FACE_BOTTOM 2u // 010
#define FACE_LEFT 4u // 100
#define FACE_RIGHT 8u // 1000
#define FACE_FRONT 16u  // 10000
#define FACE_BACK 32u // 100000

#endif // !BD_OCTREE_NODE_HPP_
