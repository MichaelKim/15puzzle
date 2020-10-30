#ifndef IDASTAR_H
#define IDASTAR_H

#include <atomic>
#include <vector>

#include "Board.h"
#include "Direction.h"

class Idastar {
private:
    int minCost;
    int limit;
    std::atomic_llong nodes;

    std::atomic_bool found;

public:
    Idastar();

    std::vector<Direction> solve(const Board& start);
    bool dfs(Board& root, int g, Direction prevMove,
             std::vector<Direction>& path);
};

#endif
