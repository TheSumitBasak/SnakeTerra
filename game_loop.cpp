#include <iostream>
#include <deque>
#include <chrono>
#include <thread>

struct Point { int r, c; };

class Snake {
public:
    Snake(int sr=5,int sc=5) { reset(sr,sc); }
    void reset(int sr,int sc) {
        body_.clear();
        body_.push_back({sr,sc-1});
        body_.push_back({sr,sc});
        body_.push_back({sr,sc+1});
        dir_r = 0; dir_c = 1;
    }
    void step() {
        Point h = body_.back();
        Point nh{h.r + dir_r, h.c + dir_c};
        body_.push_back(nh);
        body_.pop_front();
    }
    const std::deque<Point>& body() const { return body_; }
private:
    std::deque<Point> body_;
    int dir_r, dir_c;
};

int main() {
    Snake snake;
    for (int tick = 0; tick < 10; ++tick) {
        snake.step();
        auto h = snake.body().back();
        std::cout << "Tick " << tick << " head=(" << h.r << "," << h.c << ")\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return 0;
}