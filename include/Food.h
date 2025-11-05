#ifndef SNAKE_TERRA_FOOD_H
#define SNAKE_TERRA_FOOD_H

#include "Point.h"

using namespace std;

namespace snaketerra {

class Snake; // forward

class Food {
public:
    Food();
    Point pos() const;
    void spawn(int rows, int cols, const Snake& snake);

private:
    Point pos_;
};

} // namespace snaketerra

#endif // SNAKE_TERRA_FOOD_H