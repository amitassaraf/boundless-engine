 #include "platform/opengl/opengl_context.hpp"
 #include "layers/window_layer.hpp"
 #include "core/events/pop_layer_event.hpp"
 #include "core/events/window_closed_event.hpp"
 #include "core/events/game_close_event.hpp"
 #include "core/events/key_event.hpp"
 #include "logging/logger.hpp"
 #include <memory>

void error_callback(int error, const char* description)
{
    BD_CORE_ERROR("Error {} {}", error, description);
}


namespace Boundless {

    EventManager* WindowLayer::s_eventManager = NULL;

    WindowLayer::WindowLayer(EventManager& eventManager) : Layer(eventManager, "WindowLayer"), 
    m_width(800), m_height(640), m_title(std::move("Boundless")) {
        WindowLayer::s_eventManager = &eventManager;
    }

    WindowLayer::~WindowLayer() {
        delete m_context;
    }

    void WindowLayer::resizeCallback(GLFWwindow* window, int width, int height) {
        UNUSED(window);
        WindowLayer::s_eventManager->enqueue(EventType::WINDOW_RESIZE, std::shared_ptr<Event>(new WindowResizeEvent(width, height)));
    }  

    void WindowLayer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        UNUSED(window);
        UNUSED(scancode);
        UNUSED(mods);
        if (action == GLFW_PRESS) {
            WindowLayer::s_eventManager->enqueue(EventType::KEY_PRESSED, std::shared_ptr<Event>(new KeyPressedEvent(key)));
        } else if (GLFW_RELEASE) {
            WindowLayer::s_eventManager->enqueue(EventType::KEY_RELEASED, std::shared_ptr<Event>(new KeyReleasedEvent(key)));
        }
    }

    void WindowLayer::onAttach() {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit()) {
            BD_CORE_ERROR("Couldn't initialize glfw");
            throw std::runtime_error("Failed initializing GLFW.");
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        BD_CORE_TRACE("Creating window.");
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
        if (!m_window)
        {
            BD_CORE_ERROR("Window creations failed");
            throw std::runtime_error("Failed initializing window.");
        }

        glfwSetFramebufferSizeCallback(m_window, WindowLayer::resizeCallback);  
        glfwSetKeyCallback(m_window, WindowLayer::keyCallback);

        m_context = new OpenGLContext(m_window, m_eventManager);
        m_context->init();

        m_eventManager.enqueue(EventType::WINDOW_RESIZE, std::shared_ptr<Event>(new WindowResizeEvent(m_width, m_height)));
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
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            m_context->swapBuffers();
            glfwPollEvents();
        } else {
            // Send event to detach window
            m_eventManager.enqueue(EventType::POP_LAYER, std::shared_ptr<Event>(new PopLayerEvent(this)));
            m_eventManager.enqueue(EventType::GAME_CLOSED, std::shared_ptr<Event>(new GameCloseEvent()));
        }
    }

}
