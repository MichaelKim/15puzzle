#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include "Board.h"
#include "PartialDatabase.h"

#include <vector>

class DisjointDatabase {
private:
    std::vector<PartialDatabase*> databases;
    int numDatabases;
    std::vector<int> where;

public:
    static const int MAX_DATABASE = 4;
    DisjointDatabase(int len, std::string name,
                     std::vector<std::vector<std::vector<int>>> grids);
    virtual ~DisjointDatabase();

    int getHeuristic(const Board& board);
};

#endif  // DISJOINTDATABASE_H
