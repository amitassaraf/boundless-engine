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
        OctreeNode rootNode = m_octree->getRootNode();
        glm::vec3 chunkLocation = rootNode.getChunkOffset() + glm::vec3(m_location.x, 0, m_location.y);
        m_octree->divideNode(rootNode, chunkLocation, shouldSubdivide);
    }

    void Tile::updateLOD(const glm::vec3 &lodCenter,
                         const std::function<int(const glm::vec3 &, uint16_t)> &shouldSubdivide) {
        glm::vec3 camera = lodCenter;

        BD_CORE_TRACE("Updating LOD of {}, {}", m_location.x, m_location.y);

        OctreeNode rootNode = m_octree->getRootNode();
        m_octree->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode, OctreeNode &node) {
            UNUSED(nodeLocationalCode);

            glm::vec3 chunkLocation = node.getChunkOffset() + glm::vec3(m_location.x, 0, m_location.y);
            uint16_t size = node.getSize();
            glm::vec3 chunkCenter(chunkLocation.x + (size / 2.0f), chunkLocation.y + (size / 2.0f),
                                  chunkLocation.z + (size / 2.0f));
            auto distance = abs(glm::length(camera - chunkCenter));

            if (distance < size * 400) {
                if (size > 1 && m_octree->isLeaf(node)) {
                    m_octree->divideNode(node, chunkLocation, shouldSubdivide);
                }

                return true;
            } else if (!m_octree->isLeaf(node) && size < TILE_SIZE) {
                m_octree->collapseNode(node);
            }
            return false;
        });

        BD_CORE_TRACE("Done {}, {}", m_location.x, m_location.y);

    }

}