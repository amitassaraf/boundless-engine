#pragma once
#ifndef BD_LAYER_HPP_
#define BD_LAYER_HPP_

#include <string>
#include "core/core.hpp"

namespace Boundless {

    enum class EventType {
        NONE = 0,
        WINDOW_CLOSE, WINDOW_RESIZE, WINDOW_MOVED, WINDOW_FOCUS, WINDOW_BLUR,
        GAME_TICK, GAME_UPDATE, GAME_RENDER, 
        KEY_PRESSED, KEY_RELEASED,
        MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOUSE_MOVED, MOUSE_SCROLLED,
    };

    enum class EventCategory {
        NONE = 0,
        WINDOW_EVENTS = BIT(1),
        MOUSE_EVENTS = BIT(2),
        KEYBOARD_EVENTS = BIT(3),
        GAME_EVENTS = BIT(4),
    };

    class Event {
        public:
            Event() {};
            virtual ~Event();

            virtual EventType getType() const = 0;
            virtual EventType getCategoryFlags() const = 0;
            virtual std::string toString() const {
                return "Event";
            };

            inline bool inCategory(EventCategory category) {
                return getCategoryFlags() & category;
            }
        protected:
            bool m_handled = false;
    };

}


#endif // !BD_LAYER_HPP_
