#ifndef IDASTAR_H
#define IDASTAR_H

#include "Board.h"
#include "Direction.h"

#include <vector>

class Idastar {
private:
    std::vector<Direction> path;
    int minCost;
    int limit;
    uint64_t nodes;

public:
    Idastar();
    virtual ~Idastar();

    std::vector<Direction> solve(Board start);
    bool dfs(Board& root, int g, Direction prevMove);
};

#endif
