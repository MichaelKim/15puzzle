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
    Board goal({{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,0});

    // Nodes already evaluated
    unordered_set<string> closedSet;
    // Currently discovered nodes, but not evaluated yet
    unordered_set<string> openSet;
    // Which node a node can be most efficiently be reached from
    unordered_map<string, pair<string, Direction>> cameFrom;
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
        string currId = ""; // node in openSet with lowest fScore
        int minF = 1000000;
        for (string id: openSet) {
            if (fScore[id] < minF) {
                minF = fScore[id];
                currId = id;
            }
        }

        if (currId == goal) {
            vector<Direction> moves(0);

            while (cameFrom.find(currId) != cameFrom.end()) {
                pair<string, Direction> prev = cameFrom[currId];
                currId = prev.first;
                moves.push_back(prev.second);
            }

            return moves;
        }

        openSet.erase(currId);
        closedSet.emplace(currId);


    }

    return vector<Direction>{};
}

int DisjointDatabase::getTotalDist(const vector<vector<int>>& grid) {
    int totalDist = 0;
    for (PartialDatabase pd: databases) {
        totalDist += pd.getDist(grid);
    }
    return totalDist;
}

DisjointDatabase::~DisjointDatabase() {}
