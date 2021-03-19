#include "voxel/world.hpp"
#include "core/core.hpp"
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include <bitset>
#include <glm/gtx/string_cast.hpp>
#include <algorithm> 

float scale     = 30.f;
float lacunarity    = 0.1f;
float persistance   = 1.2f;

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
        m_size = 1024u;
        m_octree.reset(new Octree(m_size));
    }

    World::~World() {
    }

    int World::shouldDivide(const glm::vec3& chunkOffset, uint16_t nodeSize) {
        int above = 1;

        int step = 1;
        
        if (nodeSize > 64) {
            step = round(nodeSize / 10);    
        }
        
        for (int x = chunkOffset.x; x < chunkOffset.x + nodeSize; x+=step) {
            for (int z = chunkOffset.z; z < chunkOffset.z + nodeSize; z+=step) {
                float normalized;
                uint32_t loc = (x << 16u) | z;
                if (m_samples.count(loc) > 0) {
                    normalized = m_samples[loc];
                } else {
                    normalized = floor(normalize(m_noise.fractal(octaves, x, z)) * m_size);
                    m_samples[loc] = normalized;
                }
                if (nodeSize > 1.0f && normalized >= chunkOffset.y && normalized < chunkOffset.y + nodeSize) {
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
        BD_CORE_INFO("Generating world...");

        renderWorldAround(glm::vec3(0,0,0));
    }

    void World::renderWorldAround(glm::vec3 playerPosition) {
        std::vector<uint64_t > chunkChanges;
        std::vector<uint64_t > nodesToCalculateFaces;
        glm::vec3 camera = playerPosition;

        BD_CORE_INFO("Calculating LODs...");
        m_octree->visitAllConditional(m_octree->getRootNode(), [&](uint64_t nodeLocationalCode, Boundless::Ref<Boundless::OctreeNode>& node) {
            glm::vec3 chunkLocation = node->getChunkOffset();
            glm::vec3 chunkCenter(chunkLocation.x + (node->getSize() / 2.0f), chunkLocation.y + (node->getSize() / 2.0f), chunkLocation.z + (node->getSize() / 2.0f));
            auto distance = abs(glm::length(camera - chunkCenter));

            if (distance < (node->getSize() * 100)) {
                if (node->getSize() > 1 && node->isLeaf()) {
                    bool divided = changeLOD(node, DIVIDE);
                    if (divided) {
                        for (int i=0; i<8; i++) {
                            const uint64_t locCodeChild = (node->getLocationalCode()<<3)|i;
                            nodesToCalculateFaces.push_back(locCodeChild);
                        }
                    }
                    nodesToCalculateFaces.push_back(nodeLocationalCode);
                }

                return true;
            } else if (!node->isLeaf() && node->getSize() < m_octree->m_size) {
                chunkChanges.push_back(nodeLocationalCode);
            }
            return false;
        });

        BD_CORE_INFO("Finishing up LOD calculations...");

        for (uint64_t location : chunkChanges) {
            if (m_octree->nodeExists(location)) {
                Boundless::Ref<Boundless::OctreeNode>& node = m_octree->getNodeAt(location);
                changeLOD(node, COLLAPSE);
                nodesToCalculateFaces.push_back(location);
            }
        }

        BD_CORE_INFO("Calculating faces...");

        for (uint64_t location : nodesToCalculateFaces) {
            if (m_octree->nodeExists(location)) {
                m_octree->calculateFaceMask(m_octree->getNodeAt(location));
            }
        }
        BD_CORE_INFO("Done!");
    }

    bool World::changeLOD(Ref<OctreeNode>& lodNode, uint8_t command) {
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
            return true;
        } else if (command == DIVIDE) {
            glm::vec3 offset = lodNode->getChunkOffset();
            int aboveBelowOrDivide = this->shouldDivide(offset, lodNode->getSize());
            if (aboveBelowOrDivide == 0) {
                m_octree->divide(lodNode);
                lodNode->getVoxelData().setSolid(false);
                return true;
            } else if (aboveBelowOrDivide == 1) {
                lodNode->getVoxelData().setSolid(true);
            } else if (aboveBelowOrDivide == -1) {
                lodNode->getVoxelData().setSolid(false);
            }
        }
        return false;
    }

}