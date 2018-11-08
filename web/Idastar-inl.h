#include "Idastar.h"

#include <future>
#include <iostream>
#include <thread>

#define INF 1000000

template <class THeuristic, class TState>
Idastar<THeuristic, TState>::Idastar(THeuristic* h)
    : heuristic(h),
      path({}),
      minCost(INF),
      limit(0),
      nodes(0),
      found(false) {}

template <class THeuristic, class TState>
std::vector<typename TState::Move> Idastar<THeuristic, TState>::solve(
    TState start) {
    // Uses IDA* with additive pattern disjoint database heuristics
    path.clear();
    found = false;

    std::cout << "Running single threaded" << std::endl;

    limit = heuristic->getHeuristic(start);

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
    }
    else {
        std::cout << "Already solved" << std::endl;
    }

    return path;
}

template <class THeuristic, class TState>
bool Idastar<THeuristic, TState>::dfs(TState& node, int g, Move prevMove) {
    if (found) return false;

    int h = heuristic->getHeuristic(node);
    int f = g + h;

    if (f <= limit) {
        if (h == 0) {
            found = true;
            return true;
        }
    }
    else {
        if (f < minCost) {
            minCost = f;
        }
        return false;
    }

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

template <class THeuristic, class TState>
Idastar<THeuristic, TState>::~Idastar() {
    delete heuristic;
}
