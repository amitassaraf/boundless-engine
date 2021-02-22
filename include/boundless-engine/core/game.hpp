#pragma once
#ifndef BD_GAME_HPP_
#define BD_GAME_HPP_

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

#endif // !BD_GAME_HPP_
