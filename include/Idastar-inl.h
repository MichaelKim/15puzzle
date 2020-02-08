#include "Idastar.h"

#include <iostream>

#define INF 1000000

template <class TState>
Idastar<TState>::Idastar() : path({}), minCost(INF), limit(0), nodes(0) {}

template <class TState>
std::vector<typename TState::Move> Idastar<TState>::solve(TState start) {
    // Uses IDA* with additive pattern disjoint database heuristics
    path.clear();
    nodes = 0;

    std::cout << "Running single threaded" << std::endl;

    limit = start.getHeuristic();

    if (limit > 0) {
        std::cout << "Limit, Nodes:";

        Move prevMove = Move::Null;
        while (path.size() == 0) {
            minCost = INF;
            std::cout << " " << limit << ", " << nodes << std::endl;
            dfs(start, 0, prevMove);
            limit = minCost;
        }

        std::cout << "Nodes expanded: " << nodes << std::endl;
    } else {
        std::cout << "Already solved" << std::endl;
    }

    return path;
}

template <class TState>
bool Idastar<TState>::dfs(TState& node, int g, Move prevMove) {
    int h = node.getHeuristic();
    int f = g + h;

    if (f <= limit) {
        if (h == 0) {
            return true;
        }
    } else {
        if (f < minCost) {
            minCost = f;
        }
        return false;
    }
    nodes += 1;

    const auto& moves = node.getMoves(prevMove);
    for (auto move : moves) {
        int cost = node.applyMove(move);

        if (dfs(node, g + cost, move)) {
            path.push_back(move);
            return true;
        }

        node.undoMove(move);
    }

    return false;
}

template <class TState>
Idastar<TState>::~Idastar() {}
