#include "../include/Idastar.h"

#include <iostream>

#define INF 1000000

Idastar::Idastar() : path({}), minCost(INF), limit(0), nodes(0) {}

std::vector<Direction> Idastar::solve(Board start) {
    // Uses IDA* with additive pattern disjoint database heuristics
    std::cout << "Running single threaded" << std::endl;

    path.clear();
    nodes = 0;
    limit = start.getHeuristic();

    // Starting moves (for prevMove)
    const std::vector<Direction> startMoves = start.getMoves();

    if (limit > 0) {
        std::cout << "Limit, Nodes:";

        while (path.empty()) {
            minCost = INF;
            std::cout << " " << limit << ", " << nodes << std::endl;
            for (auto startDir : startMoves) {
                if (dfs(start, 0, startDir)) break;
            }
            limit = minCost;
        }

        std::cout << "Nodes expanded: " << nodes << std::endl;
    } else {
        std::cout << "Already solved" << std::endl;
    }

    return path;
}

bool Idastar::dfs(Board& node, int g, Direction prevMove) {
    int h = node.getHeuristic();
    int f = g + h;

    if (f <= limit) {
        if (h == 0) {
            // Found goal state (heuristic = 0)
            return true;
        }
    } else {
        // Exceeded search depth, store next smallest depth
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

Idastar::~Idastar() = default;
