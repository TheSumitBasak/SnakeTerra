#include "GameBoard.h"

using namespace std;

int main() {
    snaketerra::GameBoard gb(20, 30);
    gb.init_ncurses();
    gb.run();
    // shutdown handled inside run on quit
    return 0;
}