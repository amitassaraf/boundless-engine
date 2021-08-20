//
// Created by Amit Assaraf on 17/07/2021.
//

#pragma once
#ifndef BD_WORLD_HPP_
#define BD_WORLD_HPP_

#include "core/core.hpp"
#include "tile.hpp"
#include <SimplexNoise.h>

namespace Boundless {

    class World {
    public:
        World();
        ~World();

        void update(const glm::vec3 &lodCenter);
        static int shouldDivide(const glm::vec3 &chunkOffset, uint16_t nodeSize, std::vector<std::vector<float> >&);

        inline const std::vector<Ref<Tile> >& getTiles() const { return m_tiles; }

    private:
        std::vector<Ref<Tile> > m_tiles;

    };

}


#endif //BD_WORLD_HPP_
