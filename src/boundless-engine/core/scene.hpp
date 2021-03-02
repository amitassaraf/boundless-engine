#pragma once
#ifndef BD_SCENE_HPP_
#define BD_SCENE_HPP_

#include <entityx/entityx.h>
#include "core/core.hpp"

namespace Boundless {

    class Scene : public entityx::EntityX {
    public:
        Scene();

        void update(entityx::TimeDelta dt);

    };

}

#endif // !BD_SCENE_HPP_
