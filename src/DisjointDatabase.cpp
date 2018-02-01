#include "../include/DisjointDatabase.h"

#include "../include/Board.h"

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

#define INF 1000000

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
    struct Node {
        ID id;
        int f;

        bool operator< (const Node& other) const {
            return f > other.f;
        }
    };

    // Uses A* with additive pattern disjoint database heuristics
    Board start(grid);
    Board goal({{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,0}});

    Node startNode {start.getId(), getTotalDist(start)};
    ID goalID = goal.getId();

    // Nodes already evaluated
    unordered_set<ID> closedSet;
    // Currently discovered nodes, but not evaluated yet
    priority_queue<Node> openSet;
    openSet.push(startNode);
    // Which node a node can be most efficiently be reached from
    unordered_map<ID, pair<ID, Direction>> cameFrom;
    // Cost of getting from start node to that node
    // Default value is infinity; don't check with [], use find()
    unordered_map<ID, int> gScore;
    // Starting grid has no cost
    gScore[startNode.id] = 0;

    while(!openSet.empty()) {
        Node currNode = openSet.top();
        ID currId = currNode.id;

        if (currId == goalID) {
            vector<Direction> moves(0);

            while (cameFrom.find(currId) != cameFrom.end()) {
                pair<ID, Direction> prev = cameFrom[currId];
                currId = prev.first;
                moves.push_back(prev.second);
            }

            return moves;
        }

        // cout << currId << " " << currNode.f << endl;

        openSet.pop();
        closedSet.emplace(currId);

        Board curr(currId);
        Point blank = curr.getBlank();

        for (int i = 0; i < 4; i++) {
            Direction dir = static_cast<Direction>(i);
            if (curr.canShiftBlank(dir)) {
                Board next = curr;
                next.shiftBlank(dir);

                ID nextId = next.getId();
                if (closedSet.find(nextId) == closedSet.end()) {
                    int nextG = gScore[currId] + 1;
                    auto it = gScore.find(nextId);
                    if (it == gScore.end() || nextG < it->second) {
                        cameFrom[nextId] = make_pair(currId, dir);
                        gScore[nextId] = nextG;
                        openSet.push(Node{nextId, nextG + getTotalDist(next)});
                    }
                }
            }
        }
    }

    return vector<Direction>{};
}

int DisjointDatabase::getTotalDist(const Board& board) {
    vector<unsigned long long int> ids(databases.size(), 0);

    for (int y = Board::SIZE - 1; y >= 0; y--) {
        for (int x = Board::SIZE - 1; x >= 0; x--) {
            int n = board.getCell(x, y);

            for (int i = 0; i < databases.size(); i++) {
                if (databases[i].cells.find(n) != databases[i].cells.end()) {
                    ids[i] = ids[i] * 16 + n;
                }
                else {
                    ids[i] *= 16;
                }
            }
        }
    }
    int totalDist = 0;
    for (int i = 0; i < databases.size(); i++) {
        totalDist += databases[i].distMap[ids[i]];
    }
    // for (PartialDatabase& pd: databases) {
    //     totalDist += pd.getDist(board);
    // }
    return totalDist;
}

DisjointDatabase::~DisjointDatabase() {}
