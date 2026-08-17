# Ubiquitous Language

## Gameplay Entities & Grid

| Term | Definition | Aliases to avoid | In code |
| --- | --- | --- | --- |
| **Snake** | A controllable entity composed of an ordered sequence of connected grid cells that moves in a cardinal direction and grows upon eating food. | Serpent, worm, player body | `Snake` — `include/Snake.h:11` |
| **Food** | An item spawned on an unoccupied grid cell that increases player score and snake length when eaten. | Item, pellet, apple, bonus | `Food` — `include/Food.h:12` |
| **Grid** | A bounded two-dimensional playing field defined by rows and columns where game entities reside and move. | Board, gameboard, arena, map | `GameBoard` — `include/GameBoard.h:19` ⚠ |
| **Cell** | A discrete coordinate position on the grid specified by a row and column index. | Point, spot, location, tile | `Point` — `include/Point.h:10` ⚠ |
| **Direction** | One of four cardinal orientations (Up, Down, Left, Right) governing the movement vector of the snake. | Dir, heading, orientation | `Dir` — `include/Point.h:16` ⚠ |

## Game State & Mechanics

| Term | Definition | Aliases to avoid | In code |
| --- | --- | --- | --- |
| **Player** | A human participant who controls a snake during a game session and records high scores. | User, client, account | `name` — `include/Leaderboard.h:12` / `"Player"` — `src/Leaderboard.cpp:71` ⚠ |
| **Game Session** | An active gameplay instance starting at game initialization and continuing until a collision occurs. | Play loop, match, game run | `running_` — `include/GameBoard.h:58` |
| **Tick** | A single discrete update cycle in which movement, input processing, collisions, and food consumption are evaluated. | Step, frame, update cycle, delay | `step()` — `include/GameBoard.h:41` / `src/GameBoard.cpp:353` ⚠ |
| **Collision** | An event triggered when the snake's head hits a grid boundary wall or its own body segment, ending the game session. | Crash, hit, game over event | `collides_with_self()` — `include/Snake.h:27` / boundary check — `src/GameBoard.cpp:356` |
| **Difficulty Level** | A game setting that determines tick speed intervals to adjust gameplay speed and challenge. | Speed, game mode, handicap | `Difficulty` — `include/GameBoard.h:17` |

## Leaderboard & High Scores

| Term | Definition | Aliases to avoid | In code |
| --- | --- | --- | --- |
| **Leaderboard** | A persistent store maintaining historical high score records sorted by score. | Scoreboard, high score table, ranking list | `Leaderboard` — `include/Leaderboard.h:16` |
| **High Score Record** | A persistent entry associating a player's name with their achieved score. | Score entry, record, result | `ScoreEntry` — `include/Leaderboard.h:11` ⚠ |

## Relationships

- A **Player** controls one **Snake** during a **Game Session**.
- A **Snake** moves across a **Grid** composed of multiple **Cells** in a specified **Direction**.
- A **Snake** grows by consuming **Food** spawned on an unoccupied **Cell**.
- A **Collision** occurs when a **Snake** head intersects a **Grid** boundary wall or its own body segment, terminating the **Game Session**.
- A **Game Session** records a final score that can be saved as a **High Score Record** on the **Leaderboard**.
- The **Difficulty Level** determines the time interval between consecutive **Ticks** in a **Game Session**.

## Example dialogue

> **Dev:** "Should the **Game Session** end immediately if the **Snake** attempts a 180-degree turn into itself?"
> **Domain expert:** "No — opposite **Direction** inputs are ignored. A **Collision** only occurs when the **Snake**'s head moves into a **Cell** occupied by its own body segment or a **Grid** boundary wall during a **Tick**."
> **Dev:** "Understood. When the **Snake** consumes **Food**, does the body grow immediately on that same **Tick**?"
> **Domain expert:** "Eating **Food** increments the **Player**'s score, causes the **Snake** to grow on the following **Tick**, and spawns a new **Food** item on an unoccupied **Cell**."
> **Dev:** "When saving scores to the **Leaderboard**, do we save a **High Score Record** for every finished **Game Session**?"
> **Domain expert:** "Yes, prompt the **Player** for their name at the end of the **Game Session**, then insert the **High Score Record** into the **Leaderboard**, which keeps the entries sorted."

## Flagged ambiguities

- **"User" vs "Player"**: `README.md:30` refers to players as "user" ("As a new user, I want to enter my name..."), whereas conversation and fallback code use "Player" (`src/Leaderboard.cpp:71`). **Recommendation**: Canonicalize on **Player** for game participants and high score entities, reserving "User" strictly for terminal/system interface interactions.
- **"GameBoard" as Grid vs Application Controller**: In domain terminology, **Grid** refers to the 2D arena field (`rows_` x `cols_`). In code (`include/GameBoard.h:19`), `GameBoard` is an overloaded class managing spatial dimensions, ncurses terminal screens, menus, game tick loops, and score popups. **Recommendation**: Separate spatial **Grid** domain representation from system UI controller logic.
- **"Point" vs "Cell"**: Code uses generic geometry identifier `Point` (`include/Point.h:10`) for grid coordinates. In domain terms, a location on the grid is a **Cell**. **Recommendation**: Adopt **Cell** as the canonical domain term for discrete grid coordinates.
- **"Step" vs "Tick"**: Code defines `step()` (`include/GameBoard.h:41`) and tick delay timers (`src/GameBoard.cpp:271`). Domain language uses **Tick** to denote discrete atomic update cycles. **Recommendation**: Standardize on **Tick** for time progression units.

## Code drift

| Canonical term | Called in code | Location | Note |
| --- | --- | --- | --- |
| **Grid** | `GameBoard` | `include/GameBoard.h:19` | Code overloads spatial grid concept with UI menu, ncurses rendering, and game loop controller |
| **Cell** | `Point` | `include/Point.h:10` | Code uses generic geometric coordinate `Point` with `r`/`c` fields for 2D grid cells |
| **Direction** | `Dir` | `include/Point.h:16` | Code uses abbreviated identifier `Dir` for cardinal directions |
| **Player** | `ScoreEntry::name` / `"Player"` | `include/Leaderboard.h:12`, `src/Leaderboard.cpp:71` | Code lacks explicit `Player` entity, storing only string `name` inside `ScoreEntry` |
| **High Score Record** | `ScoreEntry` | `include/Leaderboard.h:11` | Code uses implementation-focused struct name `ScoreEntry` |
| **Tick** | `step()` | `include/GameBoard.h:41`, `src/GameBoard.cpp:353` | Code names update step function `step()` rather than domain term `tick` |
