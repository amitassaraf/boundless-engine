#pragma once
#ifndef BD_OCTREE_NODE_HPP_
#define BD_OCTREE_NODE_HPP_

#include <cstdint>
#include <cstddef>

namespace Boundless {

    class OctreeNode {
        public:
            std::size_t getDepth();
            std::uint64_t m_locationalCode;
            std::uint8_t m_childrenMask;
    };

}

#endif // !BD_OCTREE_NODE_HPP_
