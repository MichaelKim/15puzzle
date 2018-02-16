#include "../include/DisjointDatabase.h"

#include "../include/Board.h"

#define INF 1000000

using namespace std;

DisjointDatabase::DisjointDatabase(vector<vector<vector<int>>> grids) {
    for (int i = 0; i < grids.size(); i++) {
        databases.push_back(new PartialDatabase(grids[i], to_string(i)));
    }
}

int DisjointDatabase::getHeuristic(const Board& board) {
    vector<ID> ids(databases.size(), 0);

    for (int y = Board::SIZE - 1; y >= 0; y--) {
        for (int x = Board::SIZE - 1; x >= 0; x--) {
            int n = board.getCell(x, y);

            for (int i = 0; i < databases.size(); i++) {
                if (databases[i]->cells.find(n) != databases[i]->cells.end()) {
                    ids[i] = ids[i] * 16 + n;
                }
                else {
                    ids[i] *= 16;
                }
            }
        }
    }
    int totalDist = 0;
    for (int i = 0; i < databases.size(); i++) {
        totalDist += databases[i]->distMap[ids[i]];
    }

    return totalDist;
}

DisjointDatabase::~DisjointDatabase() {
    for (PartialDatabase* pd: databases) {
        delete pd;
    }
}
