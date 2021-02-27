#include "core/event_manager.hpp"


namespace Boundless {
    EventManager::EventManager() {}
    EventManager::~EventManager() {}

    void EventManager::enqueue(EventType type, const std::shared_ptr<Boundless::Event> event) {
        eventpp::EventQueue<int, void(const std::shared_ptr<Boundless::Event>)>::enqueue(enumToInt(type), event);
    }

    void EventManager::appendListener(EventType type, const Callback & callback) {
        eventpp::EventQueue<int, void(const std::shared_ptr<Boundless::Event>)>::appendListener(enumToInt(type), callback);
    }
}