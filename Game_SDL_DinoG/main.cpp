#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;
    if (game.init()) {
        game.run();
    }
    game.close();
    return 0;
}
