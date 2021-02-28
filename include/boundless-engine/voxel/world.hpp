#pragma once
#ifndef BD_WORLD_HPP_
#define BD_WORLD_HPP_

#include <cstdint>
#include <cstddef>

namespace Boundless {

    class World {

        private:
            uint32_t m_localLocation;
            std::byte m_lighting;
    
    };

}

#endif // !BD_WORLD_HPP_
