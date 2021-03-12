#pragma once
#ifndef BD_OCTREE_HPP_
#define BD_OCTREE_HPP_

#include <cstdint>
#include "octree_node.hpp"
#include "core/core.hpp"
#include <unordered_map>

namespace Boundless {

    class Octree {
        public:
            Octree(uint16_t octreeSize);

            Scope<OctreeNode>& getParentNode(Scope<OctreeNode>& node);
            Scope<OctreeNode>& getRootNode();
            void divide(Scope<OctreeNode>&);
            void visitAll(Scope<OctreeNode>& node, std::function< void(uint64_t nodeLocationalCode, Scope<OctreeNode>& node) > lambda);
            void visitAllBottomUp(Scope<OctreeNode>& node, std::function< void(uint64_t nodeLocationalCode, Scope<OctreeNode>& node) > lambda);
            void calculateFaceMask(Scope<OctreeNode>& node);
            bool nodeExists(uint64_t locationalCode) const;
            Scope<OctreeNode>& getNodeAt(uint64_t locCode);
            void erase(uint64_t);

        private:
            std::unordered_map<uint64_t, Scope<OctreeNode> > m_nodes;
    };

}

#endif // !BD_OCTREE_HPP_
