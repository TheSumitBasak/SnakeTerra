#ifndef SNAKE_TERRA_POINT_H
#define SNAKE_TERRA_POINT_H

#include <cstddef>

using namespace std;

namespace snaketerra {

struct Point {
    int r;
    int c;
    bool operator==(const Point& o) const noexcept { return r == o.r && c == o.c; }
};

enum class Dir { UP, DOWN, LEFT, RIGHT };

} // namespace snaketerra

#endif // SNAKE_TERRA_POINT_H