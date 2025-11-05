CXX = g++
CXXFLAGS = -std=c++17 -O2 -Iinclude
LDLIBS = -lncurses

SRC = src/main.cpp src/Snake.cpp src/Food.cpp src/Leaderboard.cpp src/GameBoard.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = snake.out

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

clean:
	rm -f $(TARGET) $(OBJ)