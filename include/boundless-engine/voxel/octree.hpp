#pragma once
#ifndef BD_OCTREE_HPP_
#define BD_OCTREE_HPP_

#include <cstdint>
#include "octree_node.hpp"
#include <unordered_map>

namespace Boundless {

    class Octree {
        public:
            OctreeNode* getParentNode(OctreeNode* node);
            void visitAll(OctreeNode *node);
    
        private:
            OctreeNode* lookupNode(uint64_t locCode);

            std::unordered_map<uint64_t, OctreeNode> m_nodes;
    };

}

#endif // !BD_OCTREE_HPP_
