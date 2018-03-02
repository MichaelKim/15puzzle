#include "Idastar.h"

#include <iostream>

#define INF 1000000

template <class THeuristic, class TState>
Idastar<THeuristic, TState>::Idastar(THeuristic* h)
    : heuristic(h), path({}), minCost(INF), limit(0), nodes(0) {}

template <class THeuristic, class TState>
std::vector<int> Idastar<THeuristic, TState>::solve(TState start) {
    // Uses IDA* with additive pattern disjoint database heuristics
    path.clear();

    limit = heuristic->getHeuristic(start);
    std::cout << "Limit, Nodes:";

    int prevMove = -1;
    while (path.size() == 0) {
        minCost = INF;
        std::cout << " " << limit << ", " << nodes << std::endl;
        dfs(start, 0, prevMove);
        limit = minCost;
    }

    std::cout << "Nodes expanded: " << nodes << std::endl;

    return path;
}

template <class THeuristic, class TState>
bool Idastar<THeuristic, TState>::dfs(TState& node, int g, int prevMove) {
    int h = heuristic->getHeuristic(node);
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
    const std::vector<int>& moves = node.getMoves(prevMove);
    for (int move : moves) {
        node.applyMove(move);

        if (dfs(node, g + 1, move)) {
            path.push_back(move);
            return true;
        }

        node.undoMove(move);
    }

    return false;
}

template <class THeuristic, class TState>
Idastar<THeuristic, TState>::~Idastar() {
    delete heuristic;
}
