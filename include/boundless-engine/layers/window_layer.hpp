#pragma once
#ifndef BD_WINDOW_LAYER_HPP_
#define BD_WINDOW_LAYER_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "core/layer.hpp"

namespace Boundless {

    class WindowLayer : public Layer {
        public:
            WindowLayer(EventManager& eventManager);
            ~WindowLayer();

            void onAttach() override;
            void onDetach() override;
            void onUpdate() override;
        private:
            int m_width;
            int m_height;
            const std::string m_title;
            GLFWwindow* m_window;
    };

}


#endif // !BD_WINDOW_LAYER_HPP_
