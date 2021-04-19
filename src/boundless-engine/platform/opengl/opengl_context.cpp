#include "opengl_context.hpp"
#include "logging/logger.hpp"


namespace Boundless {

    OpenGLContext::OpenGLContext(GLFWwindow* window, EventManager& eventManager) 
    : GraphicsContext(eventManager), m_windowHandle(window) {

    }

    OpenGLContext::~OpenGLContext() {}

    void OpenGLContext::init() {
        m_eventManager.appendListener(EventType::WINDOW_RESIZE, [&](const Ref<Event> event) {
            Ref<WindowResizeEvent> windowResizeEvent = std::dynamic_pointer_cast<WindowResizeEvent> (event);
            glViewport(0, 0, windowResizeEvent->getWidth(), windowResizeEvent->getHeight());
        });

        BD_CORE_TRACE("Setting context.");
        glfwMakeContextCurrent(m_windowHandle);
        // glfwSwapInterval( 0 );

        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            BD_CORE_ERROR("{}", reinterpret_cast<const char*>(glewGetErrorString(err)));
            throw std::runtime_error("Failed initializing GLEW.");
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

        glfwWindowHint(GLFW_SAMPLES, 4);
        glEnable(GL_MULTISAMPLE);  

    }

    void OpenGLContext::swapBuffers() {
        glfwSwapBuffers(m_windowHandle);
    }

}