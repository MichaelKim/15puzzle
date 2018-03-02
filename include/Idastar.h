#ifndef IDASTAR_H
#define IDASTAR_H

#include <vector>

template <class THeuristic, class TState>
class Idastar {
private:
    THeuristic* heuristic;
    std::vector<int> path;
    int minCost;
    int limit;
    uint64_t nodes;

public:
    Idastar(THeuristic* h);
    virtual ~Idastar();

    std::vector<int> solve(TState start);
    bool dfs(TState& root, int g, int prevMove);
};

#include "Idastar-inl.h"

#endif
