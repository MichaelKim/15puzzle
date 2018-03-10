#ifndef IDASTAR_H
#define IDASTAR_H

#include "ctpl_stl.h"

#include <mutex>
#include <vector>

template <class THeuristic, class TState>
class Idastar {
private:
    using Move = typename TState::Move;

    std::mutex mutex;
    ctpl::thread_pool pool;
    THeuristic* heuristic;
    std::vector<Move> path;
    int minCost;
    int limit;
    uint64_t nodes;
    bool found;

public:
    Idastar(THeuristic* h);
    virtual ~Idastar();

    std::vector<Move> solve(TState start);
    bool dfs(TState& root, int g, Move prevMove, bool parallel);
    bool dfsParallel(TState& node, int g, Move prevMove);
};

#include "Idastar-inl.h"

#endif
