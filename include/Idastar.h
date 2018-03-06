#ifndef IDASTAR_H
#define IDASTAR_H

#include <vector>

template <class TDomain>
class Idastar {
private:
    using Move = typename TDomain::Move;
    using State = typename TDomain::State;

    TDomain& domain;
    std::vector<Move> path;
    int minCost;
    int limit;
    uint64_t nodes;

public:
    Idastar(TDomain d);
    virtual ~Idastar();

    std::vector<Move> solve(State start);
    bool dfs(State& root, int g, Move prevMove);
};

#include "Idastar-inl.h"

#endif
