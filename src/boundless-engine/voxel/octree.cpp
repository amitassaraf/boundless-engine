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
        return getNodeAt(locCodeParent);
    }

    bool Octree::nodeExists(uint32_t locationalCode) const { 
        if (m_nodes.find(locationalCode) == m_nodes.end()) {
            return false; 
        }
        return true; 
    } 

    Ref<OctreeNode>& Octree::getNodeAt(uint32_t locationalCode) {
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

    uint32_t calculateSibling(uint32_t startLocation, uint8_t mask, uint8_t whileThis, bool backwardIsOr) {
        uint32_t target = startLocation;
        uint32_t path = 0u;
        uint8_t count = 0u;
        uint8_t latest = 0u;
        while ((target & mask) == whileThis && target > 1u) {
            latest = target & 7u;
            if (backwardIsOr) {
                path = path | ((latest | mask) << (3u * count));
            } else {
                path = path | ((latest ^ mask) << (3u * count));
            }
            target = target >> 3u;
            count += 1u;
        }
        if (target == 1u) {
            target = target << 3u;
            target = target | latest;
        }

        if (backwardIsOr) {
            target = target ^ mask;
        } else {
            target = target | mask;
        }
        target = target << (3u * count);
        target = target | path;
        return target;
    }

    bool checkIfSiblingIsSolid(Octree* octree, uint32_t siblingLocationalCode, uint32_t faceBitsTestMask, uint32_t testMaskExpectedResult) {
        while (siblingLocationalCode > 1u) {
            if (octree->nodeExists(siblingLocationalCode)) {
                Ref<OctreeNode>& sibling = octree->getNodeAt(siblingLocationalCode);
                if (sibling->getLocationalCode() != 1 && !sibling->isLeaf()) {
                    // Find its smaller children that might be obscuring our view
                    bool solidFlag = true;
                    octree->visitAll(sibling, [&](uint32_t nodeLocationalCode, Ref<OctreeNode>& node) {
                        if (!node->isLeaf())
                            return;
                        uint8_t depth = node->getDepth() - sibling->getDepth();
                        uint32_t hyperLocalCode = ((nodeLocationalCode >> (3u * depth)) << (3u * depth)) ^ nodeLocationalCode;
                        
                        if ((hyperLocalCode & faceBitsTestMask) == testMaskExpectedResult) {
                            if (!node->getVoxelData().isSolid() && nodeLocationalCode != 1) {
                                solidFlag = false;
                            }
                        }
                    });
                    if (!solidFlag) {
                        return false;
                    }
                } else if (!sibling->getVoxelData().isSolid() && sibling->getLocationalCode() != 1) {
                    return false;
                }

                return true;
            }
            siblingLocationalCode = siblingLocationalCode >> 3u;
        }

        // Couldn't find surface
        return true;
    }

    void Octree::calculateFaceMask(Ref<OctreeNode>& node) {
        node->setFaceMask(0);
        if (!node->getVoxelData().isSolid() || !node->isLeaf()) {
            return;
        }

        // First we need to get the locations of all our neighbours
        uint32_t left = 0;
        uint32_t right = 0;
        uint32_t back = 0;
        uint32_t front = 0;
        uint32_t north = 0;
        uint32_t south = 0;
        if ((node->getLocationalCode() & 4u) == 4u) { // Are we a top node
            north = calculateSibling(node->getLocationalCode(), 4u, 4u, false);
            
            south = node->getLocationalCode() ^ 4u;
        } else { // We are a bottom node
            south = calculateSibling(node->getLocationalCode(), 4u, 0u, true);
            
            north = node->getLocationalCode() | 4u;
        }
        // Get left and right
        if ((node->getLocationalCode() & 1u) == 1u) { // Are we a right node?
            right = calculateSibling(node->getLocationalCode(), 1u, 1u, false);

            left = node->getLocationalCode() ^ 1u;
        } else { // We are a right node
            left = calculateSibling(node->getLocationalCode(), 1u, 0u, true);

            right = node->getLocationalCode() | 1u;
        }

        // Get back and front
        if ((node->getLocationalCode() & 2u) == 2u) { // Are we a back node?
            back = calculateSibling(node->getLocationalCode(), 2u, 2u, false);

            front = node->getLocationalCode() ^ 2u;
        } else { // We are a front node
            front = calculateSibling(node->getLocationalCode(), 2u, 0u, true);

            back = node->getLocationalCode() | 2u;
        }


        if (!checkIfSiblingIsSolid(this, north, TOP_BOTTOM_FACE_BITS_TEST, 0u)) {
            node->setFaceMask(node->getFaceMask() | FACE_TOP);
        }

        if (!checkIfSiblingIsSolid(this, south, TOP_BOTTOM_FACE_BITS_TEST, 1u)) {
            node->setFaceMask(node->getFaceMask() | FACE_BOTTOM);
        }

        if (!checkIfSiblingIsSolid(this, front, FRONT_BACK_FACE_BITS_TEST, 1u)) {
            node->setFaceMask(node->getFaceMask() | FACE_FRONT);
        }

        if (!checkIfSiblingIsSolid(this, back, FRONT_BACK_FACE_BITS_TEST, 0u)) {
            node->setFaceMask(node->getFaceMask() | FACE_BACK);
        }

        if (!checkIfSiblingIsSolid(this, left, LEFT_RIGHT_FACE_BITS_TEST, 1u)) {
            node->setFaceMask(node->getFaceMask() | FACE_LEFT);
        }

        if (!checkIfSiblingIsSolid(this, right, LEFT_RIGHT_FACE_BITS_TEST, 0u)) {
            node->setFaceMask(node->getFaceMask() | FACE_RIGHT);
        }
    }

    void Octree::visitAll(Ref<OctreeNode>& node, std::function< void(uint32_t nodeLocationalCode, Ref<OctreeNode>& node) > lambda) {
        for (int i=0; i<8; i++) {
            if (node->getChildrenMask()&(1<<i)) {
                const uint32_t locCodeChild = (node->getLocationalCode()<<3)|i;
                Ref<OctreeNode>& child = getNodeAt(locCodeChild);
                lambda(locCodeChild, child);
                visitAll(child, lambda);
            }
        }
    }

}