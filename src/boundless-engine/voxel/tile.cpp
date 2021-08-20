#include "voxel/tile.hpp"
#include "core/core.hpp"
#include <memory>
#include <utility>
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include <glm/gtx/string_cast.hpp>
#include <cmath>
#include <ratio>


namespace Boundless {

    float normalize(float min, float max, float input) {
        float average = (min + max) / 2.0f;
        float range = (max - min) / 2.0f;
        float normalized_x = (input - average) / range;
        return (normalized_x + 1.0f) / 2.0f;
    }

    auto key_selector = [](auto pair) { return pair.first; };

    Tile::Tile(float xOffset, float yOffset, float zOffset, const std::string &noiseFunction) : m_location(
            xOffset * TILE_SIZE, yOffset * TILE_SIZE, zOffset * TILE_SIZE), m_noiseGenerator(
            FastNoise::NewFromEncodedNodeTree(noiseFunction.c_str())) {
        m_octree = std::make_unique<Octree>(TILE_SIZE);

    }

    Tile::~Tile() = default;

    Ref<std::vector<uint64_t> > Tile::getRawNodes() const {
        std::vector<uint64_t> chunks(m_octree->m_nodes.size());
        transform(m_octree->m_nodes.begin(), m_octree->m_nodes.end(), chunks.begin(), key_selector);

        std::sort(chunks.begin(), chunks.end());
        return std::make_shared<std::vector<uint64_t> >(chunks);
    }

    void Tile::initialize(
            const std::function<int(const glm::vec3 &, uint16_t,
                                    std::vector<std::vector<float> > &)> &shouldSubdivide) {
        std::vector<std::vector<float> > noise = this->prepareNoise();

        uint64_t rootNode = m_octree->getRootNode();
        glm::vec3 chunkLocation = OctreeNode::getChunkOffset(rootNode, m_octree->m_size);
        uint16_t size = OctreeNode::getSize(rootNode, m_octree->m_size);
        m_octree->divideNode(rootNode, chunkLocation, size, shouldSubdivide, noise);
    }

    std::vector<std::vector<float> > Tile::prepareNoise() const {
        // Create an array of floats to store the noise output in
        std::vector<std::vector<float> > noise(TILE_SIZE, std::vector<float>(TILE_SIZE, 0));

        std::vector<float> noiseOutput(TILE_SIZE * TILE_SIZE);

        m_noiseGenerator->GenUniformGrid2D(noiseOutput.data(), (int) m_location.x,
                                           (int) m_location.z, TILE_SIZE, TILE_SIZE,
                                           0.003f, 1337);

        int index = 0;
        for (int z = 0; z < TILE_SIZE; z++) {
            for (int x = 0; x < TILE_SIZE; x++) {
                noise[x][z] = floor(normalize(-1, 1, noiseOutput[index++]) * (TILE_SIZE * WORLD_HEIGHT) - m_location.y);
            }
        }

        return noise;
    }

    void Tile::updateLOD(const glm::vec3 &lodCenter,
                         const std::function<int(const glm::vec3 &, uint16_t,
                                                 std::vector<std::vector<float> > &)> &shouldSubdivide) {
        glm::vec3 camera = lodCenter;
        std::vector<std::vector<float> > noise = this->prepareNoise();

        BD_CORE_TRACE("Updating LOD of {}, {}", m_location.x, m_location.z);
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        uint64_t rootNode = m_octree->getRootNode();
        m_octree->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode) {
            glm::vec3 chunkLocation = OctreeNode::getChunkOffset(nodeLocationalCode, m_octree->m_size) + m_location;
            uint16_t size = OctreeNode::getSize(nodeLocationalCode, m_octree->m_size);
            glm::vec3 chunkCenter(chunkLocation.x + (size / 2.0f), chunkLocation.y + (size / 2.0f),
                                  chunkLocation.z + (size / 2.0f));
            auto distance = abs(glm::length(camera - chunkCenter));

            if (distance < size * LOD_DISTANCE) {
                if (size > 1 && m_octree->isLeaf(nodeLocationalCode)) {
                    return m_octree->divideNode(nodeLocationalCode, chunkLocation - m_location, size, shouldSubdivide,
                                                noise);
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

        BD_CORE_TRACE("Done {}, {}", m_location.x, m_location.z);
    }

}