#include <boundless.h>

class LeagueOfDwarves : public Boundless::Game {

public:
    LeagueOfDwarves() {
        BD_GAME_INFO("Starting league of dwarves.");
        this->pushLayer(new Boundless::WindowLayer(m_eventManager));
        Boundless::World world;
        world.generateWorld();
    }

    ~LeagueOfDwarves() {

    }

};

namespace Boundless {
    Game* create_boundless_game() {
    return new LeagueOfDwarves();
    }
}
