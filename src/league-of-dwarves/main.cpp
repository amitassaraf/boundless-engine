#include <boundless.h>

class LeagueOfDwarves : public Boundless::Game {

public:
    LeagueOfDwarves() {

    }

    ~LeagueOfDwarves() {

    }

};

namespace Boundless {
    Game* create_boundless_game() {
    return new LeagueOfDwarves();
    }
}
