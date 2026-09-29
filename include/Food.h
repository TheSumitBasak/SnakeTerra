#ifndef SNAKE_TERRA_FOOD_H
#define SNAKE_TERRA_FOOD_H

#include "Point.h"

using namespace std;

namespace snaketerra {

class Snake; // forward

class Food {
public:
    Food();
    virtual ~Food() = default;

    // Virtual so a test can substitute a collaborator (object seam).
    virtual Point pos() const;
    virtual void spawn(int rows, int cols, const Snake& snake);

private:
    Point pos_;
};

} // namespace snaketerra

#endif // SNAKE_TERRA_FOOD_H