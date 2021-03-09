#include "voxel/octree_node.hpp"
#include "core/core.hpp"
#include "logging/logger.hpp"
#include <bitset>

namespace Boundless {

    OctreeNode::OctreeNode(uint32_t locationalCode, uint32_t nodeSize) 
    : m_voxel(false), m_locationalCode(locationalCode), m_nodeSize(nodeSize) {

    }

    bool OctreeNode::isLeaf() const {
        return m_childrenMask == 0;
    }

    glm::vec3 OctreeNode::getChunkOffset() const {
        uint32_t locationalCode = m_locationalCode;
        glm::vec3 chunkOffset(0, 0, 0);
        uint32_t size = m_nodeSize;

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

            locationalCode >>= 3;
            size *= 2;
        }
        return chunkOffset;
    }

    std::uint32_t OctreeNode::getSize() const {
        return m_nodeSize;
    }

    std::size_t OctreeNode::getDepth() const {
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

    Voxel& OctreeNode::getVoxelData() {
        return m_voxel;
    }

    std::uint32_t OctreeNode::getLocationalCode() const {
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

    std::uint32_t OctreeNode::getLOD() const {
        return m_lod;
    }



}