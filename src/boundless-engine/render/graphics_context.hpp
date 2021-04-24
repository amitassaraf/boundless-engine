#pragma once
#ifndef BD_GRAPHICS_CONTEXT_HPP_
#define BD_GRAPHICS_CONTEXT_HPP_

#include "core/event_manager.hpp"

namespace Boundless {

    class GraphicsContext {
        public:
            GraphicsContext(EventManager& eventManager);
            virtual ~GraphicsContext();
            virtual void init() = 0;
            virtual void swapBuffers() = 0;
            virtual void updateViewport(int width, int height) = 0;
        protected:
            EventManager& m_eventManager;
    };

}

#endif // !BD_GRAPHICS_CONTEXT_HPP_
