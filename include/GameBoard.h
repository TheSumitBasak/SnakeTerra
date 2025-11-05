#ifndef SNAKE_TERRA_GAMEBOARD_H
#define SNAKE_TERRA_GAMEBOARD_H

#include "Point.h"
#include "Snake.h"
#include "Food.h"
#include "Leaderboard.h"
#include <string>

// forward-declare ncurses internal window struct type
struct _win_st;

using namespace std;

namespace snaketerra {

enum class Difficulty { EASY = 220, NORMAL = 140, HARD = 80 };

class GameBoard {
public:
    GameBoard(int rows = 20, int cols = 30);
    ~GameBoard();

    void init_ncurses();
    void shutdown_ncurses();

    // Entry point
    void run();

private:
    // UI helpers
    int show_main_menu();
    void draw_banner_to_win(void* win_ptr, int start_y, int max_w);
    void draw_hash_banner_to_win(struct _win_st* win, int start_y, int max_w);

    void show_leaderboard_screen();
    void change_difficulty_screen();

    // game
    void play_game();
    void step();
    void handle_input(int ch);

    // game-over & prompts
    string prompt_name_and_save();
    void show_game_over_screen(const string& name);

    // utilities
    static string difficulty_to_string(Difficulty d);
    string difficulty_str() const;

private:
    int rows_;
    int cols_;
    Snake snake_;
    Food food_;
    int score_;
    bool running_;
    int cell_w_;
    Leaderboard leaderboard_;
    Difficulty difficulty_;

    int play_rows_;
    int play_cols_;
};

} // namespace snaketerra

#endif // SNAKE_TERRA_GAMEBOARD_H