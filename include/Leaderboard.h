#ifndef SNAKE_TERRA_LEADERBOARD_H
#define SNAKE_TERRA_LEADERBOARD_H

#include <string>
#include <vector>

using namespace std;

namespace snaketerra {

struct ScoreEntry {
    string name;
    int score;
};

class Leaderboard {
public:
    explicit Leaderboard(const string& path = "leaderboard.txt");

    void load();
    void save();
    void add(const string& name, int score);

    vector<ScoreEntry> top(int n = 3) const;
    vector<ScoreEntry> all() const;

private:
    void sort_and_trim();
    static string sanitize_name(const string& s);

    string path_;
    vector<ScoreEntry> entries_;
};

} // namespace snaketerra

#endif // SNAKE_TERRA_LEADERBOARD_H