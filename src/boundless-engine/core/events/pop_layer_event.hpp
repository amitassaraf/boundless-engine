#pragma once
#ifndef BD_POP_LAYER_EVENT_HPP_
#define BD_POP_LAYER_EVENT_HPP_

#include "../event.hpp"

namespace Boundless {

    class PopLayerEvent : public Event {
        public:
            PopLayerEvent(Layer* layerToPop) : m_layerToPop(layerToPop) {};
            
            std::string toString() const override {
                return "PopLayerEvent";
            };

            inline Layer* getLayerToPop() const {
                return m_layerToPop;
            }

            EVENT_CATEGORY(EventCategory::GAME_EVENTS)
            EVENT_TYPE(EventType::POP_LAYER)
        private:
            Layer* m_layerToPop;
    };

}


#endif // !BD_POP_LAYER_EVENT_HPP_
