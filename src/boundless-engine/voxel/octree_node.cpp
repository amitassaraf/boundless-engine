#include "voxel/octree_node.hpp"
#include "core/core.hpp"
#include "logging/logger.hpp"
#include <bitset>

namespace Boundless {

    OctreeNode::OctreeNode(uint64_t locationalCode, uint16_t octreeSize, uint8_t lod) 
    : m_voxel(false), m_locationalCode(locationalCode), m_octreeSize(octreeSize), m_lod(lod) {

    }

    bool OctreeNode::isLeaf() const {
        return m_childrenMask == 0;
    }

    glm::vec3 OctreeNode::getChunkOffset() const {
        uint64_t locationalCode = m_locationalCode;
        glm::vec3 chunkOffset(0, 0, 0);
        uint16_t size = m_octreeSize / pow(2, getDepth());

        while (locationalCode > 1) {
            uint8_t localCode = locationalCode & 7u;
            float locationalDifference = size;
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

            locationalCode >>= 3u;
            size *= 2u;
        }
        return chunkOffset;
    }

    std::uint16_t OctreeNode::getSize() const {
        // BD_CORE_TRACE("SIZES: {}", m_octreeSize / pow(2, getDepth() - 1));
        return m_octreeSize / pow(2, getDepth() - 1);
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

    std::uint8_t OctreeNode::getLOD() const {
        return m_lod;
    }

    void OctreeNode::setLOD(std::uint8_t lod) {
        m_lod = lod;
    }

}