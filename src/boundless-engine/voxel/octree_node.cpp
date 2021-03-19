#include "voxel/octree_node.hpp"
#include "core/core.hpp"
#include "logging/logger.hpp"
#include <bitset>

namespace Boundless {

    OctreeNode::OctreeNode(uint64_t locationalCode, uint16_t octreeSize) 
    : m_voxel(false), m_locationalCode(locationalCode), m_octreeSize(octreeSize) {

    }

    bool OctreeNode::isLeaf() const {
        return m_childrenMask == 0;
    }

    glm::vec3 OctreeNode::getChunkOffset() const {
        // uint64_t locationalCode = m_locationalCode;
        // glm::vec3 chunkOffset(0, 0, 0);
        // uint16_t size = m_octreeSize / pow(2, getDepth());

        // while (locationalCode > 1) {
        //     uint8_t localCode = locationalCode & 7u;
        //     float locationalDifference = size;
        //     switch (localCode) {
        //         case BOTTOM_LEFT_BACK:
        //             // Root Node, always at 0
        //             break;
        //         case BOTTOM_LEFT_FRONT:
        //             chunkOffset.x = chunkOffset.x + locationalDifference;
        //             break;
        //         case BOTTOM_RIGHT_BACK:
        //             chunkOffset.z = chunkOffset.z + locationalDifference;
        //             break;
        //         case BOTTOM_RIGHT_FRONT:
        //             chunkOffset.z = chunkOffset.z + locationalDifference;
        //             chunkOffset.x = chunkOffset.x + locationalDifference;
        //             break;
        //         case TOP_LEFT_BACK:
        //             chunkOffset.y = chunkOffset.y + locationalDifference;
        //             break;
        //         case TOP_LEFT_FRONT:
        //             chunkOffset.y = chunkOffset.y + locationalDifference;
        //             chunkOffset.x = chunkOffset.x + locationalDifference;
        //             break;
        //         case TOP_RIGHT_BACK:
        //             chunkOffset.y = chunkOffset.y + locationalDifference;
        //             chunkOffset.z = chunkOffset.z + locationalDifference;
        //             break;
        //         case TOP_RIGHT_FRONT:
        //             chunkOffset.y = chunkOffset.y + locationalDifference;
        //             chunkOffset.z = chunkOffset.z + locationalDifference;
        //             chunkOffset.x = chunkOffset.x + locationalDifference;
        //             break;
        //         default:
        //             break;
        //     }

        //     locationalCode >>= 3u;
        //     size *= 2u;
        // }

        // BD_CORE_TRACE("Offset: {}.{}.{}", chunkOffset.x, chunkOffset.y, chunkOffset.z);
        
        uint16_t nodeSize = getSize();
        uint64_t tops = m_locationalCode & BIT64_TOP_BOTTOM_FACE_BITS_TEST;

        uint64_t rights = m_locationalCode & BIT64_LEFT_RIGHT_FACE_BITS_TEST;
        rights = rights ^ (1 << (63 - __builtin_clzll(rights)));

        uint16_t lsb = __builtin_clzll(m_locationalCode);
        uint64_t fronts = ((m_locationalCode ^ BIT64_FRONT_BACK_FACE_BITS_TEST) << lsb) >> lsb & BIT64_FRONT_BACK_FACE_BITS_TEST;

        uint16_t topOffset = 0;
        uint16_t frontsOffset = 0;
        uint16_t rightsOffset = 0;
        while (tops > 0) {
            uint16_t index = 63 - __builtin_clzll(tops);
            tops = tops ^ (1 << index);
            uint16_t depth = index / 3;
            topOffset += pow(2u, log2(nodeSize) + depth);
        }

        while (fronts > 0) {
            uint16_t index = 63u - __builtin_clzll(fronts);
            fronts = fronts ^ (1u << index);
            uint16_t depth = (index / 3u);
            frontsOffset += pow(2u, log2(nodeSize) + depth);
        }

        // Remove root node
        while (rights > 0) {
            uint16_t index = 63 - __builtin_clzll(rights);
            rights = rights ^ (1 << index);
            uint16_t depth = index / 3;
            rightsOffset += pow(2u, log2(nodeSize) + depth);
        }

        return glm::vec3(frontsOffset, topOffset, rightsOffset);
    }

    std::uint16_t OctreeNode::getSize() const {
        return m_octreeSize / pow(2u, getDepth());
    }

    std::uint16_t OctreeNode::getOctreeSize() const {
        return m_octreeSize;
    }

    std::size_t OctreeNode::getDepth() const {
        if (m_locationalCode == 0) {
            throw std::runtime_error("No depth.");
        }

    #if defined(__GNUC__)
        return (63-__builtin_clzll(m_locationalCode))/3;
    #elif defined(_MSC_VER)
        long msb;
        _BitScanReverse(&msb, m_locationalCode);
        return msb/3;
    #endif
    }

    Voxel& OctreeNode::getVoxelData() {
        return m_voxel;
    }

    std::uint64_t OctreeNode::getLocationalCode() const {
        return m_locationalCode;
    }

    std::uint8_t OctreeNode::getChildrenMask() const {
        return m_childrenMask;
    }

    void OctreeNode::setChildrenMask(std::uint8_t childMask) {
        m_childrenMask = childMask;
    }

    std::uint8_t OctreeNode::getFaceMask() const {
        return m_faceMask;
    }

    void OctreeNode::setFaceMask(std::uint8_t faceMask) {
        m_faceMask = faceMask;
    }

}