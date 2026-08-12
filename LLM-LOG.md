# LLM-LOG.md

## 1. Setup

| | |
|---|---|
| **Model(s)** | GPT-5.6 Luna |
| **Tool / harness** | ChatGPT |
| **IDE / editor** | Visual Studio Code |
| **Did you paste this assignment document into it?** | **Yes** — Part A, Part B, and Part D were provided in the chat. |

## 2. Session

### Prompt 1
**Prompt:** > We are building a two-player mode for our terminal-based C++ snake game. We want to add a second snake controlled by W A S D. The first snake (Player 1) keeps using Arrow keys. How should we declare this second snake in our GameBoard.h file?

Here is our existing header:

```cpp
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

```

**Relevant result:** The model suggested adding the following private member variables to GameBoard.h for tracking the second snake, its score, and tracking which player lost:

```cpp
// Added to include/GameBoard.h private section
Snake snake2_;
int score2_;
int loser_; // 1 for Player 1, 2 for Player 2
```

### Prompt 2
**Prompt:** In our game, the food is spawned randomly on empty spots. Now that we have a second snake, the food shouldn't spawn on top of either snake's body. How do we update our Food class to check both snakes? Here are our current food files:

```cpp
#ifndef SNAKE_TERRA_FOOD_H
#define SNAKE_TERRA_FOOD_H

#include "Point.h"

using namespace std;

namespace snaketerra {

class Snake; // forward

class Food {
public:
    Food();
    Point pos() const;
    void spawn(int rows, int cols, const Snake& snake);

private:
    Point pos_;
};

} // namespace snaketerra

#endif // SNAKE_TERRA_FOOD_H

void Food::spawn(int rows, int cols, const Snake& snake) {
    vector<Point> empties;
    empties.reserve(rows * cols);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Point p{r, c};
            if (!snake.occupies(p)) empties.push_back(p);
        }
    }
    if (empties.empty()) {
        pos_ = {-1, -1};
        return;
    }
    pos_ = empties[rand() % empties.size()];
}

```

**Relevant result:** `Food::spawn()` controls the fruit's position, while `GameBoard::step()` handles the event of a snake eating the fruit.

```cpp
void Food::spawn(int rows, int cols, const Snake& snake1, const Snake& snake2) {
    vector<Point> empties;
    empties.reserve(rows * cols);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Point p{r, c};
            if (!snake1.occupies(p) && !snake2.occupies(p))
                empties.push_back(p);
        }
    }
    if (empties.empty()) {
        pos_ = {-1, -1};
        return;
    }
    pos_ = empties[rand() % empties.size()];
}

void GameBoard::step() {
    snake_.move();
    snake2_.move();

    Point h1 = snake_.head();
    Point h2 = snake2_.head();

    // Player 1 hits wall
    if (h1.r < 0 || h1.r >= play_rows_ ||
        h1.c < 0 || h1.c >= play_cols_) {
        loser_ = 1;
        running_ = false;
        return;
    }

    // Player 2 hits wall
    if (h2.r < 0 || h2.r >= play_rows_ ||
        h2.c < 0 || h2.c >= play_cols_) {
        loser_ = 2;
        running_ = false;
        return;
    }

    // Player 1 hits itself
    if (snake_.collides_with_self()) {
        loser_ = 1;
        running_ = false;
        return;
    }

    // Player 2 hits itself
    if (snake2_.collides_with_self()) {
        loser_ = 2;
        running_ = false;
        return;
    }

    // Player 1 hits Player 2
    for (const auto& seg : snake2_.body()) {
        if (h1 == seg) {
            loser_ = 1;
            running_ = false;
            return;
        }
    }

    // Player 2 hits Player 1
    for (const auto& seg : snake_.body()) {
        if (h2 == seg) {
            loser_ = 2;
            running_ = false;
            return;
        }
    }

    // Player 1 eats fruit
    if (h1 == food_.pos()) {
        score_++;
        snake_.grow();
        food_.spawn(rows_, cols_, snake_, snake2_);
    }

    // Player 2 eats fruit
    else if (h2 == food_.pos()) {
        score2_++;
        snake2_.grow();
        food_.spawn(rows_, cols_, snake_, snake2_);
    }
}
```
### Prompt 3
**Prompt:** > How do we map keyboard inputs for the second snake to W, A, S, D and keep the Arrow keys for the first snake?
Here is our existing handle_input function:
```cpp
case KEY_UP: case 'w': case 'W': snake_.set_dir(Dir::UP); break;
case KEY_DOWN: case 's': case 'S': snake_.set_dir(Dir::DOWN); break;
case KEY_LEFT: case 'a': case 'A': snake_.set_dir(Dir::LEFT); break;
case KEY_RIGHT: case 'd': case 'D': snake_.set_dir(Dir::RIGHT); break;
```

**Relevant result:** 
```cpp
case KEY_UP:
    snake_.set_dir(Dir::UP);
    break;

case KEY_DOWN:
    snake_.set_dir(Dir::DOWN);
    break;

case KEY_LEFT:
    snake_.set_dir(Dir::LEFT);
    break;

case KEY_RIGHT:
    snake_.set_dir(Dir::RIGHT);
    break;

case 'w':
case 'W':
    snake2_.set_dir(Dir::UP);
    break;

case 's':
case 'S':
    snake2_.set_dir(Dir::DOWN);
    break;

case 'a':
case 'A':
    snake2_.set_dir(Dir::LEFT);
    break;

case 'd':
case 'D':
    snake2_.set_dir(Dir::RIGHT);
    break;

```

### Prompt 4
**Prompt:** > We need to update the step() physics update to move both snakes. We also need to check if either snake hits a wall, itself, or the other snake, and update who the loser is. How can we do this?
Here is our existing single-player step() function:
```cpp
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
```

**Relevant result:** The model provided the updated collision checks for multiplayer:
```cpp
void GameBoard::step() {
    snake_.move();
    snake2_.move();

    Point h1 = snake_.head();
    Point h2 = snake2_.head();

    // Player 1 hits wall
    if (h1.r < 0 || h1.r >= play_rows_ ||
        h1.c < 0 || h1.c >= play_cols_) {
        loser_ = 1;
        running_ = false;
        return;
    }

    // Player 2 hits wall
    if (h2.r < 0 || h2.r >= play_rows_ ||
        h2.c < 0 || h2.c >= play_cols_) {
        loser_ = 2;
        running_ = false;
        return;
    }

    // Player 1 hits itself
    if (snake_.collides_with_self()) {
        loser_ = 1;
        running_ = false;
        return;
    }

    // Player 2 hits itself
    if (snake2_.collides_with_self()) {
        loser_ = 2;
        running_ = false;
        return;
    }

    // Player 1 hits Player 2
    for (const auto& seg : snake2_.body()) {
        if (h1 == seg) {
            loser_ = 1;
            running_ = false;
            return;
        }
    }

    // Player 2 hits Player 1
    for (const auto& seg : snake_.body()) {
        if (h2 == seg) {
            loser_ = 2;
            running_ = false;
            return;
        }
    }

    // Player 1 eats fruit
    if (h1 == food_.pos()) {
        score_++;
        snake_.grow();
        food_.spawn(rows_, cols_, snake_, snake2_);
    }

    // Player 2 eats fruit
    else if (h2 == food_.pos()) {
        score2_++;
        snake2_.grow();
        food_.spawn(rows_, cols_, snake_, snake2_);
    }
}
```
### Prompt 5
**Prompt:** When we restart the game, the scores and positions of both snakes aren't resetting correctly. Can you fix the reset and restart logic inside show_game_over_screen?

**Relevant result:** 
```cpp
if (ch == 'r' || ch == 'R') {
            delwin(win);
            running_ = true;
            score_ = 0;
            score2_ = 0;
            loser_ = 0;
            snake_.reset(play_rows_ / 2, play_cols_ / 2);
            snake2_.reset(play_rows_ / 2, (play_cols_ * 3) / 4);
            food_.spawn(rows_, cols_, snake_, snake2_);
            play_game();
            return;
        } else if (ch == 'm' || ch == 'M') { delwin(win); return; }
```

## 3. Results Summary

* *Which attempt finally worked*: Attempt 1 worked.
* *Total number of prompts to get working code*: 5 prompts.
* *Code provided to the model*: We pasted the contents of include/GameBoard.h, src/GameBoard.cpp, include/Food.h, and src/Food.cpp as context.