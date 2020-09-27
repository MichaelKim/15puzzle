#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include <array>
#include <string>
#include <vector>

namespace DisjointDatabase {

using Grid = std::array<int, 16>;

extern std::array<int, 16> where;
extern Grid tileDeltas;
extern Grid mirrPos;

constexpr auto mirror = [] {
    Grid m{};
    // TODO: test with blank not in top-left or bottom-right
    for (int i = 0; i < 16; i++) {
        int y = i / 4;
        int x = i % 4;
        m[i] = (x * 4) + y;
    }
    return m;
}();

void load(const std::vector<Grid>& grids);
std::vector<uint64_t> calculatePatterns(const Grid& grid);
int getHeuristic(const std::vector<uint64_t>& patterns);

}  // namespace DisjointDatabase

#endif  // DISJOINTDATABASE_H
