#include "../include/Idastar.h"

#include <iostream>

#define INF 1000000

Idastar::Idastar(DisjointDatabase* d):
    db(d),
    path({}),
    minCost(INF),
    limit(0),
    nodes(0) {}

std::vector<int> Idastar::solve(Board start) {
    // Uses IDA* with additive pattern disjoint database heuristics
    path.clear();

    limit = db->getHeuristic(start);
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

bool Idastar::dfs(Board &node, int g, int prevMove) {
    int h = db->getHeuristic(node);
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
    for (int move: moves) {
        node.applyMove(move);

        if (dfs(node, g + 1, move)) {
            path.push_back(move);
            return true;
        }

        node.undoMove(move);
    }

    return false;
}

Idastar::~Idastar() {
    delete db;
}
