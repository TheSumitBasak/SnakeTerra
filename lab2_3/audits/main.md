| Code Smell              | Location                   | Confidence | Simple Explanation                                                                                                                            |
| ----------------------- | -------------------------- | ---------- | --------------------------------------------------------------------------------------------------------------------------------------------- |
| **Large Class**         | `include/GameBoard.h:19`   | C4         | `GameBoard` is doing too many things. It handles the game board, UI, input, game loop, rendering, and leaderboard.                            |
| **Long Method**         | `src/GameBoard.cpp:221`    | C4         | `play_game()` is very long and does many jobs, such as handling the game loop, drawing the game, checking input, and closing the screen.      |
| **Duplicate Code**      | `src/GameBoard.cpp:74`     | C4         | `draw_hash_banner_to_win()` and `draw_banner_to_win()` have almost the same code for drawing banners.                                         |
| **Divergent Change**    | `include/GameBoard.h:19`   | C3         | `GameBoard` may need changes for many different reasons, such as UI changes, game logic changes, rendering changes, or leaderboard changes.   |
| **Feature Envy**        | `src/GameBoard.cpp:290`    | C3         | `play_game()` directly uses the internal data of `Snake` and `Food` to draw them instead of asking those classes to draw themselves.          |
| **Primitive Obsession** | `include/Point.h:10`       | C3         | `Point` only uses basic integers for row and column. There are no useful functions for working with positions.                                |
| **Data Class**          | `include/Point.h:10`       | C3         | `Point` only stores `r` and `c` values. It does not contain any useful behavior or functions.                                                 |
| **Data Class**          | `include/Leaderboard.h:11` | C3         | `ScoreEntry` only stores a player's name and score. It does not have any functions or logic.                                                  |
| **Temporary Field**     | `include/GameBoard.h:63`   | C3         | `play_rows_` and `play_cols_` are only used while the game is running. They are temporary variables and duplicate `rows_` and `cols_`.        |
| **Dead Code**           | `include/Snake.h:14`       | C4         | `Snake::init()` does not do anything new. It only calls `reset()`, so it is unnecessary.                                                      |
| **Long Method**         | `src/GameBoard.cpp:117`    | C4         | `show_main_menu()` is quite long because it handles the window, banner, menu, and user input all in one function.                             |
| **Long Method**         | `src/GameBoard.cpp:403`    | C3         | `show_game_over_screen()` handles too many things together, including displaying scores, taking input, and starting the game again.           |
| **Switch Statement**    | `src/Snake.cpp:30`         | C3         | `Snake::set_dir()` has several conditions to check which direction the snake can move. This makes the method more complicated than necessary. |
| **Duplicate Code**      | `src/Leaderboard.cpp:24`   | C3         | `Leaderboard::load()` has two similar pieces of code for reading player names and scores from the file.                                       |


