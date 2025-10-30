#include <iostream>
#include <deque>
#include <vector>

struct Point { int r, c; };

class Snake {
public:
    Snake(int sr = 5, int sc = 5) { reset(sr, sc); }
    void reset(int sr, int sc) {
        body_.clear();
        body_.push_back({sr, sc-1});
        body_.push_back({sr, sc});
        body_.push_back({sr, sc+1});
    }
    const std::deque<Point>& body() const { return body_; }

private:
    std::deque<Point> body_;
};

int main() {
    Snake s(5, 5);
    std::cout << "Snake body positions:\n";
    for (auto &p : s.body()) std::cout << "(" << p.r << "," << p.c << ")\n";
    return 0;
}
