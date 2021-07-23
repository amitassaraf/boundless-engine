#pragma once
#ifndef BD_OCTREE_HPP_
#define BD_OCTREE_HPP_

#include <cstdint>
#include "octree_node.hpp"
#include "core/core.hpp"
#include <unordered_map>
#include <glm/vec3.hpp>

namespace Boundless {

    class Octree {
    public:
        Octree(uint16_t octreeSize);

        OctreeNode getParentNode(OctreeNode &node);

        OctreeNode getRootNode();

        bool isLeaf(OctreeNode &node) const;

        OctreeNode findIntersectingNode(const glm::vec3 &position);

        bool collapseNode(OctreeNode &node);

        bool divideNode(OctreeNode &node, const glm::vec3 &referenceOffset,
                        const std::function<int(const glm::vec3 &chunkOffset, uint16_t nodeSize)>& lambda);


        void visitAll(OctreeNode &node, std::function<void(uint64_t nodeLocationalCode, OctreeNode &node)> lambda);

        void visitAllConditional(OctreeNode &node,
                                 std::function<bool(uint64_t nodeLocationalCode, OctreeNode &node)> lambda);

        void
        visitAllBottomUp(OctreeNode &node, std::function<void(uint64_t nodeLocationalCode, OctreeNode &node)> lambda);

        void visitAllBy3DDistance(OctreeNode &node, uint16_t maxDistance,
                                  std::function<void(uint64_t nodeLocationalCode, OctreeNode &node)> lambda);

        uint8_t calculateFaceMask(uint64_t locationalCode);

        bool nodeExists(uint64_t locationalCode) const;

        OctreeNode getNodeAt(uint64_t locCode);

        void setNodeAt(uint64_t locCode, uint8_t voxel);

        void erase(uint64_t);

        std::unordered_map<uint64_t, uint8_t> m_nodes;
        uint16_t m_size;
    };

}

#endif // !BD_OCTREE_HPP_
