#pragma once
#ifndef BD_MOUSE_EVENT_HPP_
#define BD_MOUSE_EVENT_HPP_

#include "../event.hpp"

namespace Boundless {

    class BaseMouseEvent : public Event {
        public:
            EVENT_CATEGORY(EventCategory::INPUT_EVENTS)
    };

    class MouseMovedEvent : public BaseMouseEvent {
        public:
            MouseMovedEvent(int xpos, int ypos) : m_x(xpos), m_y(ypos) {};
            
            std::string toString() const override {
                return "MouseMovedEvent";
            };

            EVENT_TYPE(EventType::MOUSE_MOVED)
            
            inline int getX() const { return m_x; }
            inline int getY() const { return m_y; }
        private:
            int m_x;
            int m_y;
    };

}


#endif // !BD_MOUSE_EVENT_HPP_
