#include "../include/DisjointDatabase.h"

#include "../include/Board.h"

#include <queue>
#include <unordered_set>
#include <iostream>

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
    typedef string ID;

    // Uses A* with additive pattern disjoint database heuristics
    Board start(grid);
    Board goal({{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,0}});
    ID startId = start.getId();
    ID goalId = goal.getId();

    // Nodes already evaluated
    unordered_set<ID> closedSet;
    // Currently discovered nodes, but not evaluated yet
    unordered_set<ID> openSet;
    openSet.emplace(startId);
    // Which node a node can be most efficiently be reached from
    unordered_map<ID, pair<ID, Direction>> cameFrom;
    // Cost of getting from start node to that node
    // Default value is infinity; don't check with [], use find()
    unordered_map<ID, int> gScore;
    // Starting grid has no cost
    gScore[startId] = 0;
    // Cost of getting from start node to goal by passing that node
    unordered_map<ID, int> fScore;
    // Starting grid is heuristical
    fScore[startId] = getTotalDist(grid);

    while(!openSet.empty()) {
        ID currId; // node in openSet with lowest fScore
        int minF = 1000000;
        for (ID id: openSet) {
            if (fScore.find(id) == fScore.end()) {
                fScore[id] = 1000000;
            }
            if (fScore[id] < minF) {
                minF = fScore[id];
                currId = id;
            }
        }

        if (currId == goalId) {
            vector<Direction> moves(0);

            while (cameFrom.find(currId) != cameFrom.end()) {
                pair<ID, Direction> prev = cameFrom[currId];
                currId = prev.first;
                moves.push_back(prev.second);
            }

            return moves;
        }

        openSet.erase(currId);
        closedSet.emplace(currId);

        Board curr(currId, 4, 4);
        Point blank = curr.getBlank();

        for (int i = 0; i < 4; i++) {
            Direction dir = static_cast<Direction>(i);
            if (curr.canShiftBlank(dir)) {
                Board next = curr;
                next.shiftBlank(dir);
                ID nextId = next.getId();
                if (closedSet.find(nextId) == closedSet.end()) {
                    if (openSet.find(nextId) == openSet.end()) {
                        openSet.emplace(nextId);
                    }

                    if (gScore.find(nextId) == gScore.end()) {
                        gScore[nextId] = 1000000;
                    }

                    int nextG = gScore[currId] + 1;
                    if (nextG < gScore[nextId]) {
                        cameFrom[nextId] = make_pair(currId, dir);
                        gScore[nextId] = nextG;
                        fScore[nextId] = nextG + getTotalDist(next.grid);
                    }
                }
            }
        }
    }

    return vector<Direction>{};
}

int DisjointDatabase::getTotalDist(const vector<vector<int>>& grid) {
    vector<string> ids(databases.size(), "");

    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            for (int i = 0; i < databases.size(); i++) {
                if (ids[i].length() > 0) {
                    ids[i] += "_";
                }
                if (databases[i].cells.find(grid[y][x]) != databases[i].cells.end()) {
                    ids[i] += to_string(grid[y][x]);
                }
                else {
                    ids[i] += "0";
                }
            }
        }
    }
    int totalDist = 0;
    for (int i = 0; i < databases.size(); i++) {
        totalDist += databases[i].distMap[ids[i]];
    }
    cout << totalDist << endl;
    // for (PartialDatabase& pd: databases) {
    //     totalDist += pd.getDist(grid);
    // }
    return totalDist;
}

DisjointDatabase::~DisjointDatabase() {}
