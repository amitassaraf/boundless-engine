#pragma once
#ifndef BD_CHUNK_HPP_
#define BD_CHUNK_HPP_

#include <cstdint>
#include "octree.hpp"
#include <glm/vec3.hpp> // glm::vec3

namespace Boundless {

    class Chunk {
        public:

            const glm::vec3& getLocation() const;
            void setLocation(glm::vec3& newLocation);

        private:
            Octree m_Octree;
            glm::vec3 m_location;

    };

}

#endif // !BD_CHUNK_HPP_
