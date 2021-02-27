#pragma once
#ifndef BD_LAYER_HPP_
#define BD_LAYER_HPP_

#include <string>
#include "event_manager.hpp"

namespace Boundless {

    class Layer {
        public:
            Layer(EventManager& eventManager, const std::string& name = "Layer");
            virtual ~Layer();

            virtual void onAttach() {};
            virtual void onDetach() {};
            virtual void onUpdate() {};
            virtual void onEvent() {};

            inline const std::string& getName() const { return m_layerName; }
        protected:
            std::string m_layerName;
            EventManager& m_eventManager;
    };

}


#endif // !BD_LAYER_HPP_
