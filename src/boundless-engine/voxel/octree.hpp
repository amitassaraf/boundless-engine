#pragma once
#ifndef BD_OCTREE_HPP_
#define BD_OCTREE_HPP_

#include <cstdint>
#include "octree_node.hpp"
#include <unordered_map>

namespace Boundless {

    class Octree {
        public:
            Octree(uint32_t octreeSize);

            OctreeNode* getParentNode(OctreeNode* node);
            OctreeNode* getRootNode();
            void divide(OctreeNode*);
            void visitAll(OctreeNode *node, std::function< void(uint32_t nodeLocationalCode, OctreeNode* node) > lambda);
    
        private:
            OctreeNode* lookupNode(uint32_t locCode);

            std::unordered_map<uint32_t, OctreeNode*> m_nodes;
    };

}

#endif // !BD_OCTREE_HPP_
