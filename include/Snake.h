#ifndef SNAKE_TERRA_SNAKE_H
#define SNAKE_TERRA_SNAKE_H

#include "Point.h"
#include <deque>

using namespace std;

namespace snaketerra {

class Snake {
public:
    Snake();
    void init(int start_r, int start_c);
    void reset(int start_r, int start_c);

    const deque<Point>& body() const;
    Point head() const;

    Dir dir() const;

    void set_dir(Dir d);
    void move();
    void grow();

    bool occupies(const Point& p) const;
    bool collides_with_self() const;

private:
    deque<Point> body_;
    Dir dir_;
    bool grow_next_;
};

} // namespace snaketerra

#endif // SNAKE_TERRA_SNAKE_H