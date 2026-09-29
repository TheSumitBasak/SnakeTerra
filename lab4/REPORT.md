| | |
|---|---|
| Repository | https://github.com/TheSumitBasak/SnakeTerra.git |
| Base tag | `lab4-base` at commit `03d108b` |
| Pull request | Not opened from this environment — branch `lab4` (`b431c24`) is ready to push |

---

## 1. Five rules — [5]

Written before opening the source. Behaviour, with an observable outcome.

| # | Rule |
|---|---|
| 1 | Eating a piece of food adds exactly one point to the score and lengthens the snake by one segment (the extra segment appears on the following move). |
| 2 | The snake cannot reverse into itself: a 180° turn is ignored, while perpendicular turns are accepted. |
| 3 | Steering the snake off any edge of the board ends the game. |
| 4 | Steering the snake into its own body ends the game. |
| 5 | Food always spawns on an in-bounds cell the snake does not occupy; on a full board it is left off-board at `(-1,-1)`. |

I could state all five from playing, but two needed the source to make precise. Rule 1's *exactly one* point, and the fact that the new segment appears on the **next** move rather than the move that ate the food, cannot be read off the HUD (which only shows a running total). Rule 5's full-board sentinel of `(-1,-1)` never occurs in normal play and had to be read from the code.

---

## 2. What you could test, and what stopped you — [10]

No source changes in this part. Every `file:line` below is a line in `lab4-base`.

| # | Rule | Test written? | Blocking dependency (`file:line` + what it is) |
|---|---|---|---|
| 1 | Eat food → +1 score, +1 length | No | `include/GameBoard.h:41` — `void step();` is private, so the scoring branch at `src/GameBoard.cpp:359` is unreachable; `include/GameBoard.h:57` — `int score_;` is private; the whole TU also needs ncurses (`src/GameBoard.cpp:2`) |
| 2 | 180° reversal is ignored | Yes — `tests/snake_test.cpp` | None: `Snake::set_dir` is public (`include/Snake.h:22`) and `Snake.cpp` links with no I/O or UI |
| 3 | Leaving the board ends the game | No | `include/GameBoard.h:41` — `void step();` is private (wall check at `src/GameBoard.cpp:356`); `include/GameBoard.h:58` — `bool running_;` is private |
| 4 | Hitting itself ends the game | Predicate only — `tests/snake_test.cpp` | `Snake::collides_with_self()` is public (`include/Snake.h:27`), but the game-over transition lives in private `step()` (`include/GameBoard.h:41`) and private `running_` (`include/GameBoard.h:58`) |
| 5 | Food spawns off the snake, in bounds | Yes — `tests/food_test.cpp` | None: `Food::spawn` is public (`include/Food.h:16`); the only wrinkle is the global `rand()` at `src/Food.cpp:27`, so the test is seed-based rather than fully controllable |

> **Rules testable without modifying the source: 2 / 5**

Rule 4's *predicate* is testable, but its observable outcome (the game ending) is not, so it does not count as a full rule. Rules 1, 3 and 4's outcome all need the same private `GameBoard` seam, and Rule 1 additionally needs the food position to be controllable.

---

## 3. Coverage, and what it missed — [6]

| | |
|---|---|
| Line coverage | 95.4 % (103 / 108) |
| Branch coverage | 65.7 % (111 / 169) |
| Command used | `cd tests && make base && ./base_tests && gcovr -r .. --filter '.*/src/(Snake\|Food\|Leaderboard)\.cpp' --print-summary` |

Scope: line and branch coverage are measured only over the three translation units the base suite can link (`Snake.cpp`, `Food.cpp`, `Leaderboard.cpp`). `GameBoard.cpp` and `main.cpp` cannot be exercised headlessly at `lab4-base`, so they are excluded rather than reported as 0 %.

The uncovered lines are `src/Snake.cpp:12-14` (`Snake::init` is never called — the game uses `reset`) and `src/Leaderboard.cpp:53,73` (the closing braces of `top()` and `sanitize_name()`, which gcov counts as lines even though every statement inside runs). Branch coverage is dragged down by error paths the suite never provokes (`if (!ifs)` / `if (!ofs)` in `Leaderboard::load/save`, stream `throw` branches, and the short-circuit sub-branches of the `Snake::set_dir` reversal test).

**One rule that is executed by the suite but not verified by it:**

| | |
|---|---|
| Rule | Rule 5 — food is placed at a *random* free cell |
| Line that runs | `src/Food.cpp:27` — `pos_ = empties[rand() % empties.size()];` |
| The assertion that is missing | The suite checks the chosen cell is in-bounds and off the snake, but never that different seeds produce different cells — i.e. that the selection actually varies instead of being a constant |

---

## 4. The seam — [10]

| | |
|---|---|
| Rule made testable | Rule 1 — eating food adds one point and grows the snake |
| Commit 1 (seam) | `0346213` — `lab4: seam - make GameBoard simulation testable via injected Food collaborator` |
| Commit 2 (test) | `b431c24` — `lab4: test the eating rule through the seam with a stub Food` |
| Seam kind | object |
| Enabling point | `GameBoard::set_food(Food*)` (`include/GameBoard.h`), which rebinds the `food_ptr_` member; supported by making `start_round()`, `step()`, `score()`, `running()` and `snake()` public. The collaborator is swapped without editing `step()`. |
| What production code gave up | Encapsulation. `step()`, the score, the running flag and the snake are now public API purely so a test can drive and observe the simulation; `Food` became polymorphic (a vtable and a virtual dispatch on every `pos()`/`spawn()`), and `GameBoard` now reaches its food through a non-owning `Food*`, adding a lifetime assumption that did not exist before. During real play the behaviour and the default object graph are unchanged (the pointer defaults to the owned `food_`), so the only genuine runtime cost is the virtual call. |

The seam also removed duplication: the round setup that used to be copied in `play_game()` and in the game-over restart path is now the single `start_round()`.

---

## 5. The double — [4]

| | |
|---|---|
| What you passed through the seam | stub |
| The method under test | `GameBoard::step()` |

The collaborator (`Food`) is **asked a question** — `pos()` — and its canned coordinates decide whether the head eats; because the test only needs that predetermined answer and never asserts anything about the collaborator's interactions, a stub is the right double rather than a mock. `spawn()` is neutralised to a no-op so the canned position stays fixed across steps.

---

## 6. Two smells in your own tests — [5]

| | Smell | `file:line` | One-line fix |
|---|---|---|---|
| 1 | Assertion roulette | `tests/food_test.cpp:21-25` | Five bare `EXPECT_*` inside a 50-seed loop: a failure names the source line but not the seed. Add `SCOPED_TRACE(seed)`, or collect the positions and assert once against the whole structure. |
| 2 | Eager test | `tests/leaderboard_test.cpp:115-129` | One test asserts unquoted parsing, blank-line skipping and malformed-line skipping at once. Split the three acts into separate tests so a failure names the behaviour that broke. |
