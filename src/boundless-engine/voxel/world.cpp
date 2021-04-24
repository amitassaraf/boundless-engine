#include "voxel/world.hpp"
#include "core/core.hpp"
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include <bitset>
#include <glm/gtx/string_cast.hpp>
#include <algorithm> 


float scale     = 100.f;
float lacunarity    = 0.1f;
float persistance   = 1.6f;

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
        
float noise[2048][2048];

namespace Boundless {

    World::World() : m_noise(SimplexNoise(0.1f/scale, 0.5f, lacunarity, persistance)) {
        m_size = 2048u;
        m_octree.reset(new Octree(m_size));

        for (int x = 0; x < 2048; x++) {
            for (int z = 0; z < 2048; z++) {
                noise[x][z] = floor(normalize(m_noise.fractal(octaves, x, z)) * m_size);
            }     
        }
        
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
                float normalized = noise[x][z];
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
        OctreeNode rootNode = m_octree->getRootNode();
        m_octree->divide(rootNode);
        BD_CORE_INFO("Generating world...");

        renderWorldAround(glm::vec3(1024,1024,1024));
    }

    OctreeNode World::findIntersectingNode(const glm::vec3& position) {
        OctreeNode rootNode = m_octree->getRootNode();
        uint64_t closestNode = 1u;
        m_octree->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode, OctreeNode& node) {
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

        return m_octree->getNodeAt(closestNode);
    }

    void World::renderWorldAround(const glm::vec3& playerPosition) {
        glm::vec3 camera = playerPosition;

        BD_CORE_INFO("Calculating LODs...");
        OctreeNode rootNode = m_octree->getRootNode();
        m_octree->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode, OctreeNode& node) {
            UNUSED(nodeLocationalCode);

            glm::vec3 chunkLocation = node.getChunkOffset();
            uint16_t size = node.getSize();
            glm::vec3 chunkCenter(chunkLocation.x + (size / 2.0f), chunkLocation.y + (size / 2.0f), chunkLocation.z + (size / 2.0f));
            auto distance = abs(glm::length(camera - chunkCenter));

            if (distance < (size * 250)) {
                if (size > 1 && m_octree->isLeaf(node)) {
                    divideNode(node, chunkLocation);
                }

                return true;
            } else if (!m_octree->isLeaf(node) && size < m_octree->m_size) {
                collapseNode(node);
            }
            return false;
        });

        BD_CORE_INFO("Done!");
    }

    bool World::collapseNode(OctreeNode& lodNode) {
        bool solid = false;
        m_octree->visitAllBottomUp(lodNode, [&](uint64_t nodeLocationalCode, OctreeNode& node) {
            UNUSED(nodeLocationalCode);
            
            if (node.isSolid()) {
                solid = true;
            }
            m_octree->erase(nodeLocationalCode);
        });
        m_octree->setNodeAt(lodNode.getLocationalCode(), solid ? 1u : 0u);
        return true;
    }

    bool World::divideNode(OctreeNode& lodNode, const glm::vec3& referenceOffset) {
        int aboveBelowOrDivide = this->shouldDivide(referenceOffset, lodNode.getSize());
        if (aboveBelowOrDivide == 0) {
            m_octree->divide(lodNode);
            m_octree->setNodeAt(lodNode.getLocationalCode(), 0u);
            return true;
        } else if (aboveBelowOrDivide == 1) {
            m_octree->setNodeAt(lodNode.getLocationalCode(), 1u);
        } else if (aboveBelowOrDivide == -1) {
            m_octree->setNodeAt(lodNode.getLocationalCode(), 0u);
        }
        return false;
    }

}