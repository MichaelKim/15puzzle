#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include "PartialDatabase.h"

#include <vector>

class DisjointDatabase {
private:
    std::vector<std::shared_ptr<PartialDatabase>> databases;

public:
    DisjointDatabase(int len, std::string name,
                     std::vector<std::vector<std::vector<int>>> grids);
    virtual ~DisjointDatabase();

    std::vector<int> where;

    int numPatterns();
    int getHeuristic(const std::vector<uint64_t>& positions);
};

#endif  // DISJOINTDATABASE_H
