#pragma once
#ifndef BD_OCTREE_NODE_HPP_
#define BD_OCTREE_NODE_HPP_

#include <cstdint>
#include <cstddef>
#include "voxel.hpp"

namespace Boundless {

    class OctreeNode {
        public:
            Voxel voxel;
            std::size_t getDepth();
            std::uint32_t m_locationalCode;
            std::uint8_t m_childrenMask;
    };

}

#endif // !BD_OCTREE_NODE_HPP_
