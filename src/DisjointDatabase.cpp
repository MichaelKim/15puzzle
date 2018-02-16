#include "../include/DisjointDatabase.h"

#include "../include/Board.h"

#include <iostream>

#define INF 1000000

using namespace std;

DisjointDatabase::DisjointDatabase(vector<vector<vector<int>>> grids): numDatabases(grids.size()) {
    where = vector<int>(Board::SIZE * Board::SIZE, -1);

    for (int i = 0; i < grids.size(); i++) {
        PartialDatabase* pd = new PartialDatabase(grids[i], to_string(i));
        databases.push_back(pd);

        for (int j = 0; j < Board::SIZE * Board::SIZE; j++) {
            if (pd->cells.find(j) != pd->cells.end()) {
                where[j] = i;
            }
        }
    }
}

int DisjointDatabase::getHeuristic(const Board& board) {
    vector<ID> ids(numDatabases, 0);
    ID temp = board.getId();

    for (int i = 0; i < Board::SIZE * Board::SIZE; i++) {
        int n = temp & 0xf;
        temp >>= 4;

        int j = where[n];
        if (j >= 0 && j < numDatabases) {
            ID grid = ids[j];
            int k = 4 * (Board::SIZE * Board::SIZE - i - 1);
            grid &= ~(0xfull << k);
            grid |= (ID) n << k;

            ids[j] = grid;
        }
    }

    int totalDist = 0;
    for (int i = 0; i < numDatabases; i++) {
        totalDist += databases[i]->distMap[ids[i]];
    }

    return totalDist;
}

DisjointDatabase::~DisjointDatabase() {
    for (PartialDatabase* pd: databases) {
        delete pd;
    }
}
