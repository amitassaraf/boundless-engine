#include "voxel/world.hpp"
#include "core/core.hpp"
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include <bitset>

float scale     = 100.f;
float lacunarity    = 1.99f;
float persistance   = 0.1f;

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
        m_size = 256u;
        m_octree = new Octree(m_size);
    }

    World::~World() {
        delete m_octree;
    }

    int World::shouldDivide(const glm::vec3& chunkOffset, uint32_t nodeSize, uint32_t lod) {
        int above = 1;
        
        for (int x = chunkOffset.x; x < chunkOffset.x + nodeSize; x++) {
            for (int z = chunkOffset.z; z < chunkOffset.z + nodeSize; z++) {
                float yValue = m_noise.fractal(octaves, x, z);
                float normalized = floor(normalize(yValue) * m_size);
                if (nodeSize > lod && normalized >= chunkOffset.y && normalized < chunkOffset.y + nodeSize) {
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

        BD_CORE_INFO("Generating world.");
        
        m_octree->visitAll(rootNode, [&](uint32_t nodeLocationalCode, Ref<OctreeNode>& node) {
            UNUSED(nodeLocationalCode);
            glm::vec3 offset = node->getChunkOffset();
            int aboveBelowOrDivide = this->shouldDivide(offset, node->getSize(), node->getLOD());
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

        m_octree->visitAll(rootNode, [&](uint32_t nodeLocationalCode, Ref<OctreeNode>& node) {
            UNUSED(nodeLocationalCode);
            m_octree->calculateFaceMask(node);
        });

        BD_CORE_TRACE("TOTAL NODES: {}", totalNodes);
    }

    void World::changeLOD(Ref<OctreeNode>& lodNode, uint32_t lod) {
        uint32_t currentLod = lodNode->getLOD();
        if (currentLod < lod) {
            lodNode->setLOD(lod);
            m_octree->visitAllBottomUp(lodNode, [&](uint32_t nodeLocationalCode, Ref<OctreeNode>& node) {
                UNUSED(nodeLocationalCode);
                
                if (node->getSize() < lod) {
                    Ref<OctreeNode>& parent = m_octree->getParentNode(node);
                    parent->setChildrenMask(parent->getChildrenMask() ^ (nodeLocationalCode & 7u));
                    if (!parent->getVoxelData().isSolid()) {
                        parent->getVoxelData().setSolid(node->getVoxelData().isSolid());
                    }
                    m_octree->erase(nodeLocationalCode);
                }
            });
        } else if (lod < currentLod) {
            lodNode->setLOD(lod);
            m_octree->visitAllBottomUp(lodNode, [&](uint32_t nodeLocationalCode, Ref<OctreeNode>& node) {
                UNUSED(nodeLocationalCode);
                
                if (node->isLeaf() && node->getSize() > lod) {
                    glm::vec3 offset = node->getChunkOffset();
                    int aboveBelowOrDivide = this->shouldDivide(offset, node->getSize(), node->getLOD());
                    if (aboveBelowOrDivide == 0) {
                        m_octree->divide(node);
                        node->getVoxelData().setSolid(false);
                    } else if (aboveBelowOrDivide == 1) {
                        node->getVoxelData().setSolid(true);
                    } else if (aboveBelowOrDivide == -1) {
                        node->getVoxelData().setSolid(false);
                    }
                    node->getVoxelData().setSolid(false);
                }
            });
        }
        
        // m_octree->calculateFaceMask(node);
    }

}