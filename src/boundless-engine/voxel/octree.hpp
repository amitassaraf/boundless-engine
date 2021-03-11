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
            Octree(uint64_t octreeSize);

            Ref<OctreeNode>& getParentNode(Ref<OctreeNode>& node);
            Ref<OctreeNode>& getRootNode();
            void divide(Ref<OctreeNode>&);
            void visitAll(Ref<OctreeNode>& node, std::function< void(uint64_t nodeLocationalCode, Ref<OctreeNode>& node) > lambda);
            void visitAllBottomUp(Ref<OctreeNode>& node, std::function< void(uint64_t nodeLocationalCode, Ref<OctreeNode>& node) > lambda);
            void calculateFaceMask(Ref<OctreeNode>& node);
            bool nodeExists(uint64_t locationalCode) const;
            Ref<OctreeNode>& getNodeAt(uint64_t locCode);
            void erase(uint64_t);

        private:
            std::unordered_map<uint64_t, Ref<OctreeNode> > m_nodes;
    };

}

#endif // !BD_OCTREE_HPP_
