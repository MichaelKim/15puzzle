#include "Idastar.h"

#include "InputParser.h"

#include <future>
#include <iostream>
#include <thread>

#define INF 1000000

template <class THeuristic, class TState>
Idastar<THeuristic, TState>::Idastar(THeuristic* h)
    : pool(std::thread::hardware_concurrency()),
      heuristic(h),
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

    bool runParallel = InputParser::runParallel() && pool.size() > 0;

    if (runParallel) {
        std::cout << "Running with " << pool.size() << " threads" << std::endl;
    }
    else {
        std::cout << "Running single threaded" << std::endl;
    }

    limit = heuristic->getHeuristic(start);

    if (limit > 0) {
        std::cout << "Limit, Nodes:";

        Move prevMove = Move::Null;
        while (path.size() == 0) {
            minCost = INF;
            std::cout << " " << limit << ", " << nodes << std::endl;
            dfs(start, 0, prevMove, runParallel);
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
bool Idastar<THeuristic, TState>::dfs(TState& node, int g, Move prevMove,
                                      bool parallel) {
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

    if (parallel && f - 42 >= 10) {
        if (dfsParallel(node, g, prevMove)) {
            return true;
        }
    }
    else {
        const auto& moves = node.getMoves(prevMove);
        for (auto move : moves) {
            int cost = node.applyMove(move);

            if (dfs(node, g + cost, move, parallel)) {
                path.push_back(move);
                return true;
            }

            node.undoMove(move);
        }
    }

    return false;
}

template <class THeuristic, class TState>
bool Idastar<THeuristic, TState>::dfsParallel(TState& node, int g,
                                              Move prevMove) {
    std::vector<std::future<bool>> results;

    const auto& moves = node.getMoves(prevMove);
    for (auto move : moves) {
        results.emplace_back(pool.push(
            [this, g, move](int id, TState node) {
                int cost = node.applyMove(move);

                if (dfs(node, g + cost, move, false)) {
                    path.push_back(move);
                    return true;
                }

                node.undoMove(move);
                return false;
            },
            node));
    }

    for (auto&& result : results) {
        if (result.get()) return true;
    }

    return false;
}

template <class THeuristic, class TState>
Idastar<THeuristic, TState>::~Idastar() {
    delete heuristic;
}
