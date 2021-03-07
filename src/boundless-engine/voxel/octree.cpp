#include "voxel/octree_node.hpp"
#include "voxel/octree.hpp"
#include "logging/logger.hpp"
#include <bitset>

namespace Boundless {
    Octree::Octree(uint32_t octreeSize) {
        m_nodes[1] = new OctreeNode(1, octreeSize); // Root Node
    }

    OctreeNode* Octree::getRootNode() {
        return m_nodes[1];
    }

    OctreeNode* Octree::getParentNode(OctreeNode* node) {
        const uint32_t locCodeParent = node->m_locationalCode>>3;
        return lookupNode(locCodeParent);
    }

    OctreeNode* Octree::lookupNode(uint32_t locationalCode) {
        return m_nodes[locationalCode];
    }

    void Octree::divide(OctreeNode* node) {
        uint32_t newSize = node->m_nodeSize / 2u;
        for (int i=0; i<8; i++)
        {
            node->m_childrenMask = node->m_childrenMask | (1<<i);
            uint32_t childLocationalCode = (node->m_locationalCode << 3) | i;
            auto* child = new OctreeNode(childLocationalCode, newSize); 
            m_nodes[childLocationalCode] = child;
        }
    }

    void Octree::visitAll(OctreeNode* node, std::function< void(uint32_t nodeLocationalCode, OctreeNode* node) > lambda) {
        for (int i=0; i<8; i++)
        {
            if (node->m_childrenMask&(1<<i))
            {
                const uint32_t locCodeChild = (node->m_locationalCode<<3)|i;
                auto* child = lookupNode(locCodeChild);
                if (!!child) {
                    lambda(locCodeChild, child);
                    visitAll(child, lambda);
                }
            }
        }
    }

}