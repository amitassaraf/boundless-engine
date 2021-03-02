 #include "layers/window_layer.hpp"
 #include "core/events/pop_layer_event.hpp"
 #include "core/events/window_closed_event.hpp"
 #include "core/events/game_close_event.hpp"
 #include "logging/logger.hpp"
 #include <memory>

void error_callback(int error, const char* description)
{
    BD_CORE_ERROR("Error {} {}", error, description);
}

namespace Boundless {

    WindowLayer::WindowLayer(EventManager& eventManager) : Layer(eventManager, "WindowLayer"), 
    m_width(800), m_height(640), m_title(std::move("Boundless")) {

    }

    WindowLayer::~WindowLayer() {
    }

    void WindowLayer::onAttach() {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit()) {
            BD_CORE_ERROR("Couldn't initialize glfw");
            throw std::runtime_error("Failed initializing GLFW.");
        }
        
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        BD_CORE_TRACE("Creating window.");
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
        if (!m_window)
        {
            BD_CORE_ERROR("Window creations failed");
            throw std::runtime_error("Failed initializing window.");
        }

        BD_CORE_TRACE("Setting context.");
        glfwMakeContextCurrent(m_window);

        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            BD_CORE_ERROR("{}", reinterpret_cast<const char*>(glewGetErrorString(err)));
            glfwTerminate();
            throw std::runtime_error("Failed initializing GLEW.");
        }
    }

    void WindowLayer::onDetach() {
        BD_CORE_TRACE("Destoying window.");
        glfwDestroyWindow(m_window);
        // GLFW does not know to close the window until another poll events call is made.
        glfwPollEvents();
        glfwTerminate();
    }

    void WindowLayer::onUpdate() {
        if (!glfwWindowShouldClose(m_window)) {

            glfwSwapBuffers(m_window);
            glfwPollEvents();
        } else {
            // Send event to detach window
            m_eventManager.enqueue(EventType::POP_LAYER, std::shared_ptr<Event>(new PopLayerEvent(this)));
            m_eventManager.enqueue(EventType::GAME_CLOSED, std::shared_ptr<Event>(new GameCloseEvent()));
        }
    }

}
