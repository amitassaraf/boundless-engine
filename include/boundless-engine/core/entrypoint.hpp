#pragma once
#ifndef BND_ENTRYPOINT_HPP_
#define BND_ENTRYPOINT_HPP_

#include "game.hpp"

namespace Boundless { 

    /*
    To be defined in client
    */
    extern Game* create_boundless_game();

}

int main() {
    Boundless::Game* boundless_game = Boundless::create_boundless_game();
    int returnValue = boundless_game->run();
    delete boundless_game;

    return returnValue;
}

#endif // !BND_ENTRYPOINT_HPP_
