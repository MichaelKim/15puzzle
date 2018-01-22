#include "DisjointDatabase.h"

#include <priority_queue>
#include <unordered_set>

using namespace std;

DisjointDatabase::DisjointDatabase(vector<vector<vector<int>>> grids) {
    for (int i = 0; i < grids.size(); i++) {
        databases.push_back(PartialDatabase(grids[i], to_string(i)));
    }
}

void DisjointDatabase::setup() {
    for (PartialDatabase& pd: databases) {
        pd.setup();
    }
}

vector<Direction> DisjointDatabase::solve(vector<vector<int>> grid) {
    // Uses A* with additive pattern disjoint database heuristics
    Board start(grid);

    // Nodes already evaluated
    unordered_set<string> closedSet;
    // Currently discovered nodes, but not evaluated yet
    priority_queue<pair<int, string>> openSet;
    // Which node a node can be most efficiently be reached from
    unordered_map<string, string> cameFrom;
    // Cost of getting from start node to that node
    // Default value is infinity; don't check with [], use find()
    unordered_map<string, int> gScore;
    // Starting grid has no cost
    gScore[start.getId()] = 0;
    // Cost of getting from start node to goal by passing that node
    unordered_map<string, int> fScore;
    // Starting grid is heuristical
    fScore[start.getId()] = getTotalDist(grid);

    while(!openSet.empty()) {
        string currId = // node in openSet with lowest fScore

        if (currId == goal) {
            return generatePath(cameFrom, currId);
        }
    }



    return vector<Direction>{Direction::N, Direction::E};
}

int DisjointDatabase::getTotalDist(const vector<vector<int>>& grid) {
    int totalDist = 0;
    for (PartialDatabase pd: databases) {
        totalDist += pd.getDist(grid);
    }
    return totalDist;
}

DisjointDatabase::~DisjointDatabase() {}
