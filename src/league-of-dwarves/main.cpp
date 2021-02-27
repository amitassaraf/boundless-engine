#include <boundless.h>

class LeagueOfDwarves : public Boundless::Game {

public:
    LeagueOfDwarves() {
        this->pushLayer(new Boundless::WindowLayer(m_eventManager));
    }

    ~LeagueOfDwarves() {

    }

};

namespace Boundless {
    Game* create_boundless_game() {
    return new LeagueOfDwarves();
    }
}
