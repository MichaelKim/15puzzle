#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include <array>
#include <vector>

#include "PartialDatabase.h"

class DisjointDatabase {
private:
    std::vector<PartialDatabase> databases;
    std::vector<std::vector<int>> costs;

    std::vector<int> loadDatabase(std::string filename, int size);

public:
    DisjointDatabase(const std::string& name,
                     const std::vector<std::array<int, 16>>& grids);

    std::array<int, 16> where;
    // The reflected positions of the tiles
    std::array<int, 16> mirrPos;

    std::size_t numPatterns() const;
    int getHeuristic(const std::vector<uint64_t>& patterns) const;
};

#endif  // DISJOINTDATABASE_H
