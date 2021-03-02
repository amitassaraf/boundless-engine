#pragma once
#ifndef BD_LAYER_STACK_HPP_
#define BD_LAYER_STACK_HPP_

#include <vector>
#include <memory>
#include "layer.hpp"

namespace Boundless {

    class LayerStack {
        public:
            LayerStack();
            ~LayerStack();

            void pushLayer(Layer* layer);
            void popLayer(Layer* layer);
            void pushOverlay(Layer* layer);
            void popOverlay(Layer* layer);

            std::vector< Layer* >::iterator begin() { return m_layers.begin(); }
            std::vector< Layer* >::iterator end() { return m_layers.end(); }
        protected:
            std::vector< Layer* > m_layers;
            std::vector< Layer* >::iterator m_layersInsert;
    };
}


#endif // !BD_LAYER_STACK_HPP_
