#include "core/scene.hpp"

namespace Boundless {

    Scene::Scene() {
        systems.configure();
    }

    void Scene::update(entityx::TimeDelta dt) {
        UNUSED(dt);
    }

}