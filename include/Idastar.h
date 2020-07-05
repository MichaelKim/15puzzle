#ifndef IDASTAR_H
#define IDASTAR_H

#include <vector>

#include "Board.h"
#include "Direction.h"

class Idastar {
private:
    std::vector<Direction> path;
    int minCost;
    int limit;
    uint64_t nodes;

public:
    Idastar();

    std::vector<Direction> solve(Board start);
    bool dfs(Board& root, int g, Direction prevMove);
};

#endif
