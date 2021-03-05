#include "core/event_manager.hpp"
#include "logging/logger.hpp"


namespace Boundless {
    EventManager::EventManager() {}
    EventManager::~EventManager() {}

    void EventManager::enqueue(EventType type, const Ref<Boundless::Event> event) {
        if (!!event) {
            BD_CORE_TRACE("Queueing Event {}", event->toString());
        }
        eventpp::EventQueue<int, void(const Ref<Boundless::Event>)>::enqueue(enumToInt(type), event);
    }

    void EventManager::appendListener(EventType type, const Callback & callback) {
        BD_CORE_TRACE("Added listener to Event Type: {}", type);
        eventpp::EventQueue<int, void(const Ref<Boundless::Event>)>::appendListener(enumToInt(type), callback);
    }
}