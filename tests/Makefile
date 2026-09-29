CXX ?= g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0 --coverage -I../include
LDLIBS = -lgtest -lgtest_main -pthread

BASE_SRC = ../src/Snake.cpp ../src/Food.cpp ../src/Leaderboard.cpp
BASE_TESTS = snake_test.cpp food_test.cpp leaderboard_test.cpp
BASE_BIN = base_tests

SEAM_SRC = $(BASE_SRC) ../src/GameBoard.cpp
SEAM_TESTS = $(BASE_TESTS) gameboard_seam_test.cpp
SEAM_BIN = seam_tests

.PHONY: all base seam test seam-test coverage clean

all: base seam

base: $(BASE_BIN)
seam: $(SEAM_BIN)

$(BASE_BIN): $(BASE_SRC) $(BASE_TESTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

$(SEAM_BIN): $(SEAM_SRC) $(SEAM_TESTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS) -lncurses

test: base
	./$(BASE_BIN)

seam-test: seam
	./$(SEAM_BIN)

coverage: base
	./$(BASE_BIN)
	gcovr -r .. --filter '.*/src/(Snake|Food|Leaderboard)\.cpp' --print-summary

clean:
	rm -f $(BASE_BIN) $(SEAM_BIN) *.gcov
	find . .. -name '*.gcda' -delete
	find . .. -name '*.gcno' -delete
