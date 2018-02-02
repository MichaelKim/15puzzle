#include "../include/Idastar.h"

#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace std;

Idastar::Idastar(DisjointDatabase* d): db(d) {}

vector<Direction> Idastar::solve(Board start) {
    struct Node {
        ID id;
        int f;

        bool operator< (const Node& other) const {
            return f > other.f;
        }
    };

    // Uses A* with additive pattern disjoint database heuristics
    Board goal({{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,0}});

    Node startNode {start.getId(), db->getTotalDist(start)};
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
                        openSet.push(Node{nextId, nextG + db->getTotalDist(next)});
                    }
                }
            }
        }
    }

    return vector<Direction>{};
}



Idastar::~Idastar() {
    delete db;
}
