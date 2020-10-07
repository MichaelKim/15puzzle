#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include <cstdint>
#include <vector>

namespace DisjointDatabase {

using Grid = std::vector<int>;
using Hash = uint64_t;

extern int width;
extern int height;
extern Grid where;
extern Grid tileDeltas;
extern Grid mirrPos;
extern Grid mirror;

void load(const std::vector<Grid>& grids, int width, int height);
std::vector<Hash> calculatePatterns(const Grid& grid);
int getHeuristic(const std::vector<Hash>& patterns);

}  // namespace DisjointDatabase

#endif  // DISJOINTDATABASE_H
