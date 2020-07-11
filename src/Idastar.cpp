#include "../include/Idastar.h"

#include <iostream>

#define INF 1000000
#define COST 1  // Cost of move

Idastar::Idastar() : path({}), minCost(INF), limit(0), nodes(0) {}

std::vector<Direction> Idastar::solve(Board start) {
    // Uses IDA* with additive pattern disjoint database heuristics
    std::cout << "Running single threaded" << std::endl;

    path.clear();
    nodes = 1;
    limit = start.getHeuristic();

    if (limit > 0) {
        std::cout << "Limit, Nodes:";

        // Starting moves (for prevMove)
        const std::vector<Direction> startMoves = start.getMoves();

        // Boards after first move
        std::vector<Board> firstSlide;
        for (auto startDir : startMoves) {
            auto copy = start;
            copy.applyMove(startDir);
            firstSlide.push_back(copy);
        }

        while (path.empty()) {
            minCost = INF;
            std::cout << " " << limit << ", " << nodes << std::endl;
            for (size_t i = 0; i < startMoves.size(); i++) {
                if (dfs(firstSlide[i], COST, startMoves[i])) {
                    path.push_back(startMoves[i]);
                    break;
                }
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
    auto h = node.getHeuristic();
    auto f = g + h;

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

    if (prevMove != Direction::D && node.canMove(Direction::U)) {
        auto prev = node.applyMove(Direction::U);

        if (dfs(node, g + COST, Direction::U)) {
            path.push_back(Direction::U);
            return true;
        }

        node.undoMove(prev, Direction::U);
    }

    if (prevMove != Direction::L && node.canMove(Direction::R)) {
        auto prev = node.applyMove(Direction::R);

        if (dfs(node, g + COST, Direction::R)) {
            path.push_back(Direction::R);
            return true;
        }

        node.undoMove(prev, Direction::R);
    }

    if (prevMove != Direction::U && node.canMove(Direction::D)) {
        auto prev = node.applyMove(Direction::D);

        if (dfs(node, g + COST, Direction::D)) {
            path.push_back(Direction::D);
            return true;
        }

        node.undoMove(prev, Direction::D);
    }

    if (prevMove != Direction::R && node.canMove(Direction::L)) {
        auto prev = node.applyMove(Direction::L);

        if (dfs(node, g + COST, Direction::L)) {
            path.push_back(Direction::L);
            return true;
        }

        node.undoMove(prev, Direction::L);
    }

    return false;
}
