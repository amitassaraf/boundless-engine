#pragma once
#ifndef BD_VOXEL_HPP_
#define BD_VOXEL_HPP_

#include <cstdint>
#include <cstddef>
#include <glm/vec3.hpp> // glm::vec3

namespace Boundless {

    class Voxel {

        public:
            
            glm::vec3 getChunkLocalLocation() const;

        private:
            uint32_t m_localLocation;
            // std::byte m_lighting;

    
    };

}

#endif // !BD_VOXEL_HPP_
