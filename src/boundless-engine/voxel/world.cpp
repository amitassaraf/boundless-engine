#include "voxel/world.hpp"
#include <SimplexNoise.h>

namespace Boundless {

    bool shouldDivide(SimplexNoise& noise, glm::vec3 chunkOffset) {

    }

    void generateWorld() {
        SimplexNoise noise;
        Octree octree;



        for (int x = 0; x < 8000; x++) {
            for (int z = 0; z < 8000; z++) {
                int y = noise.noise(x, z);

            }
        }
        
    }

}