#include "core/layer_stack.hpp"
#include <algorithm>
#include "logging/logger.hpp"


namespace Boundless {

    LayerStack::LayerStack() {
        m_layersInsert = m_layers.begin();
    }

    LayerStack::~LayerStack() {
        // for (Layer* layer : m_layers) {
        //     delete layer;
        // }
    }

    void LayerStack::pushLayer(Layer* layer) {
        m_layersInsert = m_layers.emplace(m_layersInsert, layer);
    }

    void LayerStack::pushOverlay(Layer* overlay) {
        m_layers.emplace_back(overlay);
    }

    void LayerStack::popLayer(Layer* layer) {
        auto it = std::find(m_layers.begin(), m_layers.end(), layer);
        if (it != m_layers.end()) {
            (*it)->onDetach();
            m_layers.erase(it);
            m_layersInsert--;
        }
    }

    void LayerStack::popOverlay(Layer* overlay) {
        auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
        if (it != m_layers.end()) {
            m_layers.erase(it);
        }
    }

}