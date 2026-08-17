#include "Food.h"
#include "Snake.h"
#include <vector>
#include <cstdlib>

using namespace std;

namespace snaketerra {

Food::Food() : pos_{-1, -1} {}

Point Food::pos() const { return pos_; }

void Food::spawn(int rows, int cols, const vector<Snake>& snakes) {
    vector<Point> empties;
    empties.reserve(rows * cols);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Point p{r, c};
            bool occupied = false;
            for (const auto& s : snakes) {
                if (s.occupies(p)) {
                    occupied = true;
                    break;
                }
            }
            if (!occupied) empties.push_back(p);
        }
    }
    if (empties.empty()) {
        pos_ = {-1, -1};
        return;
    }
    pos_ = empties[rand() % empties.size()];
}

} // namespace snaketerra