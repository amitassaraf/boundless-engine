#include "voxel/octree_node.hpp"
#include "voxel/octree.hpp"
#include "core/core.hpp"
#include <memory>
#include <bitset>

namespace Boundless {
    Octree::Octree(uint16_t octreeSize) {
        m_size = octreeSize;
        m_nodes[1] = 1; // Root Node
    }

    uint64_t Octree::getRootNode() {
        return 1u;
    }

    void Octree::erase(uint64_t locationalCode) {
        m_nodes.erase(locationalCode);
    }

    uint64_t Octree::getParentNode(uint64_t node) {
        return node >> 3;
    }

    bool Octree::nodeExists(uint64_t locationalCode) const {
        return m_nodes.count(locationalCode) > 0;
    }

    uint8_t Octree::getNodeAt(uint64_t locationalCode) const {
        return m_nodes.at(locationalCode);
    }

    void Octree::setNodeAt(uint64_t locCode, uint8_t voxel) {
        m_nodes[locCode] = voxel;
    }

    bool Octree::collapseNode(uint64_t lodNode) {
        bool solid = false;
        this->visitAllBottomUp(lodNode, [&](uint64_t nodeLocationalCode) {
            if (this->isSolid(nodeLocationalCode)) {
                solid = true;
            }
            this->erase(nodeLocationalCode);
        });
        this->setNodeAt(lodNode, solid ? 1u : 0u);
        return true;
    }

    bool Octree::divideNode(uint64_t lodNode, const glm::vec3 &referenceOffset,
                            const std::uint16_t &referenceSize,
                            const std::function<int(const glm::vec3 &, uint16_t, std::vector<std::vector<float> >&)>& shouldDivide,
                            std::vector<std::vector<float> >& noise) {
        int aboveBelowOrDivide = shouldDivide(referenceOffset, referenceSize, noise);
        if (aboveBelowOrDivide == 0) {

            for (int i = 0; i < 8; i++) {
                uint64_t childLocationalCode = (lodNode << 3) | i;
                m_nodes[childLocationalCode] = 0u;
            }

            this->setNodeAt(lodNode, 0u);
            return true;
        } else if (aboveBelowOrDivide == 1) {
            this->setNodeAt(lodNode, 1u);
        } else if (aboveBelowOrDivide == -1) {
            this->setNodeAt(lodNode, 0u);
        }
        return false;
    }

    uint64_t Octree::findIntersectingNode(const glm::vec3 &position) {
        uint64_t rootNode = this->getRootNode();
        uint64_t closestNode = 1u;
        this->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode) {
            glm::vec3 chunkLocation = OctreeNode::getChunkOffset(nodeLocationalCode, m_size);
            uint16_t size = OctreeNode::getSize(nodeLocationalCode, m_size);

            if (chunkLocation.x >= position.x && chunkLocation.x + size < position.x) {
                if (chunkLocation.z >= position.z && chunkLocation.z + size < position.z) {
                    if (chunkLocation.y >= position.y && chunkLocation.y + size < position.y) {
                        closestNode = nodeLocationalCode;
                        return true;
                    }
                }
            }

            return false;
        });

        return closestNode;
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

    bool isDirection(uint8_t localCode, uint8_t direction) {
        switch (direction) {
            case FACE_TOP:
                return (localCode & 4u) == 0u;
            case FACE_BOTTOM:
                return (localCode & 4u) == 4u;
            case FACE_FRONT:
                return (localCode & 2u) == 2u;
            case FACE_BACK:
                return (localCode & 2u) == 0u;
            case FACE_LEFT:
                return (localCode & 1u) == 1u;
            case FACE_RIGHT:
                return (localCode & 1u) == 0u;
        }
        return false;
    }

    bool
    checkIfSiblingIsSolid(Octree *octree, uint64_t siblingLocationalCode, uint16_t nodeSize, uint64_t faceBitsTestMask,
                          bool expectingZeroResult, uint8_t direction) {
        while (siblingLocationalCode > 1u) {
            if (octree->nodeExists(siblingLocationalCode)) {
                uint64_t sibling = siblingLocationalCode;
                if (!octree->isLeaf(sibling)) {
                    // Find its smaller children that might hiding our face
                    bool solidFlag = true;
                    octree->visitAll(sibling, [&](uint64_t nodeLocationalCode) {
                        if (!isDirection(nodeLocationalCode, direction) || !octree->isLeaf(nodeLocationalCode) ||
                            OctreeNode::getSize(nodeLocationalCode, octree->m_size) > nodeSize)
                            return;

                        uint8_t depth = OctreeNode::getDepth(nodeLocationalCode) - OctreeNode::getDepth(sibling);
                        uint64_t hyperLocalCode =
                                ((nodeLocationalCode >> (3u * depth)) << (3u * depth)) ^ nodeLocationalCode;
                        uint64_t hyperFaceBitsTestMask = faceBitsTestMask;
                        for (uint8_t i = 0u; i < depth - 1u; i++) {
                            hyperFaceBitsTestMask = (hyperFaceBitsTestMask << 3u) | faceBitsTestMask;
                        }

                        if (expectingZeroResult) {
                            if ((hyperLocalCode & hyperFaceBitsTestMask) == hyperFaceBitsTestMask) {
                                if (!octree->isSolid(nodeLocationalCode)) {
                                    solidFlag = false;
                                }
                            }
                        } else {
                            if ((hyperLocalCode & hyperFaceBitsTestMask) == 0) {
                                if (!octree->isSolid(nodeLocationalCode)) {
                                    solidFlag = false;
                                }
                            }
                        }

                    });
                    if (!solidFlag) {
                        return false;
                    }
                } else if (!octree->isSolid(sibling)) {
                    return false;
                }

                return true;
            }
            siblingLocationalCode = siblingLocationalCode >> 3u;
        }

        // Couldn't find surface
        return true;
    }

    bool Octree::isLeaf(uint64_t node) const {
        return !nodeExists(node << 3u);
    }

    bool Octree::isSolid(uint64_t node) const {
        return this->getNodeAt(node) != 0u;
    }


    uint8_t Octree::calculateFaceMask(uint64_t locationalCode) {
        if (!this->isSolid(locationalCode) || !this->isLeaf(locationalCode)) {
            return 0;
        }

        // First we need to get the locations of all our neighbours
        uint64_t left = 0;
        uint64_t right = 0;
        uint64_t back = 0;
        uint64_t front = 0;
        uint64_t north = 0;
        uint64_t south = 0;
        if ((locationalCode & 4u) == 4u) { // Are we a top node
            north = calculateSibling(locationalCode, 4u, 4u, false);

            south = locationalCode ^ 4u;
        } else { // We are a bottom node
            south = calculateSibling(locationalCode, 4u, 0u, true);

            north = locationalCode | 4u;
        }
        // Get left and right
        if ((locationalCode & 1u) == 1u) { // Are we a right node?
            right = calculateSibling(locationalCode, 1u, 1u, false);

            left = locationalCode ^ 1u;
        } else { // We are a right node
            left = calculateSibling(locationalCode, 1u, 0u, true);

            right = locationalCode | 1u;
        }

        // Get back and front
        if ((locationalCode & 2u) == 2u) { // Are we a back node?
            back = calculateSibling(locationalCode, 2u, 2u, false);

            front = locationalCode ^ 2u;
        } else { // We are a front node
            front = calculateSibling(locationalCode, 2u, 0u, true);

            back = locationalCode | 2u;
        }


        // BD_CORE_TRACE("DEPTH: {}", node.getDepth());
        // BD_CORE_TRACE("SIZE: {}", node.getSize());
        // BD_CORE_TRACE("OCTREE: {}", node.getOctreeSize());
        uint8_t faceMask = 0u;
        uint16_t size = OctreeNode::getSize(locationalCode, this->m_size);
        if (!checkIfSiblingIsSolid(this, north, size, TOP_BOTTOM_FACE_BITS_TEST, false, FACE_TOP)) {
            faceMask |= FACE_TOP;
        }

        if (!checkIfSiblingIsSolid(this, south, size, TOP_BOTTOM_FACE_BITS_TEST, true, FACE_BOTTOM)) {
            faceMask |= FACE_BOTTOM;
        }

        if (!checkIfSiblingIsSolid(this, front, size, FRONT_BACK_FACE_BITS_TEST, true, FACE_FRONT)) {
            faceMask |= FACE_FRONT;
        }

        if (!checkIfSiblingIsSolid(this, back, size, FRONT_BACK_FACE_BITS_TEST, false, FACE_BACK)) {
            faceMask |= FACE_BACK;
        }

        if (!checkIfSiblingIsSolid(this, left, size, LEFT_RIGHT_FACE_BITS_TEST, true, FACE_LEFT)) {
            faceMask |= FACE_LEFT;
        }

        if (!checkIfSiblingIsSolid(this, right, size, LEFT_RIGHT_FACE_BITS_TEST, false, FACE_RIGHT)) {
            faceMask |= FACE_RIGHT;
        }

        return faceMask;
    }

    void Octree::visitAll(uint64_t node, std::function<void(uint64_t nodeLocationalCode)> lambda) {
        if (isLeaf(node)) {
            return;
        }

        for (int i = 0; i < 8; i++) {
            const uint64_t locCodeChild = (node << 3) | i;
            lambda(locCodeChild);
            visitAll(locCodeChild, lambda);
        }
    }

    void Octree::visitAllConditional(uint64_t node,
                                     std::function<bool(uint64_t nodeLocationalCode)> lambda) {
        if (isLeaf(node)) {
            return;
        }

        for (int i = 0; i < 8; i++) {
            const uint64_t locCodeChild = (node << 3) | i;
            bool visit = lambda(locCodeChild);
            if (visit) {
                visitAllConditional(locCodeChild, lambda);
            }
        }
    }

    void Octree::visitAllBottomUp(uint64_t node,
                                  std::function<void(uint64_t nodeLocationalCode)> lambda) {
        if (isLeaf(node)) {
            return;
        }

        for (int i = 0; i < 8; i++) {
            const uint64_t locCodeChild = (node << 3) | i;
            visitAllBottomUp(locCodeChild, lambda);
            lambda(locCodeChild);
        }
    }

    void Octree::visitAllBy3DDistance(uint64_t node, uint16_t maxDistance,
                                      std::function<void(uint64_t nodeLocationalCode)> lambda) {
        if (isLeaf(node)) {
            return;
        }

        if (maxDistance <= 0) {
            return;
        }

        if (OctreeNode::getDepth(node) > 1) {
            uint64_t parent = getParentNode(node);
            visitAllBy3DDistance(parent, maxDistance - OctreeNode::getSize(node, m_size), lambda);
        }

        for (int i = 0; i < 8; i++) {
            const uint64_t locCodeChild = (node << 3) | i;
            lambda(locCodeChild);
            visitAllBy3DDistance(locCodeChild, maxDistance - OctreeNode::getSize(node, m_size), lambda);
        }
    }


}