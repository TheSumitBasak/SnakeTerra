#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <conio.h> // optional; on Unix replace with termios handling

int main() {
    const int ROWS = 10;
    const int COLS = 20;
    std::vector<std::string> board(ROWS, std::string(COLS, '.'));

    std::cout << "Minimal Snake - press any key to exit\n\n";
    for (int r = 0; r < ROWS; ++r) {
        std::cout << board[r] << "\n";
    }

    std::cout << "\nPress any key to quit...\n";
#if defined(_WIN32)
    _getch();
#else
    std::cin.get();
#endif
    return 0;
}