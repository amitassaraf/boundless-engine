#pragma once
#ifndef BD_LAYER_STACK_HPP_
#define BD_LAYER_STACK_HPP_

#include <vector>

namespace Boundless {

    class LayerStack {
        public:
            LayerStack();
            ~LayerStack();

            virtual void onAttach() {};
            virtual void onDetach() {};
            virtual void onUpdate() {};
            virtual void onEvent() {};

            inline const std::string& getName() const { return m_layerName; }
        protected:
            const std::string& m_layerName;
    }

}


#endif // !BD_LAYER_STACK_HPP_
