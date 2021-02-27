#include "core/layer.hpp"
#include "logging/logger.hpp"

namespace Boundless {

    Layer::Layer(EventManager& eventManager, const std::string& name) : m_layerName(name), m_eventManager(eventManager) {}
    Layer::~Layer() {}

}