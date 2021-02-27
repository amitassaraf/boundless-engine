#include "voxel/octree_node.hpp"

namespace Boundless {

    std::size_t OctreeNode::getDepth() {
        if (m_locationalCode == 0) {
            throw std::runtime_error("No depth.");
        }

    #if defined(__GNUC__)
        return (63-__builtin_clz(m_locationalCode))/3;
    #elif defined(_MSC_VER)
        long msb;
        _BitScanReverse(&msb, m_locationalCode);
        return msb/3;
    #endif
    }

    

}