#pragma once
#ifndef BD_EVENT_HPP_
#define BD_EVENT_HPP_

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
        INPUT_EVENTS = BIT(5),
    };


    class Event {
        public:
            Event() {};
            virtual ~Event();

            virtual EventType getEventType() const = 0;
            virtual int getCategoryFlags() const = 0;

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

/* Define macros to use as definitions for static types */
#define EVENT_TYPE(type) inline static EventType getEventTypeStatic()  { return type; }\
                         inline virtual EventType getEventType() const override { return getEventTypeStatic(); }

#define EVENT_CATEGORY(category) inline virtual int getCategoryFlags() const override { return category; }

#endif // !BD_EVENT_HPP_
