#pragma once
#ifndef BD_OPENGL_CONTEXT_HPP_
#define BD_OPENGL_CONTEXT_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "render/graphics_context.hpp"
#include "core/events/window_resize_event.hpp"

namespace Boundless {

    class OpenGLContext : public GraphicsContext {
        public:
            OpenGLContext(GLFWwindow* window, EventManager& eventManager);
            virtual ~OpenGLContext();

            virtual void init() override;
            virtual void swapBuffers() override;
            virtual void updateViewport(int width, int height) override;
        private:
            GLFWwindow* m_windowHandle;
    };

}

#endif // !BD_OPENGL_CONTEXT_HPP_
