#pragma once
#ifndef BD_KEY_EVENT_HPP_
#define BD_KEY_EVENT_HPP_

#include "../event.hpp"

namespace Boundless {

    class BaseKeyEvent : public Event {
        public:
            EVENT_CATEGORY(EventCategory::INPUT_EVENTS)
        protected:
            
            BaseKeyEvent(int keyCode) : m_keyCode(keyCode) {}
            
            int m_keyCode;
    };

    class KeyPressedEvent : public BaseKeyEvent {
        public:
            KeyPressedEvent(int keyCode) : BaseKeyEvent(keyCode) {};
            std::string toString() const override {
                return "KeyPressedEvent";
            };

            EVENT_TYPE(EventType::KEY_PRESSED)
    };


    class KeyReleasedEvent :public BaseKeyEvent {
        public:
            KeyReleasedEvent(int keyCode) : BaseKeyEvent(keyCode) {};
            std::string toString() const override {
                return "KeyReleasedEvent";
            };

            EVENT_TYPE(EventType::KEY_RELEASED)
    };

}


#endif // !BD_KEY_EVENT_HPP_
