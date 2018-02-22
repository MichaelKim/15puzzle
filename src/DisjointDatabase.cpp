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
    vector<uint64_t> ids(numDatabases, 0);
    uint64_t temp = board.getId();

    int len = Board::SIZE * Board::SIZE;

    for (int i = 0; i < len; i++) {
        int n = temp & 0xf;
        temp /= 16;

        int j = where[n];
        if (j >= 0 && j < numDatabases) {
            int k = 4 * (len - i - 1);
            ids[j] = (ids[j] & ~(0xfull << k)) | ((uint64_t) n << k);
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
