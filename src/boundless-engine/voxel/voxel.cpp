#include "voxel/voxel.hpp"

namespace Boundless {

    glm::vec3 Voxel::getChunkLocalLocation() const {
        return glm::vec3(m_localLocation & 0x7fu, (m_localLocation >> 7u) & 0x7f, (m_localLocation >> 14u) & 0x7fu);
    }

}