#include "voxel/tile.hpp"
#include "core/core.hpp"
#include <memory>
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include <glm/gtx/string_cast.hpp>
#include <cmath>
#include <ratio>

namespace Boundless {

    auto key_selector = [](auto pair){return pair.first;};

    Tile::Tile(int xOffset, int zOffset) : m_location(xOffset * TILE_SIZE, zOffset * TILE_SIZE) {
        m_octree = std::make_unique<Octree>(TILE_SIZE);
    }

    Tile::~Tile() = default;

    Ref<std::vector<uint64_t> > Tile::getRawNodes() const {
        std::vector<uint64_t> chunks(m_octree->m_nodes.size());
        transform(m_octree->m_nodes.begin(), m_octree->m_nodes.end(), chunks.begin(), key_selector);

        std::sort(chunks.begin(), chunks.end());
        return std::make_shared<std::vector<uint64_t> >(chunks);
    }

    void Tile::initialize(const std::function<int(const glm::vec3 &, uint16_t)> &shouldSubdivide) {
        uint64_t rootNode = m_octree->getRootNode();
        glm::vec3 chunkLocation = OctreeNode::getChunkOffset(rootNode, m_octree->m_size) + glm::vec3(m_location.x, 0, m_location.y);
        uint16_t size = OctreeNode::getSize(rootNode, m_octree->m_size);
        m_octree->divideNode(rootNode, chunkLocation, size, shouldSubdivide);
    }

    void Tile::updateLOD(const glm::vec3 &lodCenter,
                         const std::function<int(const glm::vec3 &, uint16_t)> &shouldSubdivide) {
        glm::vec3 camera = lodCenter;

        BD_CORE_TRACE("Updating LOD of {}, {}", m_location.x, m_location.y);
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        uint64_t rootNode = m_octree->getRootNode();
        m_octree->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode) {
            UNUSED(nodeLocationalCode);

            glm::vec3 chunkLocation = OctreeNode::getChunkOffset(nodeLocationalCode, m_octree->m_size) + glm::vec3(m_location.x, 0, m_location.y);
            uint16_t size = OctreeNode::getSize(nodeLocationalCode, m_octree->m_size);
            glm::vec3 chunkCenter(chunkLocation.x + (size / 2.0f), chunkLocation.y + (size / 2.0f),
                                  chunkLocation.z + (size / 2.0f));
            auto distance = abs(glm::length(camera - chunkCenter));

            if (distance < size * 600) {
                if (size > 1 && m_octree->isLeaf(nodeLocationalCode)) {
                    m_octree->divideNode(nodeLocationalCode, chunkLocation, size, shouldSubdivide);
                }

                return true;
            } else if (!m_octree->isLeaf(nodeLocationalCode) && size < TILE_SIZE) {
                m_octree->collapseNode(nodeLocationalCode);
            }
            return false;
        });

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> time_span = t2 - t1;
        BD_CORE_TRACE("It took: {} milliseconds.", time_span.count());

        BD_CORE_TRACE("Done {}, {}", m_location.x, m_location.y);

    }

}