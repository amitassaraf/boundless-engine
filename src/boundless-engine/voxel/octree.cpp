#include "voxel/octree_node.hpp"
#include "voxel/octree.hpp"
#include "core/core.hpp"
#include "logging/logger.hpp"
#include <memory>
#include <bitset>

namespace Boundless {
    Octree::Octree(uint16_t octreeSize) {
        m_size = octreeSize;
        OctreeNode::m_octreeSize = octreeSize;
        m_nodes[1] = 1; // Root Node
    }

    OctreeNode Octree::getRootNode() {
        return OctreeNode(1u, m_nodes.at(1u));
    }

    void Octree::erase(uint64_t locationalCode) {
        m_nodes.erase(locationalCode);
    }

    OctreeNode Octree::getParentNode(OctreeNode &node) {
        const uint64_t locCodeParent = node.getLocationalCode() >> 3;
        return getNodeAt(locCodeParent);
    }

    bool Octree::nodeExists(uint64_t locationalCode) const {
        return m_nodes.count(locationalCode) > 0;
    }

    OctreeNode Octree::getNodeAt(uint64_t locationalCode) {
        return OctreeNode(locationalCode, m_nodes.at(locationalCode));
    }

    void Octree::setNodeAt(uint64_t locCode, uint8_t voxel) {
        m_nodes[locCode] = voxel;
    }

    bool Octree::collapseNode(OctreeNode &lodNode) {
        bool solid = false;
        this->visitAllBottomUp(lodNode, [&](uint64_t nodeLocationalCode, OctreeNode &node) {
            UNUSED(nodeLocationalCode);

            if (node.isSolid()) {
                solid = true;
            }
            this->erase(nodeLocationalCode);
        });
        this->setNodeAt(lodNode.getLocationalCode(), solid ? 1u : 0u);
        return true;
    }

    bool Octree::divideNode(OctreeNode &lodNode, const glm::vec3 &referenceOffset,
                            const std::function<int(const glm::vec3 &, uint16_t)>& shouldDivide) {
        int aboveBelowOrDivide = shouldDivide(referenceOffset, lodNode.getSize());
        if (aboveBelowOrDivide == 0) {

            for (int i = 0; i < 8; i++) {
                uint64_t childLocationalCode = (lodNode.getLocationalCode() << 3) | i;
                m_nodes[childLocationalCode] = 1u;
            }

            this->setNodeAt(lodNode.getLocationalCode(), 0u);
            return true;
        } else if (aboveBelowOrDivide == 1) {
            this->setNodeAt(lodNode.getLocationalCode(), 1u);
        } else if (aboveBelowOrDivide == -1) {
            this->setNodeAt(lodNode.getLocationalCode(), 0u);
        }
        return false;
    }

    OctreeNode Octree::findIntersectingNode(const glm::vec3 &position) {
        OctreeNode rootNode = this->getRootNode();
        uint64_t closestNode = 1u;
        this->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode, OctreeNode &node) {
            glm::vec3 chunkLocation = node.getChunkOffset();
            uint16_t size = node.getSize();

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

        return this->getNodeAt(closestNode);
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
                OctreeNode sibling = octree->getNodeAt(siblingLocationalCode);
                if (!octree->isLeaf(sibling)) {
                    // Find its smaller children that might hiding our face
                    bool solidFlag = true;
                    octree->visitAll(sibling, [&](uint64_t nodeLocationalCode, OctreeNode &node) {
                        if (!isDirection(nodeLocationalCode, direction) || !octree->isLeaf(node) ||
                            node.getSize() > nodeSize)
                            return;

                        uint8_t depth = node.getDepth() - sibling.getDepth();
                        uint64_t hyperLocalCode =
                                ((nodeLocationalCode >> (3u * depth)) << (3u * depth)) ^ nodeLocationalCode;
                        uint64_t hyperFaceBitsTestMask = faceBitsTestMask;
                        for (uint8_t i = 0u; i < depth - 1u; i++) {
                            hyperFaceBitsTestMask = (hyperFaceBitsTestMask << 3u) | faceBitsTestMask;
                        }

                        if (expectingZeroResult) {
                            if ((hyperLocalCode & hyperFaceBitsTestMask) == hyperFaceBitsTestMask) {
                                if (!node.isSolid()) {
                                    solidFlag = false;
                                }
                            }
                        } else {
                            if ((hyperLocalCode & hyperFaceBitsTestMask) == 0) {
                                if (!node.isSolid()) {
                                    solidFlag = false;
                                }
                            }
                        }

                    });
                    if (!solidFlag) {
                        return false;
                    }
                } else if (!sibling.isSolid()) {
                    return false;
                }

                return true;
            }
            siblingLocationalCode = siblingLocationalCode >> 3u;
        }

        // Couldn't find surface
        return true;
    }

    bool Octree::isLeaf(OctreeNode &node) const {
        return !nodeExists(node.getLocationalCode() << 3u);
    }


    uint8_t Octree::calculateFaceMask(uint64_t locationcalCode) {
        OctreeNode node(locationcalCode, m_nodes.at(locationcalCode));

        if (!node.isSolid() || !isLeaf(node)) {
            return 0;
        }

        // First we need to get the locations of all our neighbours
        uint64_t left = 0;
        uint64_t right = 0;
        uint64_t back = 0;
        uint64_t front = 0;
        uint64_t north = 0;
        uint64_t south = 0;
        if ((node.getLocationalCode() & 4u) == 4u) { // Are we a top node
            north = calculateSibling(node.getLocationalCode(), 4u, 4u, false);

            south = node.getLocationalCode() ^ 4u;
        } else { // We are a bottom node
            south = calculateSibling(node.getLocationalCode(), 4u, 0u, true);

            north = node.getLocationalCode() | 4u;
        }
        // Get left and right
        if ((node.getLocationalCode() & 1u) == 1u) { // Are we a right node?
            right = calculateSibling(node.getLocationalCode(), 1u, 1u, false);

            left = node.getLocationalCode() ^ 1u;
        } else { // We are a right node
            left = calculateSibling(node.getLocationalCode(), 1u, 0u, true);

            right = node.getLocationalCode() | 1u;
        }

        // Get back and front
        if ((node.getLocationalCode() & 2u) == 2u) { // Are we a back node?
            back = calculateSibling(node.getLocationalCode(), 2u, 2u, false);

            front = node.getLocationalCode() ^ 2u;
        } else { // We are a front node
            front = calculateSibling(node.getLocationalCode(), 2u, 0u, true);

            back = node.getLocationalCode() | 2u;
        }


        // BD_CORE_TRACE("DEPTH: {}", node.getDepth());
        // BD_CORE_TRACE("SIZE: {}", node.getSize());
        // BD_CORE_TRACE("OCTREE: {}", node.getOctreeSize());
        uint8_t faceMask = 0u;
        if (!checkIfSiblingIsSolid(this, north, node.getSize(), TOP_BOTTOM_FACE_BITS_TEST, false, FACE_TOP)) {
            faceMask |= FACE_TOP;
        }

        if (!checkIfSiblingIsSolid(this, south, node.getSize(), TOP_BOTTOM_FACE_BITS_TEST, true, FACE_BOTTOM)) {
            faceMask |= FACE_BOTTOM;
        }

        if (!checkIfSiblingIsSolid(this, front, node.getSize(), FRONT_BACK_FACE_BITS_TEST, true, FACE_FRONT)) {
            faceMask |= FACE_FRONT;
        }

        if (!checkIfSiblingIsSolid(this, back, node.getSize(), FRONT_BACK_FACE_BITS_TEST, false, FACE_BACK)) {
            faceMask |= FACE_BACK;
        }

        if (!checkIfSiblingIsSolid(this, left, node.getSize(), LEFT_RIGHT_FACE_BITS_TEST, true, FACE_LEFT)) {
            faceMask |= FACE_LEFT;
        }

        if (!checkIfSiblingIsSolid(this, right, node.getSize(), LEFT_RIGHT_FACE_BITS_TEST, false, FACE_RIGHT)) {
            faceMask |= FACE_RIGHT;
        }

        return faceMask;
    }

    void Octree::visitAll(OctreeNode &node, std::function<void(uint64_t nodeLocationalCode, OctreeNode &node)> lambda) {
        if (isLeaf(node)) {
            return;
        }

        for (int i = 0; i < 8; i++) {
            const uint64_t locCodeChild = (node.getLocationalCode() << 3) | i;
            OctreeNode child = getNodeAt(locCodeChild);
            lambda(locCodeChild, child);
            visitAll(child, lambda);
        }
    }

    void Octree::visitAllConditional(OctreeNode &node,
                                     std::function<bool(uint64_t nodeLocationalCode, OctreeNode &node)> lambda) {
        if (isLeaf(node)) {
            return;
        }

        for (int i = 0; i < 8; i++) {
            const uint64_t locCodeChild = (node.getLocationalCode() << 3) | i;
            OctreeNode child = getNodeAt(locCodeChild);
            bool visit = lambda(locCodeChild, child);
            if (visit) {
                visitAllConditional(child, lambda);
            }
        }
    }

    void Octree::visitAllBottomUp(OctreeNode &node,
                                  std::function<void(uint64_t nodeLocationalCode, OctreeNode &node)> lambda) {
        if (isLeaf(node)) {
            return;
        }

        for (int i = 0; i < 8; i++) {
            const uint64_t locCodeChild = (node.getLocationalCode() << 3) | i;
            OctreeNode child = getNodeAt(locCodeChild);
            visitAllBottomUp(child, lambda);
            lambda(locCodeChild, child);
        }
    }

    void Octree::visitAllBy3DDistance(OctreeNode &node, uint16_t maxDistance,
                                      std::function<void(uint64_t nodeLocationalCode, OctreeNode &node)> lambda) {
        if (isLeaf(node)) {
            return;
        }

        if (maxDistance <= 0) {
            return;
        }

        if (node.getDepth() > 1) {
            OctreeNode parent = getParentNode(node);
            visitAllBy3DDistance(parent, maxDistance - node.getSize(), lambda);
        }

        for (int i = 0; i < 8; i++) {
            const uint64_t locCodeChild = (node.getLocationalCode() << 3) | i;
            OctreeNode child = getNodeAt(locCodeChild);
            lambda(locCodeChild, child);
            visitAllBy3DDistance(child, maxDistance - node.getSize(), lambda);
        }
    }


}