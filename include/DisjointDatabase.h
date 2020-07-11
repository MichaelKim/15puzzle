#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include <vector>

#include "PartialDatabase.h"

class DisjointDatabase {
private:
    std::vector<PartialDatabase> databases;

public:
    DisjointDatabase(
        const std::string& name,
        const std::vector<std::vector<std::vector<unsigned>>>& grids);

    std::vector<int> where;
    std::vector<unsigned> mirrPos;

    std::size_t numPatterns() const;
    int getHeuristic(const std::vector<uint64_t>& patterns) const;
};

#endif  // DISJOINTDATABASE_H
