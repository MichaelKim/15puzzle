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

    public:
        Idastar(DisjointDatabase* d);
        virtual ~Idastar();

        vector<Direction> solve(Board start);
};

#endif
