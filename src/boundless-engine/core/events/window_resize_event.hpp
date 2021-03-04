#pragma once
#ifndef BD_WINDOW_RESIZE_EVENT_HPP_
#define BD_WINDOW_RESIZE_EVENT_HPP_

#include "../event.hpp"

namespace Boundless {

    class WindowResizeEvent : public Event {
        public:
            WindowResizeEvent(int width, int height) : m_height(height), m_width(width) {};
            ~WindowResizeEvent() {};
            
            std::string toString() const override {
                return "WindowResizeEvent";
            };

            int getWidth() const {
                return m_width;
            }

            int getHeight() const {
                return m_height;
            }

            EVENT_CATEGORY(EventCategory::WINDOW_EVENTS)
            EVENT_TYPE(EventType::WINDOW_RESIZE)
        private:
            int m_height;
            int m_width;
    };

}


#endif // !BD_WINDOW_RESIZE_EVENT_HPP_
