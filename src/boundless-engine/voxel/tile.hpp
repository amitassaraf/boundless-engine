#pragma once
#ifndef BD_TILE_HPP_
#define BD_TILE_HPP_

#include <cstdint>
#include <cstddef>
#include <vector>
#include "voxel/octree.hpp"
#include "core/core.hpp"
#include <glm/vec2.hpp>

#define TILE_SIZE 1024

namespace Boundless {

    class Tile {
    public:
        Tile(int xOffset, int zOffset);

        ~Tile();

        void initialize(const std::function<int(const glm::vec3 &, uint16_t)> &shouldSubdivide);
        void updateLOD(const glm::vec3 &lodCenter,
                       const std::function<int(const glm::vec3 &, uint16_t)> &shouldSubdivide);

        inline Scope<Octree> &getOctree() { return m_octree; }
        [[nodiscard]] inline glm::vec3 getTileOffset() const { return glm::vec3((float) m_location.x, 0, (float) m_location.y); }
        [[nodiscard]] Ref<std::vector<uint64_t> > getRawNodes() const;

    private:
        Scope<Octree> m_octree;
        glm::ivec2 m_location;

    };

}

#endif // !BD_TILE_HPP_
