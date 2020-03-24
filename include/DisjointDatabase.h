#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include "PartialDatabase.h"

#include <memory>
#include <vector>

class DisjointDatabase {
private:
    std::vector<std::shared_ptr<PartialDatabase>> databases;

public:
    DisjointDatabase(int len, const std::string& name,
                     std::vector<std::vector<std::vector<int>>> grids);
    virtual ~DisjointDatabase();

    std::vector<int> where;

    int numPatterns() const;
    int getHeuristic(const std::vector<uint64_t>& patterns) const;
    int getHeuristicDelta(int index, uint64_t prevPattern,
                          uint64_t nextPattern) const;
};

#endif  // DISJOINTDATABASE_H
