#pragma once
#ifndef BND_GAME_HPP_
#define BND_GAME_HPP_

namespace Boundless {

    /**
     * A class for the game
     */
    class Game {

    public:

        Game();
        virtual ~Game();

        int run();
        
    };

}

#endif // !BND_GAME_HPP_
