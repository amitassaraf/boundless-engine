#include "voxel/octree_node.hpp"

namespace Boundless {

    OctreeNode* Octree::(OctreeNode* node) {
        const uint64_t locCodeParent = node->m_locationalCode>>3;
        return lookupNode(locCodeParent);
    }

    OctreeNode* Octree::lookupNode(uint64_t locationalCode) {
        const auto iter = m_nodes.find(locationalCode);
        return (iter == m_nodes.end() ? nullptr : &(*iter));
    }

    void Octree::visitAll(OctreeNode *node) {
        for (int i=0; i<8; i++)
        {
            if (node->m_childrenMask&(1<<i))
            {
                const uint32_t locCodeChild = (node->m_locationalCode<<3)|i;
                const auto *child = lookupNode(locCodeChild);
                visitAll(child);
            }
        }
    }

}