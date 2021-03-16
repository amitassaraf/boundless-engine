 #include "fps_counter_layer.hpp"

namespace Boundless {

    FPSCounterLayer::FPSCounterLayer(EventManager& eventManager) : Layer(eventManager, "FPSCounterLayer") {
        
    }

    FPSCounterLayer::~FPSCounterLayer() {
        
    }

    void FPSCounterLayer::onAttach() {
    }

    void FPSCounterLayer::onDetach() {
    }

    void FPSCounterLayer::onUpdate() {
        // Measure speed
        double currentTime = glfwGetTime();
        m_nbFrames++;
        if ( currentTime - m_lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            BD_CORE_TRACE("{} ms/frame\n", 1000.0/double(m_nbFrames));
            m_nbFrames = 0;
            m_lastTime += 1.0;
        }
    }

}
