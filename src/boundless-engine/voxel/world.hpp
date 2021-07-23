//
// Created by Amit Assaraf on 17/07/2021.
//

#pragma once
#ifndef BD_WORLD_HPP_
#define BD_WORLD_HPP_

#include "tile.hpp"
#include <SimplexNoise.h>

#define WORLD_SIZE 16

namespace Boundless {

    class World {
    public:
        World();
        ~World();

        void update(const glm::vec3 &lodCenter);
        static int shouldDivide(const glm::vec3 &chunkOffset, uint16_t nodeSize);

        inline const std::vector<Ref<Tile> >& getTiles() const { return m_tiles; }

    private:
        std::vector<Ref<Tile> > m_tiles;
        SimplexNoise m_noise;

    };

}


#endif //BD_WORLD_HPP_
