#include "voxel/octree_node.hpp"
#include "voxel/octree.hpp"
#include "core/core.hpp"
#include "logging/logger.hpp"
#include <memory>
#include <bitset>

namespace Boundless {
    Octree::Octree(uint16_t octreeSize) {
        m_size = octreeSize;
        m_nodes[1] = Ref<OctreeNode>(new OctreeNode(1, octreeSize)); // Root Node
    }

    Ref<OctreeNode>& Octree::getRootNode() {
        return m_nodes.at(1u);
    }

    void Octree::erase(uint64_t locationalCode) {
        m_nodes.erase(locationalCode);
    }

    Ref<OctreeNode>& Octree::getParentNode(Ref<OctreeNode>& node) {
        const uint64_t locCodeParent = node->getLocationalCode()>>3;
        return getNodeAt(locCodeParent);
    }

    bool Octree::nodeExists(uint64_t locationalCode) const { 
        return m_nodes.count(locationalCode) > 0;
    } 

    Ref<OctreeNode>& Octree::getNodeAt(uint64_t locationalCode) {
        return m_nodes.at(locationalCode);
    }

    void Octree::divide(Ref<OctreeNode>& node) {
        for (int i=0; i<8; i++) {
            uint64_t childLocationalCode = (node->getLocationalCode() << 3) | i;
            m_nodes[childLocationalCode].reset(new OctreeNode(childLocationalCode, m_size));
        }
    }

    uint64_t calculateSibling(uint64_t startLocation, uint8_t mask, uint8_t whileThis, bool backwardIsOr) {
        uint64_t target = startLocation;
        uint64_t path = 0u;
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
            return 1u;
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

    bool checkIfSiblingIsSolid(Octree* octree, uint64_t siblingLocationalCode, uint16_t nodeSize, uint64_t faceBitsTestMask, bool expectingZeroResult) {
        while (siblingLocationalCode > 1u) {
            if (octree->nodeExists(siblingLocationalCode)) {
                Ref<OctreeNode>& sibling = octree->getNodeAt(siblingLocationalCode);
                if (!octree->isLeaf(sibling)) {
                    // Find its smaller children that might hiding our face
                    bool solidFlag = true;
                    octree->visitAll(sibling, [&](uint64_t nodeLocationalCode, Ref<OctreeNode>& node) {
                        if (!octree->isLeaf(node) || node->getSize() > nodeSize)
                            return;
                        
                        uint8_t depth = node->getDepth() - sibling->getDepth();
                        uint64_t hyperLocalCode = ((nodeLocationalCode >> (3u * depth)) << (3u * depth)) ^ nodeLocationalCode;
                        uint64_t hyperFaceBitsTestMask = faceBitsTestMask;
                        for (uint8_t i = 0u; i < depth - 1u; i++) {
                            hyperFaceBitsTestMask = (hyperFaceBitsTestMask << 3u) | faceBitsTestMask;
                        }
                        
                        if (expectingZeroResult) {
                            if ((hyperLocalCode & hyperFaceBitsTestMask) == hyperFaceBitsTestMask) {
                                if (!node->getVoxelData().isSolid()) {
                                    solidFlag = false;
                                }
                            }
                        } else {
                            if ((hyperLocalCode & hyperFaceBitsTestMask) == 0) {
                                if (!node->getVoxelData().isSolid()) {
                                    solidFlag = false;
                                }
                            }
                        }
                        
                    });
                    if (!solidFlag) {
                        return false;
                    }
                } else if (!sibling->getVoxelData().isSolid()) {
                    return false;
                }

                return true;
            }
            siblingLocationalCode = siblingLocationalCode >> 3u;
        }

        // Couldn't find surface
        return true;
    }

    bool Octree::isLeaf(Ref<OctreeNode>& node) const {
        return !nodeExists(node->getLocationalCode() << 3u);
    }


    uint8_t Octree::calculateFaceMask(Ref<OctreeNode>& node) {
        if (!node->getVoxelData().isSolid() || !isLeaf(node)) {
            return 0;
        }

        // First we need to get the locations of all our neighbours
        uint64_t left = 0;
        uint64_t right = 0;
        uint64_t back = 0;
        uint64_t front = 0;
        uint64_t north = 0;
        uint64_t south = 0;
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


        // BD_CORE_TRACE("DEPTH: {}", node->getDepth());
        // BD_CORE_TRACE("SIZE: {}", node->getSize());
        // BD_CORE_TRACE("OCTREE: {}", node->getOctreeSize());
        uint8_t faceMask = 0u;
        if (!checkIfSiblingIsSolid(this, north, node->getSize(), TOP_BOTTOM_FACE_BITS_TEST, false)) {
            faceMask |= FACE_TOP;
        }

        if (!checkIfSiblingIsSolid(this, south, node->getSize(), TOP_BOTTOM_FACE_BITS_TEST, true)) {
            faceMask |= FACE_BOTTOM;
        }

        if (!checkIfSiblingIsSolid(this, front, node->getSize(), FRONT_BACK_FACE_BITS_TEST, true)) {
            faceMask |= FACE_FRONT;
        }

        if (!checkIfSiblingIsSolid(this, back, node->getSize(), FRONT_BACK_FACE_BITS_TEST, false)) {
            faceMask |= FACE_BACK;
        }

        if (!checkIfSiblingIsSolid(this, left, node->getSize(), LEFT_RIGHT_FACE_BITS_TEST, true)) {
            faceMask |= FACE_LEFT;
        }

        if (!checkIfSiblingIsSolid(this, right, node->getSize(), LEFT_RIGHT_FACE_BITS_TEST, false)) {
            faceMask |= FACE_RIGHT;
        }

        return faceMask;
    }

    void Octree::visitAll(Ref<OctreeNode>& node, std::function< void(uint64_t nodeLocationalCode, Ref<OctreeNode>& node) > lambda) {
        if (isLeaf(node)) {
            return;
        }

        for (int i=0; i<8; i++) {
            const uint64_t locCodeChild = (node->getLocationalCode()<<3)|i;
            Ref<OctreeNode>& child = getNodeAt(locCodeChild);
            lambda(locCodeChild, child);
            visitAll(child, lambda);
        }
    }

    void Octree::visitAllConditional(Ref<OctreeNode>& node, std::function< bool(uint64_t nodeLocationalCode, Ref<OctreeNode>& node) > lambda) {
        if (isLeaf(node)) {
            return;
        }

        for (int i=0; i<8; i++) {
            const uint64_t locCodeChild = (node->getLocationalCode()<<3)|i;
            Ref<OctreeNode>& child = getNodeAt(locCodeChild);
            bool visit = lambda(locCodeChild, child);
            if (visit) {
                visitAllConditional(child, lambda);
            }
        }
    }

    void Octree::visitAllBottomUp(Ref<OctreeNode>& node, std::function< void(uint64_t nodeLocationalCode, Ref<OctreeNode>& node) > lambda) {
        if (isLeaf(node)) {
            return;
        }

        for (int i=0; i<8; i++) {
            const uint64_t locCodeChild = (node->getLocationalCode()<<3)|i;
            Ref<OctreeNode>& child = getNodeAt(locCodeChild);
            visitAllBottomUp(child, lambda);
            lambda(locCodeChild, child);
        }
    }

}