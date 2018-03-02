#ifndef IDASTAR_H
#define IDASTAR_H

#include <vector>

template <class THeuristic, class TState>
class Idastar {
private:
    using Move = typename TState::Move;

    THeuristic* heuristic;
    std::vector<Move> path;
    int minCost;
    int limit;
    uint64_t nodes;

public:
    Idastar(THeuristic* h);
    virtual ~Idastar();

    std::vector<Move> solve(TState start);
    bool dfs(TState& root, int g, Move prevMove);
};

#include "Idastar-inl.h"

#endif
