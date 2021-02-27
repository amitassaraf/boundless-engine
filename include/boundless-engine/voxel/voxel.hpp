#pragma once
#ifndef BD_VOXEL_HPP_
#define BD_VOXEL_HPP_

#include <cstdint>

namespace Boundless {

    class Voxel {
        private:
            uint32_t m_location;
            uint8_t m_lighting;

    };

}

#endif // !BD_VOXEL_HPP_
