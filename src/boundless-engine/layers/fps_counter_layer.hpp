#pragma once
#ifndef BD_FPS_COUNT_LAYER_HPP_
#define BD_FPS_COUNT_LAYER_HPP_

#include <GLFW/glfw3.h>
#include "logging/logger.hpp"
#include "core/layer.hpp"


namespace Boundless {

    class FPSCounterLayer : public Layer {
        public:
            FPSCounterLayer(EventManager& eventManager);
            ~FPSCounterLayer();

            void onAttach() override;
            void onDetach() override;
            void onUpdate() override;
        private:
            double m_lastTime = 0;
            int m_nbFrames = 0;
    };

}


#endif // !BD_FPS_COUNT_LAYER_HPP_
