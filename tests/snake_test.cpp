#include <gtest/gtest.h>

#include "Snake.h"

using namespace snaketerra;

// Rule 2 (direction): a 180-degree reversal is ignored, perpendicular turns are allowed.
TEST(SnakeDirection, ReversalIsIgnored) {
    Snake s;
    s.reset(0, 0); // facing RIGHT

    s.set_dir(Dir::LEFT);
    EXPECT_EQ(s.dir(), Dir::RIGHT);

    s.set_dir(Dir::UP); // allowed
    ASSERT_EQ(s.dir(), Dir::UP);
    s.set_dir(Dir::DOWN);
    EXPECT_EQ(s.dir(), Dir::UP);
}

TEST(SnakeDirection, PerpendicularTurnsAreAllowed) {
    Snake s;
    s.reset(5, 5); // facing RIGHT

    s.set_dir(Dir::UP);
    EXPECT_EQ(s.dir(), Dir::UP);
    s.set_dir(Dir::LEFT);
    EXPECT_EQ(s.dir(), Dir::LEFT);
    s.set_dir(Dir::DOWN);
    EXPECT_EQ(s.dir(), Dir::DOWN);
    s.set_dir(Dir::RIGHT);
    EXPECT_EQ(s.dir(), Dir::RIGHT);
}

TEST(SnakeInit, ResetCreatesLengthThreeFacingRight) {
    Snake s;
    s.reset(5, 5);

    ASSERT_EQ(s.body().size(), 3u);
    EXPECT_EQ(s.head(), (Point{5, 6}));
    EXPECT_EQ(s.dir(), Dir::RIGHT);
    EXPECT_EQ(s.body()[0], (Point{5, 4}));
    EXPECT_EQ(s.body()[1], (Point{5, 5}));
    EXPECT_EQ(s.body()[2], (Point{5, 6}));
}

TEST(SnakeMove, MovesHeadAndDropsTail) {
    Snake s;
    s.reset(5, 5);

    s.move();

    EXPECT_EQ(s.head(), (Point{5, 7}));
    EXPECT_EQ(s.body().size(), 3u);
    EXPECT_FALSE(s.occupies(Point{5, 4})); // old tail is gone
}

TEST(SnakeGrow, GrowThenMoveAddsOneSegment) {
    Snake s;
    s.reset(5, 5);

    s.grow();
    s.move();

    EXPECT_EQ(s.body().size(), 4u);
    EXPECT_EQ(s.head(), (Point{5, 7}));
    EXPECT_TRUE(s.occupies(Point{5, 4})); // old tail retained
}

TEST(SnakeOccupies, ReportsMembership) {
    Snake s;
    s.reset(5, 5);

    EXPECT_TRUE(s.occupies(Point{5, 4}));
    EXPECT_TRUE(s.occupies(Point{5, 6}));
    EXPECT_FALSE(s.occupies(Point{0, 0}));
}

// Rule 4 (self collision): predicate part. A straight snake is fine...
TEST(SnakeSelfCollision, StraightSnakeDoesNotCollide) {
    Snake s;
    s.reset(5, 5);
    s.move();
    s.move();

    EXPECT_FALSE(s.collides_with_self());
}

// ...but a snake long enough to close a tight loop does collide.
TEST(SnakeSelfCollision, DetectsTightLoop) {
    Snake s;
    s.reset(1, 1); // head (1,2) facing RIGHT

    s.grow();
    s.move(); // head (1,3), length 4
    s.grow();
    s.move(); // head (1,4), length 5

    s.set_dir(Dir::DOWN);
    s.move(); // head (2,4)
    s.set_dir(Dir::LEFT);
    s.move(); // head (2,3)
    s.set_dir(Dir::UP);
    s.move(); // head (1,3) - already occupied

    EXPECT_TRUE(s.collides_with_self());
}
