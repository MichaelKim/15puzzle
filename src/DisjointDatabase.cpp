#include "../include/DisjointDatabase.h"

#include "../include/Board.h"

DisjointDatabase::DisjointDatabase(
    int len, std::string name, std::vector<std::vector<std::vector<int>>> grids)
    : numDatabases(grids.size()),
      where(std::vector<int>(len, -1)),
      ids(std::vector<uint64_t>(numDatabases, 0)) {
    for (int i = 0; i < grids.size(); i++) {
        PartialDatabase* pd = new PartialDatabase(grids[i], name, i);
        databases.push_back(pd);

        for (int j = 0; j < len; j++) {
            if (pd->cells.find(j) != pd->cells.end()) {
                where[j] = i;
            }
        }
    }
}

int DisjointDatabase::getHeuristic(const Board& board) {
    fill(ids.begin(), ids.end(), 0);
    uint64_t temp = board.getId();

    for (int i = 0; i < board.WIDTH * board.HEIGHT; i++) {
        uint64_t n = (temp & (0xfull << (4 * i))) >> (4 * i);

        int j = where[n];
        if (j >= 0 && j < numDatabases) {
            ids[j] |= n << (4 * i);
        }
    }

    int totalDist = 0;
    for (int i = 0; i < numDatabases; i++) {
        totalDist += databases[i]->distMap[ids[i]];
    }

    return totalDist;
}

DisjointDatabase::~DisjointDatabase() {
    for (PartialDatabase* pd : databases) {
        delete pd;
    }
}
