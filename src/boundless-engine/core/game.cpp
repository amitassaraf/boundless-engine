#include "core/game.hpp"
#include "core/events/pop_layer_event.hpp"
#include "logging/logger.hpp"

#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>

// #include <AL/al.h>

// #ifdef __APPLE__ //Mac OSX has a different name for the header file
// #include <OpenCL/opencl.h>
// #else
// #include <CL/cl.h>
// #endif

namespace Boundless {

    Game::Game() {
        m_layerStack.reset(new LayerStack());
    }

    Game::~Game() {
    }

    void Game::pushLayer(Layer* layer) {
        BD_CORE_TRACE("Pushing Layer {}", layer->getName());
        m_layerStack->pushLayer(layer);
    }

    void Game::popLayer(Layer* layer) {
        BD_CORE_TRACE("Popping Layer {}", layer->getName());
        m_layerStack->popLayer(layer);
    }
    
    void Game::pushOverlay(Layer* overlay) {
        BD_CORE_TRACE("Pushing Overlay {}", overlay->getName());
        m_layerStack->pushOverlay(overlay);
    }

    void Game::popOverlay(Layer* overlay) {
        BD_CORE_TRACE("Popping Overlay {}", overlay->getName());
        m_layerStack->popOverlay(overlay);
    }

    int Game::run() {
        BD_CORE_TRACE("Game loop starting..");
        m_eventManager.appendListener(EventType::POP_LAYER, [&](const std::shared_ptr<Event> event) {
            std::shared_ptr<PopLayerEvent> popLayerEvent = std::dynamic_pointer_cast<PopLayerEvent> (event);
            m_layerStack->popLayer(popLayerEvent->getLayerToPop());
        });

        m_eventManager.appendListener(EventType::GAME_CLOSED, [&](const std::shared_ptr<Event> event) {
            BD_CORE_TRACE("Game Close Requested.");
            UNUSED(event);
            m_gameClosed = true;
        });
        
        for (Layer* layer : *m_layerStack) {
            layer->onAttach();
        }

        while (!m_gameClosed) {
            m_eventManager.process();
            for (Layer* layer : *m_layerStack) {
                layer->onUpdate();
            }
            if (m_currentScene != nullptr) {
                m_currentScene->update(0.1);
            }
        }

        BD_CORE_TRACE("Game Layers Detaching..");
        for (Layer* layer : *m_layerStack) {
            layer->onDetach();
        }

        return EXIT_SUCCESS;
    }

}