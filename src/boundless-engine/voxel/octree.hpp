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
        explicit Octree(uint16_t octreeSize);

        uint64_t getParentNode(uint64_t node);

        uint64_t getRootNode();

        bool isLeaf(uint64_t node) const;
        bool isSolid(uint64_t node) const;

        uint64_t findIntersectingNode(const glm::vec3 &position);

        bool collapseNode(uint64_t node);

        bool divideNode(uint64_t node, const glm::vec3 &referenceOffset,
                        const std::function<int(const glm::vec3 &chunkOffset, uint16_t nodeSize)>& lambda);


        void visitAll(uint64_t node, std::function<void(uint64_t nodeLocationalCode)> lambda);

        void visitAllConditional(uint64_t node,
                                 std::function<bool(uint64_t nodeLocationalCode)> lambda);

        void
        visitAllBottomUp(uint64_t node, std::function<void(uint64_t nodeLocationalCode)> lambda);

        void visitAllBy3DDistance(uint64_t node, uint16_t maxDistance,
                                  std::function<void(uint64_t nodeLocationalCode)> lambda);

        uint8_t calculateFaceMask(uint64_t locationalCode);

        bool nodeExists(uint64_t locationalCode) const;

        uint8_t getNodeAt(uint64_t locCode) const;

        void setNodeAt(uint64_t locCode, uint8_t voxel);

        void erase(uint64_t);

        std::unordered_map<uint64_t, uint8_t> m_nodes;
        uint16_t m_size;
    };

}

#endif // !BD_OCTREE_HPP_
