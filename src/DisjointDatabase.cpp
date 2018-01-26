#include "../include/DisjointDatabase.h"

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
    typedef vector<int> ID;
    struct Hash {
        size_t operator() (const ID& id) const {
            size_t hash = 0;
            for (int i = 0; i < id.size(); i++) {
                hash += id[i] * i;
            }
            return hash;
        }
    };

    // Uses A* with additive pattern disjoint database heuristics
    Board start(grid);
    Board goal({{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,0}});
    ID startId = start.getId2();
    ID goalId = goal.getId2();

    // Nodes already evaluated
    unordered_set<ID, Hash> closedSet;
    // Currently discovered nodes, but not evaluated yet
    unordered_set<ID, Hash> openSet;
    openSet.emplace(startId);
    // Which node a node can be most efficiently be reached from
    unordered_map<ID, pair<ID, Direction>, Hash> cameFrom;
    // Cost of getting from start node to that node
    // Default value is infinity; don't check with [], use find()
    unordered_map<ID, int, Hash> gScore;
    // Starting grid has no cost
    gScore[startId] = 0;
    // Cost of getting from start node to goal by passing that node
    unordered_map<ID, int, Hash> fScore;
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

        Board curr(currId, 4, 4);

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

        Point blank = curr.getBlank();
        // Up
        if (blank.y > 0) {
            Board next = curr;
            swap(next.grid[blank.y][blank.x], next.grid[blank.y - 1][blank.x]);
            ID nextId = next.getId2();
            if (closedSet.find(nextId) == closedSet.end()) {
                if (openSet.find(nextId) == openSet.end()) {
                    openSet.emplace(nextId);
                }

                if (gScore.find(nextId) == gScore.end()) {
                    gScore[nextId] = 1000000;
                }

                int nextG = gScore[currId] + 1;
                if (nextG < gScore[nextId]) {
                    cameFrom[nextId] = make_pair(currId, Direction::N);
                    gScore[nextId] = nextG;
                    fScore[nextId] = nextG + getTotalDist(next.grid);
                }
            }
        }
        // Right
        if (blank.x < curr.grid[blank.y].size() - 1) {
            Board next = curr;
            swap(next.grid[blank.y][blank.x], next.grid[blank.y][blank.x + 1]);
            ID nextId = next.getId2();

            if (closedSet.find(nextId) == closedSet.end()) {
                if (openSet.find(nextId) == openSet.end()) {
                    openSet.emplace(nextId);
                }

                if (gScore.find(nextId) == gScore.end()) {
                    gScore[nextId] = 1000000;
                }

                int nextG = gScore[currId] + 1;
                if (nextG < gScore[nextId]) {
                    cameFrom[nextId] = make_pair(currId, Direction::E);
                    gScore[nextId] = nextG;
                    fScore[nextId] = nextG + getTotalDist(next.grid);
                }
            }
        }
        // Down
        if (blank.y < curr.grid.size() - 1) {
            Board next = curr;
            swap(next.grid[blank.y][blank.x], next.grid[blank.y + 1][blank.x]);
            ID nextId = next.getId2();
            if (closedSet.find(nextId) == closedSet.end()) {
                if (openSet.find(nextId) == openSet.end()) {
                    openSet.emplace(nextId);
                }

                if (gScore.find(nextId) == gScore.end()) {
                    gScore[nextId] = 1000000;
                }

                int nextG = gScore[currId] + 1;
                if (nextG < gScore[nextId]) {
                    cameFrom[nextId] = make_pair(currId, Direction::S);
                    gScore[nextId] = nextG;
                    fScore[nextId] = nextG + getTotalDist(next.grid);
                }
            }
        }
        // Left
        if (blank.x > 0) {
            Board next = curr;
            swap(next.grid[blank.y][blank.x], next.grid[blank.y][blank.x - 1]);
            ID nextId = next.getId2();
            if (closedSet.find(nextId) == closedSet.end()) {
                if (openSet.find(nextId) == openSet.end()) {
                    openSet.emplace(nextId);
                }

                if (gScore.find(nextId) == gScore.end()) {
                    gScore[nextId] = 1000000;
                }

                int nextG = gScore[currId] + 1;
                if (nextG < gScore[nextId]) {
                    cameFrom[nextId] = make_pair(currId, Direction::W);
                    gScore[nextId] = nextG;
                    fScore[nextId] = nextG + getTotalDist(next.grid);
                }
            }
        }
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
