#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include <memory>
#include <vector>

#include "PartialDatabase.h"

class DisjointDatabase {
private:
    std::vector<std::unique_ptr<PartialDatabase>> databases;

public:
    DisjointDatabase(const std::string& name,
                     std::vector<std::vector<std::vector<uint>>> grids);
    virtual ~DisjointDatabase();

    std::vector<int> where;
    std::vector<uint> mirrPos;

    std::size_t numPatterns() const;
    int getHeuristic(const std::vector<uint64_t>& patterns) const;
};

#endif  // DISJOINTDATABASE_H
