#pragma once
#ifndef BD_GAME_CLOSE_EVENT_HPP_
#define BD_GAME_CLOSE_EVENT_HPP_

#include "../event.hpp"

namespace Boundless {

    class GameCloseEvent : public Event {
        public:
            GameCloseEvent() {};
            ~GameCloseEvent() {};
            
            std::string toString() const override {
                return "GameCloseEvent";
            };

            EVENT_CATEGORY(EventCategory::GAME_EVENTS)
            EVENT_TYPE(EventType::GAME_CLOSED)
    };

}


#endif // !BD_GAME_CLOSE_EVENT_HPP_
