#pragma once
#ifndef BD_LAYER_HPP_
#define BD_LAYER_HPP_

#include <string>

namespace Boundless {

    class Layer {
        public:
            Layer(const std::string& name = "Layer");
            virtual ~Layer();

            virtual void onAttach() {};
            virtual void onDetach() {};
            virtual void onUpdate() {};
            virtual void onEvent() {};

            inline const std::string& getName() const { return m_layerName; }
        protected:
            const std::string& m_layerName;
    }

}


#endif // !BD_LAYER_HPP_
