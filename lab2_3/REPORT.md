# Lab 2_3 — Group A__

> Copy this file to `lab2_3/REPORT.md` in your fork and fill it in. Keep the eight
> headings exactly as they are — they are the marking scheme, in order. Delete this
> quote block and every `` when you are done.

---

## 1. Tool and install route — [3]

| | |
|---|---|
| Agent used for run 2 | Gemini 3.6  (Claude Code / Codex CLI / Gemini CLI / other — name it) |
| `ubiquitous-language` install route | manually installed from the course repository (`~/.claude/skills` / project `.claude/skills` / pasted `SKILL.md` / other) |
| `refactoring/` pack install route | manually installed |

If anything would not install, say what failed here. The paste fallback is a documented
route and costs no marks.

Both skills were installed manually and no installation errors occurred.

---

## 2. What I changed in the glossary — [4]

The generated file is at `lab2_3/UBIQUITOUS_LANGUAGE.md`. Three or four lines here on what
you corrected and why: terms it invented, definitions it got wrong, ambiguities it missed.

I simplified several definitions to make the terminology easier to understand and more consistent with the SnakeTerra project. I changed terms such as `Point` to the domain term **Cell** and `Dir` to **Direction**, while keeping the original code names as references. I also clarified the ambiguity between **Grid** and **GameBoard**, since `GameBoard` handles both the playing area and other game functions. I removed or corrected terms that were too generic or not clearly supported by the code.

---

## 3. Smell delta — [6]

Reports: `lab2_3/audits/main.md` (the code as you received it) and `lab2_3/audits/lab1-head.md`
(after your Lab-1 PR).

| | count | representative site (`file:line`) |
|---|---|---|
| Smells my Lab-1 PR **introduced** | 0 | — |
| Smells my Lab-1 PR **left untouched** | 15 | `include/GameBoard.h:20` |
| Smells my Lab-1 PR **removed** | 0 | — 

The third row will probably be zero. Leave it in.
---

## 4. Rejected candidates — [6]

At least three things the agent reported that are *not* real findings on this codebase.

| smell reported | `file:line` | why it does not hold |
|---|---|---|

| **Dead Code** | `include/Snake.h:14` | C4 | `Snake::init()` only calls `reset()`, but it is still part of the existing initialization interface, so removing it is not necessary for this project. |
| **Data Class** | `include/Leaderboard.h:11` | C3 | `ScoreEntry` mainly stores a player's name and score. This is reasonable for a simple record used by the leaderboard. |
| **Primitive Obsession** | `include/Point.h:10` | C3 | `Point` uses two integers for a row and column, which is simple and appropriate for representing a grid position in this small game. |


---

## 5. Commit map — [7]

Run `lab2_3/check-lab2_3.sh` and paste the table it prints.

| # | sha | subject | what it is |
|---|---|---|---|
| 1 | 408a607 | Commit 1 | glossary |
| 2 | 54e654c | Commit 2 | smell report |
| 3 | 5c1aeb4 | Commit 3: Refactoring | the refactor alone |
| 4 | 910d83a | Commit 4: Add Second Player | the feature alone |

---

## 6. Two-run measurement — [4]

Run 1 is your Lab-1 branch — the numbers you already reported. Run 2 is commit 4 alone.

| | Run 1 (Lab 1) | Run 2 (commit 4) |
|---|---|---|
| Smells introduced | 0 | 0 |
| Lines changed, `git diff --shortstat -w` | 260 | 115 |
| Lines changed, **raw** (no `-w`) | 260 | 115 |
| Functions reached | 6 | 6 |
| Prompts to working code | 5 | 5 |
| Wall-clock time | not recorded | not recorded |

Commit 3 (the refactor) on its own: 53 lines `-w`, 69 raw.

`check-lab2_3.sh` prints the four line-count numbers for run 2. Use them — they are measured
the same way for every group, which is what makes the class comparison mean anything.

---

## 7. Analysis Q1–Q2 — [5]

**Q1. Which smell did commit 3 actually fix?** Name it from your section 3 report. What was
expensive before, what does it cost now.


Commit 3 did not completely remove any of the named smells from the smell report. The main change was reducing the repeated single-snake assumption by storing snakes in `std::vector<Snake>`. Before the refactor, adding another snake required changing several places that assumed there was only one snake. After the refactor, the snakes are kept in one container and `Food::spawn()` can work with the collection. So the refactor reduced the cost of extending the game, but the audit still reports the same smells such as Large Class and Long Method.

**Q2. Compare commit 4 to your Lab-1 diff.** Same feature, same codebase. What changed in
the cost and what did not? If it got worse, say so and explain — that marks the same.

Commit 4 was smaller than the Lab-1 implementation. The Lab-1 change was 260 lines raw, while commit 4 was 115 lines raw. The main reason is that commit 3 had already changed the design to use `std::vector<Snake>`, so commit 4 did not need to introduce another separate snake representation or change the food system again. The feature still reached several parts of `GameBoard`, including initialization, the game loop, input, scoring, and game-over handling. The number of functions reached remained about the same at six.


---

## 8. Analysis Q3–Q4 — [5]

**Q3. Go back through your Lab-1 `LLM-LOG.md`. Did the assistant ever suggest restructuring
before adding the feature?** Quote it if it did. If it did not, what would have had to be
different in your prompt?

No. The five prompts in the Lab-1 LLM log focused directly on implementing the second player. They asked about adding a second snake, updating food spawning, keyboard controls, collision handling, and restart logic. The assistant did not first suggest changing the single-snake design. A better prompt would have asked the assistant to first inspect the code for assumptions that a single snake exists and suggest a small design change before implementing the second player.

**Q4. How do you know commit 3 did not change behaviour?** Answer honestly. Most of you will
find that you do not know. Say that plainly if it is true, and describe what you would have
needed in order to actually know.

We do not know for certain. We checked the code and kept the existing single-player controls and flow while changing the internal representation from one snake to a vector. However, code inspection alone does not prove that behaviour stayed identical. To actually know, we would need tests covering the existing single-player behaviour and run the same tests before and after commit 3, including movement, food, collisions, scoring, restart, and game-over behaviour.

---

Sections 7 and 8 together: **500 words maximum.**

## If you did not finish

Say so here and lose no marks. What you got to, where it broke, what you tried.

N/A — the required glossary, smell reports, refactoring, feature implementation, commit map, measurements, and analysis were completed.

---