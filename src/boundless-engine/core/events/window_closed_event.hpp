#pragma once
#ifndef BD_WINDOW_CLOSED_EVENT_HPP_
#define BD_WINDOW_CLOSED_EVENT_HPP_

#include "../event.hpp"

namespace Boundless {

    class WindowClosedEvent : public Event {
        public:
            WindowClosedEvent() {};
            ~WindowClosedEvent() {};
            
            std::string toString() const override {
                return "WindowClosedEvent";
            };

            EVENT_CATEGORY(EventCategory::WINDOW_EVENTS)
            EVENT_TYPE(EventType::WINDOW_CLOSE)
    };

}


#endif // !BD_WINDOW_CLOSED_EVENT_HPP_
