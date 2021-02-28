#include "voxel/chunk.hpp"

namespace Boundless {

    const glm::vec3& Chunk::getLocation() const {
        return m_location;
    }

    void Chunk::setLocation(glm::vec3& newLocation) {
        m_location = newLocation;
    }

}