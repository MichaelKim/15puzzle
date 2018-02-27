#include "../include/DisjointDatabase.h"

#include "../include/Board.h"

#include <iostream>

#define INF 1000000

using namespace std;

DisjointDatabase::DisjointDatabase(vector<vector<vector<int>>> grids):
    numDatabases(grids.size()),
    ids(vector<uint64_t>(numDatabases, 0))
{
    where = vector<int>(Board::LEN, -1);

    for (int i = 0; i < grids.size(); i++) {
        PartialDatabase* pd = new PartialDatabase(grids[i], to_string(i));
        databases.push_back(pd);

        for (int j = 0; j < Board::LEN; j++) {
            if (pd->cells.find(j) != pd->cells.end()) {
                where[j] = i;
            }
        }
    }
}

int DisjointDatabase::getHeuristic(const Board& board) {
    fill(ids.begin(), ids.end(), 0);
    uint64_t temp = board.getId();

    for (int i = 0; i < Board::LEN; i++) {
        uint64_t n = (temp & (0xfull << (4 * i))) >> (4 * i);

        int j = where[n];
        if (j >= 0 && j < numDatabases) {
            ids[j] |= n << (4 * (Board::LEN - i - 1));
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
