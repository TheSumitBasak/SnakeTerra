#include <gtest/gtest.h>

#include "GameBoard.h"

using namespace snaketerra;

namespace {

// Test double for the Food collaborator. It answers pos() from a canned value
// (a stub) and treats spawn() as a no-op so the canned value stays put.
class StubFood : public Food {
public:
    explicit StubFood(Point p) : p_(p) {}

    Point pos() const override { return p_; }
    void spawn(int, int, const Snake&) override {}

private:
    Point p_;
};

} // namespace

// Rule 1: eating a piece of food adds one point to the score.
TEST(EatFoodRule, HeadOnFoodAddsOnePoint) {
    GameBoard gb(20, 30);
    StubFood food({10, 17}); // one cell ahead of the centred head
    gb.set_food(&food);
    gb.start_round();

    gb.step(); // head moves onto the food

    EXPECT_EQ(gb.score(), 1);
    EXPECT_TRUE(gb.running());
}

// Rule 1: the growth is realised on the following move.
TEST(EatFoodRule, GrowthIsRealisedOnNextMove) {
    GameBoard gb(20, 30);
    StubFood food({10, 17});
    gb.set_food(&food);
    gb.start_round();

    gb.step(); // eat; length is unchanged until the next move
    const size_t length_after_eating = gb.snake().body().size();

    gb.step(); // the pending growth is applied

    EXPECT_EQ(gb.snake().body().size(), length_after_eating + 1);
}

// Control: a head that misses the food scores nothing and does not grow.
TEST(EatFoodRule, HeadMissingFoodAddsNothing) {
    GameBoard gb(20, 30);
    StubFood food({0, 0}); // nowhere near the head
    gb.set_food(&food);
    gb.start_round();

    gb.step();
    gb.step();

    EXPECT_EQ(gb.score(), 0);
    EXPECT_EQ(gb.snake().body().size(), 3u);
}
