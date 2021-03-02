#pragma once
#ifndef BD_ENTRYPOINT_HPP_
#define BD_ENTRYPOINT_HPP_

#include "game.hpp"
#include "logging/logger.hpp"

namespace Boundless { 

    /*
    To be defined in client
    */
    extern Game* create_boundless_game();

}

int main() {
    Boundless::Logger::init();
    BD_CORE_INFO("Boundless Engine Initialized.");
    BD_CORE_INFO("Creating game..");
    Boundless::Game* boundless_game = Boundless::create_boundless_game();
    
    int returnValue = boundless_game->run();
    delete boundless_game;

    return returnValue;
}

#endif // !BD_ENTRYPOINT_HPP_
