#include "Idastar.h"

#include <iostream>

#define INF 1000000

template <class TDomain>
Idastar<TDomain>::Idastar(TDomain d)
    : domain(d), path({}), minCost(INF), limit(0), nodes(0) {}

template <class TDomain>
std::vector<typename TDomain::Move> Idastar<TDomain>::solve(State start) {
    // Uses IDA* with additive pattern disjoint database heuristics
    path.clear();

    limit = start.getHeuristic();
    std::cout << "Limit, Nodes:";

    Move prevMove = Move::Null;
    while (path.size() == 0) {
        minCost = INF;
        std::cout << " " << limit << ", " << nodes << std::endl;
        dfs(start, 0, prevMove);
        limit = minCost;
    }

    std::cout << "Nodes expanded: " << nodes << std::endl;

    return path;
}

template <class TDomain>
bool Idastar<TDomain>::dfs(State& node, int g, Move prevMove) {
    int h = node.getHeuristic();
    int f = g + h;

    if (f <= limit) {
        if (h == 0) {
            return true;
        }
    }
    else {
        if (f < minCost) {
            minCost = f;
        }
        return false;
    }
    nodes++;

    const auto& moves = domain.getMoves(node, prevMove);
    for (auto move : moves) {
        int cost = domain.applyMove(node, move);

        if (dfs(node, g + cost, move)) {
            path.push_back(move);
            return true;
        }

        domain.undoMove(node, move);
    }

    return false;
}

template <class TDomain>
Idastar<TDomain>::~Idastar() {}
