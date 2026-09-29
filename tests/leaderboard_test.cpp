#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "Leaderboard.h"

using namespace snaketerra;

namespace {

std::string temp_path(const std::string& name) {
    return "lab4_" + name + ".txt";
}

struct TempFile {
    explicit TempFile(const std::string& name) : path(temp_path(name)) {
        std::remove(path.c_str());
    }
    ~TempFile() { std::remove(path.c_str()); }
    std::string path;
};

} // namespace

TEST(Leaderboard, StartsEmptyWhenFileMissing) {
    TempFile f("missing");
    Leaderboard lb(f.path);
    EXPECT_TRUE(lb.all().empty());
}

TEST(Leaderboard, AddSortsByScoreDescending) {
    TempFile f("sort");
    Leaderboard lb(f.path);

    lb.add("alice", 10);
    lb.add("bob", 30);
    lb.add("carol", 20);

    auto top = lb.top(3);
    ASSERT_EQ(top.size(), 3u);
    EXPECT_EQ(top[0].name, "bob");
    EXPECT_EQ(top[1].name, "carol");
    EXPECT_EQ(top[2].name, "alice");
}

TEST(Leaderboard, TiesBrokenByNameAscending) {
    TempFile f("ties");
    Leaderboard lb(f.path);

    lb.add("zoe", 5);
    lb.add("amy", 5);

    auto all = lb.all();
    ASSERT_EQ(all.size(), 2u);
    EXPECT_EQ(all[0].name, "amy");
    EXPECT_EQ(all[1].name, "zoe");
}

TEST(Leaderboard, PersistsAcrossInstances) {
    TempFile f("persist");
    {
        Leaderboard lb(f.path);
        lb.add("dave", 42);
    }

    Leaderboard reloaded(f.path);
    auto all = reloaded.all();
    ASSERT_EQ(all.size(), 1u);
    EXPECT_EQ(all[0].name, "dave");
    EXPECT_EQ(all[0].score, 42);
}

TEST(Leaderboard, TopRespectsLimit) {
    TempFile f("limit");
    Leaderboard lb(f.path);
    for (int i = 0; i < 5; ++i) lb.add("p" + std::to_string(i), i);

    EXPECT_EQ(lb.top(2).size(), 2u);
    EXPECT_EQ(lb.top(2)[0].score, 4);
}

TEST(Leaderboard, SanitizesNamesToAlnumUnderscoreDash) {
    TempFile f("sanitize");
    Leaderboard lb(f.path);

    lb.add("a b!c@d#e$f%g^h&i*j_k-l", 1);

    auto all = lb.all();
    ASSERT_EQ(all.size(), 1u);
    EXPECT_EQ(all[0].name, "abcdefghij_k-l");
}

TEST(Leaderboard, EmptySanitizedNameBecomesPlayer) {
    TempFile f("empty");
    Leaderboard lb(f.path);

    lb.add("!!!", 1);

    auto all = lb.all();
    ASSERT_EQ(all.size(), 1u);
    EXPECT_EQ(all[0].name, "Player");
}

TEST(Leaderboard, TrimsToTwoHundredEntries) {
    TempFile f("trim");
    Leaderboard lb(f.path);
    for (int i = 0; i < 205; ++i) lb.add("p" + std::to_string(i), i);

    EXPECT_EQ(lb.all().size(), 200u);
    EXPECT_EQ(lb.top(1)[0].score, 204);
}

TEST(Leaderboard, LoadsUnquotedLegacyLinesAndSkipsMalformed) {
    TempFile f("legacy");
    {
        std::ofstream out(f.path);
        out << "legacy 7\n";
        out << "\n";          // blank line is skipped
        out << "garbage\n";   // unparseable is skipped
    }

    Leaderboard lb(f.path);
    auto all = lb.all();
    ASSERT_EQ(all.size(), 1u);
    EXPECT_EQ(all[0].name, "legacy");
    EXPECT_EQ(all[0].score, 7);
}
