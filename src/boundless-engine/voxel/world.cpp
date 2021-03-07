#include "voxel/world.hpp"
#include "core/core.hpp"
#include <SimplexNoise.h>
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include <bitset>

float scale     = 100.f;
float lacunarity    = 1.99f;
float persistance   = 0.5f;

SimplexNoise noise(0.1f/scale, 0.5f, lacunarity, persistance);
const int octaves = static_cast<int>(3 + std::log(scale)); // Estimate number of octaves needed for the current scale
        
namespace Boundless {
    World::World() {
        uint32_t size = 128u;
        m_octree = new Octree(size);
    }

    World::~World() {
        delete m_octree;
    }

    bool shouldDivide(glm::vec3 chunkOffset, uint32_t nodeSize) {
        if (nodeSize <= 1) {
            return false;
        } 
        
        for (int x = chunkOffset.x; x < chunkOffset.x + nodeSize; x++) {
            for (int z = chunkOffset.z; z < chunkOffset.z + nodeSize; z++) {
                float yValue = noise.fractal(octaves, x, z);
                float normalized = (128/2) * ((yValue - 2) + 2);
                normalized = floor(normalized);
                if (normalized > chunkOffset.y && normalized < chunkOffset.y + nodeSize) {
                    return true;
                }
            }
        }
        return false;
    }

    void World::generateWorld() {
        OctreeNode* rootNode = m_octree->getRootNode();
        m_octree->divide(rootNode);
        int totalNodes = 8;

        BD_CORE_INFO("Generating world.");
        
        m_octree->visitAll(rootNode, [&](uint32_t nodeLocationalCode, OctreeNode* node) {
            UNUSED(nodeLocationalCode);
            BD_CORE_TRACE("Visting node.. {}", std::bitset<32>(nodeLocationalCode).to_string());
            glm::vec3 offset = node->getChunkOffset();
            BD_CORE_TRACE("Node offset: X:{} Z:{} Y:{}", offset.x, offset.z, offset.y);
            if (shouldDivide(offset, node->m_nodeSize)) {
                BD_CORE_TRACE("Dividing node node..");
                m_octree->divide(node);
                totalNodes +=8;
            }
        });

        BD_CORE_TRACE("TOTAL NODES: {}", totalNodes);
    }

}