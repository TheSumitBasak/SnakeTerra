#include "GameBoard.h"
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstring>
#include <vector>

using namespace std;
using namespace std::chrono_literals;

namespace snaketerra {

GameBoard::GameBoard(int rows, int cols)
    : rows_(rows),
      cols_(cols),
      snake_(),
      food_(),
      score_(0),
      running_(false),
      cell_w_(2), // keep cell width fixed
      leaderboard_("leaderboard.txt"),
      difficulty_(Difficulty::NORMAL),
      play_rows_(rows),
      play_cols_(cols)
{
    srand((unsigned)time(nullptr));
    food_.spawn(rows_, cols_, snake_);
}

GameBoard::~GameBoard() = default;

void GameBoard::init_ncurses() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_BLACK, COLOR_GREEN); // snake
        init_pair(2, COLOR_RED, -1); // food
        init_pair(3, COLOR_WHITE, -1); // borders / text
        init_pair(4, COLOR_YELLOW, -1); // highlights
        init_pair(5, COLOR_CYAN, -1); // menu / banner
    }
}

void GameBoard::shutdown_ncurses() {
    nodelay(stdscr, FALSE);
    getch();
    endwin();
}

void GameBoard::run() {
    while (true) {
        int choice = show_main_menu();
        if (choice == 0) {
            play_game();
        } else if (choice == 1) {
            change_difficulty_screen();
        } else if (choice == 2) {
            show_leaderboard_screen();
        } else {
            shutdown_ncurses();
            return;
        }
    }
}

// draw a big "SNAKE TERRA" made of '#' characters into the provided window
void GameBoard::draw_hash_banner_to_win(struct _win_st* win, int start_y, int max_w) {
    // Convert win to WINDOW* (ncurses real type) and render
    WINDOW* w = reinterpret_cast<WINDOW*>(win);

    static const vector<string> banner = {
        "  _____ _   _      _      _  __  ____  _______  ____ ____    ____       _",
        " / ____| \\ | |    / \\    | |/ / | ___||__   __|| ___||  _ \\ |  _ \\     / \\    ",
        "| (___ |  \\| |   / _ \\   | ' /  | |___   | |   | |___| |_) || |_) |   / _ \\   ",
        " \\___ \\| . ` |  / ___ \\  | . \\  | ___|   | |   | ___||    < |    <   / ___ \\  ",
        " ____) | |\\  | / /   \\ \\ | |\\ \\ | |___   | |   | |___| |\\ \\ | |\\ \\  / /   \\ \\ ",
        "|_____/|_| \\_|/_/     \\_\\|_| \\_\\|____|   |_|   |____||_| \\_\\|_| \\_\\/_/     \\_\\"
    };


    int bw = (int)banner.front().size();
    int xpos = max(1, (max_w - bw) / 2);
    wattron(w, COLOR_PAIR(5));
    for (size_t i = 0; i < banner.size(); ++i) {
        if (start_y + (int)i >= getmaxy(w) - 1) break;
        mvwprintw(w, start_y + (int)i, xpos, "%s", banner[i].c_str());
    }
    wattroff(w, COLOR_PAIR(5));
}

void GameBoard::draw_banner_to_win(void* win_ptr, int start_y, int max_w) {
    WINDOW* win = static_cast<WINDOW*>(win_ptr);
    static const vector<string> banner = {
        "  #####  #     #  ######   #    #  #######  #######  #######  #     #  ",
        " #     # #     # #     #  #    #  #         #     #  #     #  ##   ##  ",
        " #       #     # #        #    #  #         #     #  #     #  # # # #  ",
        "  #####  ####### #  ####  #    #  #####     #     #  #     #  #  #  #  ",
        "       # #     # #     #  #    #  #         #     #  #     #  #     #  ",
        " #     # #     # #     #  #    #  #         #     #  #     #  #     #  ",
        "  #####  #     #  ######    ####   #######  #######  #######  #     #  "
    };
    int bw = (int)banner[0].size();
    int xpos = max(1, (max_w - bw) / 2);
    for (size_t i = 0; i < banner.size(); ++i) {
        if (start_y + (int)i >= getmaxy(win) - 1) break;
        mvwprintw(win, start_y + (int)i, xpos, "%s", banner[i].c_str());
    }
}

int GameBoard::show_main_menu() {
    const vector<string> items = {
        "Start Game",
        "Change Difficulty",
        "Leaderboards",
        "Quit"
    };
    int menu_h = 22, menu_w = 85;
    int sy = (LINES - menu_h) / 2, sx = (COLS - menu_w) / 2;
    if (sy < 1) sy = 1;
    if (sx < 1) sx = 1;

    // Clear the whole screen each time the main menu opens
    clear();
    refresh();

    WINDOW* menu_win = newwin(menu_h, menu_w, sy, sx);
    keypad(menu_win, TRUE);
    int choice = 0;
    while (true) {
        werase(menu_win);
        box(menu_win, 0, 0);

        // draw the hash banner at the top of the menu window
        draw_hash_banner_to_win(reinterpret_cast<struct _win_st*>(menu_win), 1, menu_w - 2);

        mvwprintw(menu_win, 10, 3, "Use Up/Down to navigate. Enter to select. Q to quit.");
        mvwprintw(menu_win, 12, 3, "Current Difficulty: %s", difficulty_str().c_str());

        for (size_t i = 0; i < items.size(); ++i) {
            if ((int)i == choice) wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, 14 + (int)i, 6, "%s", items[i].c_str());
            if ((int)i == choice) wattroff(menu_win, A_REVERSE);
        }
        wrefresh(menu_win);

        int ch = wgetch(menu_win);
        if (ch == KEY_UP) choice = (choice - 1 + (int)items.size()) % (int)items.size();
        else if (ch == KEY_DOWN) choice = (choice + 1) % (int)items.size();
        else if (ch == '\n' || ch == KEY_ENTER) { delwin(menu_win); return choice; }
        else if (ch == 'q' || ch == 'Q') { delwin(menu_win); return (int)items.size() - 1; }
        this_thread::sleep_for(40ms);
    }
}

void GameBoard::show_leaderboard_screen() {
    leaderboard_.load();
    int h = min(LINES - 4, 20);
    int w = min(COLS - 8, 60);
    int sy = (LINES - h) / 2, sx = (COLS - w) / 2;

    // Clear screen when opening leaderboard too
    clear();
    refresh();

    WINDOW* win = newwin(h, w, sy, sx);
    keypad(win, TRUE);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "Leaderboards (top entries)");
    auto all = leaderboard_.all();
    if (all.empty()) mvwprintw(win, 3, 4, "No scores yet.");
    else {
        for (size_t i = 0; i < all.size() && i < (size_t)(h - 6); ++i) {
            mvwprintw(win, 3 + (int)i, 4, "%2zu. %-16s %6d", i + 1, all[i].name.c_str(), all[i].score);
        }
    }
    mvwprintw(win, h - 2, 2, "Press any key to go back.");
    wrefresh(win);
    nodelay(win, FALSE);
    wgetch(win);
    delwin(win);
}

void GameBoard::change_difficulty_screen() {
    vector<Difficulty> diffs = {Difficulty::EASY, Difficulty::NORMAL, Difficulty::HARD};
    int idx = 1;
    for (size_t i = 0; i < diffs.size(); ++i) if (diffs[i] == difficulty_) idx = (int)i;
    int h = 10, w = 50;
    int sy = (LINES - h) / 2, sx = (COLS - w) / 2;

    // Clear screen when opening difficulty screen
    clear();
    refresh();

    WINDOW* win = newwin(h, w, sy, sx);
    keypad(win, TRUE);
    while (true) {
        werase(win); box(win, 0, 0);
        mvwprintw(win, 1, 2, "Change Difficulty (Left/Right to change, Enter to accept)");
        for (size_t i = 0; i < diffs.size(); ++i) {
            if ((int)i == idx) wattron(win, A_REVERSE);
            mvwprintw(win, 3, 4 + (int)i * 15, "%s", difficulty_to_string(diffs[i]).c_str());
            if ((int)i == idx) wattroff(win, A_REVERSE);
        }
        wrefresh(win);
        int ch = wgetch(win);
        if (ch == KEY_LEFT) idx = (idx - 1 + (int)diffs.size()) % (int)diffs.size();
        else if (ch == KEY_RIGHT) idx = (idx + 1) % (int)diffs.size();
        else if (ch == '\n' || ch == KEY_ENTER) { difficulty_ = diffs[idx]; delwin(win); return; }
        else if (ch == 27) { delwin(win); return; } // ESC
        this_thread::sleep_for(30ms);
    }
}

void GameBoard::play_game() {
    // Clear the screen when the game opens
    clear();
    refresh();

    // Fixed-size play box (independent of terminal size).
    const int used_cell_w = cell_w_; // fixed 2 characters per cell
    const int left_box_w = cols_ * used_cell_w + 2; // +2 for box borders
    const int left_box_h = rows_ + 2;                // +2 for box borders

    const int info_w = max(28, COLS / 4);
    const int total_required_w = left_box_w + info_w + 6; // spacing + margins
    const int total_required_h = left_box_h + 4;

    if (COLS < total_required_w || LINES < total_required_h) {
        WINDOW* w = newwin(6, 70, (LINES - 6) / 2, max(2, (COLS - 70) / 2));
        box(w, 0, 0);
        mvwprintw(w, 1, 2, "Terminal too small for fixed-size game box.");
        mvwprintw(w, 2, 2, "Required: at least %d cols x %d rows. Current: %d x %d.",
                  total_required_w, total_required_h, COLS, LINES);
        mvwprintw(w, 4, 2, "Resize terminal and press any key to continue.");
        wrefresh(w);
        nodelay(w, FALSE);
        wgetch(w);
        delwin(w);
        return;
    }

    score_ = 0;

    const int top = 2;
    const int left = 2;
    const int right_box_x = left + left_box_w + 2;

    WINDOW* left_win = newwin(left_box_h, left_box_w, top, left);
    WINDOW* right_win = newwin(left_box_h, info_w, top, right_box_x);
    keypad(left_win, TRUE);
    keypad(right_win, TRUE);

    WINDOW* right_score = derwin(right_win, 7, info_w - 2, 1, 1);
    WINDOW* right_top3 = derwin(right_win, left_box_h - 10, info_w - 2, 8, 1);

    play_cols_ = cols_;
    play_rows_ = rows_;

    snake_.reset(play_rows_ / 2, play_cols_ / 2);
    food_.spawn(play_rows_, play_cols_, snake_);
    running_ = true;

    auto last_tick = chrono::steady_clock::now();
    int delay_ms = static_cast<int>(difficulty_);

    nodelay(stdscr, TRUE);
    curs_set(0);

    while (running_) {
        int ch = getch();
        handle_input(ch);

        auto now = chrono::steady_clock::now();
        if (now - last_tick >= chrono::milliseconds(delay_ms)) {
            step();
            last_tick = now;
        }

        werase(left_win);
        box(left_win, 0, 0);
        mvwprintw(left_win, 0, 2, " Game ");

        Point f = food_.pos();
        if (f.r >= 0 && f.c >= 0 && f.r < play_rows_ && f.c < play_cols_) {
            wattron(left_win, COLOR_PAIR(2));
            mvwprintw(left_win, 1 + f.r, 1 + f.c * used_cell_w, "%s", "<>");
            wattroff(left_win, COLOR_PAIR(2));
        }

        for (const auto& seg : snake_.body()) {
            if (seg.r < 0 || seg.r >= play_rows_) continue;
            if (seg.c < 0 || seg.c >= play_cols_) continue;
            wattron(left_win, COLOR_PAIR(1));
            if (used_cell_w == 1) {
                mvwaddch(left_win, 1 + seg.r, 1 + seg.c * used_cell_w, ' ' | A_REVERSE);
            } else {
                mvwaddstr(left_win, 1 + seg.r, 1 + seg.c * used_cell_w, "  ");
            }
            wattroff(left_win, COLOR_PAIR(1));
        }
        wrefresh(left_win);

        werase(right_win);
        box(right_win, 0, 0);
        mvwprintw(right_win, 0, 2, " Info ");

        werase(right_score);
        box(right_score, 0, 0);
        mvwprintw(right_score, 0, 2, " Current ");
        wattron(right_score, COLOR_PAIR(4));
        mvwprintw(right_score, 1, 2, "Score: %d", score_);
        mvwprintw(right_score, 2, 2, "Difficulty: %s", difficulty_str().c_str());
        mvwprintw(right_score, 3, 2, "Length: %zu", snake_.body().size());
        wattroff(right_score, COLOR_PAIR(4));
        wrefresh(right_score);

        werase(right_top3);
        box(right_top3, 0, 0);
        mvwprintw(right_top3, 0, 2, " Top 3 ");
        auto t3 = leaderboard_.top(3);
        if (t3.empty()) {
            mvwprintw(right_top3, 1, 2, "No scores yet.");
        } else {
            for (size_t i = 0; i < t3.size(); ++i) {
                mvwprintw(right_top3, 1 + (int)i, 2, "%d) %-12s %6d", (int)i + 1, t3[i].name.c_str(), t3[i].score);
            }
        }
        wrefresh(right_top3);
        wrefresh(right_win);

        delay_ms = max(30, static_cast<int>(difficulty_) - score_ * 2);
        this_thread::sleep_for(8ms);
    }

    delwin(right_top3);
    delwin(right_score);
    delwin(right_win);
    delwin(left_win);

    nodelay(stdscr, FALSE);
    string name = prompt_name_and_save();
    leaderboard_.add(name, score_);
    show_game_over_screen(name);
}

void GameBoard::step() {
    snake_.move();
    Point h = snake_.head();
    if (h.r < 0 || h.r >= play_rows_ || h.c < 0 || h.c >= play_cols_) { running_ = false; return; }
    if (snake_.collides_with_self()) { running_ = false; return; }
    if (h == food_.pos()) {
        score_ += 1;
        snake_.grow();
        food_.spawn(play_rows_, play_cols_, snake_);
    }
}

void GameBoard::handle_input(int ch) {
    switch (ch) {
        case KEY_UP: case 'w': case 'W': snake_.set_dir(Dir::UP); break;
        case KEY_DOWN: case 's': case 'S': snake_.set_dir(Dir::DOWN); break;
        case KEY_LEFT: case 'a': case 'A': snake_.set_dir(Dir::LEFT); break;
        case KEY_RIGHT: case 'd': case 'D': snake_.set_dir(Dir::RIGHT); break;
        case 'p': case 'P': {
            nodelay(stdscr, FALSE);
            mvprintw(0, 2, "PAUSED - press any key to continue");
            refresh();
            getch();
            nodelay(stdscr, TRUE);
            clear();
        } break;
        case 'q': case 'Q': running_ = false; break;
        default: break;
    }
}

string GameBoard::prompt_name_and_save() {
    echo(); curs_set(1);
    int wy = LINES / 2 - 2;
    int wx = max(2, (COLS - 60) / 2);
    WINDOW* w = newwin(6, 60, wy, wx);
    box(w, 0, 0);
    mvwprintw(w, 1, 2, "Game Over! Your score: %d", score_);
    mvwprintw(w, 2, 2, "Enter your name (alnum, max 16). Press Enter to save:");
    mvwprintw(w, 4, 2, "> ");
    wrefresh(w);
    char buf[64]; memset(buf, 0, sizeof(buf));
    wgetnstr(w, buf, 32);
    string name(buf);
    if (name.empty()) name = "Player";
    delwin(w);
    noecho(); curs_set(0);
    return name;
}

void GameBoard::show_game_over_screen(const string& name) {
    int h = 12, w = 60;
    int sy = (LINES - h) / 2, sx = (COLS - w) / 2;
    WINDOW* win = newwin(h, w, sy, sx);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "Game Over!");
    mvwprintw(win, 2, 2, "Final Score for %s: %d", name.c_str(), score_);
    mvwprintw(win, 4, 2, "Top Scores:");
    auto top = leaderboard_.top(5);
    for (size_t i = 0; i < top.size() && i < (size_t)(h - 7); ++i) {
        mvwprintw(win, 6 + (int)i, 4, "%2zu. %-12s %6d", i + 1, top[i].name.c_str(), top[i].score);
    }
    mvwprintw(win, h - 2, 2, "Press R to Restart, M for Menu, Q to Quit.");
    wrefresh(win);
    nodelay(win, FALSE);
    while (true) {
        int ch = wgetch(win);
        if (ch == 'r' || ch == 'R') {
            delwin(win);
            running_ = true;
            score_ = 0;
            snake_.reset(play_rows_ / 2, play_cols_ / 2);
            food_.spawn(play_rows_, play_cols_, snake_);
            play_game();
            return;
        } else if (ch == 'm' || ch == 'M') { delwin(win); return; }
        else if (ch == 'q' || ch == 'Q') { delwin(win); shutdown_ncurses(); exit(0); }
    }
}

string GameBoard::difficulty_str() const {
    return difficulty_to_string(difficulty_);
}

string GameBoard::difficulty_to_string(Difficulty d) {
    switch (d) {
        case Difficulty::EASY: return "Easy";
        case Difficulty::NORMAL: return "Normal";
        case Difficulty::HARD: return "Hard";
    }
    return "Normal";
}

} // namespace snaketerra