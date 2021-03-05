#pragma once
#ifndef BD_EVENT_MANAGER_HPP_
#define BD_EVENT_MANAGER_HPP_

#include <string>
#include "event.hpp"
#include "core.hpp"
#include <eventpp/eventqueue.h>
#include <memory>


namespace Boundless {

    struct EventQueuePolicy {
        static int getEvent(const Ref<Boundless::Event> e) {
            return enumToInt(e->getEventType());
        }
    };

    class EventManager : public eventpp::EventQueue<int, void(const Ref<Boundless::Event>)>
    {
    public:
        EventManager();
        ~EventManager();

        void enqueue(EventType type, const Ref<Boundless::Event> event);
        void appendListener(EventType type, const Callback & callback);
    };

}

#endif // !BD_EVENT_MANAGER_HPP_
