#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include "Board.h"
#include "Direction.h"
#include "PartialDatabase.h"

#include <vector>

using namespace std;

class DisjointDatabase {
    private:
        vector<PartialDatabase> databases;

    public:
        DisjointDatabase(vector<vector<vector<int>>> grids);
        virtual ~DisjointDatabase();

        void setup();
        vector<Direction> solve(vector<vector<int>> grid);
        int getTotalDist(const Board& board);

};

#endif // DISJOINTDATABASE_H
