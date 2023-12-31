#pragma once
#ifndef BD_WINDOW_LAYER_HPP_
#define BD_WINDOW_LAYER_HPP_

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "render/graphics_context.hpp"
#include "core/layer.hpp"

namespace Boundless {

    class WindowLayer : public Layer {
        public:
            WindowLayer(EventManager& eventManager);
            ~WindowLayer();

            void onAttach() override;
            void onDetach() override;
            void onUpdate() override;

            void updateViewport() const;
            
            static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
            static void resizeCallback(GLFWwindow* window, int width, int height);

            inline int getWidth() const { return m_width; }
            inline int getHeight() const { return m_height; }
        private:
            int m_width;
            int m_height;
            const std::string m_title;
            GLFWwindow* m_window;
            GraphicsContext* m_context;
            static EventManager* s_eventManager;
    };

}


#endif // !BD_WINDOW_LAYER_HPP_
