#include <gtest/gtest.h>

#include <cstdlib>

#include "Food.h"
#include "Snake.h"

using namespace snaketerra;

// Rule 5 (food placement): food always lands on an in-bounds cell that the snake
// does not occupy.
TEST(FoodSpawn, StaysInBoundsAndOffSnake) {
    Snake s;
    s.reset(5, 5);

    for (int seed = 0; seed < 50; ++seed) {
        srand(seed);
        Food f;
        f.spawn(10, 10, s);

        EXPECT_GE(f.pos().r, 0);
        EXPECT_LT(f.pos().r, 10);
        EXPECT_GE(f.pos().c, 0);
        EXPECT_LT(f.pos().c, 10);
        EXPECT_FALSE(s.occupies(f.pos()));
    }
}

TEST(FoodDefault, StartsOffBoard) {
    Food f;
    EXPECT_EQ(f.pos(), (Point{-1, -1}));
}

TEST(FoodSpawn, FullBoardHasNoFreeCell) {
    // A 1x3 board fully covered by the snake leaves nowhere to spawn.
    Snake s;
    s.reset(0, 1); // occupies (0,0), (0,1), (0,2)

    Food f;
    srand(1);
    f.spawn(1, 3, s);

    EXPECT_EQ(f.pos(), (Point{-1, -1}));
}
