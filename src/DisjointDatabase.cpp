#include "../include/DisjointDatabase.h"

#include "../include/Board.h"

DisjointDatabase::DisjointDatabase(
    int len, std::string name, std::vector<std::vector<std::vector<int>>> grids)
    : numDatabases(grids.size()), where(std::vector<int>(len, -1)) {
    for (int i = 0; i < grids.size(); i++) {
        PartialDatabase* pd = new PartialDatabase(grids[i], name, i);
        databases.push_back(pd);

        for (auto it : pd->cells) {
            if (where[it.first] != -1) {
                throw "Error: patterns overlapping";
            }

            where[it.first] = i;
        }
    }

    // Check that the patterns perfectly cover the board
    // (except for the blank tile)
    bool foundBlank = false;
    for (auto& i : where) {
        if (i == -1) {
            if (foundBlank) {
                throw "Error: found multiple blank tiles";
            }

            // Assume blank tile
            foundBlank = true;
            i = 0;
        }
    }
}

int DisjointDatabase::getHeuristic(const Board& board) {
    uint64_t ids[MAX_DATABASE] = {};
    uint64_t temp = board.getId();

    for (int i = 0; i < 16; i++) {
        int n = temp & 0xf;
        temp >>= 4;

        int j = where[n];

        // If n is the blank tile, n = 0
        // The blank tile (or any tile that isn't covered by a pattern)
        // has where[n] = 0. So, performing this operation
        // is safe for the blank tile, and a noop.
        ids[j] |= (uint64_t)n << (4 * i);
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
