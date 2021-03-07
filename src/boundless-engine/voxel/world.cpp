#include "voxel/world.hpp"
#include "core/core.hpp"
#include <SimplexNoise.h>
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include <bitset>

float scale     = 100.f;
float lacunarity    = 1.99f;
float persistance   = 0.1f;

SimplexNoise noise(0.1f/scale, 0.5f, lacunarity, persistance);
const int octaves = static_cast<int>(3 + std::log(scale)); // Estimate number of octaves needed for the current scale

const float min = -1.0f;
const float max = 1.0f;
float normalize(float input)
{
    float average      = (min + max) / 2.0f;
    float range        = (max - min) / 2.0f;
    float normalized_x = (input - average) / range;
    return (normalized_x + 1.0f) / 2.0f;
}

        
namespace Boundless {
    World::World() {
        uint32_t size = 128u;
        m_octree = new Octree(size);
    }

    World::~World() {
        delete m_octree;
    }

    int shouldDivide(glm::vec3 chunkOffset, uint32_t nodeSize) {
        int above = 1;
        
        for (int x = chunkOffset.x; x < chunkOffset.x + nodeSize; x++) {
            for (int z = chunkOffset.z; z < chunkOffset.z + nodeSize; z++) {
                float yValue = noise.fractal(octaves, x, z);
                BD_CORE_TRACE("T: {}", yValue);
                float normalized = floor(normalize(yValue) * 128.0f);
                BD_CORE_TRACE("TN: {}", normalized);
                if (nodeSize > 1 && normalized >= chunkOffset.y && normalized < chunkOffset.y + nodeSize) {
                    return 0;
                } else if (normalized >= chunkOffset.y + nodeSize) {
                    above = 1;
                } else if (normalized < chunkOffset.y) {
                    above = -1;
                }
            }
        }
        return above;
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
            int aboveBelowOrDivide = shouldDivide(offset, node->m_nodeSize);
            if (aboveBelowOrDivide == 0) {
                BD_CORE_TRACE("Dividing node node..");
                m_octree->divide(node);
                totalNodes +=8;
            } else if (aboveBelowOrDivide == 1) {
                node->m_solid = true;
            } else if (aboveBelowOrDivide == -1) {
                node->m_solid = false;
            }
        });

        BD_CORE_TRACE("TOTAL NODES: {}", totalNodes);
    }

}