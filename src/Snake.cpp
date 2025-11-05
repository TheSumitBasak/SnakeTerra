#include "Snake.h"
#include <algorithm>

using namespace std;

namespace snaketerra {

Snake::Snake() {
    reset(0, 0);
}

void Snake::init(int start_r, int start_c) {
    reset(start_r, start_c);
}

void Snake::reset(int start_r, int start_c) {
    body_.clear();
    // horizontal line length 3, moving right
    body_.push_back({start_r, start_c - 1});
    body_.push_back({start_r, start_c});
    body_.push_back({start_r, start_c + 1});
    dir_ = Dir::RIGHT;
    grow_next_ = false;
}

const deque<Point>& Snake::body() const { return body_; }
Point Snake::head() const { return body_.back(); }
Dir Snake::dir() const { return dir_; }

void Snake::set_dir(Dir d) {
    // Prevent 180-degree reversal
    if ((dir_ == Dir::UP && d == Dir::DOWN) ||
        (dir_ == Dir::DOWN && d == Dir::UP) ||
        (dir_ == Dir::LEFT && d == Dir::RIGHT) ||
        (dir_ == Dir::RIGHT && d == Dir::LEFT)) {
        return;
    }
    dir_ = d;
}

void Snake::move() {
    Point h = head();
    Point nh = h;
    switch (dir_) {
        case Dir::UP:    nh.r -= 1; break;
        case Dir::DOWN:  nh.r += 1; break;
        case Dir::LEFT:  nh.c -= 1; break;
        case Dir::RIGHT: nh.c += 1; break;
    }
    body_.push_back(nh);
    if (!grow_next_) body_.pop_front();
    else grow_next_ = false;
}

void Snake::grow() { grow_next_ = true; }

bool Snake::occupies(const Point& p) const {
    return find(body_.begin(), body_.end(), p) != body_.end();
}

bool Snake::collides_with_self() const {
    Point h = head();
    for (size_t i = 0; i + 1 < body_.size(); ++i) {
        if (body_[i] == h) return true;
    }
    return false;
}

} // namespace snaketerra