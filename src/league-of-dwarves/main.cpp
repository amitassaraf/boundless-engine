#include <boundless.h>


class LeagueOfDwarves : public Boundless::Game {

public:
    LeagueOfDwarves() {

    }

    ~LeagueOfDwarves() {

    }

};


Boundless::Game* Boundless::create_boundless_game() {
    return new LeagueOfDwarves();
}