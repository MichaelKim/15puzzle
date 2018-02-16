#include "../include/Idastar.h"

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>

#define INF 1000000

using namespace std;

Idastar::Idastar(DisjointDatabase* d): db(d), path({}), minCost(INF), limit(0) {}

vector<int> Idastar::solve(Board& start) {
    // Uses IDA* with additive pattern disjoint database heuristics
    path.clear();

    limit = db->getHeuristic(start);
    cout << "Limit:";
    do {
        minCost = INF;
        cout << " " << limit;
        dfs(start, 0);
        limit = minCost;
    } while (path.size() == 0);
    cout << endl;
    return path;
}

bool Idastar::dfs(Board &node, int g) {
    int f = g + db->getHeuristic(node);

    if (f <= limit && node.getId() == 0xfedcba987654321) {
        return true;
    }

    if (f > limit) {
        if (f < minCost) {
            minCost = f;
        }
        return false;
    }

    vector<int> moves = node.getMoves();

    for (int move: moves) {
        node.applyMove(move);

        if (dfs(node, g + 1)) {
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
