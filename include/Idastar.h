#ifndef IDASTAR_H
#define IDASTAR_H

#include "DisjointDatabase.h"
#include "Board.h"
#include "Direction.h"

#include <vector>

using namespace std;

class Idastar {
    private:
        DisjointDatabase* db;
        vector<Direction> path;
        int minCost;
        int limit;

        struct Node {
            ID id;
            int f;

            bool operator< (const Node& other) const {
                return f > other.f;
            }
        };

    public:
        Idastar(DisjointDatabase* d);
        virtual ~Idastar();

        vector<Direction> solve(Board& start);
        bool dfs(Board& root, int g, int prevDir);
};

#endif
