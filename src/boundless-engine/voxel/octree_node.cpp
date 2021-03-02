#include "voxel/octree_node.hpp"
#include "core/core.hpp"
#include "logging/logger.hpp"

namespace Boundless {

    OctreeNode::OctreeNode(uint32_t locationalCode, uint32_t nodeSize) 
    : m_locationalCode(locationalCode), m_nodeSize(nodeSize) {

    }

    glm::vec3 OctreeNode::getChunkOffset() {
        uint32_t locationalCode = m_locationalCode;
        glm::vec3 chunkOffset(0, 0, 0);
        uint8_t count = 0;
        while (locationalCode > 1) {
            uint8_t localCode = locationalCode & 7u;
            float locationalDifference = m_nodeSize * (2u * count);
            switch (localCode) {
                case BOTTOM_LEFT_BACK:
                    // Root Node, always at 0
                    break;
                case BOTTOM_LEFT_FRONT:
                    chunkOffset.x = chunkOffset.x + locationalDifference;
                    break;
                case BOTTOM_RIGHT_BACK:
                    chunkOffset.z = chunkOffset.z + locationalDifference;
                    break;
                case BOTTOM_RIGHT_FRONT:
                    chunkOffset.z = chunkOffset.z + locationalDifference;
                    chunkOffset.x = chunkOffset.x + locationalDifference;
                    break;
                case TOP_LEFT_BACK:
                    chunkOffset.y = chunkOffset.y + locationalDifference;
                    break;
                case TOP_LEFT_FRONT:
                    chunkOffset.y = chunkOffset.y + locationalDifference;
                    chunkOffset.x = chunkOffset.x + locationalDifference;
                    break;
                case TOP_RIGHT_BACK:
                    chunkOffset.y = chunkOffset.y + locationalDifference;
                    chunkOffset.z = chunkOffset.z + locationalDifference;
                    break;
                case TOP_RIGHT_FRONT:
                    chunkOffset.y = chunkOffset.y + locationalDifference;
                    chunkOffset.z = chunkOffset.z + locationalDifference;
                    chunkOffset.x = chunkOffset.x + locationalDifference;
                    break;
                default:
                    break;
            }

            locationalCode >>= 3;
            count++;
        }
        return chunkOffset;
    }

    std::size_t OctreeNode::getDepth() {
        if (m_locationalCode == 0) {
            throw std::runtime_error("No depth.");
        }

    #if defined(__GNUC__)
        return (31-__builtin_clz(m_locationalCode))/3;
    #elif defined(_MSC_VER)
        long msb;
        _BitScanReverse(&msb, m_locationalCode);
        return msb/3;
    #endif
    }



}