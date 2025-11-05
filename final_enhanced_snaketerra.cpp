#include <ncurses.h>
#include <deque>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <cctype>
#include <iomanip> // std::quoted
#include <cstring>

using namespace std::chrono_literals;

struct Point { int r, c; bool operator==(const Point& o) const { return r==o.r && c==o.c; } };
enum class Dir { UP, DOWN, LEFT, RIGHT };

class Snake {
public:
    Snake() { reset(0,0); }
    void init(int sr, int sc) { reset(sr, sc); }
    const std::deque<Point>& body() const { return body_; }
    Point head() const { return body_.back(); }
    void set_dir(Dir d) {
        if ((dir_==Dir::UP && d==Dir::DOWN) || (dir_==Dir::DOWN && d==Dir::UP) ||
            (dir_==Dir::LEFT && d==Dir::RIGHT) || (dir_==Dir::RIGHT && d==Dir::LEFT)) return;
        dir_ = d;
    }
    void move() {
        Point h = head();
        Point nh = h;
        switch (dir_) {
            case Dir::UP:    nh.r -= 1; break;
            case Dir::DOWN:  nh.r += 1; break;
            case Dir::LEFT:  nh.c -= 1; break;
            case Dir::RIGHT: nh.c += 1; break;
        }
        body_.push_back(nh);
        if (!grow_next_) body_.pop_front();
        else grow_next_ = false;
    }
    void grow() { grow_next_ = true; }
    bool occupies(const Point& p) const {
        return std::find(body_.begin(), body_.end(), p) != body_.end();
    }
    bool collides_with_self() const {
        Point h = head();
        for (size_t i=0;i+1<body_.size();++i) if (body_[i]==h) return true;
        return false;
    }
    void reset(int sr, int sc) {
        body_.clear();
        body_.push_back({sr, sc-1});
        body_.push_back({sr, sc});
        body_.push_back({sr, sc+1});
        dir_ = Dir::RIGHT;
        grow_next_ = false;
    }
private:
    std::deque<Point> body_;
    Dir dir_;
    bool grow_next_;
};

class Food {
public:
    Food(): pos_{-1,-1} {}
    Point pos() const { return pos_; }
    void spawn(int rows, int cols, const Snake& snake) {
        std::vector<Point> empties;
        empties.reserve(rows*cols);
        for (int r=0;r<rows;++r) for (int c=0;c<cols;++c) {
            Point p{r,c};
            if (!snake.occupies(p)) empties.push_back(p);
        }
        if (empties.empty()) { pos_ = {-1,-1}; return; }
        pos_ = empties[std::rand()%empties.size()];
    }
private:
    Point pos_;
};

struct ScoreEntry { std::string name; int score; };

class Leaderboard {
public:
    Leaderboard(const std::string& path="leaderboard.txt"): path_(path) { load(); }
    void load() {
        entries_.clear();
        std::ifstream ifs(path_);
        if (!ifs) return;
        std::string line;
        while (std::getline(ifs, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            std::string name;
            int score;
            if (iss >> std::quoted(name) >> score) entries_.push_back({name,score});
            else {
                std::istringstream iss2(line);
                if (iss2 >> name >> score) entries_.push_back({name,score});
            }
        }
        sort_and_trim();
    }
    void save() {
        std::ofstream ofs(path_, std::ios::trunc);
        if (!ofs) return;
        for (auto &e: entries_) ofs << std::quoted(e.name) << " " << e.score << "\n";
    }
    void add(const std::string& name, int score) {
        std::string clean = sanitize_name(name);
        entries_.push_back({clean, score});
        sort_and_trim(); save();
    }
    std::vector<ScoreEntry> top(int n=3) const {
        std::vector<ScoreEntry> out;
        for (size_t i=0;i<entries_.size() && (int)i<n;++i) out.push_back(entries_[i]);
        return out;
    }
    std::vector<ScoreEntry> all() const { return entries_; }
private:
    void sort_and_trim() {
        std::sort(entries_.begin(), entries_.end(), [](const ScoreEntry& a, const ScoreEntry& b){
            if (a.score!=b.score) return a.score > b.score;
            return a.name < b.name;
        });
        if (entries_.size() > 200) entries_.resize(200);
    }
    static std::string sanitize_name(const std::string& s) {
        std::string o;
        for (char ch: s) {
            if (std::isalnum((unsigned char)ch) || ch=='_' || ch=='-') o.push_back(ch);
            if (o.size()>=16) break;
        }
        if (o.empty()) o = "Player";
        return o;
    }
    std::string path_;
    std::vector<ScoreEntry> entries_;
};

enum class Difficulty { EASY=220, NORMAL=140, HARD=80 };

class GameBoard {
public:
    GameBoard(int rows=20,int cols=30)
      : rows_(rows), cols_(cols), snake_(), score_(0), running_(false),
        cell_w_(2), leaderboard_("leaderboard.txt"), difficulty_(Difficulty::NORMAL)
    {
        std::srand((unsigned)std::time(nullptr));
        food_.spawn(rows_, cols_, snake_);
    }

    void init_ncurses() {
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

    void shutdown_ncurses() {
        nodelay(stdscr, FALSE);
        getch();
        endwin();
    }

    void run() {
        while (true) {
            int choice = show_main_menu();
            if (choice == 0) play_game();
            else if (choice == 1) change_difficulty_screen();
            else if (choice == 2) show_leaderboard_screen();
            else { shutdown_ncurses(); return; }
        }
    }

private:
    // Banner printed with '#' style
    void draw_banner_to_win(WINDOW* win, int start_y, int max_w) {
        static const std::vector<std::string> banner = {
            "  #####  #     #  ######   #    #  #######  #######  #######  #     #  ",
            " #     # #     # #     #  #    #  #         #     #  #     #  ##   ##  ",
            " #       #     # #        #    #  #         #     #  #     #  # # # #  ",
            "  #####  ####### #  ####  #    #  #####     #     #  #     #  #  #  #  ",
            "       # #     # #     #  #    #  #         #     #  #     #  #     #  ",
            " #     # #     # #     #  #    #  #         #     #  #     #  #     #  ",
            "  #####  #     #  ######    ####   #######  #######  #######  #     #  "
        };
        int bw = (int)banner[0].size();
        int xpos = std::max(1, (max_w - bw)/2);
        for (size_t i=0;i<banner.size();++i) {
            if (start_y + (int)i >= getmaxy(win)-1) break;
            mvwprintw(win, start_y + (int)i, xpos, "%s", banner[i].c_str());
        }
    }

    int show_main_menu() {
        const std::vector<std::string> items = {
            "Start Game",
            "Change Difficulty",
            "Leaderboards",
            "Quit"
        };
        int menu_h = 18, menu_w = 72;
        int sy = (LINES - menu_h)/2, sx = (COLS - menu_w)/2;
        if (sy < 1) sy = 1; if (sx < 1) sx = 1;

        WINDOW* menu_win = newwin(menu_h, menu_w, sy, sx);
        keypad(menu_win, TRUE);
        int choice = 0;
        while (true) {
            werase(menu_win);
            box(menu_win, 0, 0);
            wattron(menu_win, COLOR_PAIR(5));
            draw_banner_to_win(menu_win, 1, menu_w-2);
            wattroff(menu_win, COLOR_PAIR(5));

            mvwprintw(menu_win, 9, 3, "Use Up/Down to navigate. Enter to select. Q to quit.");
            mvwprintw(menu_win, 11, 3, "Current Difficulty: %s", difficulty_str().c_str());

            for (size_t i=0;i<items.size();++i) {
                if ((int)i == choice) wattron(menu_win, A_REVERSE);
                mvwprintw(menu_win, 13 + (int)i, 6, "%s", items[i].c_str());
                if ((int)i == choice) wattroff(menu_win, A_REVERSE);
            }
            wrefresh(menu_win);

            int ch = wgetch(menu_win);
            if (ch == KEY_UP) choice = (choice - 1 + (int)items.size()) % (int)items.size();
            else if (ch == KEY_DOWN) choice = (choice + 1) % (int)items.size();
            else if (ch == '\n' || ch == KEY_ENTER) { delwin(menu_win); return choice; }
            else if (ch == 'q' || ch == 'Q') { delwin(menu_win); return (int)items.size()-1; }
            std::this_thread::sleep_for(40ms);
        }
    }

    void show_leaderboard_screen() {
        leaderboard_.load();
        int h = std::min(LINES-4, 20);
        int w = std::min(COLS-8, 60);
        int sy = (LINES - h)/2, sx = (COLS - w)/2;
        WINDOW* win = newwin(h, w, sy, sx);
        keypad(win, TRUE);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "Leaderboards (top entries)");
        auto all = leaderboard_.all();
        if (all.empty()) mvwprintw(win, 3, 4, "No scores yet.");
        else {
            for (size_t i=0;i<all.size() && i< (size_t) (h-6); ++i) {
                mvwprintw(win, 3 + (int)i, 4, "%2zu. %-16s %6d", i+1, all[i].name.c_str(), all[i].score);
            }
        }
        mvwprintw(win, h-2, 2, "Press any key to go back.");
        wrefresh(win);
        nodelay(win, FALSE);
        wgetch(win);
        delwin(win);
    }

    void change_difficulty_screen() {
        std::vector<Difficulty> diffs = {Difficulty::EASY, Difficulty::NORMAL, Difficulty::HARD};
        int idx = 1;
        for (size_t i=0;i<diffs.size();++i) if (diffs[i]==difficulty_) idx=(int)i;
        int h = 10, w = 50;
        int sy = (LINES - h)/2, sx = (COLS - w)/2;
        WINDOW* win = newwin(h, w, sy, sx);
        keypad(win, TRUE);
        while (true) {
            werase(win); box(win,0,0);
            mvwprintw(win,1,2,"Change Difficulty (Left/Right to change, Enter to accept)");
            for (size_t i=0;i<diffs.size();++i) {
                if ((int)i==idx) wattron(win, A_REVERSE);
                mvwprintw(win, 3, 4 + (int)i*15, "%s", difficulty_to_string(diffs[i]).c_str());
                if ((int)i==idx) wattroff(win, A_REVERSE);
            }
            wrefresh(win);
            int ch = wgetch(win);
            if (ch == KEY_LEFT) idx = (idx - 1 + (int)diffs.size()) % (int)diffs.size();
            else if (ch == KEY_RIGHT) idx = (idx + 1) % (int)diffs.size();
            else if (ch == '\n' || ch == KEY_ENTER) { difficulty_ = diffs[idx]; delwin(win); return; }
            else if (ch == 27) { delwin(win); return; } // ESC
            std::this_thread::sleep_for(30ms);
        }
    }

    void play_game() {
        if (LINES < rows_ + 6 || COLS < cols_*cell_w_ + 40) {
            // terminal too small
            WINDOW* w = newwin(6, 60, (LINES-6)/2, std::max(2, (COLS-60)/2));
            box(w,0,0);
            mvwprintw(w,1,2,"Terminal too small for optimal UI.");
            mvwprintw(w,2,2,"Please resize terminal (recommended at least %dx%d).", cols_*cell_w_ + 40, rows_ + 6);
            mvwprintw(w,4,2,"Press any key to return to menu.");
            wrefresh(w);
            nodelay(w, FALSE);
            wgetch(w);
            delwin(w);
            return;
        }

        score_ = 0;
        snake_.reset(rows_/2, cols_/2);
        food_.spawn(rows_, cols_, snake_);
        running_ = true;

        int info_w = std::max(28, COLS/4);
        int left_w = COLS - info_w - 6;
        int left_h = rows_ + 2;
        int top = 2;
        int left = 2;
        WINDOW* left_win = newwin(left_h, left_w, top, left);
        WINDOW* right_win = newwin(left_h, info_w, top, left + left_w + 2);
        keypad(left_win, TRUE);
        keypad(right_win, TRUE);

        // subwindows inside right_win
        int r_inner_w = info_w - 2;
        WINDOW* right_score = derwin(right_win, 7, r_inner_w, 1, 1);
        WINDOW* right_top3 = derwin(right_win, left_h - 10, r_inner_w, 8, 1);

        // main loop timing
        auto last_tick = std::chrono::steady_clock::now();
        int delay_ms = (int)difficulty_;

        nodelay(stdscr, TRUE);
        curs_set(0);

        while (running_) {
            int ch = getch();
            handle_input(ch);

            auto now = std::chrono::steady_clock::now();
            if (now - last_tick >= std::chrono::milliseconds(delay_ms)) {
                step();
                last_tick = now;
            }

            // draw left_win (game area)
            werase(left_win);
            box(left_win, 0, 0);
            mvwprintw(left_win, 0, 2, " Game ");

            // draw grid inside left_win at (1,1) .. we map game cells to characters starting at x=1
            int visible_cols = (left_w - 2) / cell_w_;
            int visible_rows = left_h - 2;
            // draw food if visible
            Point f = food_.pos();
            if (f.r >=0 && f.c >=0 && f.r < visible_rows && f.c < visible_cols) {
                wattron(left_win, COLOR_PAIR(2));
                mvwprintw(left_win, 1 + f.r, 1 + f.c*cell_w_, "<>");
                wattroff(left_win, COLOR_PAIR(2));
            }
            // draw snake
            for (const auto& seg : snake_.body()) {
                if (seg.r < 0 || seg.r >= visible_rows) continue;
                if (seg.c < 0 || seg.c >= visible_cols) continue;
                wattron(left_win, COLOR_PAIR(1));
                mvwaddstr(left_win, 1 + seg.r, 1 + seg.c*cell_w_, "  ");
                wattroff(left_win, COLOR_PAIR(1));
            }
            wrefresh(left_win);

            // draw right_win (info)
            werase(right_win);
            box(right_win, 0, 0);
            mvwprintw(right_win, 0, 2, " Info ");

            // score box
            werase(right_score);
            box(right_score, 0, 0);
            mvwprintw(right_score, 0, 2, " Current ");
            wattron(right_score, COLOR_PAIR(4));
            mvwprintw(right_score, 1, 2, "Score: %d", score_);
            mvwprintw(right_score, 2, 2, "Difficulty: %s", difficulty_str().c_str());
            mvwprintw(right_score, 3, 2, "Length: %zu", snake_.body().size());
            wattroff(right_score, COLOR_PAIR(4));
            wrefresh(right_score);

            // top3 box
            werase(right_top3);
            box(right_top3, 0, 0);
            mvwprintw(right_top3, 0, 2, " Top 3 ");
            auto t3 = leaderboard_.top(3);
            if (t3.empty()) {
                mvwprintw(right_top3, 1, 2, "No scores yet.");
            } else {
                for (size_t i=0;i<t3.size();++i) {
                    mvwprintw(right_top3, 1 + (int)i, 2, "%d) %-12s %6d", (int)i+1,
                              t3[i].name.c_str(), t3[i].score);
                }
            }
            wrefresh(right_top3);
            wrefresh(right_win);

            // adjust speed by score
            delay_ms = std::max(30, (int)difficulty_ - score_*2);
            std::this_thread::sleep_for(8ms);
        }

        // cleanup windows
        delwin(right_top3);
        delwin(right_score);
        delwin(right_win);
        delwin(left_win);

        // game over: prompt name and save
        nodelay(stdscr, FALSE);
        std::string name = prompt_name_and_save();
        leaderboard_.add(name, score_);
        show_game_over_screen(name);
    }

    void step() {
        snake_.move();
        Point h = snake_.head();
        if (h.r < 0 || h.r >= rows_ || h.c < 0 || h.c >= cols_) { running_ = false; return; }
        if (snake_.collides_with_self()) { running_ = false; return; }
        if (h == food_.pos()) {
            score_ += 1;
            snake_.grow();
            food_.spawn(rows_, cols_, snake_);
        }
    }

    void handle_input(int ch) {
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

    std::string prompt_name_and_save() {
        echo(); curs_set(1);
        int wy = LINES/2 - 2, wx = std::max(2, (COLS-60)/2);
        WINDOW* w = newwin(6, 60, wy, wx);
        box(w,0,0);
        mvwprintw(w,1,2,"Game Over! Your score: %d", score_);
        mvwprintw(w,2,2,"Enter your name (alnum, max 16). Press Enter to save:");
        mvwprintw(w,4,2,"> ");
        wrefresh(w);
        char buf[64]; memset(buf,0,sizeof(buf));
        wgetnstr(w, buf, 32);
        std::string name(buf);
        if (name.empty()) name = "Player";
        delwin(w);
        noecho(); curs_set(0);
        return name;
    }

    void show_game_over_screen(const std::string& name) {
        int h = 12, w = 60;
        int sy = (LINES - h)/2, sx = (COLS - w)/2;
        WINDOW* win = newwin(h, w, sy, sx);
        box(win,0,0);
        mvwprintw(win,1,2,"Game Over!");
        mvwprintw(win,2,2,"Final Score for %s: %d", name.c_str(), score_);
        mvwprintw(win,4,2,"Top Scores:");
        auto top = leaderboard_.top(5);
        for (size_t i=0;i<top.size() && i< (size_t)(h-7); ++i) {
            mvwprintw(win,6 + (int)i, 4, "%2zu. %-12s %6d", i+1, top[i].name.c_str(), top[i].score);
        }
        mvwprintw(win,h-2,2,"Press R to Restart, M for Menu, Q to Quit.");
        wrefresh(win);
        nodelay(win, FALSE);
        while (true) {
            int ch = wgetch(win);
            if (ch == 'r' || ch == 'R') { delwin(win); running_ = true; score_ = 0; snake_.reset(rows_/2, cols_/2); food_.spawn(rows_, cols_, snake_); return; }
            else if (ch == 'm' || ch == 'M') { delwin(win); return; }
            else if (ch == 'q' || ch == 'Q') { delwin(win); shutdown_ncurses(); exit(0); }
        }
    }

    std::string difficulty_str() const {
        return difficulty_to_string(difficulty_);
    }
    static std::string difficulty_to_string(Difficulty d) {
        switch (d) {
            case Difficulty::EASY: return "Easy";
            case Difficulty::NORMAL: return "Normal";
            case Difficulty::HARD: return "Hard";
        }
        return "Normal";
    }

    int rows_, cols_;
    Snake snake_;
    Food food_;
    int score_;
    bool running_;
    int cell_w_;
    Leaderboard leaderboard_;
    Difficulty difficulty_;
};

int main() {
    GameBoard gb(20, 30);
    gb.init_ncurses();
    gb.run();
    // shutdown handled inside run on quit
    return 0;
}