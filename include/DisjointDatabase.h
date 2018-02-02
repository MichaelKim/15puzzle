#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include "Board.h"
#include "Direction.h"
#include "PartialDatabase.h"

#include <vector>

using namespace std;

class DisjointDatabase {
    private:
        vector<PartialDatabase*> databases;

    public:
        DisjointDatabase(vector<vector<vector<int>>> grids);
        virtual ~DisjointDatabase();

        int getTotalDist(const Board& board);

};

#endif // DISJOINTDATABASE_H
