#include "voxel/world.hpp"
#include "core/core.hpp"
#include <SimplexNoise.h>
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include <bitset>

namespace Boundless {
    World::World() {
        uint32_t size = 128u;
        m_octree = new Octree(size);
    }

    World::~World() {
        delete m_octree;
    }

    bool shouldDivide(SimplexNoise& noise, glm::vec3 chunkOffset, uint32_t nodeSize) {
        if (nodeSize <= 1) {
            return false;
        } 
        
        for (int x = chunkOffset.x; x < chunkOffset.x + nodeSize; x++) {
            for (int z = chunkOffset.z; z < chunkOffset.z + nodeSize; z++) {
                float yValue = noise.noise(x, z) + 1;
                float normalized = (512/2) * ((yValue - 2) + 2);
                normalized = floor(normalized);
                if (normalized > chunkOffset.y && normalized < chunkOffset.y + nodeSize) {
                    return true;
                }
            }
        }
        return false;
    }

    void World::generateWorld() {
        BD_CORE_INFO("Generating world.");
        SimplexNoise noise;
        
        OctreeNode* rootNode = m_octree->getRootNode();
        m_octree->divide(rootNode);
        int totalNodes = 8;
        
        m_octree->visitAll(rootNode, [&](uint32_t nodeLocationalCode, OctreeNode* node) {
            UNUSED(nodeLocationalCode);
            BD_CORE_TRACE("Visting node.. {}", std::bitset<32>(nodeLocationalCode).to_string());
            glm::vec3 offset = node->getChunkOffset();
            BD_CORE_TRACE("Node offset: X:{} Z:{} Y:{}", offset.x, offset.z, offset.y);
            if (shouldDivide(noise, offset, node->m_nodeSize)) {
                BD_CORE_TRACE("Dividing node node..");
                m_octree->divide(node);
                totalNodes +=8;
            }
        });

        BD_CORE_TRACE("TOTAL NODES: {}", totalNodes);
    }

}