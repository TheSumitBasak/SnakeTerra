# SnakeTerra 🐍

A terminal-based C++ snake game for quenching the never-ending hunger of the snake.

---

Table of contents
- [About](#about)
- [Features / User Stories](#features--user-stories)
- [Prerequisites](#prerequisites)
- [Get started](#get-started)
  - [Clone repository](#clone-repository)
  - [Build (compile)](#build-compile)
  - [Run](#run)
- [Configuration & Controls](#configuration--controls)
- [Leaderboards](#leaderboards)
- [Issues & Bug reports](#issues--bug-reports)
- [Contributing](#contributing)

---

## About

SnakeTerra is a lightweight terminal/console C++ implementation of the classic Snake game. It is intended to be simple, fun, and easy to extend. Play from your terminal, compete for top scores on the leaderboards, and tweak difficulty to sharpen your skills.

---

## Features / User Stories

- As a new user, I want to enter my name so that my results appear on the leaderboards.
- As a user, I want to start the game so that I can play.
- As a user, I want to pause and re-start the game so I can take breaks.
- As a user, I want to change difficulty so I can train or show off my skills.
- As a user, I want to view leaderboards to see current top players.
  - As a user, I want to filter leaderboards by difficulty so I know who is best at which level.

---

## Prerequisites

Before building and running SnakeTerra, make sure you have the following installed:

- A C++ compiler that supports C++11/14/17 (g++ or clang recommended).
  - Example: g++ (GCC) >= 5.0
- make (optional, if a Makefile is provided)
- ncurses development library (if the game uses ncurses for terminal control)
  - On Debian/Ubuntu: sudo apt-get install libncurses5-dev libncursesw5-dev
  - On Fedora: sudo dnf install ncurses-devel
  - On macOS with Homebrew: brew install ncurses
- (Optional) A POSIX-like terminal (Linux, macOS, WSL on Windows). If you are on Windows native, use MSYS2/MinGW or WSL for best compatibility.

Note: If the repository uses a different terminal library (termcap/termios), install the relevant dependencies. Adjust the build commands below if required.

---

## Get started

### Clone repository

```bash
git clone https://github.com/TheSumitBasak/SnakeTerra.git
cd SnakeTerra
```

### Build (compile)

Check if there is a Makefile or build instructions in the repo root. If a Makefile exists, the simplest approach is:

```bash
make
```

If no Makefile exists, you can compile with a generic g++ command. Replace `src/*.cpp` with the actual source files or the main source file if the layout differs:

```bash
g++ -std=c++17 -O2 -Wall -o SnakeTerra src/*.cpp -lncurses
```

Notes:
- Add or remove `-lncurses` depending on whether the project uses ncurses.
- If the project uses an `include/` directory, add `-I include`.
- For multi-file projects you may need to list files explicitly or use a provided build script.

If you prefer to build with clang:

```bash
clang++ -std=c++17 -O2 -Wall -o SnakeTerra src/*.cpp -lncurses
```

### Run

After building, run the binary from the project root or the output directory:

```bash
./SnakeTerra
```

If the executable is placed in `bin/`:

```bash
./bin/SnakeTerra
```

---

## Configuration & Controls

- When you launch the game you will be prompted to enter your name for leaderboards.
- Choose difficulty from the menu (if provided) or pass an argument (if supported).
- Controls (typical):
  - Arrow keys or WASD to move the snake
  - Q to quit (or Esc)
- If your terminal does not respond to arrow keys, try using WASD or run in a compatible terminal emulator.

Check source code or the in-game help screen for exact key bindings and command-line options.

---

## Leaderboards

- Scores are stored locally (check the repo for a `leaderboard` file or similar).
- You can view leaderboards from the main menu.
- Select difficulty while viewing leaderboards to filter the results by difficulty level.
- If you want to reset leaderboards, look for the leaderboard file (commonly JSON, CSV, or plain text) and delete or edit it.

---

## Issues & Bug reports

Found a bug or want a new feature? Please open an issue:

https://github.com/TheSumitBasak/SnakeTerra/issues

When filing an issue, include:
- A clear title and description.
- Steps to reproduce.
- What you expected vs what happened.
- Your OS and terminal (e.g., Ubuntu 22.04 + GNOME Terminal, macOS 12 + iTerm2).
- Compiler and version (e.g., g++ 12.2).
- Any error messages or logs (copy/paste or attach screenshots).

This helps us reproduce and fix problems faster.

---

## Contributing

Contributions are welcome! Typical ways to contribute:
- Open issues for bugs and feature requests.
- Fork the repo and open pull requests with small, focused changes.
- Add or improve tests and documentation.
- Improve portability (Windows/FreeBSD/macOS) or add CI.

When opening a PR:
- Describe the change, why it’s needed, and any limitations.
- Keep commits focused and add tests if applicable.

---