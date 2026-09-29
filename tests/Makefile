CXX ?= g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0 --coverage -I../include
LDLIBS = -lgtest -lgtest_main -pthread

BASE_SRC = ../src/Snake.cpp ../src/Food.cpp ../src/Leaderboard.cpp
BASE_TESTS = snake_test.cpp food_test.cpp leaderboard_test.cpp
BASE_BIN = base_tests

.PHONY: all base test coverage clean

all: base

base: $(BASE_BIN)

$(BASE_BIN): $(BASE_SRC) $(BASE_TESTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

test: base
	./$(BASE_BIN)

coverage: base
	./$(BASE_BIN)
	gcovr -r .. --filter '.*/src/(Snake|Food|Leaderboard)\.cpp' --print-summary

clean:
	rm -f $(BASE_BIN) *.gcov
	find . .. -name '*.gcda' -delete
	find . .. -name '*.gcno' -delete
