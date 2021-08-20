#pragma once
#ifndef BD_TILE_HPP_
#define BD_TILE_HPP_

#include <cstdint>
#include <cstddef>
#include <vector>
#include "voxel/octree.hpp"
#include "core/core.hpp"
#include <glm/vec2.hpp>
#include "FastNoise/FastNoise.h"

#define LOD_DISTANCE 200

namespace Boundless {

    class Tile {
    public:
        Tile(float xOffset, float yOffset, float zOffset, const std::string& noiseFunction);

        ~Tile();

        void initialize(const std::function<int(const glm::vec3 &, uint16_t, std::vector<std::vector<float> >&)> &shouldSubdivide);
        void updateLOD(const glm::vec3 &lodCenter,
                       const std::function<int(const glm::vec3 &, uint16_t, std::vector<std::vector<float> >&)> &shouldSubdivide);

        inline Scope<Octree> &getOctree() { return m_octree; }
        [[nodiscard]] inline glm::vec3 getTileOffset() const { return m_location; }
        [[nodiscard]] Ref<std::vector<uint64_t> > getRawNodes() const;

    private:
        [[nodiscard]] std::vector<std::vector<float> > prepareNoise() const;

        Scope<Octree> m_octree;
        glm::vec3 m_location;
        FastNoise::SmartNode<> m_noiseGenerator;

    };

}

#endif // !BD_TILE_HPP_
