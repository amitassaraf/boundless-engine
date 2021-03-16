#include "voxel/world.hpp"
#include "core/core.hpp"
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include <bitset>

float scale     = 50.f;
float lacunarity    = 1.99f;
float persistance   = 0.3f;

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
    World::World() : m_noise(SimplexNoise(0.1f/scale, 0.5f, lacunarity, persistance)) {
        m_size = 128u;
        m_octree.reset(new Octree(m_size));
    }

    World::~World() {
    }

    int World::shouldDivide(const glm::vec3& chunkOffset, uint16_t nodeSize) {
        int above = 1;
        
        for (int x = chunkOffset.x; x < chunkOffset.x + nodeSize; x++) {
            for (int z = chunkOffset.z; z < chunkOffset.z + nodeSize; z++) {
                float yValue = m_noise.fractal(octaves, x, z);
                float normalized = floor(normalize(yValue) * m_size);
                if (normalized >= chunkOffset.y && normalized < chunkOffset.y + nodeSize) {
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
        Ref<OctreeNode>& rootNode = m_octree->getRootNode();
        m_octree->divide(rootNode);
        int totalNodes = 8;

        BD_CORE_INFO("Generating world...");
        
        m_octree->visitAll(rootNode, [&](uint64_t nodeLocationalCode, Ref<OctreeNode>& node) {
            UNUSED(nodeLocationalCode);
            glm::vec3 offset = node->getChunkOffset();
            int aboveBelowOrDivide = this->shouldDivide(offset, node->getSize());
            if (aboveBelowOrDivide == 0) {
                m_octree->divide(node);
                totalNodes +=8;
                node->getVoxelData().setSolid(false);
            } else if (aboveBelowOrDivide == 1) {
                node->getVoxelData().setSolid(true);
            } else if (aboveBelowOrDivide == -1) {
                node->getVoxelData().setSolid(false);
            }
        });

        BD_CORE_INFO("Calculating face masks...");

        m_octree->visitAll(rootNode, [&](uint64_t nodeLocationalCode, Ref<OctreeNode>& node) {
            UNUSED(nodeLocationalCode);
            m_octree->calculateFaceMask(node);
        });

        BD_CORE_TRACE("TOTAL NODES: {}", totalNodes);
        BD_CORE_TRACE("TOTAL NODES MBs: {}", (totalNodes * sizeof(OctreeNode)) / 1024 / 1024);
    }

    void World::changeLOD(Ref<OctreeNode>& lodNode, uint8_t command) {
        if (command == COLLAPSE) {
            bool solid = false;
            m_octree->visitAllBottomUp(lodNode, [&](uint64_t nodeLocationalCode, Ref<OctreeNode>& node) {
                UNUSED(nodeLocationalCode);
                
                if (node->getVoxelData().isSolid()) {
                    solid = true;
                }
                m_octree->erase(nodeLocationalCode);
            });
            lodNode->getVoxelData().setSolid(solid);
            lodNode->setChildrenMask(0);
        } else if (command == DIVIDE) {
            glm::vec3 offset = lodNode->getChunkOffset();
            int aboveBelowOrDivide = this->shouldDivide(offset, lodNode->getSize());
            if (aboveBelowOrDivide == 0) {
                m_octree->divide(lodNode);
                lodNode->getVoxelData().setSolid(false);
            } else if (aboveBelowOrDivide == 1) {
                lodNode->getVoxelData().setSolid(true);
            } else if (aboveBelowOrDivide == -1) {
                lodNode->getVoxelData().setSolid(false);
            }
        }

        m_octree->calculateFaceMask(lodNode);
    }

}