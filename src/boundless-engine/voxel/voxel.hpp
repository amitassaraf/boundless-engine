#pragma once
#ifndef BD_VOXEL_HPP_
#define BD_VOXEL_HPP_

#include <cstdint>
#include <cstddef>
#include <glm/vec3.hpp> // glm::vec3

namespace Boundless {

    class Voxel {

        public:
            Voxel() {}
            Voxel(bool is_solid) : m_solid(is_solid) {}
            ~Voxel() {}

            inline bool isSolid() const { return m_solid; }
            inline void setSolid(bool solid) { m_solid = solid; }
        private:
            bool m_solid = false;
    };

}

#endif // !BD_VOXEL_HPP_
