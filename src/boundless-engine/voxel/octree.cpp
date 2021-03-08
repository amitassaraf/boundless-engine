#include "voxel/octree_node.hpp"
#include "voxel/octree.hpp"
#include "core/core.hpp"
#include "logging/logger.hpp"
#include <memory>
#include <bitset>

namespace Boundless {
    Octree::Octree(uint32_t octreeSize) {
        m_nodes[1] = Ref<OctreeNode>(new OctreeNode(1, octreeSize)); // Root Node
    }

    Ref<OctreeNode>& Octree::getRootNode() {
        return m_nodes[1];
    }

    Ref<OctreeNode>& Octree::getParentNode(Ref<OctreeNode>& node) {
        const uint32_t locCodeParent = node->getLocationalCode()>>3;
        return lookupNode(locCodeParent);
    }

    Ref<OctreeNode>& Octree::lookupNode(uint32_t locationalCode) {
        return m_nodes.at(locationalCode);
    }

    void Octree::divide(Ref<OctreeNode>& node) {
        uint32_t newSize = node->getSize() / 2u;
        for (int i=0; i<8; i++)
        {
            node->setChildrenMask(node->getChildrenMask() | (1<<i));
            uint32_t childLocationalCode = (node->getLocationalCode() << 3) | i;
            m_nodes[childLocationalCode] = Ref<OctreeNode>(new OctreeNode(childLocationalCode, newSize));
        }
    }

    void Octree::visitAll(Ref<OctreeNode>& node, std::function< void(uint32_t nodeLocationalCode, Ref<OctreeNode>& node) > lambda) {
        for (int i=0; i<8; i++) {
            if (node->getChildrenMask()&(1<<i)) {
                const uint32_t locCodeChild = (node->getLocationalCode()<<3)|i;
                Ref<OctreeNode>& child = lookupNode(locCodeChild);
                lambda(locCodeChild, child);
                visitAll(child, lambda);
            }
        }
    }

}