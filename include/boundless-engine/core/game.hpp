#pragma once
#ifndef BD_GAME_HPP_
#define BD_GAME_HPP_

#include "core/layer_stack.hpp"
#include "event_manager.hpp"

namespace Boundless {

    /**
     * A class for the game
     */
    class Game {

        public:

            Game();
            virtual ~Game();

            int run();
            
            void pushLayer(Layer* layer);
            void popLayer(Layer* layer);
            void pushOverlay(Layer* layer);
            void popOverlay(Layer* layer);

        protected:
            std::unique_ptr<LayerStack> m_layerStack;
            EventManager m_eventManager;
            bool m_gameClosed = false;
        
    };

}

#endif // !BD_GAME_HPP_
