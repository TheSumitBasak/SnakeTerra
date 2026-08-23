# Ubiquitous Language

## Gameplay Entities & Grid

| Term | Definition | Aliases to avoid | In code |
| --- | --- | --- | --- |
| **Snake** | The character controlled by the player that moves around the grid and grows when it eats food. | Serpent, worm, player body | `Snake` — `include/Snake.h:11` |
| **Food** | An item placed on an empty grid cell that the snake can eat to increase the score and grow. | Item, pellet, apple, bonus | `Food` — `include/Food.h:12` |
| **Grid** | The rectangular playing area made up of rows and columns where the snake and food are placed. | Board, gameboard, arena, map | `GameBoard` — `include/GameBoard.h:19` ⚠ |
| **Cell** | One position on the grid identified by a row and column. | Point, spot, location, tile | `Point` — `include/Point.h:10` ⚠ |
| **Direction** | The direction in which the snake moves: Up, Down, Left, or Right. | Dir, heading, orientation | `Dir` — `include/Point.h:16` ⚠ |

## Game State & Mechanics

| Term | Definition | Aliases to avoid | In code |
| --- | --- | --- | --- |
| **Player** | The person controlling a snake during the game. | User, client, account | `name` — `include/Leaderboard.h:12` / `"Player"` — `src/Leaderboard.cpp:71` ⚠ |
| **Game Session** | One complete game, starting when the game begins and ending when the game is over. | Play loop, match, game run | `running_` — `include/GameBoard.h:58` |
| **Tick** | One update of the game where the snake moves and the game checks things like collisions and food. | Step, frame, update cycle, delay | `step()` — `include/GameBoard.h:41` / `src/GameBoard.cpp:353` ⚠ |
| **Collision** | When the snake's head hits a wall or its own body and the game ends. | Crash, hit, game over event | `collides_with_self()` — `include/Snake.h:27` / boundary check — `src/GameBoard.cpp:356` |
| **Difficulty Level** | A setting that controls how quickly the game updates and therefore how fast the snake moves. | Speed, game mode, handicap | `Difficulty` — `include/GameBoard.h:17` |

## Leaderboard & High Scores

| Term | Definition | Aliases to avoid | In code |
| --- | --- | --- | --- |
| **Leaderboard** | A saved list of high scores arranged by score. | Scoreboard, high score table, ranking list | `Leaderboard` — `include/Leaderboard.h:16` |
| **High Score Record** | A saved record containing a player's name and score. | Score entry, record, result | `ScoreEntry` — `include/Leaderboard.h:11` ⚠ |

## Relationships

- A **Player** controls a **Snake** during a **Game Session**.
- A **Snake** moves around the **Grid** one **Cell** at a time in a chosen **Direction**.
- A **Snake** grows and the **Player**'s score increases when the snake eats **Food**.
- A **Collision** happens when the **Snake** hits a wall or its own body.
- A **Collision** ends the **Game Session**.
- At the end of a **Game Session**, the final score can be saved as a **High Score Record** in the **Leaderboard**.
- The **Difficulty Level** controls the time between **Ticks**, which affects the speed of the game.

## Example dialogue

> **Dev:** "When does the **Game Session** end?"
>
> **Domain expert:** "It ends when the **Snake** hits a wall or its own body."
>
> **Dev:** "What happens when the **Snake** eats **Food**?"
>
> **Domain expert:** "The **Player**'s score increases and the **Snake** grows. A new **Food** item is then placed on an empty **Cell**."
>
> **Dev:** "What happens to the score when the game ends?"
>
> **Domain expert:** "The final score can be saved as a **High Score Record** in the **Leaderboard**."

## Flagged ambiguities

- **"User" vs "Player"**: `README.md:30` uses "user", while the game code uses "Player". For this project, **Player** is clearer because it means the person playing the game.
- **"GameBoard" vs "Grid"**: The code uses `GameBoard` for more than just the playing area. It also handles screens, menus, the game loop, and score display. **Grid** is clearer when talking only about the playing area.
- **"Point" vs "Cell"**: The code calls a grid position a `Point`, but **Cell** is easier to understand as a position on the game grid.
- **"Step" vs "Tick"**: The code uses `step()` for a game update. **Tick** is used here to describe one complete game update.

## Code drift

| Canonical term | Called in code | Location | Note |
| --- | --- | --- | --- |
| **Grid** | `GameBoard` | `include/GameBoard.h:19` | `GameBoard` handles the grid as well as other game and display tasks. |
| **Cell** | `Point` | `include/Point.h:10` | The code uses `Point` for a position on the grid. |
| **Direction** | `Dir` | `include/Point.h:16` | The code uses the shorter name `Dir`. |
| **Player** | `ScoreEntry::name` / `"Player"` | `include/Leaderboard.h:12`, `src/Leaderboard.cpp:71` | There is no separate `Player` object; the player's name is stored with the score. |
| **High Score Record** | `ScoreEntry` | `include/Leaderboard.h:11` | The code uses `ScoreEntry` instead of the more descriptive domain term. |
| **Tick** | `step()` | `include/GameBoard.h:41`, `src/GameBoard.cpp:353` | The code calls the game update `step()` instead of `tick()`. |