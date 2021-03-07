#pragma once
#ifndef BD_WORLD_HPP_
#define BD_WORLD_HPP_

#include <cstdint>
#include <cstddef>
#include "voxel/octree.hpp"

namespace Boundless {

    class World {
        public:
            World();
            ~World();
            void generateWorld();
            Octree* m_octree;
    };

}

#endif // !BD_WORLD_HPP_
