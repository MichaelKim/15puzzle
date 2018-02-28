#ifndef IDASTAR_H
#define IDASTAR_H

#include "Board.h"
#include "DisjointDatabase.h"

#include <vector>

class Idastar {
    private:
        DisjointDatabase* db;
        std::vector<int> path;
        int minCost;
        int limit;
        uint64_t nodes;

    public:
        Idastar(DisjointDatabase* d);
        virtual ~Idastar();

        std::vector<int> solve(Board start);
        bool dfs(Board& root, int g, int prevMove);
};

#endif
