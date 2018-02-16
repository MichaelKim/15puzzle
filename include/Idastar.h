#ifndef IDASTAR_H
#define IDASTAR_H

#include "DisjointDatabase.h"
#include "Board.h"

#include <vector>

using namespace std;

class Idastar {
    private:
        DisjointDatabase* db;
        vector<int> path;
        int minCost;
        int limit;

    public:
        Idastar(DisjointDatabase* d);
        virtual ~Idastar();

        vector<int> solve(Board& start);
        bool dfs(Board& root, int g);
};

#endif
