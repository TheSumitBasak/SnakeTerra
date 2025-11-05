#include "Leaderboard.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

namespace snaketerra {

Leaderboard::Leaderboard(const string& path) : path_(path) { load(); }

void Leaderboard::load() {
    entries_.clear();
    ifstream ifs(path_);
    if (!ifs) return;
    string line;
    while (getline(ifs, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string name;
        int score;
        if (iss >> quoted(name) >> score) {
            entries_.push_back({name, score});
        } else {
            istringstream iss2(line);
            if (iss2 >> name >> score) entries_.push_back({name, score});
        }
    }
    sort_and_trim();
}

void Leaderboard::save() {
    ofstream ofs(path_, ios::trunc);
    if (!ofs) return;
    for (auto &e : entries_) {
        ofs << quoted(e.name) << " " << e.score << "\n";
    }
}

void Leaderboard::add(const string& name, int score) {
    string clean = sanitize_name(name);
    entries_.push_back({clean, score});
    sort_and_trim();
    save();
}

vector<ScoreEntry> Leaderboard::top(int n) const {
    vector<ScoreEntry> out;
    for (size_t i = 0; i < entries_.size() && (int)i < n; ++i) out.push_back(entries_[i]);
    return out;
}

vector<ScoreEntry> Leaderboard::all() const { return entries_; }

void Leaderboard::sort_and_trim() {
    sort(entries_.begin(), entries_.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        if (a.score != b.score) return a.score > b.score;
        return a.name < b.name;
    });
    if (entries_.size() > 200) entries_.resize(200);
}

string Leaderboard::sanitize_name(const string& s) {
    string out;
    for (char ch : s) {
        if (isalnum((unsigned char)ch) || ch == '_' || ch == '-') out.push_back(ch);
        if (out.size() >= 16) break;
    }
    if (out.empty()) out = "Player";
    return out;
}

} // namespace snaketerra